"""Addon-owned bridge to existing guarded Scene.ps1; no renderer manager.

The authoritative coordinator invokes this ONLY for run --execute. Importing,
planning or testing this module starts nothing. User hold is independently gated.
"""
from pathlib import Path
import hashlib,json,os,subprocess,sys,time,uuid,threading
from evidence import sha,read_json,save,assertion,review_console,screenshot
SLOT=Path(r'C:\Users\marco\Documents\Codex\2026-10-12\task\RUNTIME_SLOT.json')
def hold_active(lease):
 if str(lease.get('status','')).startswith('user_runtime_hold'):return True
 if 'userRuntimeHold' not in lease:return False
 hold=lease['userRuntimeHold']
 return not isinstance(hold,dict) or hold.get('active') is not False
def check_sources(p):
 closures=Path(p['closures']);assert sha(closures)==p['closurePinSHA256'],'Closure descriptor pin differs'
 result=[]
 for row in read_json(closures):
  root=Path(row['root']).resolve();manifest=Path(row['manifest']);assert sha(manifest)==p['completeManifestPins'][str(manifest)],'Complete manifest pin differs'
  pins=read_json(manifest);files={x.relative_to(root).as_posix():x for x in root.rglob('*') if x.is_file()}
  assert set(files)==set(pins),'Complete source set differs'
  assert all(sha(files[n])==value for n,value in pins.items()),'Complete source bytes differ'
  result.append({'root':str(root),'manifest':str(manifest),'sha256':sha(manifest),'files':len(pins)})
 return result
def compile_gate(request,p,closures):
 adoption=read_json(p['compileAdoption']);assert adoption['status']=='SCRIPT_COMPILE_PASS_ADOPTED','Actual root compile/adoption is still pending'
 assert Path(adoption['project']).resolve()==Path(request['project']).resolve(),'Compile adoption project differs'
 assert adoption['closures']==closures,'Compile adoption source closure differs'
 run=Path(adoption['compileRun']);proof=read_json(run/'RESULT.json');identity=read_json(run/'owner-private.json')
 assert sha(run/'RESULT.json')==adoption['compilerResultSHA256'],'Compiler result pin differs'
 assert proof['status']=='SCRIPT_COMPILE_PASS' and proof['exitCode']==0 and proof['nativeAbsent'] is True and proof['leaseReleased'] is True,'Compiler not qualified/released'
 assert Path(identity['project']).resolve()==Path(request['project']).resolve(),'Compiler project differs'
 console=Path(proof['console']);assert sha(console)==adoption['compilerConsoleSHA256'],'Compiler console pin differs'
 assert console.stat().st_size<=64*1024**2,'Compiler console exceeds evidence bound'
 assert 'Script validation successful.' in console.read_bytes().decode('utf-8-sig'),'Compiler success absent'
 return {'compileRun':str(run),'resultSHA256':sha(run/'RESULT.json'),'consoleSHA256':sha(console)}
def command_for(request,p,native):
 assert request['template']=='startup' and request['caseId']=='hud-render-startup','Unsupported case/template'
 assert request['world']=='Worlds/GEP_ApprovedRender705.ent','Only existing HUD render world is bound'
 assert Path(request['project']).name=='GEP_ApprovedHUD705_QA.gproj','Wrong project'
 assert p['addonId']=='7E0050C026102500','Wrong addon identity'
 assert type(p['seconds']) is int and 30<=p['seconds']<=300
 assert type(p['screenWidth']) is int and 640<=p['screenWidth']<=7680
 assert type(p['screenHeight']) is int and 480<=p['screenHeight']<=4320
 assert type(p['childLogLimitBytes']) is int and 4096<=p['childLogLimitBytes']<=4*1024**2
 assert type(p['adapterDeadlineSeconds']) is int and 30<=p['adapterDeadlineSeconds']<=120
 return [p['powerShell'],'-NoLogo','-NoProfile','-NonInteractive','-File',p['wrapper'],'-Project',request['project'],'-World',request['world'],'-Manifest',next(n for n in p['completeManifestPins'] if Path(n).name=='QA_SOURCE_MANIFEST.json'),'-RunRoot',str(native),'-Seconds',str(p['seconds']),'-AddonId',p['addonId'],'-ScreenWidth',str(p['screenWidth']),'-ScreenHeight',str(p['screenHeight']),'-Execute']
def lifecycle(native):
 exit=read_json(native/'exit.json');owner=read_json(native/'owner.json');private=read_json(native/'owner-private.json');plan=read_json(native/'plan.json');lease=read_json(SLOT)
 assert exit['verifiedExited'] is True and exit['leaseReleased'] is True,'Exit/release incomplete'
 assert owner['leaseId']==private['leaseId']==lease['leaseId'],'Lease identity differs'
 assert owner['args']==plan['args'] and owner['project']==private['project']==plan['project']==lease['project'],'Owner/argv project receipt differs'
 assert Path(private['workRoot']).resolve()==native.resolve() and Path(lease['workRoot']).resolve()==native.resolve(),'Owner work root differs'
 assert lease['ownerKeyHash']==hashlib.sha256(private['ownerKey'].encode()).hexdigest(),'Owner capability differs'
 assert lease['pid'] is None and lease['activeRuntime'] is None and lease['status'].startswith('released'),'Lease not released'
 assert Path(owner['profile']).resolve()==(native/'profile').resolve(),'Owner profile differs'
 return exit,plan,owner
def emit(request,result_path,checks,infra,life,evidence):
 for name,reason in [('visual.approved','No human pixel/concept judgment in automated bridge'),('glyphs.reviewed','BMP dimensions/marker do not establish Unicode glyph appearance'),('input.physical','Existing probe uses scripted boarding/ADS; ordinary controls remain untested')]:checks[name]={'status':'NOT_TESTED','reason':reason}
 for name in request['assertions']:checks.setdefault(name,{'status':'NOT_TESTED','reason':'No qualified measurement'})
 out={'schema':1,'runId':request['runId'],'caseId':request['caseId'],'infrastructure':infra,'assertions':checks,'lifecycle':life,'evidence':[str(p) for p in evidence if Path(p).is_file()]}
 save(result_path,out);return out
def main(argv=None):
 request_path,result_path,cancel_path=map(Path,(argv or sys.argv[1:]));request=read_json(request_path);attempt=Path(request['attemptDir']).resolve();p=request['parameters']
 assert request['schema']==1 and str(uuid.UUID(request['runId']))==request['runId'],'Invalid request identity'
 assert request_path.resolve()==attempt/'request.json' and result_path.resolve()==attempt/'adapter-result.json' and cancel_path.resolve()==attempt/'CANCEL','Request protocol paths differ'
 assert not result_path.exists(),'Result already exists'
 native=attempt/'native-run';evidence=[];checks={};started=False;child=None;cancelled=False;overflow=threading.Event();cancel_at=None;published=False;started_at=time.time()
 try:
  assert not native.exists(),'Fresh native run root required'
  assert not cancel_path.exists(),'Cancelled before native admission'
  lease=read_json(SLOT)
  assert not hold_active(lease),'Explicit user runtime hold active; no wrapper/native admission'
  assert lease['status'].startswith('released'),'Native admission blocked by lease status: '+str(lease['status'])
  assert not lease.get('pid') and not lease.get('activeRuntime'),'Owned native reservation remains'
  assert sha(p['wrapper'])==p['wrapperSHA256'],'Wrapper pin differs'
  closures=check_sources(p);compiled=compile_gate(request,p,closures)
  command=command_for(request,p,native);receipt=attempt/'bridge-plan.json';save(receipt,{'runId':request['runId'],'caseId':request['caseId'],'command':command,'closures':closures,'compiled':compiled,'nativeNonce':'Existing native probe fixed case0; exact unique profile/process argv/source receipts bind this request.'});evidence.append(receipt)
  assert not cancel_path.exists(),'Cancelled immediately before spawn'
  env={**os.environ,'REFORGER_QA_CANCEL_PATH':str(cancel_path)}
  child=subprocess.Popen(command,stdout=subprocess.PIPE,stderr=subprocess.PIPE,creationflags=getattr(subprocess,'CREATE_NO_WINDOW',0),env=env,shell=False);started=True
  def drain(pipe,dest):
   total=0
   with dest.open('xb') as out:
    while True:
     chunk=pipe.read(4096)
     if not chunk:break
     remaining=max(0,p['childLogLimitBytes']-total);out.write(chunk[:remaining]);total+=len(chunk)
     if total>p['childLogLimitBytes']:overflow.set()
   pipe.close()
  streams=[attempt/'scene.stdout.log',attempt/'scene.stderr.log'];threads=[threading.Thread(target=drain,args=(pipe,dest),daemon=True) for pipe,dest in zip([child.stdout,child.stderr],streams)]
  for t in threads:t.start()
  evidence+=streams
  while child.poll() is None:
   live_lease=read_json(SLOT)
   if hold_active(live_lease) and not published:
    attached=read_json(native/'owner.json') if (native/'owner.json').is_file() else {}
    note=attempt/'USER-HOLD-OBSERVED.json';save(note,{'state':'USER_HOLD_NATIVE_PRESERVED','reason':'No GUI/native close request or following case; original wrapper must preserve owned process and lease','attachedPid':attached.get('pid'),'attachedCreationUtc':attached.get('creationUtc'),'leaseStatus':live_lease.get('status'),'freshProcessCensus':'NOT_PERFORMED','nativeRun':str(native)});evidence.append(note)
    emit(request,result_path,{}, {'status':'NOT_TESTED','reason':'Explicit user hold observed; native owner preserved, no follow-ons'}, {'status':'NOT_TESTED','reason':'Hold preserves session; no exit/release claimed','verifiedExited':False,'leaseReleased':False},evidence);published=True
   if published:time.sleep(.1);continue
   reason='child-log-limit' if overflow.is_set() else 'adapter-deadline' if time.time()-started_at>=p['adapterDeadlineSeconds'] else None
   if reason and not cancel_path.exists():
    try:save(cancel_path,{'runId':request['runId'],'reason':reason})
    except FileExistsError:pass
   if cancel_path.exists():
    cancelled=True
    if cancel_at is None:cancel_at=time.monotonic()
    if not published and time.monotonic()-cancel_at>25:
     note=attempt/'uncertain-owner.json';save(note,{'state':'UNCERTAIN','reason':'Wrapper still alive after cancellation grace; preserve child, native process and lease; no next case','nativeRun':str(native),'nativeControl':'NONE: wrapper alone owns normal close'});evidence.append(note)
     emit(request,result_path,{}, {'status':'NOT_TESTED','reason':'Cancellation closure uncertain; preserve wrapper/native owner'}, {'status':'NOT_TESTED','reason':'Owned wrapper has not confirmed exit/release','verifiedExited':False,'leaseReleased':False},evidence);published=True
   time.sleep(.1)
  for t in threads:t.join(5)
  assert not any(t.is_alive() for t in threads),'Child output streams did not close'
  cancelled=cancelled or cancel_path.exists() or overflow.is_set()
  evidence += [native/name for name in ['plan.json','owner.json','exit.json','FAILURE.json'] if (native/name).is_file()]
  if published:
   save(attempt/'late-wrapper-exit.json',{'exitCode':child.returncode,'censored':True,'reason':'Late completion cannot change previously censored result'});return 2
  exit,plan,owner=lifecycle(native)
  assert request['world'] in plan['args'] and p['addonId'] in plan['args'],'Plan world/addon absent'
  assert '-Execute' in command and '-KeepOpen' not in command and '-Interactive' not in command,'Unsafe non-bounded bridge command'
  closures_after=check_sources(p);assert closures_after==closures,'Sources changed during native run'
  life={'status':'PASS','reason':'Original wrapper exact owner/argv receipts, verified normal exit and same released lease','verifiedExited':True,'leaseReleased':True}
  if cancelled or exit.get('cancelRequested'):
   emit(request,result_path,{}, {'status':'NOT_TESTED','reason':'Cancelled/deadline/log-limit; behavior is censored regardless of late capture'},life,evidence);return 2
  assert child.returncode==0 and exit['exitCode']==0,'Wrapper/native exit code not zero'
  console=native/'logs/console.log';assert console.is_file() and console.stat().st_mtime>=started_at-2,'Missing/stale exact console'
  assert console.stat().st_size<=64*1024**2,'Console exceeds evidence bound'
  text=console.read_bytes().decode('utf-8-sig');assert str(native/'profile').casefold() in text.casefold(),'Console fresh profile binding absent'
  review=review_console(text,request['world']);image=screenshot(native,started_at,p['screenWidth'],p['screenHeight'])
  review.update({'runId':request['runId'],'caseId':request['caseId'],'consoleSHA256':sha(console),'image':image,'sources':closures_after})
  report=attempt/'native-review.json';save(report,review);evidence += [console,Path(image['path']),report]
  assert not hold_active(read_json(SLOT)),'User hold observed before result; no follow-on case'
  if cancel_path.exists():
   emit(request,result_path,{}, {'status':'NOT_TESTED','reason':'Cancellation arrived during evidence review; behavior remains censored'},life,evidence);return 2
  for name in ['fixture.ready','world.loaded','hud.scripted-ads','hud.capture-file','hud.actual-data']:checks[name]=assertion(True,'Fresh ordered native world/GAME/player/ADS/data/capture evidence; see native-review.json')
  errors=review['diagnostics']['beforeCapture']+review['diagnostics']['afterCapture'];checks['native.errors']={'status':'PASS' if not errors else 'FAIL','reason':'All native error lines retained; no unqualified baseline suppression','actual':review['diagnostics']}
  emit(request,result_path,checks,{'status':'PASS','reason':'Qualified source/compile, fresh fixed-case capture and exact owned closure; visual/physical acceptance remains separate'},life,evidence);return 1 if errors else 0
 except Exception as error:
  note=attempt/'bridge-failure.json';save(note,{'status':'NOT_TESTED','reason':type(error).__name__+': '+str(error),'nativeWrapperStarted':started,'nativeRun':str(native),'cancelRequested':cancel_path.exists(),'scope':'No kill, no lease repair, no user-session control performed'});evidence.append(note)
  exited=released=False
  if started:
   try:exit,_,_=lifecycle(native);exited=exit['verifiedExited'];released=exit['leaseReleased']
   except Exception:pass
  else:exited=released=True
  if not result_path.exists():emit(request,result_path,{}, {'status':'NOT_TESTED','reason':str(error)}, {'status':'PASS' if exited and released else 'NOT_TESTED','reason':'No native started by this attempt' if not started else 'Wrapper receipts determine closure; uncertainty preserved','verifiedExited':exited,'leaseReleased':released},evidence)
  return 2
if __name__=='__main__':sys.exit(main())

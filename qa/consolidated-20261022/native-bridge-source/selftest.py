"""Synthetic parser/source/receipt checks only. Never calls adapter.main/Popen."""
from pathlib import Path
import json,struct,time,tempfile,copy,hashlib
import adapter,evidence
ROOT=Path(__file__).resolve().parent;checks=[]
def check(name,fn):
 try:fn();checks.append({'name':name,'pass':True})
 except Exception as e:checks.append({'name':name,'pass':False,'error':str(e)})
def expect_reject(fn):
 try:fn()
 except (AssertionError,ValueError,KeyError,FileNotFoundError):return
 raise AssertionError('Expected fail-closed rejection')
WORLD='Worlds/GEP_ApprovedRender705.ent'
def sample():
 return '\n'.join([
 'ENGINE : Loaded addons:',
 "ENGINE : gproj: 'core.gproj' guid: '5614BBCCBB55ED1C'",
 "ENGINE : gproj: 'base.gproj' guid: '58D0FB3206B6F859'",
 "ENGINE : gproj: 'Gepard.gproj' guid: 'D561AD08932E7B40'",
 "ENGINE : gproj: 'QA.gproj' guid: '7E0050C026102500'",
 'ENGINE : config',
 "WORLD : Entities load '"+WORLD+"'",'SCRIPT : SCR_BaseGameMode::OnGameStateChanged = GAME',
 'SCRIPT : GEP_UPDATE101_SCENE_STAGED baseline=1 candidate=1 automaticBoarding=0 inputWrites=0 timeLimit=none',
 'SCRIPT : GEP_UPDATE101_SCENE_PLAYER ready=1 alive=1 canInteract=1 loading=0 menu=0 playerCamera=1 renderEnabled=1 physicalInput=NOT_TESTED',
 'SCRIPT : GEP_APPROVED_RENDER705 boarding=1 case=0 style=705 physicalInput=0',
 'SCRIPT : GEP_APPROVED_RENDER705 native_ready=1 case=0 style=705 seated=1 ADS=1 visible=1 physicalInput=0',
 'SCRIPT : GEP_APPROVED_RENDER705 data group=0 left=320/320 right=319/320 rangeState=0 rangeMetres=-1 rangeAge=-1 magnification=2',
 'SCRIPT : GEP_APPROVED_RENDER705 widgets left=320 right=319 range=-- m zoom=2,0×',
 'SCRIPT : GEP_APPROVED_RENDER705 capture=1 case=0 style=705 range=-- m zoom=2,0× physicalInput=0',
 'SCRIPT : GEP_APPROVED_RENDER705 terminal=CAPTURED cases=1 renderedInspection=PENDING physicalInput=0 accuracy=NOT_TESTED'])
check('ordered-current-source-markers',lambda:evidence.review_console(sample(),WORLD))
for name,old,new in [('wrong-world',WORLD,'Worlds/Other.ent'),('duplicate-gepard','58D0FB3206B6F859','D561AD08932E7B40'),('wrong-style','style=705','style=1'),('wrong-case','case=0','case=1'),('not-seated','seated=1','seated=0'),('hidden-hud','visible=1','visible=0'),('loading-player','loading=0','loading=1'),('stale-widget-count','widgets left=320','widgets left=319'),('non-German-zoom','zoom=2,0×','zoom=2.0x'),('invalid-group','group=0','group=7'),('unknown-zoom','magnification=2','magnification=0')]:
 check('reject:'+name,lambda old=old,new=new:expect_reject(lambda:evidence.review_console(sample().replace(old,new),WORLD)))
check('reject-duplicate-terminal',lambda:expect_reject(lambda:evidence.review_console(sample()+'\n'+sample().splitlines()[-1],WORLD)))
check('reject-editor-init-without-GAME',lambda:expect_reject(lambda:evidence.review_console(sample().replace('= GAME','= EDIT'),WORLD)))
check('reject-marker-reordering',lambda:expect_reject(lambda:evidence.review_console(sample().replace('SCRIPT : SCR_BaseGameMode::OnGameStateChanged = GAME\n','')+'\nSCRIPT : SCR_BaseGameMode::OnGameStateChanged = GAME',WORLD)))
for name,state,metres,age,label in [('valid-range',1,1234.4,0.2,'1234 m'),('no-return',2,-1,-1,'KEIN MESSWERT'),('unavailable',3,-1,-1,'MESSUNG NICHT VERFÜGBAR')]:
 t=sample().replace('rangeState=0 rangeMetres=-1 rangeAge=-1',f'rangeState={state} rangeMetres={metres} rangeAge={age}').replace('range=-- m',f'range={label}')
 check('accept-data:'+name,lambda t=t:evidence.review_console(t,WORLD))
check('native-errors-retained-not-suppressed',lambda:(_ for _ in ()).throw(AssertionError()) if not evidence.review_console(sample()+'\nSCRIPT (E): test retained',WORLD)['diagnostics']['afterCapture'] else None)
tmp=Path(tempfile.mkdtemp(prefix='gep-bridge-offline-',dir=str(ROOT)))
image_dir=tmp/'native/profile/profile';image_dir.mkdir(parents=True);bmp=image_dir/'approved705-render-case-0.bmp';w,h=8,4;payload=bytearray(54+w*h*4);payload[:2]=b'BM';struct.pack_into('<I',payload,10,54);struct.pack_into('<ii',payload,18,w,h);struct.pack_into('<H',payload,28,32);bmp.write_bytes(payload)
check('fresh-native-BMP-header-payload-contract',lambda:evidence.screenshot(tmp/'native',time.time(),w,h))
check('reject-stale-image',lambda:expect_reject(lambda:evidence.screenshot(tmp/'native',time.time()+10,w,h)))
check('reject-wrong-dimensions',lambda:expect_reject(lambda:evidence.screenshot(tmp/'native',time.time(),w+1,h)))
check('reject-truncated-pixel-body',lambda:expect_reject(lambda:(bmp.write_bytes(payload[:54]),evidence.screenshot(tmp/'native',time.time(),w,h))))
bmp.write_bytes(payload);other=tmp/'native/profile/approved705-render-case-0.bmp';other.write_bytes(payload)
check('reject-ambiguous-profile-image',lambda:expect_reject(lambda:evidence.screenshot(tmp/'native',time.time(),w,h)))
src=tmp/'source';src.mkdir();(src/'one.c').write_bytes(b'original');manifest=tmp/'manifest.json';manifest.write_text(json.dumps({'one.c':evidence.sha(src/'one.c')}),encoding='utf-8');closures=tmp/'closures.json';closures.write_text(json.dumps([{'root':str(src),'manifest':str(manifest)}]),encoding='utf-8');p={'closures':str(closures),'closurePinSHA256':evidence.sha(closures),'completeManifestPins':{str(manifest):evidence.sha(manifest)}}
check('complete-source-closure-contract',lambda:adapter.check_sources(p))
(src/'extra.c').write_bytes(b'new')
check('reject-added-unpinned-source',lambda:expect_reject(lambda:adapter.check_sources(p)))
pending=ROOT/'COMPILE-ADOPTION-PENDING.json'
check('pending-compile-refuses-admission',lambda:expect_reject(lambda:adapter.compile_gate({'project':'unused'},{'compileAdoption':str(pending)},[])))
for name,lease,want in [('status-hold',{'status':'user_runtime_hold_no_native_admission'},True),('nested-hold',{'status':'running','userRuntimeHold':{'active':True}},True),('malformed-hold',{'status':'released','userRuntimeHold':{'active':'false'}},True),('inactive-hold',{'status':'released','userRuntimeHold':{'active':False}},False),('absent-hold',{'status':'released'},False)]:
 check('hold:'+name,lambda lease=lease,want=want:None if adapter.hold_active(lease)==want else (_ for _ in ()).throw(AssertionError()))
life_root=tmp/'life';life_root.mkdir();fake_slot=tmp/'fake-slot.json';old_slot=adapter.SLOT;adapter.SLOT=fake_slot
private={'ownerKey':'synthetic-offline-key','leaseId':'synthetic-lease','project':'synthetic-project','workRoot':str(life_root)}
owner={'leaseId':'synthetic-lease','project':'synthetic-project','args':['synthetic-argv'],'profile':str(life_root/'profile')}
lease={'leaseId':'synthetic-lease','project':'synthetic-project','workRoot':str(life_root),'ownerKeyHash':hashlib.sha256(private['ownerKey'].encode()).hexdigest(),'pid':None,'activeRuntime':None,'status':'released_synthetic'}
for name,row in [('owner-private.json',private),('owner.json',owner),('plan.json',{'project':'synthetic-project','args':['synthetic-argv']}),('exit.json',{'verifiedExited':True,'leaseReleased':True,'exitCode':0})]:(life_root/name).write_text(json.dumps(row),encoding='utf-8')
fake_slot.write_text(json.dumps(lease),encoding='utf-8')
check('same-identity-normal-exit-release-contract',lambda:adapter.lifecycle(life_root))
for name,key,value in [('wrong-lease','leaseId','different'),('still-owned','pid',1234),('not-released','status','running')]:
 bad=copy.deepcopy(lease);bad[key]=value;fake_slot.write_text(json.dumps(bad),encoding='utf-8');check('lifecycle-reject:'+name,lambda:expect_reject(lambda:adapter.lifecycle(life_root)))
adapter.SLOT=old_slot
job=json.loads((ROOT/'hud-startup-job.json').read_bytes());request={'template':'startup','caseId':'hud-render-startup','project':job['project'],'world':job['world']};params=job['cases'][0]['parameters'];cmd=adapter.command_for(request,params,tmp/'fresh-native')
check('argument-array-preserves-wrapper-token',lambda:None if cmd[cmd.index('-File')+1]==params['wrapper'] else (_ for _ in ()).throw(AssertionError()))
check('bounded-hidden-only-command',lambda:None if cmd.count('-Execute')==1 and '-KeepOpen' not in cmd and '-Interactive' not in cmd else (_ for _ in ()).throw(AssertionError()))
bad=copy.deepcopy(request);bad['world']='Worlds/ProvingGround.ent'
check('future-world-unbound-rejected',lambda:expect_reject(lambda:adapter.command_for(bad,params,tmp/'fresh-native')))
cancel_req={'schema':1,'runId':'synthetic-offline','caseId':'hud-render-startup','assertions':['fixture.ready','world.loaded']};note=tmp/'synthetic-evidence.json';note.write_text('{}',encoding='utf-8');out=tmp/'synthetic-result.json'
check('censored-result-has-no-behavior-PASS',lambda:adapter.emit(cancel_req,out,{}, {'status':'NOT_TESTED','reason':'synthetic cancellation'}, {'status':'PASS','reason':'synthetic no native started','verifiedExited':True,'leaseReleased':True},[note]))
check('all-censored-and-human-checks-NOT_TESTED',lambda:None if all(r['status']=='NOT_TESTED' for r in json.loads(out.read_text(encoding='utf-8'))['assertions'].values()) else (_ for _ in ()).throw(AssertionError()))
wrapper=(ROOT/'tools/Scene.ps1').read_text(encoding='utf-8');upstream=Path(json.loads((ROOT/'WRAPPER-PATCH.json').read_bytes())['upstream']).read_text(encoding='utf-8-sig')
for name,condition in [('same-one-normal-close-path',wrapper.count('CloseMainWindow()')==upstream.count('CloseMainWindow()')==1),('same-identity-guards','Parsed native command differs' in wrapper and 'Managed birth differs' in wrapper),('no-force-kill',not any(t in wrapper for t in ['Stop-Process','taskkill','Kill('])),('cancel-before-native-and-in-owned-loops',wrapper.index("status='NOT_STARTED_CANCELLED'")<wrapper.index('Start-Process') and wrapper.count('if(CancelRequested705)')>=3),('uncertain-close-retains-lease',"throw 'Normal close incomplete; preserve owned session and lease'" in wrapper),('dry-run-still-exits-before-execute-branch',wrapper.index('if(!$Execute)')<wrapper.index("status='NOT_STARTED_CANCELLED'"))]:
 check('wrapper:'+name,lambda condition=condition:None if condition else (_ for _ in ()).throw(AssertionError()))
check('wrapper:user-hold-guards-before-GUI-close',lambda:None if "if(UserRuntimeHold705){throw 'User runtime hold active before close; preserve owned process and lease'}" in wrapper and wrapper.index("if(UserRuntimeHold705){throw 'User runtime hold active before close")<wrapper.index('CloseMainWindow()') and "if(UserRuntimeHold705){throw 'User runtime hold active in owned loop" in wrapper else (_ for _ in ()).throw(AssertionError()))
for name,token in [('atomic-claim-hold-check',"if(UserRuntimeHoldRecord705 $l){throw 'User runtime hold active at atomic claim; no native admission'}"),('inactive-shared-hold-field-preserved',"$next705['userRuntimeHold']=$l.userRuntimeHold"),('hold-never-cleared-by-release',"if(UserRuntimeHoldRecord705 $l){throw 'User runtime hold active at release; preserve lease for coordinator'}")]:
 check('wrapper:'+name,lambda token=token:None if token in wrapper else (_ for _ in ()).throw(AssertionError()))
result={'status':'PASS_OFFLINE_ONLY' if all(c['pass'] for c in checks) else 'FAIL','scope':'Synthetic parser/guard/source contracts only; adapter.main, Popen and native wrapper were never invoked','checkCount':len(checks),'checks':checks,'nativeCompile':'NOT_TESTED','nativeRuntime':'NOT_TESTED','holdRespected':True}
(ROOT/'SELFTEST-RESULT.json').write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8');print(json.dumps({'status':result['status'],'checks':len(checks),'failed':[c['name'] for c in checks if not c['pass']]}));raise SystemExit(0 if result['status'].startswith('PASS') else 1)

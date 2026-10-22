"""Synthetic/pure/source checks only. Never call adapter.main/Popen/Scene.ps1."""
from pathlib import Path
import time,tempfile,os,math,ast,json,re
from evidence import read_json,save,review_console,review_file,untested_acceptance,vector
import adapter
ROOT=Path(__file__).resolve().parent;contract=read_json(ROOT/'MARKER-CONTRACT.json')
job=read_json(ROOT/'gpg-startup-job.json');parameters=job['cases'][0]['parameters'];results=[]
def check(name,fn):
 fn();results.append({'name':name,'status':'PASS'})
def reject(name,fn):
 try:fn()
 except (AssertionError,ValueError,KeyError,IndexError):results.append({'name':name,'status':'PASS'});return
 raise AssertionError('Expected fail-closed: '+name)
def require(v):assert v
def v(p):return '<'+', '.join(f'{x:.6f}' for x in p)+'>'
def fixture():
 rows=['ENGINE : Loaded addons:']
 rows += [f"ENGINE : gproj: 'synthetic-{n}.gproj' guid: '{n}'" for n in contract['loadedAddonIds']]
 rows += ["WORLD : Entities load '{A76A420022000002}Worlds/GPG_Arland.ent'",'SCRIPT : SCR_BaseGameMode::OnGameStateChanged = GAME']
 for id,item in contract['objects'].items():rows.append(f"SCRIPT : GPG_OBJECT id={id} station={item['station']} epoch=0 prefab={item['prefab']} origin={v(item['pad'])}")
 for id,pad in contract['stations'].items():rows.append(f'SCRIPT : GPG_STATION id={id} board={v(pad)} title=synthetic title')
 for id,pad in contract['gates'].items():rows.append(f'SCRIPT : GPG_GATE id={id} origin={v(pad)} gradeDegrees=2.5 surface=NOT_TESTED')
 for id in contract['stocks']:rows.append(f'SCRIPT : GPG_STOCK source={id} supplies=1000 initializationOnly=1')
 for t in contract['targets']:
  line=[900,2,2450];aim=[t['pad'][0],1.2,t['pad'][2]]
  rows.append(f"SCRIPT : GPG_SURVEY target={t['id']} nominalHorizontal={t['nominal']} slant={math.dist(line,aim):.6f} terrainTraceFraction=1 line={v(line)} aim={v(aim)} nativeVisibilityReview=NOT_TESTED")
 rows += [f"SCRIPT : GPG_STAGED registry={contract['registrySHA256']} source={contract['runtimeSHA256']} nativeCompiled=NOT_TESTED",'SCRIPT : GPG_READY staged=1 resourceFailure=0 objects=32 nativeBehaviorAcceptance=NOT_TESTED']
 return '\n'.join(rows)+'\n'
GOOD=fixture()
def review(text):return review_console(text,job['world'],contract)
check('valid synthetic source-bound startup',lambda:require(review(GOOD)['objectCount']==32))
check('native acceptance stays NOT_TESTED',lambda:require(all(r['status']=='NOT_TESTED' for r in review(GOOD)['acceptance'].values())))
check('false/true boolean formatting supported',lambda:review(GOOD.replace('staged=1 resourceFailure=0','staged=true resourceFailure=false')))
for name,bad in [
 ('missing READY','\n'.join(x for x in GOOD.splitlines() if 'GPG_READY' not in x)),
 ('duplicate READY',GOOD+GOOD.splitlines()[-1]+'\n'),
 ('duplicate STAGED',GOOD+'\n'+next(x for x in GOOD.splitlines() if 'GPG_STAGED' in x)),
 ('wrong source hash',GOOD.replace('source='+contract['runtimeSHA256'],'source='+'0'*64)),
 ('wrong registry hash',GOOD.replace('registry='+contract['registrySHA256'],'registry='+'0'*64)),
 ('resource failure',GOOD.replace('resourceFailure=0','resourceFailure=1')),
 ('staging false',GOOD.replace('staged=1','staged=0')),
 ('wrong object count',GOOD.replace('objects=32','objects=31')),
 ('unrequested spawn failure',GOOD+'SCRIPT : GPG_SPAWN_FAIL prefab=bad\n'),
 ('unrequested resource failure',GOOD+'SCRIPT : GPG_RESOURCE_FAIL prefab=bad\n'),
 ('unrequested command',GOOD+'SCRIPT : GPG_COMMAND seq=x operation=read station=bay\n'),
 ('unrequested reset',GOOD+'SCRIPT : GPG_RESET id=D1 epoch=1 prefab=x fullReplacement=1\n'),
 ('unrequested respawn',GOOD+'SCRIPT : GPG_RESPAWN station=bay replacementAfterDeath=1\n'),
 ('hostile startup forbidden',GOOD+'SCRIPT : GPG_HOSTILE start=1\n'),
 ('NEW art not silently bound',GOOD.replace('GPG_OBJECT id=OLD-CAMO','GPG_OBJECT id=NEW-CAMO')),
 ('object missing','\n'.join(x for x in GOOD.splitlines() if 'GPG_OBJECT id=D1 ' not in x)),
 ('object duplicated',GOOD+next(x for x in GOOD.splitlines() if 'GPG_OBJECT id=D1 ' in x)+'\n'),
 ('object epoch differs',GOOD.replace('epoch=0','epoch=1',1)),
 ('wrong prefab',GOOD.replace('station=bay epoch=0 prefab={D561AD08932E5480}','station=bay epoch=0 prefab={0000000000000000}',1)),
 ('wrong native x/z',GOOD.replace('origin=<600.000000, 0.000000, 2600.000000>','origin=<650.000000, 0.000000, 2600.000000>',1)),
 ('survey absent','\n'.join(x for x in GOOD.splitlines() if 'GPG_SURVEY target=T500 ' not in x)),
 ('survey duplicated',GOOD+next(x for x in GOOD.splitlines() if 'GPG_SURVEY target=T500 ' in x)+'\n'),
 ('inconsistent slant',re.sub(r'(target=T100 nominalHorizontal=100 slant=)\S+',r'\g<1>50',GOOD)),
 ('fraction exceeds one',GOOD.replace('terrainTraceFraction=1','terrainTraceFraction=1.1',1)),
 ('nonfinite fraction',GOOD.replace('terrainTraceFraction=1','terrainTraceFraction=NaN',1)),
 ('native review marker must remain NT',GOOD.replace('nativeVisibilityReview=NOT_TESTED','nativeVisibilityReview=PASS',1)),
 ('compiled marker is not compile proof',GOOD.replace('nativeCompiled=NOT_TESTED','nativeCompiled=PASS')),
 ('wrong addons',GOOD.replace('D561AD08932E7B40','0000000000000000')),
 ('out of order',GOOD.replace('SCRIPT : SCR_BaseGameMode::OnGameStateChanged = GAME\n','')+'SCRIPT : SCR_BaseGameMode::OnGameStateChanged = GAME\n'),
 ('wrong stock count',GOOD.replace('supplies=1000','supplies=999',1)),
 ('missing station','\n'.join(x for x in GOOD.splitlines() if 'GPG_STATION id=bay ' not in x)),
 ('bad grade',GOOD.replace('gradeDegrees=2.5','gradeDegrees=91',1)),
 ]:reject(name,lambda bad=bad:review(bad))
check('blocked terrain ray is telemetry not usable LOS',lambda:require(review(GOOD.replace('terrainTraceFraction=1','terrainTraceFraction=0'))['acceptance']['range.visibility']['status']=='NOT_TESTED'))
check('signed downhill terrain grade from Atan2 source',lambda:require(min(review(GOOD.replace('gradeDegrees=2.5','gradeDegrees=-2.5'))['gateGrades'].values())==-2.5))
check('native errors retained before and after READY',lambda:require(len(review('SCRIPT (E): early error\n'+GOOD+'RESOURCES (E): late error\n')['diagnostics']['errors'])==2))
for s in ['<1, 2, 3>','(1 2 3)','"1 2 3"','1 2 3']:check('finite vector '+s,lambda s=s:require(vector(s)==[1.,2.,3.]))
reject('nonfinite vector',lambda:vector('<1, NaN, 3>'))
with tempfile.TemporaryDirectory(prefix='gpg-source-test-',dir=ROOT) as temp:
 p=Path(temp)/'console.log';profile='SYNTHETIC-FRESH-PROFILE';p.write_text(profile+'\n'+GOOD,encoding='utf-8')
 check('fresh exact console binding',lambda:review_file(p,time.time(),profile,job['world'],contract))
 reject('wrong profile',lambda:review_file(p,time.time(),'OTHER-PROFILE',job['world'],contract))
 os.utime(p,(1,1));reject('stale console',lambda:review_file(p,time.time(),profile,job['world'],contract))
 p.write_text('{"x":1,"x":2}');reject('duplicate JSON keys',lambda:read_json(p))
 p.write_text('{"x":1e999}');reject('infinite JSON float',lambda:read_json(p))
 result=Path(temp)/'adapter-result.json'
 adapter.emit({'runId':'synthetic','caseId':'gpg-startup-survey','assertions':['fixture.ready']},result,{}, {'status':'NOT_TESTED','reason':'Synthetic cancellation censor'}, {'status':'NOT_TESTED','reason':'Not started in synthetic test','verifiedExited':False,'leaseReleased':False},[p])
 check('censored result cannot invent PASS',lambda:require(read_json(result)['assertions']['fixture.ready']['status']=='NOT_TESTED'))
for lease in [{'status':'user_runtime_hold_no_native_admission'},{'status':'released','userRuntimeHold':{'active':True}},{'status':'released','userRuntimeHold':{}},{'status':'released','userRuntimeHold':None}]:check('hold fail closed '+str(lease),lambda lease=lease:require(adapter.hold_active(lease)))
check('inactive hold record',lambda:require(not adapter.hold_active({'status':'released','userRuntimeHold':{'active':False}})))
closures=adapter.check_sources(parameters);check('complete source closure1157',lambda:require(sum(r['files'] for r in closures)==1157))
reject('pending compile adoption blocks',lambda:adapter.compile_gate({'project':job['project']},parameters,closures))
request={'template':'startup','caseId':'gpg-startup-survey','world':job['world'],'project':job['project']}
command=adapter.command_for(request,parameters,ROOT/'future-test-never-executed')
check('future command is bounded exact existing wrapper',lambda:require('-Execute' in command and '-KeepOpen' not in command and '-Interactive' not in command and command[command.index('-AddonId')+1]=='A76A420022000001'))
wrapper=(ROOT/'tools/Scene.ps1').read_text();up=(ROOT.parent/'GEP-Consolidated-QA-20261022/tools/Scene.ps1').read_text()
check('one original guarded normal close',lambda:require(wrapper.count('.CloseMainWindow()')==up.count('.CloseMainWindow()')==1))
for token in ["if(UserRuntimeHold705){throw 'User runtime hold active; no native admission'}","if(UserRuntimeHoldRecord705 $l){throw 'User runtime hold active at atomic claim; no native admission'}","if(UserRuntimeHold705){throw 'User runtime hold active in owned loop; preserve process and lease'}","if(UserRuntimeHold705){throw 'User runtime hold active before close; preserve owned process and lease'}","if(UserRuntimeHold705){throw 'User runtime hold active during close wait; preserve uncertainty'}","if(UserRuntimeHoldRecord705 $l){throw 'User runtime hold active at release; preserve lease for coordinator'}"]:check('preserved guard '+token.split("throw '")[1][:45],lambda token=token:require(token in wrapper and token in up))
check('existing identity/lease unchanged',lambda:require(wrapper[wrapper.index('function SlotTransaction'):wrapper.index('function UserRuntimeHoldRecord705')]==up[up.index('function SlotTransaction'):up.index('function UserRuntimeHoldRecord705')]))
check('prelaunch hold check',lambda:require("if(UserRuntimeHold705){throw 'User runtime hold active immediately before native launch; preserve claim'}\n $game=Start-Process" in wrapper))
check('attachment preserves status-only hold',lambda:require("if(UserRuntimeHoldRecord705 $l){throw 'User runtime hold active at attachment; preserve native and lease'};$l.pid=$game.Id" in wrapper))
stage=Path(job['addonRoots'][0]);projects=list(stage.rglob('*.gproj'))
check('narrow stage exactly two addon projects',lambda:require({p.resolve() for p in projects}=={Path(job['project']).resolve(),Path(job['sourceRoots'][1])/'Gepard.gproj'}))
check('no broad consolidated root discovery',lambda:require(str(stage.parent) not in job['addonRoots']))
check('no force termination added',lambda:require('Stop-Process' not in wrapper and 'taskkill' not in wrapper.lower() and '.Kill(' not in wrapper))
check('explicit addon roots pinned',lambda:require(parameters['addonSearchPath']==','.join(job['addonRoots']) and all(r in wrapper for r in job['addonRoots'])))
art=read_json(ROOT/'ART-V11-SOURCE-RECEIPT.json')
check('prior1112 core unchanged',lambda:require(art['originalCoreUnchanged']==1112))
check('exact35 additive overlay files',lambda:require(art['addedOverlayFiles']==35))
check('five baseline seeds not V11 native models',lambda:require(art['seedModels']==5 and art['seedState']=='BASELINE_REGISTRATION_SEEDS_NOT_NATIVE_V11_GEOMETRY'))
check('NEW pads and native import remain pending',lambda:require(art['newPads']=='UNBOUND' and art['nativeImport']=='NOT_TESTED' and contract['unboundArt']==['NEW-CAMO','NEW-CHARCOAL']))
for f in ['adapter.py','evidence.py','prepare.py','selftest.py']:check('Python syntax '+f,lambda f=f:ast.parse((ROOT/f).read_text()))
save(ROOT/'SELFTEST-RESULT.json',{'status':'PASS_OFFLINE_SYNTHETIC_AND_SOURCE_ONLY','tests':len(results),'results':results,'adapterMainInvoked':False,'popenInvoked':False,'wrapperInvoked':False,'nativeInvoked':False,'hold':'UNCHANGED','nativeCompile':'NOT_TESTED','nativeGameplay':'NOT_TESTED'})
print(json.dumps({'tests':len(results),'status':'PASS_OFFLINE_ONLY','nativeInvoked':False}))

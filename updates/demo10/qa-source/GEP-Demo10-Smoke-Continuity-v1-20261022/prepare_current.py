"""Root-only pure post-compile adoption; two one-case jobs, never native."""
from pathlib import Path
import argparse,json,shlex
from evidence import read_json,save,sha
from adoption import validate_compile
from verify_source import verify
ROOT=Path(__file__).resolve().parent
BASE=Path('D:\\ArmaProjects\\Gepard-Demo10-TrackArt-20261022');STAGE=BASE/'native-demo10-smoke-stage-v1';CURRENT=STAGE;QA=CURRENT/'qaSmoke'
CASES={'camo':('smoke-demo10-camo','Worlds/GEP215_CurrentV11Camo.ent','{2150FC6F1E081188}Prefabs/ArtApproved20261022/Camo/GEP_MobileCarrier.et'),'charcoal':('smoke-demo10-charcoal','Worlds/GEP215_CurrentV11Charcoal.ent','{8095474EC9806D0D}Prefabs/ArtApproved20261022/Charcoal/GEP_MobileCarrier.et')}
MANIFEST_NAMES={'gepard':'GEPARD_SOURCE_MANIFEST.json','wcsSmoke':'WCSSMOKE_SOURCE_MANIFEST.json','qaSmoke':'QA_SOURCE_MANIFEST.json','WCS_Armaments_629B2BA37EFFD577':'WCS_ARMAMENTS_SOURCE_MANIFEST.json','integration':'INTEGRATION_SOURCE_MANIFEST.json','provingGround':'WORLD_SOURCE_MANIFEST.json','SpaceCore_5E389BB9F58B79A6':'SPACECORE_SOURCE_MANIFEST.json'}
def validate_native_closure(source,before):
 files={p.relative_to(source).as_posix():p for p in source.rglob('*') if p.is_file()}
 assert set(files)==set(before),'Unexpected compiled file set'
 now={n:sha(p) for n,p in files.items()};changes=[n for n in now if now[n]!=before[n]]
 assert not changes or (source.name=='qaSmoke' and changes==['resourceDatabase.rdb']),'Only privateQA RDB regeneration permitted; all current/optional/WCS exact'
 assert now['resourceDatabase.rdb'] if 'resourceDatabase.rdb' in now else True
 if source.name=='qaSmoke':assert files['resourceDatabase.rdb'].stat().st_size>0
 return now

def main():
 ap=argparse.ArgumentParser();ap.add_argument('--compile-run',type=Path,required=True);a=ap.parse_args()
 assert a.compile_run.resolve()==(BASE/'native-runs/compile-demo10-smoke-01').resolve(),'Exact fresh compile01 required'
 verify();proofPath=ROOT/'state/STAGING-RESULT.json';proof=read_json(proofPath)
 assert proof['status']=='SOURCE_STAGED_NATIVE_COMPILE_BINDING_AND_GAMEPLAY_PENDING' and proof['sourcePreserved'] and proof['files']==10 and proof['modifiedExistingFiles']==0 and proof['newFiles']==0
 assert Path(proof['target']).resolve()==STAGE.resolve() and proof['nativeStarted'] is False and proof['leaseMutated'] is False and proof['package08Modified'] is False
 out=ROOT/'adopted';assert not out.exists(),'Preserve prior/partial adoption; use successor'
 project=QA/'GEP_SmokeCurrentV11_QA.gproj';adopt=validate_compile(a.compile_run,project)
 identity=read_json(a.compile_run/'compiler-identity.json');args=[s[1:-1] if s.startswith('"') and s.endswith('"') else s for s in shlex.split(identity['CommandLine'],posix=False)]
 roots=[str(STAGE),'D:\\ArmaProjects\\Gepard-Demo10-TrackArt-20261022\\private-demo-source10\\single','D:\\ArmaProjects\\Gepard-Demo10-TrackArt-20261022\\private-demo-source10\\dependencies',r'D:\SteamLibrary\steamapps\common\Arma Reforger\addons']
 assert args.count('-addonsDir')==1 and args[args.index('-addonsDir')+1]==','.join(roots),'Discovery differs'
 closures=[];generated=[];manifests={}
 for row in read_json(ROOT/'EXPECTED-STAGE-CLOSURES.json'):
  source=Path(row['root']);before=read_json(row['manifest']);now=validate_native_closure(source,before)
  name=MANIFEST_NAMES[source.name];manifests[name]=now;closures.append({'root':str(source),'manifest':str(out/name)})
  generated += [{'root':str(source),'relative':n,'before':None,'after':now[n]} for n in set(now)-set(before)]
 assert sorted(map(len,manifests.values()))==[3,3,10,11,14,68,1152] and sum(map(len,manifests.values()))==1261
 assert generated==[], 'V3 adds no files; existing privateQA RDB mutation separately recorded'
 privateQAchanges=[]
 for row in read_json(ROOT/'EXPECTED-STAGE-CLOSURES.json'):
  if Path(row['root']).name=='qaSmoke':
   before=read_json(row['manifest']);after=manifests['QA_SOURCE_MANIFEST.json']
   privateQAchanges=[{'relative':n,'before':before[n],'after':after[n]} for n in before if before[n]!=after[n]]
 out.mkdir()
 for name,pins in manifests.items():save(out/name,pins)
 save(out/'SOURCE-CLOSURES.json',closures)
 adopt['closures']=[{**r,'sha256':sha(r['manifest']),'files':len(read_json(r['manifest']))} for r in closures]
 adopt['stagingReceipt']={'path':str(proofPath),'sha256':sha(proofPath)}
 adopt['privateQAGeneratedChanges']=privateQAchanges
 save(out/'COMPILE-ADOPTION.json',adopt);save(out/'GENERATED-RDB-CREATIONS.json',generated)
 seal=read_json(ROOT/'SOURCE-MANIFEST.json');required=[ROOT/r['relative'] for r in seal['files']]+[ROOT/'SOURCE-MANIFEST.json',out/'SOURCE-CLOSURES.json',out/'COMPILE-ADOPTION.json',proofPath]+[Path(r['manifest']) for r in closures]
 required += [a.compile_run/n for n in ['RESULT.json','compiler-identity.json','launcher-exit.json','owner-private.json']]+[Path(read_json(a.compile_run/'RESULT.json')['console'])]
 powershell=Path(r'C:\Users\marco\.cache\codex-runtimes\codex-primary-runtime\dependencies\native\powershell\pwsh.exe');game=Path(r'D:\SteamLibrary\steamapps\common\Arma Reforger\ArmaReforgerSteamDiag.exe');required += [powershell,game]
 assert len(required)==len(set(required)) and all(p.is_file() for p in required)
 params={'wrapper':str(ROOT/'tools/Scene.ps1'),'wrapperSHA256':sha(ROOT/'tools/Scene.ps1'),'powerShell':str(powershell),'closures':str(out/'SOURCE-CLOSURES.json'),'closurePinSHA256':sha(out/'SOURCE-CLOSURES.json'),'completeManifestPins':{r['manifest']:sha(r['manifest']) for r in closures},'compileAdoption':str(out/'COMPILE-ADOPTION.json'),'addonId':'7E0150C026106500','addonSearchPath':','.join(roots),'seconds':75,'screenWidth':1920,'screenHeight':1080,'adapterDeadlineSeconds':115,'childLogLimitBytes':4194304}
 for skin,(case,world,resource) in CASES.items():
  job={'schema':1,'template':'startup','project':str(project),'world':world,'sourceRoots':[str(QA),'D:\\ArmaProjects\\Gepard-Demo10-TrackArt-20261022\\private-demo-source10\\single\\gepard','D:\\ArmaProjects\\Gepard-Demo10-TrackArt-20261022\\private-demo-source10\\single\\wcsSmoke','D:\\ArmaProjects\\Gepard-Demo10-TrackArt-20261022\\private-demo-source10\\single\\integration'],'addonRoots':roots,'requiredFiles':[{'path':str(p.resolve()),'sha256':sha(p)} for p in required],'requiredClasses':['GEP215_CurrentV11Probe','GEP215_CurrentV11SpawnLogic','GEP_Presentation680','GEP_TurretController410'],'capabilities':['isolated-reset','owned-lifecycle','native-load'],'adapter':{'executable':r'C:\Users\marco\AppData\Local\Programs\Python\Python311\python.exe','script':str(ROOT/'adapter.py'),'args':['{script}','{request}','{result}','{cancel}']},'cases':[{'id':case,'parameters':{**params,'expectedVehiclePrefab':resource},'assertions':['fixture.ready','world.loaded','smoke.current-v11-identity','smoke.current-v11-banks','smoke.current-hud-observations','smoke.program-payload','smoke.flight-telemetry','smoke.empty-negative','native.errors']}],'timeoutSeconds':145,'closeGraceSeconds':35,'notes':'Exact final Demo10 core1152/optional14/integration68, mounted deps, original QA10 skin hullYaw0 only. Scripted FullRipple8->0/eight actual flights/one empty; passive HUD and bank ENTITY poses. Capture0: no cloud/coverage/tube matrix/nonzero hull or turret/physical input/driving/cannon/rearm/MP inference. All-domain E/F retained. Each one-case job fresh native/profile. Root separately admits execution.'}
  save(out/(case+'-job.json'),job)
 save(out/'PREPARATION-RECEIPT.json',{'status':'ACTUAL_CURRENT_V11_COMPILE_ADOPTED_RUNTIME_NOT_TESTED','sourceManifestSHA256':sha(ROOT/'SOURCE-MANIFEST.json'),'completeBoundFiles':1261,'copiedQAFiles':10,'core1152AllBytesUnchanged':True,'generatedRDBCreations':generated,'nativeStarted':False,'manualServiceBound':False})
 print(json.dumps({'status':'TWO_ONE_CASE_JOBS_PREPARED_ROOT_PLAN_AND_ADMISSION_REQUIRED','nativeInvoked':False,'adopted':str(out)}))
if __name__=='__main__':main()

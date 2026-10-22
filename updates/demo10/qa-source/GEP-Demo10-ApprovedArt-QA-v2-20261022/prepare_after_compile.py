"""C-only strict actual QA compile adoption and job preparation; no native call."""
from pathlib import Path
import hashlib,importlib.util,json,sys
sys.dont_write_bytecode=True
O=Path(__file__).resolve().parent
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def read(p):return json.loads(Path(p).read_text(encoding='utf-8-sig'))
def save(p,j):
 with Path(p).open('x',encoding='utf-8',newline='\n')as f:json.dump(j,f,indent=2,allow_nan=False);f.write('\n')
def module(p):
 s=importlib.util.spec_from_file_location('artqa_compile_validation',p);m=importlib.util.module_from_spec(s);s.loader.exec_module(m);return m
seal=read(O/'SOURCE-MANIFEST.json');assert all(sha(O/n)==h for n,h in seal['files'].items())
c=read(O/'BINDING-CONFIG.json');adopted=O/'adopted';assert not adopted.exists(),'Fresh C adoption required; preserve failures and receipts'
assert all(sha(p)==h for p,h in read(O/'INPUT-PINS.json').items())
compiled=module(O/'adoption.py').validate_compile(Path(c['compileRun']),Path(c['project']))
rows=[]
for r in read(O/'EXPECTED-STAGE-CLOSURES.json')[:-1]:
 assert sha(r['manifest'])==r['sha256']
 actual={p.relative_to(r['root']).as_posix():sha(p)for p in Path(r['root']).rglob('*')if p.is_file()};assert actual==read(r['manifest']),'Qualified Demo10 source changed during QA compile'
 rows.append({k:r[k]for k in ['root','manifest','sha256','files']})
qa=Path(c['qaRoot']);expected=read(O/'QA-EXPECTED-MANIFEST.json');actual={p.relative_to(qa).as_posix():sha(p)for p in qa.rglob('*')if p.is_file()}
assert set(actual)==set(expected)|{'resourceDatabase.rdb'}and all(actual[n]==h for n,h in expected.items())and(qa/'resourceDatabase.rdb').stat().st_size>0,'Only one fresh generated QA RDB is allowed'
adopted.mkdir();manifest=adopted/'QA_SOURCE_MANIFEST.json';save(manifest,actual)
rows.append({'root':str(qa),'manifest':str(manifest),'sha256':sha(manifest),'files':len(actual)})
save(adopted/'SOURCE-CLOSURES.json',rows);compiled['closures']=rows;save(adopted/'COMPILE-ADOPTION.json',compiled)
job=read(O/'JOB-TEMPLATE.json');job['project']=c['project'];job['world']=c['world'];job['sourceRoots']=[r['root']for r in rows];job['addonRoots']=c['addonRoots']
job['adapter']['script']=str(O/'adapter.py')
required=[O/n for n in seal['files']]+[O/'SOURCE-MANIFEST.json',manifest,adopted/'SOURCE-CLOSURES.json',adopted/'COMPILE-ADOPTION.json']
job['requiredFiles']=[{'path':str(p),'sha256':sha(p)}for p in required]
params=job['cases'][0]['parameters'];params.update(closures=str(adopted/'SOURCE-CLOSURES.json'),closurePinSHA256=sha(adopted/'SOURCE-CLOSURES.json'),completeManifestPins={r['manifest']:r['sha256']for r in rows},wrapper=str(O/'tools/Scene.ps1'),wrapperSHA256=sha(O/'tools/Scene.ps1'),compileAdoption=str(adopted/'COMPILE-ADOPTION.json'),markerContract=str(O/'MARKER-CONTRACT.json'),markerContractSHA256=sha(O/'MARKER-CONTRACT.json'),addonSearchPath=','.join(c['addonRoots']),seconds=180,screenWidth=1280,screenHeight=720,adapterDeadlineSeconds=240)
job['notes']='Bounded Demo10 approved-art static owner/node/config-material readback and fourteen near/far BMP requests. No control/art/LOD acceptance. Root sole native coordinator.'
save(adopted/'art-native-capture-job.json',job)
save(adopted/'PREPARATION-RECEIPT.json',{'status':'ACTUAL_QA_COMPILE_ADOPTED_JOB_PREPARED_NATIVE_PENDING','job':str(adopted/'art-native-capture-job.json'),'jobSHA256':sha(adopted/'art-native-capture-job.json'),'sourceSealSHA256':sha(O/'SOURCE-MANIFEST.json'),'closures':rows,'nativeStarted':False,'DWritten':False})
print(json.dumps({'status':'ACTUAL_QA_COMPILE_ADOPTED_JOB_PREPARED_NATIVE_PENDING','job':str(adopted/'art-native-capture-job.json'),'jobSHA256':sha(adopted/'art-native-capture-job.json'),'files':sum(r['files']for r in rows),'nativeStarted':False}))

from pathlib import Path
import ast,hashlib,json
R=Path(__file__).resolve().parent;A=R.parent/'GEP033-BoundedQualification-20261022';checks=[]
def load(p):return json.loads(p.read_text(encoding='utf-8-sig'))
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def check(n,v):checks.append({'name':n,'pass':bool(v)});assert v,n
for c in load(R/'binding/SOURCE-CLOSURES.json'):
 root=Path(c['root']);check('complete exact closure:'+str(root),{f.relative_to(root).as_posix():sha(f) for f in root.rglob('*') if f.is_file()}==load(Path(c['manifest'])))
Q=R/'native-stage/audioQA';old=A/'native-stage/audioQA'
for f in old.rglob('*'):
 if not f.is_file():continue
 rel=f.relative_to(old)
 if str(rel) in ['GEP_TrackRollingQA033.gproj','scripts\\Game\\GEP\\GEP_DamageScenario683.c']:continue
 check('unchanged passed QA:'+str(rel),sha(f)==sha(Q/rel))
before=(old/'scripts/Game/GEP/GEP_DamageScenario683.c').read_text().splitlines();after=(Q/'scripts/Game/GEP/GEP_DamageScenario683.c').read_text().splitlines();diff=[(a,b) for a,b in zip(before,after) if a!=b]
check('one resource allowlist line only',len(before)==len(after) and len(diff)==1 and all('if(m_Prefab!=' in x for x in diff[0]))
project=(Q/'GEP_TrackRollingQA033.gproj').read_text();check('unused Bradley reference removed','63120AE07E6C0966' not in project);check('final integration dependency','5E51C5D8F04FDB80' in project)
check('no old1158 common clone',not (R/'native-stage/gepard').exists())
for name in ['camo','charcoal']:
 j=load(R/'cases'/f'{name}.json');check('same bounded parameters:'+name,j['caseId']=='v11-wheeled-course' and j['crewRole']=='driver' and j['audioView']=='exterior' and j['maxGameSeconds']==110)
 check('actual approved target:'+name,'Prefabs/ArtApproved20261022/' in j['prefab'])
for name in ['Compile.ps1','Scene.ps1']:
 t=(R/'binding/tools'/name).read_text();check('old source root absent:'+name,str(A) not in t and 'GEP033-WCS-Reference-Diagnosis' not in t)
 check('root lifecycle guards:'+name,'AssertOwner' in t and 'RUNTIME_SLOT.json' in t and 'userRuntimeHold' in t)
for f in R.glob('*.py'):ast.parse(f.read_text());check('Python syntax:'+f.name,True)
(R/'SOURCE-CHECKS.json').write_text(json.dumps({'status':'PASS_SOURCE_ONLY','checks':checks,'nativeStarted':False,'nativeCompile':'NOT_TESTED_SUCCESSOR','nativeDriving':'NOT_TESTED_FINAL_SKINS'},indent=2)+'\n');print(len(checks),'PASS_SOURCE_ONLY')

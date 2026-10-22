from pathlib import Path
import ast,hashlib,json
R=Path(__file__).resolve().parent;checks=[]
def load(p):return json.loads(p.read_text(encoding='utf-8-sig'))
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def check(n,v):checks.append({'name':n,'pass':bool(v)});assert v,n
parent=load(R/'PARENT-ADOPTION-PINS.json');check('final parent adoption',sha(Path(parent['parentAdoption']))==parent['sha256'])
for c in load(R/'SOURCE-CLOSURES.json'):
 root=Path(c['root']);mp=Path(c['manifest']);check('manifest:'+str(root),sha(mp)==c['sha256']);check('full closure:'+str(root),{f.relative_to(root).as_posix():sha(f) for f in root.rglob('*') if f.is_file()}==load(mp))
for f in R.glob('*.py'):ast.parse(f.read_text());check('Python syntax:'+f.name,True)
old=Path(r'D:\ArmaProjects\Gepard-Consolidated-20261022/native-smoke-core-compat-stage-v2/single/qaCore/scripts/Game/GEP/GEP_DamageScenario683.c').read_text();new=(R/'native-stage/single/qaCore/scripts/Game/GEP/GEP_DamageScenario683.c').read_text()
for a,b in [('{7E0150C026103002}Prefabs/ArtV11Smoke/Camo/GEP_MobileCarrierV11Smoke.et','{2150FC6F1E081188}Prefabs/ArtApproved20261022/Camo/GEP_MobileCarrier.et'),('{7E0150C026103004}Prefabs/ArtV11Smoke/Charcoal/GEP_MobileCarrierV11Smoke.et','{8095474EC9806D0D}Prefabs/ArtApproved20261022/Charcoal/GEP_MobileCarrier.et')]:old=old.replace(a,b)
check('fixture behavior unchanged except two prefabs',old==new)
check('original collector exact',sha(R/'damage-adapter683.ts')==sha(R.parent/'GEP-Smoke-CoreCompatibility-v2-20261022/damage-adapter683.ts'))
check('exact eight addons and one scoped block',"if 'Loaded addons:' in l" in (R/'core_evidence.py').read_text() and "'5E389BB9F58B79A6'" in (R/'core_evidence.py').read_text())
for name in ['CompileSmokeCoreCompatibility.ps1','tools/Scene.ps1']:
 t=(R/name).read_text();check('no obsolete paths:'+name,'native-smoke-core-compat-stage-v2' not in t and 'smoke-core-compat-v2' not in t);check('root owner guards:'+name,'AssertOwner' in t and 'NativeInventory' in t if 'Compile' in name else 'AssertOwner' in t and 'Inventory' in t)
j=load(R/'core-native-job.template.json');check('two existing healthy cases',len(j['cases'])==2 and all(c['parameters']['nativeCase']=='v11-healthy' and c['parameters']['seconds']==105 and c['parameters']['adapterDeadlineSeconds']==125 for c in j['cases']))
check('explicit finite coordinator',j['timeoutSeconds']==180 and j['closeGraceSeconds']==35)
(R/'SOURCE-CHECKS.json').write_text(json.dumps({'status':'PASS_SOURCE_ONLY','nativeCompile':'NOT_TESTED_FRESH_QA','nativeBehavior':'NOT_TESTED_DEMO10','checks':checks},indent=2)+'\n');print(len(checks),'PASS_SOURCE_ONLY')

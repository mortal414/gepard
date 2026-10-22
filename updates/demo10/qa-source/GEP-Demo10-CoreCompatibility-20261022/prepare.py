"""Derive the existing bounded healthy fixture. Source writes stay in this C folder."""
from pathlib import Path
import hashlib,json,shutil,difflib,ast
R=Path(__file__).resolve().parent;B=R.parent/'GEP-Smoke-CoreCompatibility-v2-20261022'
P=R.parent/'Gepard-Demo10-Integration-20261022'
T=Path(r'D:\ArmaProjects\Gepard-Demo10-TrackArt-20261022/private-demo-source10')
OLD=Path(r'D:\ArmaProjects\Gepard-Consolidated-20261022/native-smoke-core-compat-stage-v2')
OLDQ=Path(r'D:\ArmaProjects\Gepard-Consolidated-20261022/qa/consolidated-20261022/smoke-core-compat-v2')
Q=R/'native-stage/single/qaCore';PROJECT=Q/'GEP_SmokeCoreCompatQA.gproj'
def load(p):return json.loads(p.read_text(encoding='utf-8-sig'))
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def save(p,j):p.parent.mkdir(parents=True,exist_ok=True);p.write_text(json.dumps(j,indent=2)+'\n')
def filemap(d):return {p.relative_to(d).as_posix():sha(p) for p in d.rglob('*') if p.is_file()}
parent=load(P/'adopted/ACTUAL-COMPILE-ADOPTION.json')
assert parent['status']=='SOURCE_MATCHED_NATIVE_COMPILE_ADOPTED'
assert sha(Path(parent['receipt']))==parent['receiptSHA256']
receipt=load(Path(parent['receipt']));assert receipt['status']=='SCRIPT_COMPILE_PASS' and receipt['exitCode']==0 and receipt['nativeAbsent'] is True and receipt['leaseReleased'] is True
assert not (R/'native-stage').exists(),'Fresh source stage only'
external=[]
for c in parent['closures']:
 if c['role']=='provingGround':continue
 root=Path(c['root']);mp=Path(c['manifest']);assert sha(mp)==c['manifestSHA256'];assert filemap(root)==load(mp)
 external.append(c)
assert sorted(c['files'] for c in external)==[3,3,14,68,1152]
shutil.copytree(OLD/'single/qaCore',Q)
overlay=R.parent/'GEP033-BoundedQualification-20261022/demo10-core-rebind/overlay'
for rel in ['GEP_SmokeCoreCompatQA.gproj','scripts/Game/GEP/GEP_DamageScenario683.c']:shutil.copy2(overlay/rel,Q/rel)
newprefabs=['{2150FC6F1E081188}Prefabs/ArtApproved20261022/Camo/GEP_MobileCarrier.et','{8095474EC9806D0D}Prefabs/ArtApproved20261022/Charcoal/GEP_MobileCarrier.et']
oldprefabs=['{7E0150C026103002}Prefabs/ArtV11Smoke/Camo/GEP_MobileCarrierV11Smoke.et','{7E0150C026103004}Prefabs/ArtV11Smoke/Charcoal/GEP_MobileCarrierV11Smoke.et']
roots=[str(R/'native-stage/single'),str(T/'single'),str(T/'dependencies'),r'D:\SteamLibrary\steamapps\common\Arma Reforger\addons'];search=','.join(roots)
def transform(s):
 s=s.replace(str(OLD/'single/qaCore'),str(Q)).replace(str(OLDQ),str(R/'adopted')).replace(str(B),str(R))
 s=s.replace(str(OLD/'single'),str(T/'single')).replace(str(OLD/'dependencies/WCS_Armaments'),str(T/'dependencies/WCS_Armaments_629B2BA37EFFD577')).replace(str(OLD/'dependencies'),str(T/'dependencies'))
 for a,b in zip(oldprefabs,newprefabs):s=s.replace(a,b)
 return s
for name in ['adapter.py','core_evidence.py','evidence.py','lease_read.py','damage-adapter683.ts','package.json','HEALTHY-ADOPTION.json']:
 txt=transform((B/name).read_text())
 if name=='core_evidence.py':
  start=txt.index(' loaded=[]\n');end=txt.index(" checks['smoke.not-triggered']",start)
  txt=txt[:start]+''' loaded=[]
  # Successor repair of the observer: scope one Loaded block; never deduplicate.
 blocks=[i for i,l in enumerate(log.splitlines()) if 'Loaded addons:' in l]
 if len(blocks)==1:
  for line in log.splitlines()[blocks[0]+1:]:
   match=re.search(r"ENGINE\\s*: gproj: '.*?' guid: '([A-Fa-f0-9]{16})'",line)
   if not match:break
   loaded.append(match[1].upper())
 expected={'5614BBCCBB55ED1C','58D0FB3206B6F859','D561AD08932E7B40','629B2BA37EFFD577','7E0150C026101500','A9F84D1626102251','5E51C5D8F04FDB80','5E389BB9F58B79A6'}
 checks['addons.exact-stack']={'status':'PASS' if len(blocks)==1 and len(loaded)==len(expected) and set(loaded)==expected else 'FAIL','reason':'One scoped Loaded addons block; exact Demo10 eight IDs, no deduplication; old scan-wide failure preserved','actual':loaded}
''' +txt[end:]
 (R/name).write_text(txt)
for name in ['CompileSmokeCoreCompatibility.ps1','tools/Scene.ps1']:
 txt=transform((B/name).read_text())
 if name.endswith('Scene.ps1'):
  old="@('"+str(T/'single')+"','"+str(T/'dependencies')+"','D:\\SteamLibrary\\steamapps\\common\\Arma Reforger\\addons')"
  new="@("+','.join("'"+x+"'" for x in roots)+")";assert txt.count(old)==1;txt=txt.replace(old,new)
 else:
  old=','.join(roots[1:]);assert txt.count(old)==1;txt=txt.replace(old,search)
 f=R/name;f.parent.mkdir(parents=True,exist_ok=True);f.write_text(txt)
shutil.copytree(B/'cases',R/'cases');shutil.copytree(B/'vehicles',R/'vehicles')
for skin,prefab in zip(['camo','charcoal'],newprefabs):
 f=R/'vehicles'/f'v11-{skin}.json';j=load(f);j['prefab']=prefab;save(f,j)
closures=[]
for c in external:
 mp=R/'manifests'/(c['role']+'.json');save(mp,load(Path(c['manifest'])))
 closures.append({'root':c['root'],'manifest':str(mp),'sha256':sha(mp),'files':c['files']})
mp=R/'manifests/qaCore.json';save(mp,filemap(Q));closures.append({'root':str(Q),'manifest':str(mp),'sha256':sha(mp),'files':len(load(mp))})
save(R/'SOURCE-CLOSURES.json',closures);save(R/'GEPARD-MANIFEST.json',filemap(Q))
j=json.loads(transform(json.dumps(load(B/'core-native-job.json')))) if False else load(B/'core-native-job.json')
def rewrite(x):
 if isinstance(x,str):return transform(x)
 if isinstance(x,list):return [rewrite(v) for v in x]
 if isinstance(x,dict):return {k:rewrite(v) for k,v in x.items()}
 return x
j=rewrite(j);j['project']=str(PROJECT);j['sourceRoots']=[c['root'] for c in closures];j['addonRoots']=roots;j['requiredFiles']=[]
for case in j['cases']:
 case['parameters']['addonSearchPath']=search
 case['parameters']['wrapperSHA256']=sha(R/'tools/Scene.ps1')
 case['parameters']['closures']=str(R/'adopted/postcompile/SOURCE-CLOSURES.json')
 case['parameters']['qaManifest']=str(R/'adopted/postcompile/qaCore.json')
save(R/'core-native-job.template.json',j)
save(R/'PARENT-ADOPTION-PINS.json',{'parentAdoption':str(P/'adopted/ACTUAL-COMPILE-ADOPTION.json'),'sha256':sha(P/'adopted/ACTUAL-COMPILE-ADOPTION.json'),'receipt':parent['receipt'],'receiptSHA256':parent['receiptSHA256'],'parentClosuresExact':True,'excludedUnloadedWorldRoot':'provingGround'})
for f in R.glob('*.py'):ast.parse(f.read_text())
save(R/'PREPARATION.json',{'status':'FROZEN_SOURCE_PREPARED_FRESH_QA_NATIVE_COMPILE_REQUIRED','project':str(PROJECT),'world':'Worlds/GEP_GunRange.ent','roots':roots,'sourceCounts':[c['files'] for c in closures],'nativeStarted':False,'sameQAWorldAndActions':True,'QAEdits':['two prefab strings','project integration dependency'],'collectorEdit':'scope exact Loaded addon block with eight Demo10 IDs; all raw/native E-F retained'})
print('SOURCE_READY',str(PROJECT),[c['files'] for c in closures])

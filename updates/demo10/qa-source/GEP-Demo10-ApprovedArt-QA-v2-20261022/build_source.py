"""C-only recovery/rebinding of proven ART4; never stages D or starts native."""
from pathlib import Path
import ast,difflib,hashlib,json,re,shutil
O=Path(__file__).resolve().parent;T=O.parent
P=T/'Gepard-Demo10-Integration-20261022';B=T/'GEP-ArtNativeScene-Bridge-NodeId-20261022';W=T/'GEP-Smoke-CurrentV11-Qualification-v3-20261022'
OLD=Path(r'D:\ArmaProjects\Gepard-Consolidated-20261022\native-stage\artNativeQA')
R=Path(r'D:\ArmaProjects\Gepard-Demo10-TrackArt-20261022');STAGE=R/'native-artqa-new-stage-v2';QA=STAGE/'artNativeQA'
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def read(p):return json.loads(Path(p).read_text(encoding='utf-8-sig'))
def write(rel,b):
 p=O/rel;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(b.encode('utf-8')if isinstance(b,str)else b)
def js(rel,j):write(rel,json.dumps(j,indent=2)+'\n')
pins={};changes=[]
def source(p,h=None):
 p=Path(p);assert h is None or sha(p)==h;pins[str(p)]=sha(p);return p.read_bytes().decode('utf-8')
def proposed(rel,before,after,origin):
 assert before!=after;write(rel,after)
 changes.append({'relative':rel,'origin':str(origin),'beforeSHA256':hashlib.sha256(before.encode()).hexdigest(),'afterSHA256':hashlib.sha256(after.encode()).hexdigest()})
 write('patches/'+Path(rel).name+'.patch',''.join(difflib.unified_diff(before.splitlines(keepends=True),after.splitlines(keepends=True),fromfile=str(origin),tofile=rel)))
handoff=read(r'D:\ArmaProjects\Gepard-ArtApproved-20261022\APPROVED_ART_HANDOFF.json');pins[r'D:\ArmaProjects\Gepard-ArtApproved-20261022\APPROVED_ART_HANDOFF.json']=sha(r'D:\ArmaProjects\Gepard-ArtApproved-20261022\APPROVED_ART_HANDOFF.json')
adopt=read(P/'adopted/ACTUAL-COMPILE-ADOPTION.json');pins[str(P/'adopted/ACTUAL-COMPILE-ADOPTION.json')]=sha(P/'adopted/ACTUAL-COMPILE-ADOPTION.json')
assert adopt['status']=='SOURCE_MATCHED_NATIVE_COMPILE_ADOPTED'and len(adopt['closures'])==6
roots=[]
for row in adopt['closures']:
 manifest=Path(row['manifest']);assert sha(manifest)==row['manifestSHA256'];pins[str(manifest)]=sha(manifest)
 files=read(manifest);assert len(files)==row['files'];actual={p.relative_to(row['root']).as_posix():sha(p)for p in Path(row['root']).rglob('*')if p.is_file()};assert actual==files
 rel='inputs/'+manifest.name;js(rel,files);roots.append({'root':row['root'],'manifest':str(O/rel),'sha256':sha(O/rel),'files':len(files),'role':row['role']})
pins[str(P/'ART-NATIVE-REBUILD-RESULT.json')]=sha(P/'ART-NATIVE-REBUILD-RESULT.json')
assert len(handoff['models'])==8
order=['MobilityRig.xob','GepardTurretRig.xob','RightStorage669.xob','GepardGunLeft.xob','GepardGunRight.xob','CoverLeft668.xob','CoverRight668.xob','RightShieldCover671.xob']
models=[next(m['new_model']for m in handoff['models']if m['new_model'].endswith('/'+name))for name in order]
# The eighth rebuilt family is a retained safe export and intentionally unbound.
# Preserve real inherited V11 storage; do not create a production binding for QA.
models[2]='{C6D9B89642E424E3}Assets/ArtV11/Gepard/RemainingExterior669/RightStorage669.xob'
op=OLD/'scripts/Game/GEPArtNativeQA/GEP_ArtNativeQualification20261022.c';before=source(op,'fddeea3f97c9f5fceea90eb4a7a0d1a88a5834a4d08e4d0e19a65eae3c6a80d0')
after=re.sub(r'(protected ref array<ResourceName> m_Models = \{).*?(\r?\n \};)',lambda m:m[1]+'\r\n'+',\r\n'.join('  "'+s+'"'for s in models)+m[2],before,count=1,flags=re.S)
after=after.replace('{9EF02C309F7E7929}Prefabs/ArtV11/Camo/GEP_MobileCarrierV11.et',handoff['prefabs']['CamoVehicle']).replace('{231917C79EBEFDDE}Prefabs/ArtV11/Charcoal/GEP_MobileCarrierV11.et',handoff['prefabs']['CharcoalVehicle'])
assert after.count('< 5;')==2;after=after.replace('< 5;','< m_Models.Count();')
after=after.replace('"guns", "reargrille"','"guns", "reargrille", "rearfar"').replace('artnative-functional-v11-','artnative-demo10-new-')
proposed('source/scripts/Game/GEPArtNativeQA/GEP_ArtNativeQualification20261022.c',before,after,op)
sp=OLD/'scripts/Game/GEPArtNativeSceneQA/GEP_ArtNativeSceneQA20261022.c';before=source(sp,'fa72fffba51fdd0989c73855c8754e9dbca3f795b10685e8526475b13b57f162')
after=before.replace('{9EF02C309F7E7929}Prefabs/ArtV11/Camo/GEP_MobileCarrierV11.et',handoff['prefabs']['CamoVehicle']).replace('{231917C79EBEFDDE}Prefabs/ArtV11/Charcoal/GEP_MobileCarrierV11.et',handoff['prefabs']['CharcoalVehicle'])
after=after.replace('"guns", "reargrille"','"guns", "reargrille", "rearfar"').replace('m_Case / 6','m_Case / 7').replace('m_Case % 6','m_Case % 7').replace('m_Case == 12','m_Case == 14').replace('plannedViews=12','plannedViews=14').replace('completedViews=12','completedViews=14')
anchor='  vector point = subjectMatrix[3] + offset;';assert after.count(anchor)==1
after=after.replace(anchor,'  // Same rear3q basis, four times horizontal distance; avoids the adjacent other subject; image evidence only.\r\n  if (viewIndex == 6) { offset = "28 0 -40"; right = "0.8192319 0 0.5734623"; up = "0 1 0"; forward = "-0.5734623 0 0.8192319"; }\r\n'+anchor)
proposed('source/scripts/Game/GEPArtNativeSceneQA/GEP_ArtNativeSceneQA20261022.c',before,after,sp)
for rel in ['GEPArtNativeSceneQA.gproj','Worlds/GEPArtNativeSceneQA.ent','Worlds/GEPArtNativeSceneQA.ent.meta','Worlds/GEPArtNativeSceneQA_default.layer']:
 b=source(OLD/rel)
 if rel.endswith('.gproj'):b=b.replace('"58D0FB3206B6F859" "D561AD08932E7B40"','"58D0FB3206B6F859" "D561AD08932E7B40" "5E51C5D8F04FDB80"')
 write('source/'+rel,b)
qaFiles={p.relative_to(O/'source').as_posix():sha(p)for p in(O/'source').rglob('*')if p.is_file()};assert len(qaFiles)==6
js('QA-EXPECTED-MANIFEST.json',qaFiles)
roots.append({'root':str(QA),'manifest':str(O/'QA-EXPECTED-MANIFEST.json'),'sha256':sha(O/'QA-EXPECTED-MANIFEST.json'),'files':6,'role':'artNativeQA'})
js('EXPECTED-STAGE-CLOSURES.json',roots)
for name in ['adapter.py','lease_read.py','adoption.py','evidence.py','review_run.py']:
 b=source(B/name)
 if name=='adapter.py':b=b.replace('twelve','fourteen').replace('Twelve','Fourteen')
 if name=='evidence.py':
  b=b.replace('len(set(loaded))==4','len(set(loaded))==len(contract[\'loadedAddonIds\'])').replace('plannedViews=12','plannedViews=14')
  b=b.replace(r"r'\(E\)|Cannot compile|\berror:'",r"r'\([EF]\)|Cannot compile|Can.t compile|Unhandled exception|Fatal error|\berror:'")
 if name=='review_run.py':
  b=b.replace('"guns", "reargrille"','"guns", "reargrille", "rearfar"').replace('completedViews=12','completedViews=14').replace('len(models) != 5','len(models) != 8').replace('set(range(5))','set(range(8))').replace('five native model','eight native model').replace('artnative-functional-v11-','artnative-demo10-new-')
  anchor='    images = []';b=b.replace(anchor,'    expected_models = '+repr(models)+'\n    for skin in SKINS:\n        records = re.findall(r"GEP_ART_NATIVE_MODEL skin=" + skin + r" modelIndex=(\\d+) count=(\\d+) expected=([^\\r\\n]+)", log)\n        if {int(row[0]): row[2].strip() for row in records} != dict(enumerate(expected_models)):\n            failures.append(f"exact approved model owner resources differ: {skin}")\n'+anchor)
 ast.parse(b);write(name,b)
contract=read(B/'MARKER-CONTRACT.json');contract.update(loadedAddonIds=['65239F780E614817','5E51C5D8F04FDB80','D561AD08932E7B40','7E0150C026101500','5E389BB9F58B79A6','629B2BA37EFFD577','58D0FB3206B6F859','5614BBCCBB55ED1C'],viewsPerSkin=7,expectedImages=14,modelResources=models)
js('MARKER-CONTRACT.json',contract)
addons=[str(STAGE),str(R/'private-demo-source10/single'),str(R/'private-demo-source10/dependencies'),r'D:\SteamLibrary\steamapps\common\Arma Reforger\addons']
oldStage=r'D:\ArmaProjects\Gepard-Consolidated-20261022\native-smoke-current-v11-stage-v3';project=str(QA/'GEPArtNativeSceneQA.gproj');compileRun=str(R/'native-runs/compile-demo10-artqa-02')
for sourceName,target in [('Scene.ps1','tools/Scene.ps1'),('CompileCurrentV11.ps1','tools/Compile.ps1')]:
 b=source(W/'tools'/sourceName)
 b=b.replace(str(W),str(O)).replace(oldStage+r'\current\qaSmoke\GEP_SmokeCurrentV11_QA.gproj',project)
 if sourceName=='Scene.ps1':
  b=b.replace('$Seconds=75','$Seconds=180').replace("$Seconds -ne 75","$Seconds -ne 180").replace('Only fixed75second full-eight-empty case is bound','Only fixed180second ART4 static capture case is bound')
  b=b.replace("$World -cnotin @('Worlds/GEP215_CurrentV11Camo.ent','Worlds/GEP215_CurrentV11Charcoal.ent')","$World -cne 'Worlds/GEPArtNativeSceneQA.ent'").replace('7E0150C026106500','65239F780E614817')
  b=re.sub(r"\$addonRootsGPG=@\([^\r\n]+\)",lambda _:"$addonRootsGPG=@("+','.join("'"+s+"'"for s in addons)+')',b,count=1)
 else:
  b=b.replace(r'D:\ArmaProjects\Gepard-Consolidated-20261022\native-runs\compile-smoke-current-v11-02',compileRun)
  oldSearch=oldStage+r'\current,'+oldStage+r'\dependencies,D:\SteamLibrary\steamapps\common\Arma Reforger\addons'
  assert oldSearch in b;b=b.replace(oldSearch,','.join(addons))
 assert oldStage not in b
 write(target,b)
template=read(B/'art-native-capture-job.json')
template=json.loads(json.dumps(template).replace('{9EF02C309F7E7929}Prefabs/ArtV11/Camo/GEP_MobileCarrierV11.et',handoff['prefabs']['CamoVehicle']).replace('{231917C79EBEFDDE}Prefabs/ArtV11/Charcoal/GEP_MobileCarrierV11.et',handoff['prefabs']['CharcoalVehicle']))
template.update(project=project,world='Worlds/GEPArtNativeSceneQA.ent',sourceRoots=[r['root']for r in roots],addonRoots=addons,requiredFiles=[])
template['adapter']['script']=str(O/'adapter.py')
template['cases'][0]['parameters'].update(compileAdoption=str(O/'adopted/COMPILE-ADOPTION.json'),wrapper=str(O/'tools/Scene.ps1'),closures=str(O/'adopted/SOURCE-CLOSURES.json'))
js('JOB-TEMPLATE.json',template)
js('INPUT-PINS.json',pins);js('SOURCE-DELTA.json',changes)
js('BINDING-CONFIG.json',{'qaRoot':str(QA),'project':project,'world':'Worlds/GEPArtNativeSceneQA.ent','compileRun':compileRun,'addonRoots':addons,'addonId':'65239F780E614817','seconds':180,'adapterDeadlineSeconds':240,'coordinatorSeconds':270,'coreCompileReceipt':str(P/'adopted/ACTUAL-COMPILE-ADOPTION.json'),'coreCompileReceiptSHA256':sha(P/'adopted/ACTUAL-COMPILE-ADOPTION.json'),'status':'SOURCE_ONLY_QA_NATIVE_COMPILE_AND_ARTIFACT_REVIEW_REQUIRED'})
print(json.dumps({'status':'SOURCE_REBOUND_NOT_COMPILED_OR_RUN','qaFiles':6,'models':models,'imageRequests':14,'allSourceFiles':sum(r['files']for r in roots),'compileRun':compileRun},indent=2))

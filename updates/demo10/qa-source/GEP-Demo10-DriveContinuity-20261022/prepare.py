"""Only resource/dependency rebinding of the passed A course. No native calls."""
from pathlib import Path
import json,hashlib,shutil,ast
R=Path(__file__).resolve().parent;A=R.parent/'GEP033-BoundedQualification-20261022';P=R.parent/'GEP-Demo10-CoreCompatibility-20261022'
T=Path(r'D:\ArmaProjects\Gepard-Demo10-TrackArt-20261022/private-demo-source10')
Q=R/'native-stage/audioQA'
def load(p):return json.loads(p.read_text(encoding='utf-8-sig'))
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def save(p,j):p.parent.mkdir(parents=True,exist_ok=True);p.write_text(json.dumps(j,indent=2)+'\n')
def mapping(root):return {p.relative_to(root).as_posix():sha(p) for p in root.rglob('*') if p.is_file()}
assert not Q.exists(),'Fresh QA successor only'
adopt=load(P/'adopted/COMPILE-ADOPTION.json');receipt=load(Path(adopt['compileRun'])/'RESULT.json')
assert receipt['status']=='SCRIPT_COMPILE_PASS' and receipt['exitCode']==0 and receipt['nativeAbsent'] is True and receipt['leaseReleased'] is True
assert sha(Path(adopt['compileRun'])/'RESULT.json')==adopt['compilerResultSHA256']
closures=[]
for c in adopt['closures']:
 if Path(c['root']).is_relative_to(P):continue
 mp=Path(c['manifest']);assert sha(mp)==c['sha256'] and mapping(Path(c['root']))==load(mp)
 target=R/'binding'/mp.name;save(target,load(mp));closures.append({'root':c['root'],'manifest':str(target)})
assert len(closures)==5
shutil.copytree(A/'native-stage/audioQA',Q)
f=Q/'GEP_TrackRollingQA033.gproj';txt=f.read_text();assert txt.count('"63120AE07E6C0966"')==1;f.write_text(txt.replace('"63120AE07E6C0966" ',''))
prefabs={'camo':'{2150FC6F1E081188}Prefabs/ArtApproved20261022/Camo/GEP_MobileCarrier.et','charcoal':'{8095474EC9806D0D}Prefabs/ArtApproved20261022/Charcoal/GEP_MobileCarrier.et'}
f=Q/'scripts/Game/GEP/GEP_DamageScenario683.c';txt=f.read_text();lines=txt.splitlines();found=[l for l in lines if l.startswith('   if(m_Prefab!=') or l.startswith('  if(m_Prefab!=')];assert len(found)==1
replacement='  if(m_Prefab!="'+prefabs['camo']+'" && m_Prefab!="'+prefabs['charcoal']+'")return false;'
txt=txt.replace(found[0],replacement);f.write_text(txt)
mp=R/'binding/QA-MANIFEST.json';save(mp,mapping(Q));closures.append({'root':str(Q),'manifest':str(mp)})
save(R/'binding/SOURCE-CLOSURES.json',closures);save(R/'binding/SOURCE-BEFORE-COMPILE.json',{c['root']:load(Path(c['manifest'])) for c in closures})
roots=[str(R/'native-stage'),str(T/'single'),str(T/'dependencies'),r'D:\SteamLibrary\steamapps\common\Arma Reforger\addons']
olddep=str(A.parent/'GEP033-WCS-Reference-Diagnosis-20261022/dependencies')
for name in ['Compile.ps1','Scene.ps1']:
 txt=(A/'binding/tools'/name).read_text().replace(str(A),str(R))
 if name=='Compile.ps1':
  old=','.join([str(R/'native-stage'),olddep,roots[-1]]);assert txt.count(old)==1;txt=txt.replace(old,','.join(roots))
 else:
  old="@("+','.join("'"+x+"'" for x in [str(R/'native-stage'),olddep,roots[-1]])+")";assert txt.count(old)==1;txt=txt.replace(old,"@("+','.join("'"+x+"'" for x in roots)+")")
 f=R/'binding/tools'/name;f.parent.mkdir(parents=True,exist_ok=True);f.write_text(txt)
for name in ['adopt_compile.py','review_run.py']:shutil.copy2(A/name,R/name)
for skin,prefab in prefabs.items():
 case=load(A/'cases/exterior.json');case.update(vehicleId='demo10-'+skin,prefab=prefab,audioView='exterior');save(R/'cases'/f'{skin}.json',case)
cmd=load(A/'recordings/exterior/command.json')['command']
cmd=[x.replace(str(A),str(R)) for x in cmd]
save(R/'COMMAND-TEMPLATE.json',{'command':cmd,'nativeStarted':False,'note':'RunRoot/nativeJob/hash replaced only by prepare_recording.py after actual compile'})
save(R/'PARENT-PINS.json',{'adoption':str(P/'adopted/COMPILE-ADOPTION.json'),'sha256':sha(P/'adopted/COMPILE-ADOPTION.json'),'passedCourse':str(A/'native-stage/audioQA/scripts/Game/GEP/GEP_CoreWheeledCourse20261022.c'),'courseSHA256':sha(A/'native-stage/audioQA/scripts/Game/GEP/GEP_CoreWheeledCourse20261022.c'),'parentAudioPairAudit':str(A/'paired-audit-01/PAIRED-EVIDENCE.json'),'parentAudioPairSHA256':sha(A/'paired-audit-01/PAIRED-EVIDENCE.json')})
assert sha(Q/'scripts/Game/GEP/GEP_CoreWheeledCourse20261022.c')==sha(A/'native-stage/audioQA/scripts/Game/GEP/GEP_CoreWheeledCourse20261022.c')
save(R/'PREPARATION.json',{'status':'SOURCE_READY_FRESH_NATIVE_COMPILE_REQUIRED','nativeStarted':False,'sourceCounts':[len(load(Path(c['manifest']))) for c in closures],'roots':roots,'project':str(Q/'GEP_TrackRollingQA033.gproj'),'world':'Worlds/GEP_TrackRollingAudition033.ent','prefabs':prefabs,'changes':'Only private resource allowlist and unused Bradley reference dependency removal. Final integration dependency already present in A project. All passed course/camera/input/physics/exit-reentry behavior unchanged. Old1158 common never copied.'})
print('SOURCE_READY',str(Q/'GEP_TrackRollingQA033.gproj'))

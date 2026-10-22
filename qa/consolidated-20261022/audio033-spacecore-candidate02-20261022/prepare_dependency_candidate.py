from pathlib import Path
import json,hashlib,shutil,secrets,uuid,sys
N=Path(__file__).resolve().parent;OLD=N/'diagnostic-v3';V=N/'dependency-track-v5';B=V/'binding'
R=Path(r'D:\ArmaProjects\Gepard-Consolidated-20261022')
def load(p):return json.loads(p.read_text(encoding='utf-8-sig'))
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def inv(p):return {x.relative_to(p).as_posix():sha(x) for x in p.rglob('*') if x.is_file()}
def save(p,d):p.parent.mkdir(parents=True,exist_ok=True);p.write_text(json.dumps(d,indent=2)+'\n')
cfg=load(Path(sys.argv[1]));addon=Path(cfg['addonRoot']);pins=cfg['manifest'];guid=cfg['addonGUID'];prefab=cfg['prefabResource']
assert inv(addon)==pins and len(pins)==3 and len(guid)==16
assert not V.exists();lease=load(Path(r'C:\Users\marco\Documents\Codex\2026-10-12\task\RUNTIME_SLOT.json'));assert lease['status'].startswith('released') and not lease['pid'] and not lease['userRuntimeHold']['active']
for x in load(OLD/'binding/SOURCE-CLOSURES.json'):assert inv(Path(x['root']))==load(Path(x['manifest']))
shutil.copytree(OLD/'native-stage',V/'native-stage');candidate=V/'native-stage/trackDependency033';shutil.copytree(addon,candidate);B.mkdir();qa=V/'native-stage/audioQA'
assert inv(candidate)==pins
project=qa/'GEP_TrackRollingQA033.gproj';text=project.read_text();needle='"629B2BA37EFFD577"';assert text.count(needle)==1;project.write_text(text.replace(needle,needle+' "'+guid+'"'),newline='\n')
scenario=qa/'scripts/Game/GEP/GEP_DamageScenario683.c';text=scenario.read_text();needle=' && m_Prefab!="{24223D0679CDD612}Prefabs/Vehicles/Tracked/M2A2/M2A2_OD.et")return false;';assert text.count(needle)==1
scenario.write_text(text.replace(needle,needle.replace(')return false;',' && m_Prefab!="'+prefab+'")return false;')),newline='\n')
for name in ('Compile.ps1','Scene.ps1'):
 src=(OLD/'binding/tools'/name).read_text().replace(str(OLD),str(V))
 if name=='Scene.ps1':
  src=src.replace("@('audio-stock','audio-candidate','audio-wcs-reference')","@('audio-stock','audio-candidate','audio-wcs-reference','audio-dependency-track')")
  needle="'audio-wcs-reference'='{24223D0679CDD612}Prefabs/Vehicles/Tracked/M2A2/M2A2_OD.et'}";assert src.count(needle)==1
  src=src.replace(needle,needle[:-1]+";'audio-dependency-track'='"+prefab+"'}")
 (B/'tools').mkdir(exist_ok=True);(B/'tools'/name).write_text(src,newline='\n')
closures=[]
for x in load(OLD/'binding/SOURCE-CLOSURES.json'):
 root=Path(str(x['root']).replace(str(OLD),str(V)));mp=B/Path(x['manifest']).name;save(mp,inv(root));closures.append({'root':str(root),'manifest':str(mp)})
mp=B/'TRACK-DEPENDENCY-MANIFEST.json';save(mp,inv(candidate));closures.append({'root':str(candidate),'manifest':str(mp)})
save(B/'SOURCE-CLOSURES.json',closures);save(B/'SOURCE-BEFORE-COMPILE.json',{x['root']:load(Path(x['manifest'])) for x in closures})
template=load(OLD/'binding/commands/GEPARD_CURRENT.json');job=load(OLD/'binding/jobs/GEPARD_CURRENT.json');job.update(runId=str(uuid.uuid4()),nonce=secrets.token_hex(32),vehicleId='audio-dependency-track',prefab=prefab)
jf=B/'jobs/SPACECORE_TRACK_DEPENDENCY.json';save(jf,job);cmd=[v.replace(str(OLD),str(V)) for v in template['command']];run=R/'native-runs/audio-dependency-v5-exterior-01';assert not run.exists()
cmd[cmd.index('-RunRoot')+1]=str(run);cmd[cmd.index('-NativeJob')+1]=str(jf);cmd[cmd.index('-NativeJobSHA256')+1]=sha(jf)
save(B/'commands/SPACECORE_TRACK_DEPENDENCY.json',{'command':cmd,'nativeRun':str(run),'wrapperSHA256':sha(B/'tools/Scene.ps1')})
assert inv(V/'native-stage/gepard')==inv(OLD/'native-stage/gepard')
assert sha(qa/'scripts/Game/GEP/GEP_CoreWheeledCourse20261022.c')==sha(OLD/'native-stage/audioQA/scripts/Game/GEP/GEP_CoreWheeledCourse20261022.c')
save(V/'SUPPLIER-BINDING.json',cfg)
save(V/'PREPARATION.json',{'status':'SOURCE_ONLY_FRESH_NATIVE_COMPILE_REQUIRED','candidateAddon':str(candidate),'candidateGUID':guid,'prefab':prefab,'common1158FilesUnchanged':True,'QAChanges':'Only exact candidate prefab admission and dependency GUID; course/readback/camera unchanged','dependencies':'Normal installed immutable M2/SpaceCore/Armaments stack shared with prior comparison; no payload copying into addon','sourceFiles':sum(len(load(Path(x['manifest']))) for x in closures),'audibleAcceptance':'NOT_TESTED_NEW_DEPENDENCY_CANDIDATE'})
print(json.dumps(load(V/'PREPARATION.json')))

from pathlib import Path
import json,hashlib,shutil
C=Path(__file__).resolve().parent;R=Path(r'D:\ArmaProjects\Gepard-Consolidated-20261022');T=Path(r'D:\ArmaProjects\Gepard-Demo10-TrackArt-20261022')
ROOT=T/'private-demo-source10';SINGLE=ROOT/'single';DEPS=ROOT/'dependencies'
N=C.parent/'GEP033-WCS-Reference-Diagnosis-20261022';SUP=C.parent.parent/'task-5/stock-roll-candidate/spacecore-dependency-candidate-02'
def load(p):return json.loads(p.read_text(encoding='utf-8-sig'))
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def inv(p):return {x.relative_to(p).as_posix():sha(x) for x in p.rglob('*') if x.is_file()}
def save(p,d):p.parent.mkdir(parents=True,exist_ok=True);p.write_text(json.dumps(d,indent=2)+'\n')
assert not ROOT.exists();lease=load(Path(r'C:\Users\marco\Documents\Codex\2026-10-12\task\RUNTIME_SLOT.json'));assert not lease['pid'] and lease['status'].startswith('released') and not lease['userRuntimeHold']['active']
protected=load(N.parent/'GEP033-Audio-Authorized-Author02-20261022/PREFLIGHT.json')['protected'];assert all(sha(Path(p))==h for p,h in protected.items())
old=R/'private-smoke-demo-source-v2/single';qualified=load(R/'qa/consolidated-20261022/private-final-source-v6/CurrentSourceManifest-gepard.json');assert inv(old/'gepard')==qualified and len(qualified)==1152
assert shutil.disk_usage(T).free>6*1024**3
SINGLE.mkdir(parents=True);DEPS.mkdir()
for role in ('gepard','wcsSmoke'):shutil.copytree(old/role,SINGLE/role);assert inv(old/role)==inv(SINGLE/role)
shutil.copytree(R/'updates/demo09/source/provingGround',SINGLE/'provingGround')
sourceManifest=load(SUP/'MANIFEST.json');candidate=SUP/'spaceCoreTracks';assert inv(candidate)=={x['path']:x['sha256'] for x in sourceManifest['files']}
shutil.copytree(candidate,SINGLE/'integration');integration=SINGLE/'integration'
gp=integration/'GEP_SpaceCoreTracks033.gproj';text=gp.read_text();needle='"629B2BA37EFFD577"';assert text.count(needle)==1;gp.write_text(text.replace(needle,needle+' "7E0150C026101500"'),newline='\n')
wrapperRows=[]
for skin,parent,identifier in [('Camo','{7E0150C026103002}Prefabs/ArtV11Smoke/Camo/GEP_MobileCarrierV11Smoke.et','A76A420022100002'),('Charcoal','{7E0150C026103004}Prefabs/ArtV11Smoke/Charcoal/GEP_MobileCarrierV11Smoke.et','A76A420022100004')]:
 rel=f'Prefabs/Demo10/{skin}/GEP_MobileCarrierDemo10.et';resource='{'+identifier+'}'+rel;dest=integration/rel;dest.parent.mkdir(parents=True)
 child='// Original Demo10 child: current V11 smoke/functional stack plus accepted normal track dependency.\nVehicle : "'+parent+'" {\n components {\n  SCR_VehicleSoundComponent "{55C2E66AD4EF2CA6}" {\n   Wheels {\n'
 for slot,graph in sourceManifest['graphs'].items():child+=f'    VehicleWheelSound {slot} {{ Filename "{graph}" }}\n'
 child+='   }\n  }\n }\n}\n';dest.write_text(child,newline='\n');Path(str(dest)+'.meta').write_text('MetaFileClass { Name "'+resource+'" }\n')
 wrapperRows.append({'skin':skin,'resource':resource,'parent':parent,'originalAudioOnlySHA256':sha(dest),'approvedArt':'PENDING_FROZEN_HANDOFF_AND_NATIVE_BUILD'})
worldGP=SINGLE/'provingGround/GepardProvingGround.gproj';text=worldGP.read_text();needle='"7E0150C026101500"';assert text.count(needle)==1;worldGP.write_text(text.replace(needle,needle+' "'+sourceManifest['projectGuid']+'"'),newline='\n')
depRows=[]
for folder in ('SpaceCore_5E389BB9F58B79A6','WCS_Armaments_629B2BA37EFFD577'):
 src=N/'dependencies'/folder;target=DEPS/folder;shutil.copytree(src,target);assert inv(src)==inv(target)
 depRows.append({'folder':folder,'files':inv(target),'mode':'Unmodified complete separately loaded dependency trio, not copied inside owned addon payload'})
save(C/'DEMO10-INITIAL-SOURCE.json',{'status':'DRAFT_SOURCE_NOT_NATIVE_ADMITTED','worktree':str(T),'root':str(ROOT),'qualifiedCoreFiles':1152,'coreFilesExact':inv(SINGLE/'gepard')==qualified,'smokeSourceExact':inv(SINGLE/'wcsSmoke')==inv(old/'wcsSmoke'),'commonAndSmokeCodeChanged':False,'integrationCandidateOrigin':str(SUP/'MANIFEST.json'),'originSHA256':sha(SUP/'MANIFEST.json'),'wrappers':wrapperRows,'dependencies':depRows,'worldBindings':'PENDING_FINAL_ART_SNAPSHOT_AND_REGISTRY_HASH_REFRESH','approvedArtScope':['V15R1','V17','V18R1'],'excludedArt':['V16','V19 or other active unreviewed proposals'],'requiredNext':['Frozen art handoff and whitelist merge preserving functional parents','Native model rebuild and material/collision/LOD/clearance evidence','Final world NEW/route bindings and exact source manifests','Compile/pack/ordinary package-only native checks'],'demo09Protected':protected})
print(json.dumps({'status':'DRAFT_ONLY_FRESH_SUCCESSOR_SOURCE_STAGED','source':str(ROOT),'coreFiles':len(inv(SINGLE/'gepard')),'smokeFiles':len(inv(SINGLE/'wcsSmoke')),'integrationFiles':len(inv(integration)),'worldFiles':len(inv(SINGLE/'provingGround')),'worldBindingsFinalized':False,'artImported':False}))

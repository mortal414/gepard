from pathlib import Path
import json,hashlib,shutil,secrets,uuid
N=Path(__file__).resolve().parent;OLD=N/'diagnostic-v3';V=N/'point-contract-v4';B=V/'binding'
R=Path(r'D:\ArmaProjects\Gepard-Consolidated-20261022');S=N.parent.parent/'task-5/stock-roll-candidate/research/diagnosis-audible-fail-01'
def load(p):return json.loads(p.read_text(encoding='utf-8-sig'))
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def inv(p):return {x.relative_to(p).as_posix():sha(x) for x in p.rglob('*') if x.is_file()}
def save(p,d):p.parent.mkdir(parents=True,exist_ok=True);p.write_text(json.dumps(d,indent=2)+'\n')
assert not V.exists();lease=load(Path(r'C:\Users\marco\Documents\Codex\2026-10-12\task\RUNTIME_SLOT.json'));assert lease['status'].startswith('released') and not lease['pid'] and not lease['userRuntimeHold']['active']
for x in load(OLD/'binding/SOURCE-CLOSURES.json'):assert inv(Path(x['root']))==load(Path(x['manifest']))
seal=load(S/'POINTINFO-PROBE-SOURCE-SEAL-v2.json');overlay=S/'pointinfo-type-probe-overlay-v2';assert inv(overlay)==seal['manifest']
helper=Path(seal['contractHelper']['path']);assert sha(helper)==seal['contractHelper']['sha256']
shutil.copytree(OLD/'native-stage',V/'native-stage');B.mkdir();common=V/'native-stage/gepard';qa=V/'native-stage/audioQA'
for rel,h in seal['manifest'].items():
 target=common/rel;assert not target.exists();target.parent.mkdir(parents=True,exist_ok=True);shutil.copyfile(overlay/rel,target);assert sha(target)==h
assert all(sha(common/rel)==h for rel,h in inv(OLD/'native-stage/gepard').items())
shutil.copyfile(helper,qa/'scripts/Game/GEP/GEP033_PointContractReadback.c');(qa/'scripts/Game/GEP/GEP033_PointContractReadback.c.meta').write_text('MetaFileClass { Name "{A0331022F0030003}scripts/Game/GEP/GEP033_PointContractReadback.c" }\n')
scenario=qa/'scripts/Game/GEP/GEP_DamageScenario683.c';text=scenario.read_text()
needle='m_AudioReadback033.Init(m_Subject,m_Case,m_Nonce);';assert text.count(needle)==1;text=text.replace(needle,needle+'\n    GEP033_PointContractReadback.Inspect(m_Subject,m_Case,m_Nonce);')
resources=[p['resource'] for p in seal['probes']]
needle=' && m_Prefab!="{24223D0679CDD612}Prefabs/Vehicles/Tracked/M2A2/M2A2_OD.et")return false;';assert text.count(needle)==1;text=text.replace(needle,needle.replace(')return false;',''.join(' && m_Prefab!="'+r+'"' for r in resources)+')return false;'))
scenario.write_text(text,newline='\n')
for name in ('Compile.ps1','Scene.ps1'):
 src=(OLD/'binding/tools'/name).read_text().replace(str(OLD),str(V))
 if name=='Scene.ps1':
  src=src.replace("@('audio-stock','audio-candidate','audio-wcs-reference')","@('audio-stock','audio-candidate','audio-wcs-reference','audio-point-control','audio-point-track')")
  needle="'audio-wcs-reference'='{24223D0679CDD612}Prefabs/Vehicles/Tracked/M2A2/M2A2_OD.et'}";assert src.count(needle)==1
  src=src.replace(needle,needle[:-1]+";'audio-point-control'='"+resources[0]+"';'audio-point-track'='"+resources[1]+"'}")
 (B/'tools').mkdir(exist_ok=True);(B/'tools'/name).write_text(src,newline='\n')
closures=[]
for x in load(OLD/'binding/SOURCE-CLOSURES.json'):
 root=Path(str(x['root']).replace(str(OLD),str(V)));mp=B/Path(x['manifest']).name;save(mp,inv(root));closures.append({'root':str(root),'manifest':str(mp)})
save(B/'SOURCE-CLOSURES.json',closures);save(B/'SOURCE-BEFORE-COMPILE.json',{x['root']:load(Path(x['manifest'])) for x in closures})
template=load(OLD/'binding/commands/GEPARD_CURRENT.json');originaljob=load(OLD/'binding/jobs/GEPARD_CURRENT.json')
cases=[('CURRENT_CONTRACT','audio-stock',originaljob['prefab']),('POINTINFO_STOCK_CONTROL','audio-point-control',resources[0]),('POINTINFO_TRACK_PROBE','audio-point-track',resources[1])]
for label,vehicle,prefab in cases:
 job={**originaljob,'runId':str(uuid.uuid4()),'nonce':secrets.token_hex(32),'vehicleId':vehicle,'prefab':prefab};jf=B/'jobs'/(label+'.json');save(jf,job)
 cmd=[v.replace(str(OLD),str(V)) for v in template['command']];run=R/'native-runs'/('audio-point-v4-'+label.lower()+'-01');assert not run.exists()
 cmd[cmd.index('-RunRoot')+1]=str(run);cmd[cmd.index('-NativeJob')+1]=str(jf);cmd[cmd.index('-NativeJobSHA256')+1]=sha(jf)
 save(B/'commands'/(label+'.json'),{'command':cmd,'nativeRun':str(run),'wrapperSHA256':sha(B/'tools/Scene.ps1')})
(V/'adopt.py').write_text((OLD/'adopt.py').read_text().replace('compile-audio-diagnostic-v3-01','compile-audio-point-v4-01'))
save(V/'PREPARATION.json',{'status':'SOURCE_ONLY_NATIVE_CONTRACT_FIRST','supplierSeal':seal,'preservedOriginal1158Files':True,'onlyCommonDelta':'4additivechildprefab/meta files; only emitter class in child overrides','helper':'Read registered type/declared base, no instantiation or LoadContainer','sourceCourseSHA256':sha(qa/'scripts/Game/GEP/GEP_CoreWheeledCourse20261022.c'),'rules':'Run CURRENT_CONTRACT first; if registered SoundPointInfo inherits declared PointInfo, do not claim emitter name is cause. Sound remains HUMAN_FAIL until audible verification.'})
print(json.dumps({'sourceFiles':sum(len(load(Path(x['manifest']))) for x in closures),'status':'ISOLATED_SOURCE_ONLY'}))

from pathlib import Path
import json,hashlib,shutil,secrets,uuid,difflib
N=Path(__file__).resolve().parent;OLD=N/'diagnostic-v2';V=N/'diagnostic-v3'
R=Path(r'D:\ArmaProjects\Gepard-Consolidated-20261022')
S=N.parent.parent/'task-5/stock-roll-candidate'
def load(p):return json.loads(p.read_text(encoding='utf-8-sig'))
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def inv(p):return {x.relative_to(p).as_posix():sha(x) for x in p.rglob('*') if x.is_file()}
def save(p,d):p.parent.mkdir(parents=True,exist_ok=True);p.write_text(json.dumps(d,indent=2)+'\n',newline='\n')
assert not V.exists();lease=load(Path(r'C:\Users\marco\Documents\Codex\2026-10-12\task\RUNTIME_SLOT.json'))
assert lease['status'].startswith('released') and not lease['pid'] and not lease['userRuntimeHold']['active']
for x in load(OLD/'binding/SOURCE-CLOSURES.json'):assert inv(Path(x['root']))==load(Path(x['manifest']))
shutil.copytree(OLD/'native-stage',V/'native-stage');B=V/'binding';B.mkdir()
qa=V/'native-stage/audioQA';course=qa/'scripts/Game/GEP/GEP_CoreWheeledCourse20261022.c';before=course.read_text()
start=before.index('  else if(t<24.2)');end=before.index('  Actions(Thrust',start)
after=before[:start]+'''  else if(t<21.3)
  {
   Phase="ordinary_unanimated_stop";
   if(!PublicStopIssued){PublicStopIssued=true;Controller.StopEngine(false);PrintFormat("GEP_CORE_WHEEL_PUBLIC_STOP case=%1 nonce=%2 animated=0 force=0 physicalR=NOT_TESTED",CaseId,Nonce);}
  }
  else if(t<28.2)Phase="public_stop_observe";
  else {Done("wheeled_course_samples_collected_review_required",true);return;}
'''+before[end:]
course.write_text(after,newline='\n');(V/'COURSE-DELTA.diff').write_text(''.join(difflib.unified_diff(before.splitlines(True),after.splitlines(True))))
helper=S/'research/diagnosis-audible-fail-01/cadence-successor/GEP033_AudioReadback.c'
receipt=load(helper.parent/'SOURCE-SEAL.json');shutil.copyfile(helper,qa/'scripts/Game/GEP/GEP033_AudioReadback.c')
save(V/'HELPER-PROVENANCE.json',{'source':str(helper),'sha256':sha(helper),'supplierSeal':receipt,'originalPreserved':sha(OLD/'native-stage/audioQA/scripts/Game/GEP/GEP033_AudioReadback.c')})
for name in ('Compile.ps1','Scene.ps1'):
 src=(OLD/'binding/tools'/name).read_text().replace(str(OLD),str(V));(B/'tools').mkdir(exist_ok=True);(B/'tools'/name).write_text(src,newline='\n')
closures=[]
for x in load(OLD/'binding/SOURCE-CLOSURES.json'):
 root=Path(str(x['root']).replace(str(OLD),str(V)));mp=B/Path(x['manifest']).name;save(mp,inv(root));closures.append({'root':str(root),'manifest':str(mp)})
save(B/'SOURCE-CLOSURES.json',closures);save(B/'SOURCE-BEFORE-COMPILE.json',{x['root']:load(Path(x['manifest'])) for x in closures})
for cf in (OLD/'binding/commands').glob('*.json'):
 cfg=load(cf);label=cf.stem;job=load(OLD/'binding/jobs'/(label+'.json'));job.update(runId=str(uuid.uuid4()),nonce=secrets.token_hex(32))
 jf=B/'jobs'/(label+'.json');save(jf,job);cmd=[v.replace(str(OLD),str(V)) for v in cfg['command']]
 run=R/'native-runs'/('audio-diagnostic-v3-'+label.lower()+'-01');assert not run.exists()
 cmd[cmd.index('-RunRoot')+1]=str(run);cmd[cmd.index('-NativeJobSHA256')+1]=sha(jf)
 save(B/'commands'/cf.name,{'command':cmd,'nativeRun':str(run),'wrapperSHA256':sha(B/'tools/Scene.ps1')})
adopt=(OLD/'adopt.py').read_text().replace('compile-audio-diagnostic-v2-01','compile-audio-diagnostic-v3-01');(V/'adopt.py').write_text(adopt)
save(V/'PREPARATION.json',{'status':'SOURCE_ONLY_COMPILE_REQUIRED','course':'4s forward,6s braking,3s rest,ordinary engine-off,7s rest; no reverse or steering','commonUnchanged':inv(V/'native-stage/gepard')==inv(OLD/'native-stage/gepard'),'helperSHA256':sha(helper),'originalV2EvidencePreserved':str(OLD),'feature':'HUMAN_LISTENING_FAILED_NO_SOUND_FIX_AUTHORED'})
assert inv(V/'native-stage/gepard')==inv(OLD/'native-stage/gepard')
print(json.dumps(load(V/'PREPARATION.json')))

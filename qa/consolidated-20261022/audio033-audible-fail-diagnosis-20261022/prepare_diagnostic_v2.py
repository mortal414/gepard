from pathlib import Path
import json,hashlib,shutil,secrets,uuid,difflib
N=Path(__file__).resolve().parent
V=N/'diagnostic-v2'; OLDSTAGE=N/'native-stage'; OLDB=N/'binding'
STAGE=V/'native-stage';B=V/'binding'
R=Path(r'D:\ArmaProjects\Gepard-Consolidated-20261022')
S=N.parent.parent/'task-5/stock-roll-candidate'
def load(p):return json.loads(p.read_text(encoding='utf-8-sig'))
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def inv(p):return {x.relative_to(p).as_posix():sha(x) for x in p.rglob('*') if x.is_file()}
def save(p,d):p.parent.mkdir(parents=True,exist_ok=True);p.write_text(json.dumps(d,indent=2)+'\n',newline='\n')
assert not V.exists()
lease=load(Path(r'C:\Users\marco\Documents\Codex\2026-10-12\task\RUNTIME_SLOT.json'))
assert lease['status'].startswith('released') and not lease['pid'] and not lease['userRuntimeHold']['active']
for x in load(OLDB/'SOURCE-CLOSURES.json'):assert inv(Path(x['root']))==load(Path(x['manifest']))
V.mkdir();STAGE.mkdir();B.mkdir()
for name in ('gepard','audioQA'):shutil.copytree(OLDSTAGE/name,STAGE/name)
qa=STAGE/'audioQA';course=qa/'scripts/Game/GEP/GEP_CoreWheeledCourse20261022.c'
original=course.read_text();assert sha(course)=='8bf0ab93ca244513fd485413bf448a85e4d069ac65316e6355920ab6365e4b1b'
# QA action timing only: keep the qualified corridor, native controls and every
# safety guard; stop the faster reference before reaching its sloped end.
start=original.index('  else if(t<20.2)');end=original.index('  Actions(Thrust',start)
timing='''  else if(t<12.2){Phase="forward_default";Thrust=1;}
  else if(t<18.2){Phase="service_brake";if(Subject.GetPhysics().GetVelocity().Length()>0.15)Brake=1;}
  else if(t<21.2)Phase="coast";
  else if(t<24.2){Phase="reverse";Brake=1;}
  else if(t<30.2){Phase="final_service_brake";if(Subject.GetPhysics().GetVelocity().Length()>0.15)Brake=1;}
  else if(t<30.4){Phase="native_stop_click";EngineStop=1;}
  else if(t<33.2)Phase="native_stop_observe";
  else if(t<35.2){Phase="native_restart";EngineStart=1;}
  else if(t<38.2)Phase="native_restart_observe";
  else if(t<38.3)
  {
   Phase="ordinary_public_restart";
   if(!PublicRestartIssued){PublicRestartIssued=true;bool restartAccepted=Controller.StartEngine();PrintFormat("GEP_CORE_WHEEL_PUBLIC_RESTART case=%1 nonce=%2 accepted=%3 physicalR=NOT_TESTED",CaseId,Nonce,restartAccepted);}
  }
  else if(t<42.2)Phase="public_restart_observe";
  else if(t<42.4)
  {
   Phase="ordinary_public_stop_animated";
   if(!PublicDefaultStopIssued){PublicDefaultStopIssued=true;Controller.StopEngine();PrintFormat("GEP_CORE_WHEEL_PUBLIC_STOP case=%1 nonce=%2 animated=1 force=0 physicalR=NOT_TESTED",CaseId,Nonce);}
  }
  else if(t<45.2)Phase="native_stop_second_observe";
  else if(t<45.3)
  {
   Phase="ordinary_unanimated_stop";
   if(!PublicStopIssued && Controller.IsEngineOn()){PublicStopIssued=true;Controller.StopEngine(false);PrintFormat("GEP_CORE_WHEEL_PUBLIC_STOP case=%1 nonce=%2 animated=0 force=0 physicalR=NOT_TESTED",CaseId,Nonce);}
  }
  else if(t<48.2)Phase="public_stop_observe";
  else {Done("wheeled_course_samples_collected_review_required",true);return;}
'''
changed=original[:start]+timing+original[end:];course.write_text(changed,newline='\n')
(V/'COURSE-ONLY.diff').write_text(''.join(difflib.unified_diff(original.splitlines(True),changed.splitlines(True),fromfile='original-accepted-course',tofile='short-straight-audio-QA-v2')))
helper=S/'research/diagnosis-audible-fail-01/GEP033_AudioReadback.c'
assert sha(helper)=='2d81cb4a94be0b4ab216c0d689efee33963bba787329b57627009ddccfd07260'
shutil.copyfile(helper,qa/'scripts/Game/GEP/GEP033_AudioReadback.c')
(qa/'scripts/Game/GEP/GEP033_AudioReadback.c.meta').write_text('MetaFileClass { Name "{A0331022F0020001}scripts/Game/GEP/GEP033_AudioReadback.c" }\n')
scenario=qa/'scripts/Game/GEP/GEP_DamageScenario683.c';text=scenario.read_text();old=text
needle=' ref GEP_CoreWheeledCourse20261022 m_WheelCourseQA;';assert text.count(needle)==1;text=text.replace(needle,needle+'\n ref GEP033_AudioReadback m_AudioReadback033;')
needle='   m_WheelCourseQA.AfterPhysics();';assert text.count(needle)==1;text=text.replace(needle,needle+'\n   if(m_AudioReadback033)m_AudioReadback033.Tick(m_WheelCourseQA.Sample,m_WheelCourseQA.Phase);')
needle='    m_Stage=20;m_StageTime=m_Time;return;';assert text.count(needle)==1;text=text.replace(needle,'    m_AudioReadback033=new GEP033_AudioReadback();m_AudioReadback033.Init(m_Subject,m_Case,m_Nonce);\n'+needle)
scenario.write_text(text,newline='\n')
(V/'SCENARIO-READBACK-ONLY.diff').write_text(''.join(difflib.unified_diff(old.splitlines(True),text.splitlines(True))))
for name in ('Compile.ps1','Scene.ps1'):
 src=(OLDB/'tools'/name).read_text(encoding='utf-8-sig').replace(str(OLDSTAGE),str(STAGE)).replace(str(OLDB),str(B))
 (B/'tools').mkdir(exist_ok=True);(B/'tools'/name).write_text(src,newline='\n')
closures=[]
for x in load(OLDB/'SOURCE-CLOSURES.json'):
 root=Path(str(x['root']).replace(str(OLDSTAGE),str(STAGE)));mp=B/Path(x['manifest']).name
 save(mp,inv(root));closures.append({'root':str(root),'manifest':str(mp)})
save(B/'SOURCE-CLOSURES.json',closures);save(B/'SOURCE-BEFORE-COMPILE.json',{x['root']:load(Path(x['manifest'])) for x in closures})
for cf in (OLDB/'commands').glob('*.json'):
 cfg=load(cf);label=cf.stem;job=load(OLDB/'jobs'/(label+'.json'));job.update(runId=str(uuid.uuid4()),nonce=secrets.token_hex(32))
 jf=B/'jobs'/(label+'.json');save(jf,job)
 cmd=[v.replace(str(OLDSTAGE),str(STAGE)).replace(str(OLDB),str(B)) for v in cfg['command']]
 run=R/'native-runs'/('audio-diagnostic-v2-'+label.lower()+'-01');assert not run.exists()
 cmd[cmd.index('-RunRoot')+1]=str(run);cmd[cmd.index('-NativeJobSHA256')+1]=sha(jf)
 save(B/'commands'/cf.name,{'command':cmd,'nativeRun':str(run),'wrapperSHA256':sha(B/'tools/Scene.ps1')})
save(V/'PREPARATION.json',{'status':'SOURCE_ONLY_COMPILE_REQUIRED','changes':['QA shorter straight action schedule shared by all vehicles','original supplier read-only runtime helper and calls'],'commonUnchanged':inv(STAGE/'gepard')==inv(OLDSTAGE/'gepard'),'dependenciesSharedImmutable':str(N/'dependencies'),'oldCourseSHA256':sha(OLDSTAGE/'audioQA/scripts/Game/GEP/GEP_CoreWheeledCourse20261022.c'),'courseSHA256':sha(course),'readbackSHA256':sha(helper),'audioFeature':'HUMAN_LISTENING_FAILED_NO_SOUND_FIX_AUTHORED','sourceFiles':sum(len(load(Path(x['manifest']))) for x in closures)})
assert inv(STAGE/'gepard')==inv(OLDSTAGE/'gepard')
print(json.dumps(load(V/'PREPARATION.json')))

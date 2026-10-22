"""Seal selected C-only source/planning evidence; no runtime control."""
from pathlib import Path
from evidence import read_json,save,sha
from adapter import check_sources
ROOT=Path(__file__).resolve().parent
files=['README.md','prepare.py','adapter.py','evidence.py','selftest.py','plan_job.py','seal.py','tools/Scene.ps1','tools/Scene-proving-ground.patch','GEPARD_SOURCE_MANIFEST.json','QA_SOURCE_MANIFEST.json','SOURCE-CLOSURES.json','MARKER-CONTRACT.json','COMPILE-ADOPTION-PENDING.json','WRAPPER-ADAPTATION.json','gpg-startup-job.json','bindings.proposed.json','SOURCE-RECEIPT.json','PLAN.json','PLAN-RECEIPT.json','SELFTEST-RESULT.json','WRAPPER-SYNTAX.json']
job=read_json(ROOT/'gpg-startup-job.json');p=job['cases'][0]['parameters'];closures=check_sources(p)
assert sum(c['files'] for c in closures)==1122
assert read_json(ROOT/'SELFTEST-RESULT.json')['tests']==75
assert read_json(ROOT/'PLAN-RECEIPT.json')['jobSHA256']==sha(ROOT/'gpg-startup-job.json')
assert read_json(ROOT/'PLAN-RECEIPT.json')['planSHA256']==sha(ROOT/'PLAN.json')
assert read_json(ROOT/'COMPILE-ADOPTION-PENDING.json')['status']=='NOT_TESTED'
assert all(sha(Path(r['path']))==r['sha256'] for r in job['requiredFiles'])
manifest={'status':'SOURCE_ONLY_READY_NATIVE_BLOCKED_BY_USER_HOLD_AND_PENDING_QUALIFICATION','scope':'ONE_GPG_STARTUP_AND_SURVEY_TELEMETRY_CONSISTENCY_CASE','selectedFiles':len(files),'files':[{'relative':name,'sha256':sha(ROOT/name),'bytes':(ROOT/name).stat().st_size} for name in files],'jobSHA256':sha(ROOT/'gpg-startup-job.json'),'closureDescriptorSHA256':sha(ROOT/'SOURCE-CLOSURES.json'),'closureFiles':1122,'offlineTests':75,'authoritativePlan':'PASS_STATIC_ONLY','nativeStageAdopted':True,'nativeCompile':'NOT_TESTED','nativeRuntime':'NOT_TESTED','stationGameplay':'NOT_TESTED','newArt':'UNBOUND','optionalSmokeAudio':'EXCLUDED','userHold':'UNCHANGED','writes':'NEW_C_PACKAGE_ONLY','nativeInvoked':False,'dWrites':False,'liveRegistryMutated':False,'runtimeLeaseMutated':False}
save(ROOT/'DELIVERABLE-MANIFEST.json',manifest)
print({'manifest':str(ROOT/'DELIVERABLE-MANIFEST.json'),'sha256':sha(ROOT/'DELIVERABLE-MANIFEST.json'),'jobSHA256':manifest['jobSHA256'],'closureDescriptorSHA256':manifest['closureDescriptorSHA256'],'files':len(files)})

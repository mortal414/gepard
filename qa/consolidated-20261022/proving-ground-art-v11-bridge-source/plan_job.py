"""Authoritative static plan and source pins only. Never call run()."""
from pathlib import Path
import importlib.util,json
from adapter import check_sources
from evidence import save,sha
ROOT=Path(__file__).resolve().parent
harness=Path(r'C:\Users\marco\Desktop\reforger-agent-harness\tools\workflow.py')
spec=importlib.util.spec_from_file_location('authoritative_workflow',harness);module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module)
job=ROOT/'gpg-startup-job.json';planned=module.plan(job);closures=check_sources(planned['cases'][0]['parameters'])
save(ROOT/'PLAN.json',planned)
save(ROOT/'PLAN-RECEIPT.json',{'status':'STATIC_AUTHORITATIVE_PLAN_AND_COMPLETE_SOURCE_PINS_PASS_NATIVE_NOT_TESTED','jobSHA256':sha(job),'planSHA256':sha(ROOT/'PLAN.json'),'coordinatorSHA256':sha(harness),'projectGuid':planned['projectGuid'],'dependencies':list(planned['dependencies']),'completeClosures':closures,'registryMutated':False,'nativeStarted':False,'adapterMainInvoked':False,'wrapperInvoked':False,'userHold':'UNCHANGED; explicit user resume required','nativeCompile':'NOT_TESTED','nativeRuntime':'NOT_TESTED','newArt':'UNBOUND','optionalSmokeAudio':'EXCLUDED'})
print(json.dumps({'status':'STATIC_PLAN_PASS_NATIVE_NOT_TESTED','sourceFiles':sum(r['files'] for r in closures),'nativeInvoked':False}))

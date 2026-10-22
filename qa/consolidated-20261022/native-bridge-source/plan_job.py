"""Invoke only authoritative static plan(), never workflow.run()."""
from pathlib import Path
import importlib.util,json
from adapter import check_sources
from evidence import save,sha
root=Path(__file__).resolve().parent;harness=Path(r'C:\Users\marco\Desktop\reforger-agent-harness\tools\workflow.py')
spec=importlib.util.spec_from_file_location('authoritative_workflow',harness);module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module)
job=root/'hud-startup-job.json';planned=module.plan(job);closures=check_sources(planned['cases'][0]['parameters'])
for name in ['PLAN.json','PLAN-RECEIPT.json']:
 p=root/name
 if p.exists():
  history=root/'plan-history';history.mkdir(exist_ok=True);dest=history/(p.stem+'-'+sha(p)[:16]+'.json')
  if not dest.exists():p.rename(dest)
  else:raise ValueError('Identical existing plan receipt; preserve and inspect instead of overwrite')
save(root/'PLAN.json',planned)
save(root/'PLAN-RECEIPT.json',{'status':'STATIC_PLAN_AND_COMPLETE_SOURCE_PINS_PASS_NATIVE_NOT_TESTED','jobSHA256':sha(job),'planSHA256':sha(root/'PLAN.json'),'coordinatorSHA256':sha(harness),'projectGuid':planned['projectGuid'],'dependencies':list(planned['dependencies']),'completeClosures':closures,'registryMutated':False,'nativeStarted':False,'adapterMainInvoked':False,'wrapperInvoked':False,'userHold':'UNCHANGED; explicit user resume still required','nativeCompile':'NOT_TESTED','nativeRuntime':'NOT_TESTED'})
print(json.dumps({'status':'STATIC_PLAN_AND_COMPLETE_SOURCE_PINS_PASS_NATIVE_NOT_TESTED','sourceFiles':sum(r['files'] for r in closures),'nativeStarted':False}))

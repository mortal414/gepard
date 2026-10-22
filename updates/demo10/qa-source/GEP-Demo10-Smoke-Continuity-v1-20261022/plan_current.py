"""Root-only pure harness plan after actual adoption; never starts native."""
from pathlib import Path
import argparse,json,subprocess,sys
from evidence import read_json,save,sha
from adapter import check_sources,compile_gate,command_for
from verify_source import verify
from prepare_current import CASES,BASE
def main():
 ap=argparse.ArgumentParser();ap.add_argument('--skin',choices=CASES,required=True);a=ap.parse_args()
 root=Path(__file__).resolve().parent;out=root/'adopted';verify();caseid,world,resource=CASES[a.skin]
 jobpath=out/(caseid+'-job.json');job=read_json(jobpath);case=job['cases'][0];p=case['parameters']
 assert len(job['cases'])==1 and case['id']==caseid and job['world']==world and p['expectedVehiclePrefab']==resource
 closures=check_sources(p);compile_gate({'project':job['project']},p,closures)
 assert all(sha(row['path'])==row['sha256'] for row in job['requiredFiles'])
 argv=command_for({'template':'startup','caseId':caseid,'world':world,'project':job['project']},p,BASE/'native-runs'/(caseid+'-01')/caseid/'native-run')
 command=[sys.executable,'-X','utf8','-B',r'C:\Users\marco\Desktop\reforger-agent-harness\tools\workflow.py','plan','--job',str(jobpath)]
 planned=subprocess.run(command,capture_output=True);assert planned.returncode==0,planned.stderr.decode(errors='replace')
 plan=json.loads(planned.stdout);assert plan['jobSha256']==sha(jobpath)
 save(out/(a.skin+'-PLAN.json'),plan);save(out/(a.skin+'-RUNTIME-ARGV.json'),argv)
 save(out/(a.skin+'-BINDING-SEAL.json'),{'status':'ACTUAL_CURRENT_V11_COMPILE_AND_READ_ONLY_PLAN_PASS_RUNTIME_NOT_TESTED','jobSHA256':sha(jobpath),'planSHA256':sha(out/(a.skin+'-PLAN.json')),'sourceManifestSHA256':sha(root/'SOURCE-MANIFEST.json'),'closurePins':closures,'compileAdoptionSHA256':sha(out/'COMPILE-ADOPTION.json'),'nativeStarted':False,'adapterMainInvoked':False,'runtimeWrapperInvoked':False})
 print(json.dumps({'job':str(jobpath),'nativeInvoked':False,'argv':argv}))
if __name__=='__main__':main()

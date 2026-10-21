from pathlib import Path
import hashlib,json,argparse,subprocess
root=Path(r'D:\ArmaProjects\Gepard-EveningSprint-20261021')
ap=argparse.ArgumentParser();ap.add_argument('action',choices=['repair-manifest','summarize','adopt','gate-audio']);ap.add_argument('--stage');ap.add_argument('--run');a=ap.parse_args()
def write(p,obj): p.write_bytes((json.dumps(obj,indent=2)+'\n').encode())
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
if a.action=='repair-manifest':
 q=root/'diagnosis/qa/evening-smoke-lifecycle-20261021';p=q/'SOURCE_MANIFEST.json'
 old=json.loads(p.read_bytes());rel='diagnostic-overlay/scripts/Game/GEP015Probe/GEP_OneRoundProbe015.c'
 record={'reason':'Native compile rejected GrenadeMoveComponent.GetOwner; lookup entity recorded explicitly instead','oldSHA256':old[rel],'newSHA256':sha(q/rel),'nativeFailedRun':'followup-base-compile-01','nativeRepairedRun':'followup-base-compile-02'}
 write(q/'NATIVE-COMPILE-REPAIR.json',record);old[rel]=record['newSHA256'];write(p,old)
elif a.action=='gate-audio':
 p=root/'audio-resolver-01/tools/Compile.ps1';text=p.read_text(encoding='utf-8-sig')
 assert '[switch]$Execute' not in text
 text=text.replace('[string]$RunRoot)','[string]$RunRoot,[switch]$Execute)',1)
 sentinel='if(Test-Path -LiteralPath $RunRoot)';assert sentinel in text
 gate="if(!$Execute){@{status='NOT_TESTED';dryRun=$true;nativeStarted=$false;leaseMutated=$false}|ConvertTo-Json;exit 0}\n"
 p.write_text(text.replace(sentinel,gate+sentinel,1),encoding='utf-8',newline='\n')
elif a.action=='adopt':
 script=root/'diagnosis/qa/evening-20261021/prepare_native.py'
 subprocess.run([r'C:\Users\marco\AppData\Local\Programs\Python\Python311\python.exe',str(script),'--repo',str(root/'diagnosis'),'--output',str(root/a.stage),'--prior-qa',str(root),'--gm-tools',str(root),'--harness',r'C:\Users\marco\Desktop\reforger-agent-harness','--python',r'C:\Users\marco\AppData\Local\Programs\Python\Python311\python.exe','--refresh-manifest','--adopt-compile',str(root/'native-runs'/a.run)],check=True)
else:
 for name in ['smoke-stock-drop-01','smoke-lifecycle-control-01','smoke-lifecycle-enabled-01','smoke-lifecycle-upward-01']:
  run=root/'native-runs'/name
  if not run.exists():continue
  files=list(run.glob('profile/**/GEP015-auto-result.json'));traces=[]
  for f in run.glob('profile/**/GEP015-life-*.json'):
   try:traces.append(json.loads(f.read_bytes()))
   except Exception:pass
  output={'run':name,'exit':json.loads((run/'exit.json').read_bytes()) if (run/'exit.json').exists() else 'PENDING','result':json.loads(files[0].read_bytes()) if files else None,'traces':len(traces)}
  print(json.dumps(output))

from pathlib import Path
import subprocess,shutil,json,hashlib
r=Path(r'D:\ArmaProjects\Gepard-EveningSprint-20261021');repo=r/'diagnosis';out=r/'audio-resolver-02'
py=r'C:\Users\marco\AppData\Local\Programs\Python\Python311\python.exe'
subprocess.run([py,str(repo/'qa/evening-20261021/prepare_native.py'),'--repo',str(repo),'--output',str(out),'--prior-qa',r'D:\ArmaProjects\Gepard-DriveHud-Update-20261020\updates\release-candidate-101-r2-20261021\qa','--gm-tools',r'D:\ArmaProjects\Gepard-GM-Hotfix-1.0.2-20261021\tools','--harness',r'C:\Users\marco\Desktop\reforger-agent-harness','--python',py],check=True)
src=repo/'qa/evening-audio-20261021/signal-probe/scripts/WorkbenchGame/GEP033'
shutil.copytree(src,out/'native-stage/gepard/scripts/WorkbenchGame/GEP033')
text=(r/'audio-resolver-01/tools/ResolveAudio.ps1').read_text(encoding='utf-8-sig')
recovery=Path(__file__).with_name('Recover-OwnedProbe.ps1').read_text()
extra="Add-Type @'"+recovery.split("Add-Type @'",1)[1].split("'@",1)[0]+"'@\n"
text=text.replace('function AssertIdentity {',extra+'function AssertIdentity {',1)
old='[void]$managed.CloseMainWindow();'
assert old in text
text=text.replace(old,'[void][GEPProbeNormalClose]::Close([uint32]$owner.pid);',1)
(out/'tools/ResolveAudio.ps1').write_text(text,encoding='utf-8',newline='\n')
path=out/'tools/Compile.ps1';text=path.read_text(encoding='utf-8-sig').replace('[string]$RunRoot)','[string]$RunRoot,[switch]$Execute)',1)
gate="if(!$Execute){@{status='NOT_TESTED';dryRun=$true;nativeStarted=$false;leaseMutated=$false}|ConvertTo-Json;exit 0}\n"
text=text.replace('if(Test-Path -LiteralPath $RunRoot)',gate+'if(Test-Path -LiteralPath $RunRoot)',1);path.write_text(text,encoding='utf-8',newline='\n')
(out/'NORMAL-CLOSE-PROVENANCE.json').write_text(json.dumps({'priorFailedRun':'native-runs/audio-resource-resolver-01','normalRecovery':'native-runs/audio-resource-resolver-01/RECOVERY.json','method':'Normal WM_CLOSE to exactly one Qt project window owned by verified child PID; no forced termination; original identity/lease checks unchanged','nativeLaunched':False},indent=2)+'\n',encoding='utf-8')
print('SOURCE_ONLY_RESOLVER_V2_PREPARED')

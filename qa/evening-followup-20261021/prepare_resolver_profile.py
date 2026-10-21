from pathlib import Path
import json,subprocess,shutil,hashlib
r=Path(r'D:\ArmaProjects\Gepard-EveningSprint-20261021');out=r/'audio-resolver-profile-01';repo=r/'diagnosis';py=r'C:\Users\marco\AppData\Local\Programs\Python\Python311\python.exe'
subprocess.run([py,str(repo/'qa/evening-20261021/prepare_native.py'),'--repo',str(repo),'--output',str(out),'--prior-qa',r'D:\ArmaProjects\Gepard-DriveHud-Update-20261020\updates\release-candidate-101-r2-20261021\qa','--gm-tools',r'D:\ArmaProjects\Gepard-GM-Hotfix-1.0.2-20261021\tools','--harness',r'C:\Users\marco\Desktop\reforger-agent-harness','--python',py],check=True)
src=repo/'qa/evening-audio-20261021/signal-probe/scripts/WorkbenchGame/GEP033';shutil.copytree(src,out/'native-stage/gepard/scripts/WorkbenchGame/GEP033')
code='\n'.join(p.read_text() for p in src.glob('*.c'));assert 'System.GetCLIParam(' not in code and 'GetCmdLine(' not in code
wrapper=(r/'audio-resolver-02/tools/ResolveAudio.ps1').read_text(encoding='utf-8-sig')
old="$nativeArgs+=@('-wbSilent','-gep033Output',(Join-Path $RunRoot 'GEP033-RESOLVER.json'))";assert old in wrapper
wrapper=wrapper.replace(old,"$nativeArgs+='-wbSilent'",1).replace("durableOutput=(Join-Path $RunRoot 'GEP033-RESOLVER.json')","durableOutput='Fresh per-run profile; locate unique GEP033-RESOLVER.json recursively after native exit'")
(out/'tools/ResolveAudio.ps1').write_text(wrapper,encoding='utf-8',newline='\n')
p=out/'tools/Compile.ps1';text=p.read_text(encoding='utf-8-sig').replace('[string]$RunRoot)','[string]$RunRoot,[switch]$Execute)',1)
text=text.replace('if(Test-Path -LiteralPath $RunRoot)',"if(!$Execute){@{status='NOT_TESTED';dryRun=$true;nativeStarted=$false;leaseMutated=$false}|ConvertTo-Json;exit 0}\nif(Test-Path -LiteralPath $RunRoot)",1);p.write_text(text,encoding='utf-8',newline='\n')
addon=out/'native-stage/gepard';pins={p.relative_to(addon).as_posix():hashlib.sha256(p.read_bytes()).hexdigest() for p in sorted(addon.rglob('*')) if p.is_file()}
(out/'checks/SOURCE_MANIFEST.json').write_bytes((json.dumps(pins,indent=2)+'\n').encode())
report={'status':'SOURCE_READY_NATIVE_BLOCKED','nativeLaunched':False,'leaseTouched':False,'blockedBy':'Owned Workbench PID56448 pending manual dismissal of authorization dialog; no retries/permission automation','project':str(addon/'Gepard.gproj'),'plugin':'GEP033_ResolveStockAudioPlugin','profileOutput':'$profile:GEP033-RESOLVER.json; no CLI introspection; fresh profile collision refusal','nativeCompile':'NOT_TESTED','resourceObservations':'NOT_TESTED','playback':'NOT_TESTED','files':len(pins)}
(out/'SOURCE-QUEUED.json').write_bytes((json.dumps(report,indent=2)+'\n').encode());print(json.dumps(report))

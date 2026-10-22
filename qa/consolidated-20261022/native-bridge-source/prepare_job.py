"""Source-only adapter/job preparation. Never starts a wrapper or native app."""
from pathlib import Path
import json,hashlib,difflib,re
ROOT=Path(__file__).resolve().parent
QA=Path(r'D:\ArmaProjects\Gepard-OpticApproved-20261022\qa\optic-approved-20261022')
STAGE=QA.parents[1]/'native-stage';HUD=STAGE/'hudQA';GEP=STAGE/'gepard'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def write(p,v):p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(v.encode() if isinstance(v,str) else v)
def js(p,v):write(p,json.dumps(v,indent=2)+'\n')
up=QA/'tools/Scene.ps1';raw=up.read_bytes();before=raw.decode('utf-8-sig');nl='\r\n' if '\r\n' in before else '\n';source=before
addition='''function UserRuntimeHoldRecord705($holdLease705) {
 if($holdLease705.status -match '^user_runtime_hold'){return $true}
 if($holdLease705.psobject.Properties.Name -contains 'userRuntimeHold'){
  $hold705=$holdLease705.userRuntimeHold
  if(!$hold705 -or !($hold705.psobject.Properties.Name -contains 'active')){return $true}
  if($hold705.active -isnot [bool]){return $true}
  if($hold705.active -ne $false){return $true}
 }
 return $false
}
function UserRuntimeHold705 {
 $holdLease705=Get-Content -LiteralPath $slot -Raw | ConvertFrom-Json
 return (UserRuntimeHoldRecord705 $holdLease705)
}
function CancelRequested705 {
 $cancel705=$env:REFORGER_QA_CANCEL_PATH
 return [bool]($cancel705 -and (Test-Path -LiteralPath $cancel705 -PathType Leaf))
}
'''.replace('\n',nl)
anchor='function AssertCompleteSources705 {';assert source.count(anchor)==1;source=source.replace(anchor,addition+nl+anchor,1)
anchor='AssertCompleteSources705'+nl+'if(Test-Path -LiteralPath $RunRoot)';assert source.count(anchor)==1
source=source.replace(anchor,"if(UserRuntimeHold705){throw 'User runtime hold active; no native admission'}"+nl+"if(CancelRequested705){@{status='NOT_STARTED_CANCELLED';nativeStarted=$false;leaseMutated=$false}|ConvertTo-Json;exit 2}"+nl+anchor,1)
source=source.replace("$until=[DateTime]::UtcNow.AddSeconds($Seconds);$closed='native-or-user-exit'","$until=[DateTime]::UtcNow.AddSeconds($Seconds);$closed='native-or-user-exit';$cancelled705=$false",1)
old='  if(!$KeepOpen -and [DateTime]::UtcNow -ge $until){';assert source.count(old)==1
source=source.replace(old,"  if(UserRuntimeHold705){throw 'User runtime hold active in owned loop; preserve process and lease'}"+nl+'  if(CancelRequested705){$cancelled705=$true}'+nl+'  if($cancelled705 -or (!$KeepOpen -and [DateTime]::UtcNow -ge $until)){',1)
old="[void]$managed.CloseMainWindow();$closed='owned-normal-window-close';$grace=";assert source.count(old)==1
source=source.replace(old,"if(UserRuntimeHold705){throw 'User runtime hold active before close; preserve owned process and lease'}"+nl+"   [void]$managed.CloseMainWindow();$closed='owned-normal-window-close';if($cancelled705){$closed='owned-normal-window-close-cancelled'};$grace=",1)
old="while(AssertIdentity){if([DateTime]::UtcNow -ge $grace)";assert source.count(old)==1
source=source.replace(old,"while(AssertIdentity){if(UserRuntimeHold705){throw 'User runtime hold active during close wait; preserve uncertainty'};if(CancelRequested705){$cancelled705=$true};if([DateTime]::UtcNow -ge $grace)",1)
old="Save 'exit.json' @{verifiedExited=$true;leaseReleased=$true;exitCode=$game.ExitCode;closeMethod=$closed;utc=";assert source.count(old)==1
source=source.replace(old,"Save 'exit.json' @{verifiedExited=$true;leaseReleased=$true;exitCode=$game.ExitCode;closeMethod=$closed;cancelRequested=($cancelled705 -or (CancelRequested705));utc=",1)
old='SlotTransaction {param($l)if($l.pid';assert source.count(old)==1
source=source.replace(old,"SlotTransaction {param($l)if(UserRuntimeHoldRecord705 $l){throw 'User runtime hold active at atomic claim; no native admission'};if($l.pid",1)
claim=re.search(r"return @\{owner=\$proof.owner;ownerKeyHash=.*?reservedUtc=\[DateTime\]::UtcNow.ToString\('o'\)\}",source).group(0)
source=source.replace(claim,claim.replace('return @{','$next705=@{',1)+";if($l.psobject.Properties.Name -contains 'userRuntimeHold'){$next705['userRuntimeHold']=$l.userRuntimeHold};return $next705",1)
old="SlotTransaction {param($l)AssertOwner $l;if($l.pid -ne $game.Id){throw 'Lease PID changed'};";assert source.count(old)==1
source=source.replace(old,"SlotTransaction {param($l)AssertOwner $l;if(UserRuntimeHoldRecord705 $l){throw 'User runtime hold active at release; preserve lease for coordinator'};if($l.pid -ne $game.Id){throw 'Lease PID changed'};",1)
wrapper=ROOT/'tools/Scene.ps1';write(wrapper,source)
write(ROOT/'tools/Scene-cancellation.patch',''.join(difflib.unified_diff(before.splitlines(True),source.splitlines(True),fromfile='a/Scene.ps1',tofile='b/Scene.ps1')))
proof={'status':'SOURCE_PATCH_ONLY_NATIVE_NOT_TESTED','upstream':str(up),'upstreamSHA256':sha(up),'patchedSHA256':sha(wrapper),'changes':['Read run-specific cancellation path from environment','Reject shared user runtime hold before admission AND immediately before any owned CloseMainWindow','Refuse launch when already cancelled','Check cancellation in both exact-owned runtime loops','Route cancellation through original guarded CloseMainWindow branch only while hold inactive','Retain cancellation bit in exit receipt'],'unchanged':['Atomic lease claim/attach/release','Exact PID/executable/birth/parsed argv','Complete source closure guard','Hidden default; no force termination','15-second close grace then preserve uncertainty'],'nativeCompile':'NOT_TESTED','nativeRuntime':'NOT_TESTED'}
js(ROOT/'WRAPPER-PATCH.json',proof)
pending={'schema':1,'status':'NOT_TESTED','project':str(HUD/'GEP_ApprovedHUD705_QA.gproj'),'compileRun':None,'compilerResultSHA256':None,'compilerConsoleSHA256':None,'closures':[], 'reason':'Root must compile exact frozen stage after explicit user resume, verify normal compiler exit/release, refresh complete source manifests if generated files changed, and record those actual hashes here before replanning. No compile occurred in this preparation.'}
if not (ROOT/'COMPILE-ADOPTION-PENDING.json').exists():js(ROOT/'COMPILE-ADOPTION-PENDING.json',pending)
required=[ROOT/'adapter.py',ROOT/'evidence.py',wrapper,ROOT/'COMPILE-ADOPTION-PENDING.json',QA/'SOURCE-CLOSURES.json',QA/'GEPARD_SOURCE_MANIFEST.json',QA/'QA_SOURCE_MANIFEST.json',Path(r'C:\Users\marco\.cache\codex-runtimes\codex-primary-runtime\dependencies\native\powershell\pwsh.exe'),Path(r'D:\SteamLibrary\steamapps\common\Arma Reforger\ArmaReforgerSteamDiag.exe')]
assert all(p.exists() for p in required),str([p for p in required if not p.exists()])
assertions=['fixture.ready','world.loaded','hud.scripted-ads','hud.capture-file','hud.actual-data','native.errors']
job={'schema':1,'template':'startup','project':str(HUD/'GEP_ApprovedHUD705_QA.gproj'),'world':'Worlds/GEP_ApprovedRender705.ent','sourceRoots':[str(HUD),str(GEP)],'addonRoots':[str(STAGE),r'D:\SteamLibrary\steamapps\common\Arma Reforger\addons'],'requiredFiles':[{'path':str(p),'sha256':sha(p)} for p in required],'requiredClasses':['GEP_Approved705Scene','GEP_ApprovedRenderProbe705','GEP_GunStatus'],'capabilities':['isolated-reset','owned-lifecycle','native-load'],'adapter':{'executable':r'C:\Users\marco\AppData\Local\Programs\Python\Python311\python.exe','script':str(ROOT/'adapter.py'),'args':['{script}','{request}','{result}','{cancel}']},'cases':[{'id':'hud-render-startup','parameters':{'wrapper':str(wrapper),'wrapperSHA256':sha(wrapper),'powerShell':r'C:\Users\marco\.cache\codex-runtimes\codex-primary-runtime\dependencies\native\powershell\pwsh.exe','closures':str(QA/'SOURCE-CLOSURES.json'),'closurePinSHA256':sha(QA/'SOURCE-CLOSURES.json'),'completeManifestPins':{str(p):sha(p) for p in [QA/'GEPARD_SOURCE_MANIFEST.json',QA/'QA_SOURCE_MANIFEST.json']},'compileAdoption':str(ROOT/'COMPILE-ADOPTION-PENDING.json'),'addonId':'7E0050C026102500','seconds':60,'screenWidth':1280,'screenHeight':720,'adapterDeadlineSeconds':100,'childLogLimitBytes':4194304},'assertions':assertions}],'timeoutSeconds':130,'closeGraceSeconds':35,'notes':'SOURCE ONLY under explicit user runtime hold. One existing HUD render case, not proving-ground world or optional modules. Execute blocked until user resume/native admission and actual compile adoption. Visual/glyph/physical input intentionally remain NOT_TESTED even after a qualified captured render.'}
js(ROOT/'hud-startup-job.json',job)
js(ROOT/'bindings.proposed.json',{'schema':1,'bindings':{'gepard-consolidated-hud-startup':{'job':str(ROOT/'hud-startup-job.json'),'description':'Private source-prepared approved HUD startup bridge; hold/compile/native qualification pending. No whole GEP acceptance.','outputRoot':str(ROOT/'future-evidence')}}})
print(json.dumps({'status':'SOURCE_JOB_PREPARED_NATIVE_NOT_TESTED','job':str(ROOT/'hud-startup-job.json'),'wrapperSHA256':sha(wrapper),'registryChanged':False,'nativeStarted':False}))

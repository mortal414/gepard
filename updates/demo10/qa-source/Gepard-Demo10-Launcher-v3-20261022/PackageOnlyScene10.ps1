param(
 [Parameter(Mandatory=$true)][string]$PlanFile,
 [Parameter(Mandatory=$true)][ValidatePattern('^[0-9a-f]{64}$')][string]$PlanSHA256,
 [ValidateScript({$_ -eq 0 -or ($_ -ge 30 -and $_ -le 180)})][int]$ProbeSeconds=0,
 [switch]$Interactive,
 [switch]$Execute
)
# Existing scene ownership/identity guards; default zero is untimed human exit.
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$parserSeal=Get-Content -LiteralPath (Join-Path $PSScriptRoot 'SOURCE-MANIFEST.json') -Raw|ConvertFrom-Json -AsHashtable
if((Get-FileHash -LiteralPath $PSCommandPath -Algorithm SHA256).Hash.ToLowerInvariant() -cne $parserSeal.files['PackageOnlyScene10.ps1']){throw 'Frozen parser successor differs'}

$slot='C:\Users\marco\Documents\Codex\2026-10-12\task\RUNTIME_SLOT.json'
$proof=$null;$game=$null;$attached=$false;$claimed=$false;$owner=$null;$RunRoot=$null
function Save($name,$value){[IO.File]::WriteAllText((Join-Path $RunRoot $name),($value|ConvertTo-Json -Depth 20))}

function KeyHash([string]$s){[Convert]::ToHexString([Security.Cryptography.SHA256]::HashData([Text.Encoding]::UTF8.GetBytes($s))).ToLowerInvariant()}

function Inventory {@(Get-CimInstance Win32_Process -OperationTimeoutSec 5 -ErrorAction Stop|Where-Object {$_.Name -match '^(ArmaReforger|Workbench|workbenchApp)'})}

function OpenExclusiveLeaseWriter {
 # Retry only transient Windows sharing/lock violations from this sole open.
 # JSON parsing, ownership/hold checks and lease writes remain outside this loop.
 $leaseOpenClock=[Diagnostics.Stopwatch]::StartNew()
 for($leaseOpenAttempt=0;$leaseOpenAttempt -lt 21;$leaseOpenAttempt++){
  try{return [IO.File]::Open($slot,[IO.FileMode]::Open,[IO.FileAccess]::ReadWrite,[IO.FileShare]::None)}
  catch {
   $leaseOpenError=$_.Exception
   while($leaseOpenError -is [System.Management.Automation.MethodInvocationException] -and $leaseOpenError.InnerException){$leaseOpenError=$leaseOpenError.InnerException}
   if($leaseOpenError -isnot [IO.IOException]){throw}
   $leaseOpenWin32=$leaseOpenError.HResult -band 0xffff
   if($leaseOpenWin32 -notin @(0x20,0x21) -or $leaseOpenAttempt -ge 20){throw}
   # 900ms budget leaves scheduling margin under the one-second requested cap;
   # at most20 sleeps of40ms and21 File.Open attempts, never a transaction retry.
   $leaseOpenRemaining=900-$leaseOpenClock.ElapsedMilliseconds
   if($leaseOpenRemaining -le 0){throw}
   Start-Sleep -Milliseconds ([int][Math]::Min(40,$leaseOpenRemaining))
   if($leaseOpenClock.ElapsedMilliseconds -ge 900){throw}
  }
 }
}

function SlotTransaction([scriptblock]$change){
 $stream=OpenExclusiveLeaseWriter
 try{$reader=[IO.StreamReader]::new($stream,[Text.Encoding]::UTF8,$true,1024,$true);try{$l=$reader.ReadToEnd()|ConvertFrom-Json}finally{$reader.Dispose()};$next=& $change $l;if(!$next){throw 'No lease result'};$bytes=[Text.Encoding]::UTF8.GetBytes(($next|ConvertTo-Json -Depth 20));$stream.Position=0;$stream.SetLength(0);$stream.Write($bytes,0,$bytes.Length);$stream.Flush($true)}finally{$stream.Dispose()}
}

function AssertOwner($l){if($l.owner -ne $proof.owner -or $l.leaseId -ne $proof.leaseId -or $l.activeRuntime -ne $proof.runId -or $l.ownerKeyHash -ne (KeyHash $proof.ownerKey) -or $l.workRoot -ne $RunRoot){throw 'Owner differs; preserve session'}}

function QuoteNative([string]$s){if(!$s -or $s -match '["\x00-\x1f]'){throw 'Unsafe argument'};if($s -notmatch '\s'){return $s};$tail=[regex]::Match($s,'\\+$').Length;return '"'+$s+('\'*$tail)+'"'}

function AssertIdentity {
 if(!$attached){throw 'Unattached child; preserve'}
 SlotTransaction {param($l)AssertOwner $l;if($l.pid -ne $game.Id){throw 'Lease PID differs'};return $l}
 $row=Get-CimInstance Win32_Process -Filter ('ProcessId='+$game.Id) -OperationTimeoutSec 5 -ErrorAction Stop
 if(!$row){$game.WaitForExit();return $false}
 if($row.ExecutablePath -ne $exe -or $row.CreationDate.ToUniversalTime().Ticks -ne ([DateTime]$owner.creationUtc).ToUniversalTime().Ticks){
  if($game.HasExited -and !$row.ExecutablePath -and $row.CreationDate.ToUniversalTime().Ticks -eq ([DateTime]$owner.creationUtc).ToUniversalTime().Ticks){Start-Sleep -Milliseconds 150;$row=Get-CimInstance Win32_Process -Filter ('ProcessId='+$game.Id) -OperationTimeoutSec 5 -ErrorAction Stop;if(!$row){return $false}}
  throw 'PID/executable/birth differs; preserve'
 }
 # Independent sources: never compare CIM precision to managed precision.
 $game.Refresh()
 if(!$game.HasExited){
  if($game.Id -ne $owner.pid){throw 'Managed process ID differs; preserve'}
  if($game.StartTime.ToUniversalTime().Ticks -ne ([DateTime]$owner.managedCreationUtc).ToUniversalTime().Ticks){throw 'Managed birth differs from own initial value; preserve'}
 }
 $actual=[GEPSceneArgs]::Parse($row.CommandLine)
 if($actual.Count -ne ($nativeArgs.Count+1) -or [IO.Path]::GetFullPath($actual[0]) -ne $exe){throw 'Parsed native command differs'}
 for($i=0;$i -lt $nativeArgs.Count;$i++){if($actual[$i+1] -cne $nativeArgs[$i]){throw 'Native arguments differ'}}
 return $true
}

function UserRuntimeHoldRecord705($holdLease705) {
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
Add-Type -TypeDefinition @'
using System;
using System.Runtime.InteropServices;
public static class GEPSceneArgs {
 [DllImport("shell32.dll",SetLastError=true)] static extern IntPtr CommandLineToArgvW([MarshalAs(UnmanagedType.LPWStr)] string s,out int n);
 [DllImport("kernel32.dll")] static extern IntPtr LocalFree(IntPtr p);
 public static string[] Parse(string s){int n;IntPtr p=CommandLineToArgvW(s,out n);if(p==IntPtr.Zero)throw new InvalidOperationException("Cannot parse command identity");try{var a=new string[n];for(int i=0;i<n;i++)a[i]=Marshal.PtrToStringUni(Marshal.ReadIntPtr(p,i*IntPtr.Size));return a;}finally{LocalFree(p);}}
}
'@
function AssertPackageClosure {
 if((Get-FileHash -LiteralPath $PlanFile -Algorithm SHA256).Hash.ToLowerInvariant() -cne $PlanSHA256){throw 'Manual plan changed'}
 if((Get-FileHash -LiteralPath $plan.installedClosure -Algorithm SHA256).Hash.ToLowerInvariant() -cne $plan.installedClosureSHA256){throw 'Installed closure pin changed'}
 if($plan.worldContract -cne 'C:\Users\marco\Documents\Codex\2026-10-21\task\Gepard-Demo10-PackTools-v2-20261022\PACKAGE10-WORLD-CONTRACT.json' -or (Get-FileHash -LiteralPath $plan.worldContract -Algorithm SHA256).Hash.ToLowerInvariant() -cne $plan.worldContractSHA256){throw 'Exact V2 world startup contract changed'}
 $closure=Get-Content -LiteralPath $plan.installedClosure -Raw|ConvertFrom-Json
 $pins=Get-Content -LiteralPath $plan.installedClosure -Raw|ConvertFrom-Json -AsHashtable
 $files=@(Get-ChildItem -LiteralPath $closure.root -File -Recurse)
 if($files.Count -ne 12 -or $pins.files.Count -ne 12){throw 'Exact four own installed trios required'}
 foreach($rel in $pins.files.Keys){if((Get-FileHash -LiteralPath (Join-Path $closure.root $rel) -Algorithm SHA256).Hash.ToLowerInvariant() -cne $pins.files[$rel]){throw 'Installed package bytes changed'}}
 if((Get-FileHash -LiteralPath $plan.dependencyClosure -Algorithm SHA256).Hash.ToLowerInvariant() -cne $plan.dependencyClosureSHA256){throw 'Separate WCS dependency closure differs'}
 $wcsClosure=Get-Content -LiteralPath $plan.dependencyClosure -Raw|ConvertFrom-Json -AsHashtable
 if($wcsClosure.root -cne 'D:\ArmaProjects\Gepard-Demo10-TrackArt-20261022\private-demo-source10\dependencies' -or $wcsClosure.files.Count -ne 6 -or @(Get-ChildItem -LiteralPath $wcsClosure.root -Recurse -File).Count -ne 6){throw 'Only exact separate SpaceCore and WCS trios permitted'}
 if((@(Get-ChildItem -LiteralPath $wcsClosure.root -Directory | ForEach-Object {$_.Name} | Sort-Object) -join ',') -cne 'SpaceCore_5E389BB9F58B79A6,WCS_Armaments_629B2BA37EFFD577'){throw 'Unexpected/Bradley dependency directory'}
 foreach($relativeWCS in $wcsClosure.files.Keys){if((Get-FileHash -LiteralPath (Join-Path $wcsClosure.root $relativeWCS) -Algorithm SHA256).Hash.ToLowerInvariant() -cne $wcsClosure.files[$relativeWCS]){throw 'Separately installed WCS dependency changed'}}
 foreach($p in $plan.nativeToolPins.psobject.Properties){if((Get-FileHash -LiteralPath $p.Name -Algorithm SHA256).Hash.ToLowerInvariant() -cne $p.Value){throw 'Native executable pin differs'}}
}
function ReadWorldStartup10([string]$text) {
 $wc=Get-Content -LiteralPath $plan.worldContract -Raw|ConvertFrom-Json
 $first=[regex]::Match($text,[regex]::Escape($wc.requiredReadyMarker))
 if(!$first.Success){return @{pass=$false;objects=0;boards=0;finiteInitial=$false;bindings=$false}}
 # Startup interval only: human explicit resets later do not alter initial proof.
 $startup=$text.Substring(0,$first.Index+$first.Length)
 $objects=@([regex]::Matches($startup,'GPG_OBJECT id=([^\s]+) station='))
 $boards=@([regex]::Matches($startup,'GPG_STATION id=([^\r\n]+?) board='))
 $uniqueObjects=@($objects|ForEach-Object {$_.Groups[1].Value}|Sort-Object -Unique)
 $uniqueBoards=@($boards|ForEach-Object {$_.Groups[1].Value}|Sort-Object -Unique)
 $bindings=$true
 foreach($row in $wc.newOwnedObjects){
  $pattern='GPG_OBJECT id='+[regex]::Escape($row.id)+' station=smoke-service epoch=0 prefab='+[regex]::Escape($row.prefab)+' origin=([^\r\n]+)'
  $matches=@([regex]::Matches($startup,$pattern))
  if($matches.Count -ne 1){$bindings=$false;continue}
  $coords=@([regex]::Matches($matches[0].Groups[1].Value,'-?\d+(?:\.\d+)?')|ForEach-Object {[double]::Parse($_.Value,[Globalization.CultureInfo]::InvariantCulture)})
  if($coords.Count -ne 3 -or [Math]::Abs($coords[0]-$row.pad[0]) -gt 0.05 -or [Math]::Abs($coords[2]-$row.pad[2]) -gt 0.05){$bindings=$false}
 }
 foreach($id in @('NEW-CAMO','NEW-CHARCOAL')){
  $resource=$wc.newWrappers.$id
  if(@([regex]::Matches($startup,'GPG_OBJECT id='+$id+' station=bay epoch=0 prefab='+[regex]::Escape($resource)+' origin=')).Count -ne 1){$bindings=$false}
 }
 $board=@([regex]::Matches($startup,'GPG_STATION id=smoke-service board=([^\r\n]+?) title='+[regex]::Escape($wc.newBoard.title)))
 if($board.Count -ne 1){$bindings=$false}
 else{
  $coords=@([regex]::Matches($board[0].Groups[1].Value,'-?\d+(?:\.\d+)?')|ForEach-Object {[double]::Parse($_.Value,[Globalization.CultureInfo]::InvariantCulture)})
  if($coords.Count -ne 3 -or [Math]::Abs($coords[0]-610) -gt 0.05 -or [Math]::Abs($coords[2]-2640) -gt 0.05){$bindings=$false}
 }
 $finite=@([regex]::Matches($startup,[regex]::Escape($wc.initialFiniteMarkerOnce))).Count -eq 1 -and @([regex]::Matches($startup,[regex]::Escape($wc.serviceMarkerOnce))).Count -eq 1 -and !$startup.Contains($wc.forbiddenFailurePrefix)
 $pass=$objects.Count -eq 34 -and $uniqueObjects.Count -eq 34 -and $boards.Count -eq 24 -and $uniqueBoards.Count -eq 24 -and $finite -and $bindings
 return @{pass=$pass;objects=$objects.Count;boards=$boards.Count;finiteInitial=$finite;bindings=$bindings;loggedOrigins='authored Ground descriptors, XZ checked; actualY/reach/shared queue NOT_TESTED'}
}
function ReadPackageConsole {
 $p=Join-Path $plan.logsDir 'console.log'
 if(!(Test-Path -LiteralPath $p -PathType Leaf)){return ''}
 $s=[IO.File]::Open($p,[IO.FileMode]::Open,[IO.FileAccess]::Read,[IO.FileShare]::ReadWrite)
 try{if($s.Length -gt 64MB){throw 'Console exceeds64MiB'};$r=[IO.StreamReader]::new($s);try{return $r.ReadToEnd()}finally{$r.Dispose()}}finally{$s.Dispose()}
}
if((Get-FileHash -LiteralPath $PlanFile -Algorithm SHA256).Hash.ToLowerInvariant() -cne $PlanSHA256){throw 'Manual plan pin differs'}
$plan=Get-Content -LiteralPath $PlanFile -Raw|ConvertFrom-Json
$RunRoot=$plan.runRoot;$Project=$plan.project;$exe=$plan.executable;$nativeArgs=@($plan.args)
if($plan.schema -ne 1 -or $plan.type -cne 'private-package-only-ordinary-scene' -or !$plan.keepOpen -or $plan.mode -notin @('manual','probe')){throw 'Only frozen ordinary package plan bound'}
if(($plan.mode -eq 'manual' -and $ProbeSeconds -ne 0) -or ($plan.mode -eq 'probe' -and $ProbeSeconds -ne 75)){throw 'Manual untimed/probe75 close policy differs'}
$R='D:\ArmaProjects\Gepard-Demo10-TrackArt-20261022'
$installed=Join-Path $R 'private-installed-demo-10b\addons'
$expectedExe='D:\SteamLibrary\steamapps\common\Arma Reforger\ArmaReforgerSteamDiag.exe'
if($plan.sessionId -cnotmatch '^[0-9a-f]{8}-[0-9a-f]{4}-4[0-9a-f]{3}-[89ab][0-9a-f]{3}-[0-9a-f]{12}$'){throw 'Only builder-generated UUID4 session bound'}
$expectedRun='native-runs\private-demo-10b-sessions\'+$plan.mode+'-'+$plan.sessionId
if($exe -cne $expectedExe -or $RunRoot -cne (Join-Path $R $expectedRun) -or $Project -cne (Join-Path $installed 'A76A420022000001\addon.gproj')){throw 'Exact package-only executable/run/project differs'}
$profile=Join-Path $RunRoot 'profile';$logs=Join-Path $RunRoot 'logs'
$expectedArgs=@('-profile',$profile,'-logsDir',$logs,'-addonDownloadDir',$profile,'-window','-screenWidth','1280','-screenHeight','720','-maxFPS','60','-noSplash','-forceUpdate','-addonsDir',($installed+',D:\ArmaProjects\Gepard-Demo10-TrackArt-20261022\private-demo-source10\dependencies,D:\SteamLibrary\steamapps\common\Arma Reforger\addons'),'-addons','D561AD08932E7B40,7E0150C026101500,5E51C5D8F04FDB80,A76A420022000001,5E389BB9F58B79A6,629B2BA37EFFD577','-world','Worlds/GPG_Arland.ent','-worldSystemsConfig','{C50579A2EC48E8EF}Configs/Systems/MissionSystems.conf','-noBackend')
if($nativeArgs.Count -ne $expectedArgs.Count){throw 'Package-only argv count differs'}
for($i=0;$i -lt $expectedArgs.Count;$i++){if($nativeArgs[$i] -cne $expectedArgs[$i]){throw 'Exact ordinary package argv differs'}}
AssertPackageClosure
$closure=Get-Content -LiteralPath $plan.installedClosure -Raw|ConvertFrom-Json
if($closure.packDiagnostics.operationalVerdict -cne 'PASS'){throw 'Source package operational gate differs'}
if(!$Execute){@{status='PACKAGE_ONLY_SOURCE_PLAN_PASS_NATIVE_PENDING';nativeStarted=$false;leaseMutated=$false;probeSeconds=$ProbeSeconds;defaultExit='human-natural-exit';privateQA=$false}|ConvertTo-Json;exit 0}
if(UserRuntimeHold705){throw 'User runtime hold active; no admission'}
if(CancelRequested705){throw 'Cancelled before admission'}
if(Test-Path -LiteralPath $RunRoot){throw 'Fresh manual/probe run required'}
foreach($v in Get-Volume -DriveLetter C,D){if($v.SizeRemaining -lt 2GB){throw 'Below2GiB preflight'}}
[IO.Directory]::CreateDirectory($RunRoot)|Out-Null
[IO.Directory]::CreateDirectory($profile)|Out-Null
[IO.Directory]::CreateDirectory($logs)|Out-Null
$proof=@{owner='Gepard ordinary private package scene';ownerKey=([Guid]::NewGuid().ToString('N')+[Guid]::NewGuid().ToString('N'));leaseId=[Guid]::NewGuid().ToString();runId=[Guid]::NewGuid().ToString();workRoot=$RunRoot;project=$Project}
Save 'owner-private.json' $proof
Copy-Item -LiteralPath $PlanFile -Destination (Join-Path $RunRoot 'plan.json')
SlotTransaction {param($l)
 if(UserRuntimeHoldRecord705 $l){throw 'User hold active at atomic admission'}
 if($l.pid -or $l.activeRuntime -or $l.status -notmatch '^released' -or ($l.psobject.Properties.Name -contains 'parallelCaptures' -and $l.parallelCaptures)){throw 'Lease busy; preserve'}
 if(@(Inventory).Count){throw 'Unexpected native session; preserve'}
 $n=@{owner=$proof.owner;ownerKeyHash=(KeyHash $proof.ownerKey);leaseId=$proof.leaseId;activeRuntime=$proof.runId;workRoot=$RunRoot;project=$Project;profile=$profile;pid=$null;status='reserved_gepard_private_package';reservedUtc=[DateTime]::UtcNow.ToString('o')}
 if($l.psobject.Properties.Name -contains 'userRuntimeHold'){$n['userRuntimeHold']=$l.userRuntimeHold};return $n
};$claimed=$true
try{
 AssertPackageClosure
 if(UserRuntimeHold705 -or (CancelRequested705)){throw 'Hold/cancel immediately before launch; preserve claim'}
 $style='Hidden';if($Interactive){$style='Normal'}
 $arguments=($nativeArgs|ForEach-Object {QuoteNative $_}) -join ' '
 $game=Start-Process -FilePath $exe -WorkingDirectory ([IO.Path]::GetDirectoryName($exe)) -ArgumentList $arguments -WindowStyle $style -PassThru
 $row=Get-CimInstance Win32_Process -Filter ('ProcessId='+$game.Id) -OperationTimeoutSec 5 -ErrorAction Stop
 if(!$row -or $row.ExecutablePath -cne $exe){throw 'Attachment uncertain; preserve native and lease'}
 $owner=@{pid=$game.Id;exe=$exe;creationUtc=$row.CreationDate.ToUniversalTime().ToString('o');managedCreationUtc=$game.StartTime.ToUniversalTime().ToString('o');project=$Project;profile=$profile;args=$nativeArgs;leaseId=$proof.leaseId}
 Save 'owner.json' $owner
 SlotTransaction {param($l)AssertOwner $l;if(UserRuntimeHoldRecord705 $l){throw 'Hold at attach; preserve native'};$l.pid=$game.Id;$l.status='running_gepard_private_package';return $l};$attached=$true
 $started=[DateTime]::UtcNow;$readyReported=$false;$closeMethod='human-or-native-natural-exit'
 while(AssertIdentity){
  if(UserRuntimeHold705 -or (CancelRequested705)){throw 'Hold/cancel active; preserve exact child and lease'}
  $text=ReadPackageConsole
  if(!$readyReported -and $text -match 'GPG_READY staged=1 resourceFailure=0 objects=34 nativeBehaviorAcceptance=NOT_TESTED'){
   $readyReported=$true;Save 'STARTUP-CHECKPOINT.json' @{status='ORDINARY_READY_MARKER_SEEN_REVIEW_PENDING';actualNativePid=$game.Id;humanGameplay='NOT_TESTED';privateQA=$false;utc=[DateTime]::UtcNow.ToString('o')}
  }
  if($ProbeSeconds -gt 0 -and ([DateTime]::UtcNow-$started).TotalSeconds -ge $ProbeSeconds){
   if(!(AssertIdentity)){break}
   if(UserRuntimeHold705 -or (CancelRequested705)){throw 'Hold/cancel before bounded normal close; preserve'}
   [void]$game.CloseMainWindow();$closeMethod='explicit-bounded-probe-owned-normal-window-close';$grace=[DateTime]::UtcNow.AddSeconds(15)
   while(AssertIdentity){if(UserRuntimeHold705 -or (CancelRequested705)){throw 'Hold/cancel during close; preserve'};if([DateTime]::UtcNow -ge $grace){throw 'Normal close incomplete; preserve owned session/lease'};Start-Sleep -Milliseconds 250};break
  }
  Start-Sleep -Milliseconds 500
 }
 $game.WaitForExit();$game.Refresh();$code=$game.ExitCode
 if(@(Inventory).Count){throw 'Unexpected native remains after owned exit; preserve lease'}
 SlotTransaction {param($l)AssertOwner $l;if(UserRuntimeHoldRecord705 $l){throw 'User hold at release; preserve'};if($l.pid -ne $game.Id){throw 'Lease PID changed'};$l.pid=$null;$l.activeRuntime=$null;$l.status='released_gepard_private_package';$l|Add-Member releasedUtc ([DateTime]::UtcNow.ToString('o')) -Force;return $l};$claimed=$false
 Save 'exit.json' @{verifiedExited=$true;leaseReleased=$true;exitCode=$code;closeMethod=$closeMethod;utc=[DateTime]::UtcNow.ToString('o')}
 AssertPackageClosure
 $text=ReadPackageConsole
 $errors=@($text -split '\r?\n'|Where-Object {$_ -match '\b[A-Z][A-Z0-9_]*\s*\([EF]\)|Cannot compile|error:'})
 Save 'raw-errors.json' $errors
 $gameState=$text -match '\bSCR_BaseGameMode::OnGameStateChanged\s*=\s*GAME(?:\s|$)'
 $ready=$text -match 'GPG_READY staged=1 resourceFailure=0 objects=34 nativeBehaviorAcceptance=NOT_TESTED'
 $hashes=$text.Contains('GPG_STAGED registry='+$plan.expectedRegistry+' source='+$plan.expectedRuntime+' ')
 $bays=$true;foreach($id in @('OLD-CAMO','OLD-CHARCOAL','NEW-CAMO','NEW-CHARCOAL')){if(!$text.Contains('GPG_OBJECT id='+$id+' station=bay ')){$bays=$false}}
 $survey=$text.Contains('GPG_SURVEY target=T100 nominalHorizontal=100 ') -and $text.Contains('GPG_SURVEY target=T300 nominalHorizontal=300 ')
 $worldStartup=ReadWorldStartup10 $text
 $functionalStartup=$code -eq 0 -and $gameState -and $ready -and $hashes -and $bays -and $survey -and $worldStartup.pass
 $passed=$functionalStartup -and $errors.Count -eq 0
 Save 'RESULT.json' @{status=$(if($passed){'PACKAGE_ONLY_ORDINARY_STARTUP_PROOF_PASS'}else{'PACKAGE_ONLY_ORDINARY_STARTUP_PROOF_FAIL'});functionalStartupObserved=$functionalStartup;worldStartup=$worldStartup;strictNativeVerdict=$(if($errors.Count){'FAIL'}else{'PASS'});exitCode=$code;nativeAbsent=$true;leaseReleased=$true;game=$gameState;ordinaryReady=$ready;actualLayoutHashes=$hashes;fourComparisonBays=$bays;twoRangeSurveys=$survey;rawErrorCount=$errors.Count;probeSeconds=$ProbeSeconds;privateQA=$false;humanMovement='NOT_TESTED';humanHUD='NOT_TESTED';humanFiring='NOT_TESTED';visualAcceptance='NEEDS_USER_TEST';sourcePackOperationalVerdict=$closure.packDiagnostics.operationalVerdict;sourcePackStrictNativeVerdict=$closure.packDiagnostics.strictNativeVerdict;sourcePackRawErrorCount=$closure.packDiagnostics.rawNativeErrorCount;published=$false}
 if(!$passed){exit 2}
}catch{Save 'FAILURE.json' @{status='NOT_TESTED_OR_FAILED_PRESERVED';reason=$_.Exception.Message;leaseRetained=$claimed;attached=$attached;forceTerminationPerformed=$false;nativePid=$(if($game){$game.Id}else{$null})};throw}

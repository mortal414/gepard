param(
 [Parameter(Mandatory=$true)][string]$PlanFile,
 [Parameter(Mandatory=$true)][ValidatePattern('^[0-9a-f]{64}$')][string]$PlanSHA256,
 [switch]$Execute
)
# Source-only preparation. Root alone executes after final pins/native admission.
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$slot='C:\Users\marco\Documents\Codex\2026-10-12\task\RUNTIME_SLOT.json'
$exe='D:\SteamLibrary\steamapps\common\Arma Reforger Tools\Workbench\ArmaReforgerWorkbenchSteamDiag.exe'
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
if((Get-FileHash -LiteralPath $PlanFile -Algorithm SHA256).Hash.ToLowerInvariant() -cne $PlanSHA256){throw 'Actual plan pin differs'}
$plan=Get-Content -LiteralPath $PlanFile -Raw|ConvertFrom-Json
if($plan.schema -ne 1 -or $plan.type -cne 'private-serial-validate-pack' -or $plan.steps.Count -ne 4){throw 'Private four-step plan required'}
$RunRoot=$plan.runRoot
$ExecutingNativeBatch=$false;$cacheObservations=@()
if((Test-Path -LiteralPath $RunRoot) -or (Test-Path -LiteralPath $plan.outputRoot)){throw 'Fresh batch run/output roots required'}
function AssertSources {
 if((Get-FileHash -LiteralPath $PlanFile -Algorithm SHA256).Hash.ToLowerInvariant() -cne $PlanSHA256){throw 'Frozen plan bytes changed'}
 if((Get-FileHash -LiteralPath $plan.contract -Algorithm SHA256).Hash.ToLowerInvariant() -cne $plan.contractSHA256){throw 'Root final source contract changed'}
 foreach($source in $plan.projects){
  if((Get-FileHash -LiteralPath $source.sourceManifest -Algorithm SHA256).Hash.ToLowerInvariant() -cne $source.sourceManifestSHA256){throw 'Source manifest pin differs'}
  $pins=Get-Content -LiteralPath $source.sourceManifest -Raw|ConvertFrom-Json -AsHashtable
  $files=@(Get-ChildItem -LiteralPath ([IO.Path]::GetDirectoryName($source.project)) -Recurse -File)
  $base=[IO.Path]::GetDirectoryName($source.project);$rdb=Join-Path $base 'resourceDatabase.rdb'
  $allowCache=$ExecutingNativeBatch -and ($plan.psobject.Properties.Name -contains 'allowGeneratedResourceDatabaseOnly') -and $plan.allowGeneratedResourceDatabaseOnly -eq $true
  $checkedKeys=@($pins.Keys|Where-Object {!$allowCache -or $_ -cne 'resourceDatabase.rdb'})
  $checkedFiles=@($files|Where-Object {!$allowCache -or $_.FullName -cne $rdb})
  if($checkedFiles.Count -ne $checkedKeys.Count){throw 'Non-cache complete source file set changed'}
  foreach($rel in $checkedKeys){if((Get-FileHash -LiteralPath (Join-Path $base $rel) -Algorithm SHA256).Hash.ToLowerInvariant() -cne $pins[$rel]){throw "Source bytes differ: $rel"}}
  if($allowCache){
   if($pins.ContainsKey('resourceDatabase.rdb') -and !(Test-Path -LiteralPath $rdb -PathType Leaf)){throw 'Original root RDB removed'}
   $current=$null;if(Test-Path -LiteralPath $rdb -PathType Leaf){if((Get-Item -LiteralPath $rdb).Length -le 0){throw 'Generated root RDB empty'};$current=(Get-FileHash -LiteralPath $rdb -Algorithm SHA256).Hash.ToLowerInvariant()}
   $script:cacheObservations+=@{root=$base;originalManifestSHA256=$source.sourceManifestSHA256;originalRDBSHA256=$pins['resourceDatabase.rdb'];currentRDBSHA256=$current;allOtherSourceExact=$true;utc=[DateTime]::UtcNow.ToString('o')}
  }
 }
 foreach($property in $plan.toolPins.psobject.Properties){if((Get-FileHash -LiteralPath $property.Name -Algorithm SHA256).Hash.ToLowerInvariant() -cne $property.Value){throw 'Maintained tool pin differs'}}
 foreach($dependency in $plan.dependencyClosures){
  if((Get-FileHash -LiteralPath $dependency.manifest -Algorithm SHA256).Hash.ToLowerInvariant() -cne $dependency.manifestSHA256){throw 'Exact dependency manifest differs'}
  $dependencyPins=Get-Content -LiteralPath $dependency.manifest -Raw|ConvertFrom-Json -AsHashtable
  $dependencyFiles=@(Get-ChildItem -LiteralPath $dependency.root -Recurse -File)
  if($dependencyFiles.Count -ne 3 -or $dependencyPins.Count -ne 3){throw 'Exact separate dependency trio required'}
  foreach($relativeDependency in $dependencyPins.Keys){if((Get-FileHash -LiteralPath (Join-Path $dependency.root $relativeDependency) -Algorithm SHA256).Hash.ToLowerInvariant() -cne $dependencyPins[$relativeDependency]){throw 'Dependency bytes changed'}}
 }
 foreach($support in $plan.supportingSourceClosures){
  if((Get-FileHash -LiteralPath $support.manifest -Algorithm SHA256).Hash.ToLowerInvariant() -cne $support.manifestSHA256){throw 'Qualified core source manifest differs'}
  $corePins=Get-Content -LiteralPath $support.manifest -Raw|ConvertFrom-Json -AsHashtable
  $expectedSupportCount=0;if($support.role -ceq 'gepard'){$expectedSupportCount=1152}elseif($support.role -ceq 'wcsSmoke'){$expectedSupportCount=14}else{throw 'Unexpected supporting source role'}
  if($corePins.Count -ne $expectedSupportCount -or @(Get-ChildItem -LiteralPath $support.root -Recurse -File).Count -ne $expectedSupportCount){throw 'Exact qualified supporting source count required'}
  foreach($relativeCore in $corePins.Keys){if((Get-FileHash -LiteralPath (Join-Path $support.root $relativeCore) -Algorithm SHA256).Hash.ToLowerInvariant() -cne $corePins[$relativeCore]){throw 'Qualified core source/RDB changed'}}
 }
 if($ExecutingNativeBatch){Save 'source-cache-observations.json' $cacheObservations}
}
AssertSources
. $plan.nativeTailClassifierPath
$names=@('integration:validate','integration:pack','provingGround:validate','provingGround:pack')
for($i=0;$i -lt 4;$i++){
 $s=$plan.steps[$i]
 if(($s.role+':'+$s.name) -cne $names[$i] -or $s.executable -cne $exe -or $s.deadlineSeconds -lt 30 -or $s.deadlineSeconds -gt 300){throw 'Native sequential step binding differs'}
 if(@($s.args|Where-Object {$_ -match '(?i)publish|workshop|changenote|password|token'}).Count){throw 'Only private validation/pack arguments allowed'}
 $project=@($plan.projects|Where-Object {$_.role -ceq $s.role})
 if($project.Count -ne 1 -or $s.project -cne $project[0].project -or $s.output -cne (Join-Path $plan.outputRoot $project[0].outputName)){throw 'Step project/output does not match frozen role'}
 $expectedLogs=Join-Path $RunRoot ($s.role+'\logs\'+$s.name)
 if($s.logsDir -cne $expectedLogs){throw 'Step logs differ from isolated run'}
 $expectedArgs=@('-gproj',$s.project,'-addonsDir',((@($plan.sourceAddonRoot)+@($plan.dependencyAddonRoots)+@($plan.coreAddonRoot)) -join ','),'-logsDir',$s.logsDir)
 if($s.name -eq 'validate'){$expectedArgs+=@('-wbSilent','-wbModule=ScriptEditor','-validate','PC')}else{$expectedArgs+=@('-wbModule=ResourceManager','-packAddon','-packAddonDir',$s.output)}
 if($s.args.Count -ne $expectedArgs.Count){throw 'Native argument count differs from maintained plan'}
 for($a=0;$a -lt $expectedArgs.Count;$a++){if($s.args[$a] -cne $expectedArgs[$a]){throw 'Native argv differs from maintained validate/pack plan'}}
}
if(!$Execute){@{status='SOURCE_BOUND_SERIAL_PACK_DRY_PLAN';nativeStarted=$false;leaseMutated=$false;steps=$plan.steps}|ConvertTo-Json -Depth 15;exit 0}
if(UserRuntimeHold705){throw 'User hold active; no native admission'}
if(CancelRequested705){throw 'Cancelled before native admission'}
foreach($volume in Get-Volume -DriveLetter C,D){if($volume.SizeRemaining -lt 2GB){throw 'Below2GiB preflight'}}
[IO.Directory]::CreateDirectory($RunRoot)|Out-Null
$proof=@{owner='Gepard private serial validate-pack';ownerKey=([Guid]::NewGuid().ToString('N')+[Guid]::NewGuid().ToString('N'));leaseId=[Guid]::NewGuid().ToString();runId=[Guid]::NewGuid().ToString();workRoot=$RunRoot;project=$plan.projects[0].project}
Save 'owner-private.json' $proof
Copy-Item -LiteralPath $PlanFile -Destination (Join-Path $RunRoot 'plan.json')
SlotTransaction {param($l)
 if(UserRuntimeHoldRecord705 $l){throw 'User hold active at atomic claim'}
 if($l.pid -or $l.activeRuntime -or $l.status -notmatch '^released' -or ($l.psobject.Properties.Name -contains 'parallelCaptures' -and $l.parallelCaptures)){throw 'Lease busy'}
 if(@(Inventory).Count){throw 'Native session present; preserve'}
 $next=@{owner=$proof.owner;ownerKeyHash=(KeyHash $proof.ownerKey);leaseId=$proof.leaseId;activeRuntime=$proof.runId;workRoot=$RunRoot;project=$proof.project;pid=$null;status='reserved_gepard_private_pack';reservedUtc=[DateTime]::UtcNow.ToString('o')}
 if($l.psobject.Properties.Name -contains 'userRuntimeHold'){$next['userRuntimeHold']=$l.userRuntimeHold}
 return $next
};$claimed=$true
$ExecutingNativeBatch=$true
$results=@();$stepExitVerified=$false
try{
 foreach($step in $plan.steps){
  if(UserRuntimeHold705){throw 'User hold active between steps; preserve lease'}
  if(CancelRequested705){throw 'Cancelled between steps; no following native step'}
  AssertSources
  if(@(Inventory).Count){throw 'Unexpected native session between steps; preserve'}
  $Project=$step.project;$nativeArgs=@($step.args);$attached=$false;$game=$null;$owner=$null
  $stepRoot=Join-Path $RunRoot ($step.role+'-'+$step.name)
  [IO.Directory]::CreateDirectory($stepRoot)|Out-Null
  [IO.Directory]::CreateDirectory($step.logsDir)|Out-Null
  if($step.name -eq 'pack'){
   if(Test-Path -LiteralPath $step.output){throw 'Fresh package output required'}
   [IO.Directory]::CreateDirectory($step.output)|Out-Null
  }
  SlotTransaction {param($l)AssertOwner $l;if(UserRuntimeHoldRecord705 $l){throw 'Hold at step reservation'};$l.project=$Project;$l.status='reserved_gepard_private_pack_step';return $l}
  $startedUtc=[DateTime]::UtcNow;$startedMs=[DateTimeOffset]::UtcNow.ToUnixTimeMilliseconds()
  $arguments=($nativeArgs|ForEach-Object {QuoteNative $_}) -join ' '
  $stepExitVerified=$false
  $game=Start-Process -FilePath $exe -WorkingDirectory ([IO.Path]::GetDirectoryName($exe)) -ArgumentList $arguments -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $stepRoot 'launcher.stdout.txt') -RedirectStandardError (Join-Path $stepRoot 'launcher.stderr.txt')
  $row=Get-CimInstance Win32_Process -Filter ('ProcessId='+$game.Id) -OperationTimeoutSec 5 -ErrorAction Stop
  if(!$row -or $row.ExecutablePath -cne $exe){throw 'Native child attachment uncertain; preserve lease and process'}
  $owner=@{pid=$game.Id;exe=$exe;creationUtc=$row.CreationDate.ToUniversalTime().ToString('o');managedCreationUtc=$game.StartTime.ToUniversalTime().ToString('o');creationSource='CIM CreationDate';managedCreationSource='Process.StartTime';project=$Project;profile=$null;args=$nativeArgs;leaseId=$proof.leaseId}
  [IO.File]::WriteAllText((Join-Path $stepRoot 'owner.json'),($owner|ConvertTo-Json -Depth 10))
  SlotTransaction {param($l)AssertOwner $l;if(UserRuntimeHoldRecord705 $l){throw 'Hold at child attachment; preserve native'};$l.pid=$game.Id;$l.status='running_gepard_private_pack_step';return $l};$attached=$true
  while(AssertIdentity){
   if(UserRuntimeHold705){throw 'Hold while native active; preserve exact child/lease'}
   if(CancelRequested705){throw 'Cancelled while native active; preserve exact child/lease'}
   if(([DateTime]::UtcNow-$startedUtc).TotalSeconds -ge $step.deadlineSeconds){throw 'Bounded native deadline; preserve exact child/lease, no following step'}
   Start-Sleep -Milliseconds 250
  }
  $game.WaitForExit();$game.Refresh();$code=$game.ExitCode
  [IO.File]::WriteAllText((Join-Path $stepRoot 'exit.json'),(@{pid=$game.Id;exitCode=$code;verifiedExited=$true;leaseReleased=$false;utc=[DateTime]::UtcNow.ToString('o')}|ConvertTo-Json))
  if(@(Inventory).Count){throw 'Unexpected native remains after step; preserve'}
  SlotTransaction {param($l)AssertOwner $l;if(UserRuntimeHoldRecord705 $l){throw 'Hold after native exit; preserve lease'};$l.pid=$null;$l.status='reserved_gepard_private_pack_between_steps';return $l}
  $stepExitVerified=$true
  if($code -ne 0){throw 'Native step nonzero; no following step'}
  AssertSources
  $console=Join-Path $step.logsDir 'console.log'
  if(!(Test-Path -LiteralPath $console -PathType Leaf) -or (Get-Item -LiteralPath $console).LastWriteTimeUtc -lt $startedUtc.AddSeconds(-1)){throw 'Missing/stale exact step console'}
  $text=[IO.File]::ReadAllText($console)
  $errors=@($text -split '\r?\n'|Where-Object {$_ -match '\b[A-Z][A-Z0-9_]*\s*\([EF]\)|Cannot compile|error:'})
  [IO.File]::WriteAllText((Join-Path $stepRoot 'raw-errors.json'),(ConvertTo-Json -InputObject @($errors)))
  $priorValidate=@($results|Where-Object {$_.role -ceq $step.role -and $_.step -ceq 'validate' -and $_.operationalVerdict -in @('PASS','PASS_WITH_EXACT_BASELINE_TEARDOWN')}).Count -eq 1
  $diagnostics=ClassifyKnownNativeTail -Text $text -StepName $step.name -ExitCode $code -BaselinePath $plan.nativeTailBaselinePath -ValidationWasAccepted $priorValidate
  $diagnosticsPath=Join-Path $stepRoot 'native-diagnostics.json'
  [IO.File]::WriteAllText($diagnosticsPath,($diagnostics|ConvertTo-Json -Depth 15))
  if(!$diagnostics.operationalPass){throw 'Operational native diagnostics failed; raw strict errors retained, no following step'}
  if($step.name -eq 'validate' -and !$text.Contains('Script validation successful.')){throw 'Actual script validation success missing'}
  if($step.name -eq 'pack'){
   $report=Join-Path $stepRoot 'package-proof.json'
   $helperArgs=@('--import',$plan.tools.tsxLoader,$plan.tools.helper,'package',$PlanFile,$step.role,$startedMs.ToString(),$report)
   $helper=Start-Process -FilePath $plan.tools.node -ArgumentList (($helperArgs|ForEach-Object {QuoteNative $_}) -join ' ') -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $stepRoot 'proof.stdout.txt') -RedirectStandardError (Join-Path $stepRoot 'proof.stderr.txt')
   if(!$helper.WaitForExit(30000)){throw 'Package verifier deadline; preserve helper/lease for review'}
   $helper.Refresh();if($helper.ExitCode -ne 0 -or !(Test-Path -LiteralPath $report -PathType Leaf)){throw 'Same-build trio/content verification failed; no following step'}
  }
  $results+=@{role=$step.role;step=$step.name;exitCode=$code;console=$console;stepRoot=$stepRoot;verifiedExited=$true;operationalVerdict=$diagnostics.operationalVerdict;strictNativeVerdict=$diagnostics.strictNativeVerdict;nativeErrorCount=$diagnostics.rawNativeErrorCount;diagnosticsReport=$diagnosticsPath;diagnosticsReportSHA256=(Get-FileHash -LiteralPath $diagnosticsPath -Algorithm SHA256).Hash.ToLowerInvariant()}
 }
 AssertSources
 if(@(Inventory).Count){throw 'Native present at final release; preserve'}
 SlotTransaction {param($l)AssertOwner $l;if(UserRuntimeHoldRecord705 $l){throw 'Hold at final release; preserve lease'};$l.pid=$null;$l.activeRuntime=$null;$l.status='released_gepard_private_pack';$l|Add-Member releasedUtc ([DateTime]::UtcNow.ToString('o')) -Force;return $l};$claimed=$false
 Save 'RESULT.json' @{status='PRIVATE_VALIDATE_PACK_SEQUENCE_OPERATIONAL_PASS';operationalVerdict='PASS';strictNativeVerdict=$(if(@($results|Where-Object {$_.strictNativeVerdict -eq 'FAIL'}).Count){'FAIL'}else{'PASS'});nativeErrorCount=($results|Measure-Object -Property nativeErrorCount -Sum).Sum;baselineTeardownDiagnosticsRetained=$true;steps=$results;nativeAbsent=$true;leaseReleased=$true;nativeGameplay='NOT_TESTED';packageOnlyWorldLoad='NOT_TESTED';published=$false}
}catch{
 $originalFailure=$_
 Save 'FAILURE.json' @{status='NOT_TESTED_OR_FAILED_PRESERVED';reason=$originalFailure.Exception.Message;completedSteps=$results;leaseRetained=$claimed;currentNativePid=$(if($game){$game.Id}else{$null});noFollowingStep=$true;forceTerminationPerformed=$false;published=$false}
 # Only a previously attached child, already observed to exit naturally and pass
 # the prior empty-census/between-step transaction, can enter terminal cleanup.
 $releaseProof=@{eligible=$false;released=$false;reason='No proven exited child; preserve';nativeStopped=$false}
 if($claimed -and $attached -and $stepExitVerified -and $game){
  try{
   $game.Refresh()
   if(!$game.HasExited){throw 'Owned native still running; preserve'}
   $exitProof=Get-Content -LiteralPath (Join-Path $stepRoot 'exit.json') -Raw|ConvertFrom-Json
   if(!$exitProof.verifiedExited -or $exitProof.pid -ne $owner.pid -or $exitProof.exitCode -ne $game.ExitCode){throw 'Actual child exit proof differs'}
   if($game.StartTime.ToUniversalTime().Ticks -ne ([DateTime]$owner.managedCreationUtc).ToUniversalTime().Ticks){throw 'Managed identity differs from own initial birth'}
   if(UserRuntimeHold705 -or (CancelRequested705)){throw 'User hold/cancel active; preserve lease'}
   if(@(Inventory).Count){throw 'Native present at first failure census; preserve'}
   Start-Sleep -Milliseconds 150
   if(@(Inventory).Count){throw 'Native present at second failure census; preserve'}
   SlotTransaction {param($l)AssertOwner $l;if(UserRuntimeHoldRecord705 $l){throw 'User hold in terminal failure cleanup'};if($l.pid -or $l.status -cne 'reserved_gepard_private_pack_between_steps'){throw 'Lease not exact owned between-step reservation'};if(@(Inventory).Count){throw 'Native appeared during atomic terminal cleanup'};$l.activeRuntime=$null;$l.status='released_gepard_private_pack_failed';$l|Add-Member releasedUtc ([DateTime]::UtcNow.ToString('o')) -Force;return $l}
   $claimed=$false;$releaseProof=@{eligible=$true;released=$true;reason='Exact owned natural exit and repeated fresh native absence';nativeStopped=$false;exitCode=$game.ExitCode;strictFailurePreserved=$true}
  }catch{$releaseProof=@{eligible=$true;released=$false;reason=$_.Exception.Message;nativeStopped=$false;strictFailurePreserved=$true}}
 }
 Save 'POST-FAILURE-RELEASE.json' $releaseProof
 throw $originalFailure
}

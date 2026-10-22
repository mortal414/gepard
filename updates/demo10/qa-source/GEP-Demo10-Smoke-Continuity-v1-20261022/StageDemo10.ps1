param([switch]$Execute)
# Parent/coordinator executes only after its own normal admission. No native launch.
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$sourceRoot='EXPLICIT_PINNED_CURRENT_CORE_OPTIONAL_QA_INSTALLED_TRIO'
$targetRoot='D:\ArmaProjects\Gepard-Demo10-TrackArt-20261022\native-demo10-smoke-stage-v1'
$closedPath='D:\ArmaProjects\Gepard-Consolidated-20261022\native-runs\smoke-current-v11-camo-01\smoke-current-v11-camo\native-run\exit.json'
$slot='C:\Users\marco\Documents\Codex\2026-10-12\task\RUNTIME_SLOT.json'
$receipt=Join-Path $PSScriptRoot 'state\STAGING-RESULT.json'
function AssertSafeOffline {
 $closed=Get-Content -LiteralPath $closedPath -Raw|ConvertFrom-Json
 if(!$closed.verifiedExited -or !$closed.leaseReleased -or $closed.exitCode -ne 0){throw 'Camo01 failed-QA fixture control normal exit/release absent'}
 $l=Get-Content -LiteralPath $slot -Raw|ConvertFrom-Json
 if($l.status -match '^user_runtime_hold' -or ($l.psobject.Properties.Name -contains 'userRuntimeHold' -and $l.userRuntimeHold.active -ne $false)){throw 'User runtime hold; preserve'}
 if($l.pid -or $l.activeRuntime -or $l.status -notmatch '^released'){throw 'Runtime lease busy; preserve'}
 if(@(Get-CimInstance Win32_Process -OperationTimeoutSec 5 -ErrorAction Stop|Where-Object {$_.Name -match '^(ArmaReforger|Workbench|workbenchApp)'}).Count){throw 'Native process present; preserve package08/session'}
}
function SourcePins {
 $count=0
 foreach($row in $script:source){
  if((Get-FileHash -LiteralPath $row.manifest -Algorithm SHA256).Hash.ToLowerInvariant() -ne $row.sha256){throw 'Source manifest changed'}
  $pins=Get-Content -LiteralPath $row.manifest -Raw|ConvertFrom-Json -AsHashtable
  $selectedInstalledTrio=($row.psobject.Properties.Name -contains 'selectedInstalledTrioOnly' -and $row.selectedInstalledTrioOnly -eq $true)
  if($selectedInstalledTrio){if($pins.Count -ne 3 -or @($pins.Keys|Where-Object {$_ -notin @('addon.gproj','data.pak','resourceDatabase.rdb')}).Count){throw 'Only exact separately pinned installedWCS3 permitted'}}
  elseif(@(Get-ChildItem -LiteralPath $row.root -Recurse -File).Count -ne $pins.Count){throw 'Complete source set changed'}
  if($pins.Count -ne $row.files){throw 'Declared source file count differs'}
  foreach($n in $pins.Keys){if((Get-FileHash -LiteralPath (Join-Path $row.root $n) -Algorithm SHA256).Hash.ToLowerInvariant() -ne $pins[$n]){throw ('Complete source changed: '+$n)}}
  $count+=$pins.Count
 }
 if($count -ne 1261){throw 'Expected exact1179 pinned current source files'}
}
if(!$Execute){@{status='SOURCE_REQUEST_DRY_RUN_ONLY';target=$targetRoot;nativeStarted=$false;stageCopied=$false}|ConvertTo-Json;exit 0}
$sealed=Get-Content -LiteralPath (Join-Path $PSScriptRoot 'SOURCE-MANIFEST.json') -Raw|ConvertFrom-Json
foreach($file in $sealed.files){if((Get-FileHash -LiteralPath (Join-Path $PSScriptRoot $file.relative) -Algorithm SHA256).Hash.ToLowerInvariant() -ne $file.sha256){throw ('Immutable source differs: '+$file.relative)}}
$source=Get-Content -LiteralPath (Join-Path $PSScriptRoot 'SOURCE-COPY-CLOSURES.json') -Raw|ConvertFrom-Json
if([IO.Path]::GetFullPath($targetRoot) -cne 'D:\ArmaProjects\Gepard-Demo10-TrackArt-20261022\native-demo10-smoke-stage-v1'){throw 'Target outside exact fresh stage'}
AssertSafeOffline
if((Test-Path -LiteralPath $targetRoot) -or (Test-Path -LiteralPath $receipt)){throw 'Preserve existing/partial stage or receipt; use a new diagnosed version'}
foreach($volume in Get-Volume -DriveLetter C,D){if($volume.SizeRemaining -lt 2GB){throw 'Below2GiB preflight'}}
SourcePins
$expected=@{}
[IO.Directory]::CreateDirectory($targetRoot)|Out-Null
foreach($row in $source){
 AssertSafeOffline
 if(!$row.copyRequired){continue}
 $dest=$row.target
 if(!$dest.StartsWith($targetRoot+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Target escaped stage'}
 $pins=Get-Content -LiteralPath $row.manifest -Raw|ConvertFrom-Json -AsHashtable
 foreach($n in $pins.Keys){$out=Join-Path $dest $n;[IO.Directory]::CreateDirectory([IO.Path]::GetDirectoryName($out))|Out-Null;[IO.File]::Copy((Join-Path $row.root $n),$out,$false);$expected[$out]=$pins[$n]}
}
AssertSafeOffline
AssertSafeOffline
SourcePins
if($expected.Count -ne 10 -or @(Get-ChildItem -LiteralPath $targetRoot -Recurse -File).Count -ne 10){throw 'Expected exact1179 staged files'}
foreach($n in $expected.Keys){if((Get-FileHash -LiteralPath $n -Algorithm SHA256).Hash.ToLowerInvariant() -ne $expected[$n]){throw ('Staged bytes differ: '+$n)}}
[IO.Directory]::CreateDirectory([IO.Path]::GetDirectoryName($receipt))|Out-Null
$payload=@{status='SOURCE_STAGED_NATIVE_COMPILE_BINDING_AND_GAMEPLAY_PENDING';source=$sourceRoot;target=$targetRoot;sourcePreserved=$true;files=10;mountedFiles=1251;modifiedExistingFiles=0;newFiles=0;nativeStarted=$false;leaseMutated=$false;package08Modified=$false}|ConvertTo-Json
$stream=[IO.File]::Open($receipt,[IO.FileMode]::CreateNew,[IO.FileAccess]::Write,[IO.FileShare]::None)
try{$encoded=[Text.Encoding]::UTF8.GetBytes($payload);$stream.Write($encoded,0,$encoded.Length)}finally{$stream.Dispose()}
Get-Content -LiteralPath $receipt -Raw

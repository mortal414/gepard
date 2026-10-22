param(
 [Parameter(Mandatory=$true)][string]$Project,
 [Parameter(Mandatory=$true)][string]$World,
 [Parameter(Mandatory=$true)][string]$Manifest,
 [Parameter(Mandatory=$true)][string]$RunRoot,
 [ValidateRange(30,300)][int]$Seconds=75,
 [ValidatePattern("^[0-9A-Fa-f]{16}$")][string]$AddonId="D561AD08932E7B40",
 [switch]$Interactive,
 [switch]$KeepOpen,
 [ValidateRange(640,7680)][int]$ScreenWidth=1280,
 [ValidateRange(480,4320)][int]$ScreenHeight=720,
 [switch]$Execute
)
# Addon-owned serial scene gate. Existing native command, legacy lease and normal
# window-close paths only. No desktop input, force kill, or parallel admission.
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$slot='C:\Users\marco\Documents\Codex\2026-10-12\task\RUNTIME_SLOT.json'
$exe='D:\SteamLibrary\steamapps\common\Arma Reforger\ArmaReforgerSteamDiag.exe'
$proof=$null;$game=$null;$attached=$false;$claimed=$false;$owner=$null
function Save($name,$value){[IO.File]::WriteAllText((Join-Path $RunRoot $name),($value|ConvertTo-Json -Depth 20))}
function KeyHash([string]$s){[Convert]::ToHexString([Security.Cryptography.SHA256]::HashData([Text.Encoding]::UTF8.GetBytes($s))).ToLowerInvariant()}
function Inventory {@(Get-CimInstance Win32_Process -OperationTimeoutSec 5 -ErrorAction Stop|Where-Object {$_.Name -match '^(ArmaReforger|Workbench|workbenchApp)'})}
function SlotTransaction([scriptblock]$change){
 $stream=[IO.File]::Open($slot,[IO.FileMode]::Open,[IO.FileAccess]::ReadWrite,[IO.FileShare]::None)
 try{$reader=[IO.StreamReader]::new($stream,[Text.Encoding]::UTF8,$true,1024,$true);try{$l=$reader.ReadToEnd()|ConvertFrom-Json}finally{$reader.Dispose()};$next=& $change $l;if(!$next){throw 'No lease result'};$bytes=[Text.Encoding]::UTF8.GetBytes(($next|ConvertTo-Json -Depth 20));$stream.Position=0;$stream.SetLength(0);$stream.Write($bytes,0,$bytes.Length);$stream.Flush($true)}finally{$stream.Dispose()}
}
function AssertOwner($l){if($l.owner -ne $proof.owner -or $l.leaseId -ne $proof.leaseId -or $l.activeRuntime -ne $proof.runId -or $l.ownerKeyHash -ne (KeyHash $proof.ownerKey) -or $l.workRoot -ne $RunRoot){throw 'Owner differs; preserve session'}}
function QuoteNative([string]$s){if(!$s -or $s -match '["\x00-\x1f]'){throw 'Unsafe argument'};if($s -notmatch '\s'){return $s};$tail=[regex]::Match($s,'\\+$').Length;return '"'+$s+('\'*$tail)+'"'}
Add-Type -TypeDefinition @'
using System;
using System.Runtime.InteropServices;
public static class GEPSceneArgs {
 [DllImport("shell32.dll",SetLastError=true)] static extern IntPtr CommandLineToArgvW([MarshalAs(UnmanagedType.LPWStr)] string s,out int n);
 [DllImport("kernel32.dll")] static extern IntPtr LocalFree(IntPtr p);
 public static string[] Parse(string s){int n;IntPtr p=CommandLineToArgvW(s,out n);if(p==IntPtr.Zero)throw new InvalidOperationException("Cannot parse command identity");try{var a=new string[n];for(int i=0;i<n;i++)a[i]=Marshal.PtrToStringUni(Marshal.ReadIntPtr(p,i*IntPtr.Size));return a;}finally{LocalFree(p);}}
}
'@
function AssertIdentity {
 if(!$attached){throw 'Unattached child; preserve'}
 SlotTransaction {param($l)AssertOwner $l;if($l.pid -ne $game.Id){throw 'Lease PID differs'};return $l}
 $row=Get-CimInstance Win32_Process -Filter ('ProcessId='+$game.Id) -OperationTimeoutSec 5 -ErrorAction Stop
 if(!$row){$game.WaitForExit();return $false}
 if($row.ExecutablePath -ne $exe -or $row.CreationDate.ToUniversalTime().Ticks -ne ([DateTime]$owner.creationUtc).ToUniversalTime().Ticks){
  if($game.HasExited -and !$row.ExecutablePath -and $row.CreationDate.ToUniversalTime().Ticks -eq ([DateTime]$owner.creationUtc).ToUniversalTime().Ticks){Start-Sleep -Milliseconds 150;$row=Get-CimInstance Win32_Process -Filter ('ProcessId='+$game.Id) -OperationTimeoutSec 5 -ErrorAction Stop;if(!$row){return $false}}
  throw 'PID/executable/birth differs; preserve'
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

function AssertCompleteSources705 {
 $closures=Get-Content -LiteralPath 'C:\Users\marco\Documents\Codex\2026-10-21\task\GEP-ProvingGround-QA-20261022\SOURCE-CLOSURES.json' -Raw | ConvertFrom-Json
 foreach($closure705 in $closures){
  $pins705=Get-Content -LiteralPath $closure705.manifest -Raw | ConvertFrom-Json -AsHashtable
  $files705=@(Get-ChildItem -LiteralPath $closure705.root -File -Recurse)
  if($files705.Count -ne $pins705.Count){throw 'Complete source file count differs; preserve and inspect generated changes'}
  foreach($rel705 in $pins705.Keys){
   $source705=Join-Path $closure705.root $rel705
   if(!(Test-Path -LiteralPath $source705 -PathType Leaf) -or (Get-FileHash -LiteralPath $source705 -Algorithm SHA256).Hash.ToLowerInvariant() -ne $pins705[$rel705]){throw "Complete source pin differs: $rel705"}
  }
 }
}

if(!$Execute){
 AssertCompleteSources705
 @{status='SOURCE_PINS_PASS_NATIVE_NOT_TESTED';dryRun=$true;nativeStarted=$false;leaseMutated=$false;project=$Project;world=$World;manifest=$Manifest;addonId=$AddonId;screenWidth=$ScreenWidth;screenHeight=$ScreenHeight;requiredAdmission='Explicit user resume, fresh ownership admission and actual compile adoption required'}|ConvertTo-Json -Depth 6
 exit 0
}
if(UserRuntimeHold705){throw 'User runtime hold active; no native admission'}
if(CancelRequested705){@{status='NOT_STARTED_CANCELLED';nativeStarted=$false;leaseMutated=$false}|ConvertTo-Json;exit 2}
AssertCompleteSources705
if(Test-Path -LiteralPath $RunRoot){throw 'Fresh run root required'}
foreach($volume in Get-Volume -DriveLetter C,D){if($volume.SizeRemaining -lt 2GB){throw 'Below2GiB preflight'}}
$addon=[IO.Path]::GetDirectoryName($Project)
if($Project -cne 'D:\ArmaProjects\Gepard-Consolidated-20261022\native-stage\provingGround\GepardProvingGround.gproj' -or $World -cne 'Worlds/GPG_Arland.ent' -or $AddonId -cne 'A76A420022000001'){throw 'Only frozen proving-ground startup project/world/addon is bound'}
$addonRootsGPG=@('D:\ArmaProjects\Gepard-Consolidated-20261022\native-stage','D:\SteamLibrary\steamapps\common\Arma Reforger\addons')
foreach($rootGPG in $addonRootsGPG){if(!(Test-Path -LiteralPath $rootGPG -PathType Container)){throw 'Explicit addon discovery root missing'}}
$addons=$addonRootsGPG -join ','
if(!(Test-Path -LiteralPath $Project -PathType Leaf) -or ($World -ne 'worlds/GameMaster/GM_Arland.ent' -and !(Test-Path -LiteralPath (Join-Path $addon $World) -PathType Leaf))){throw 'Exact project/world missing'}
$source=Get-Content -LiteralPath $Manifest -Raw|ConvertFrom-Json -AsHashtable
foreach($n in $source.Keys){if((Get-FileHash -LiteralPath (Join-Path $addon $n) -Algorithm SHA256).Hash.ToLowerInvariant() -ne $source[$n]){throw "Source pin differs: $n"}}
[IO.Directory]::CreateDirectory($RunRoot)|Out-Null
$profile=Join-Path $RunRoot 'profile';$logs=Join-Path $RunRoot 'logs';$downloads=Join-Path $RunRoot 'downloads'
foreach($dir in @($profile,$logs,$downloads)){[IO.Directory]::CreateDirectory($dir)|Out-Null}
$nativeArgs=@('-profile',$profile,'-logsDir',$logs,'-addonDownloadDir',$downloads,'-window','-screenWidth',$ScreenWidth.ToString(),'-screenHeight',$ScreenHeight.ToString(),'-maxFPS','30','-noSplash','-forceUpdate','-addonsDir',$addons,'-addons',$AddonId,'-world',$World,'-worldSystemsConfig','{C50579A2EC48E8EF}Configs/Systems/MissionSystems.conf','-noBackend')
if(!$Interactive){$nativeArgs+='-noFocus'}
$proof=@{owner='Gepard serial scene coordinator';ownerKey=([Guid]::NewGuid().ToString('N')+[Guid]::NewGuid().ToString('N'));leaseId=[Guid]::NewGuid().ToString();runId=[Guid]::NewGuid().ToString();workRoot=$RunRoot;project=$Project}
Save 'owner-private.json' $proof
Save 'plan.json' @{exe=$exe;args=$nativeArgs;project=$Project;manifest=$Manifest;manifestSHA256=(Get-FileHash -LiteralPath $Manifest -Algorithm SHA256).Hash;seconds=$Seconds;keepOpen=[bool]$KeepOpen;physicalInput='NOT_TESTED'}
SlotTransaction {param($l)if(UserRuntimeHoldRecord705 $l){throw 'User runtime hold active at atomic claim; no native admission'};if($l.pid -or $l.activeRuntime -or $l.status -notmatch '^released' -or ($l.psobject.Properties.Name -contains 'parallelCaptures' -and $l.parallelCaptures)){throw 'Lease busy'};if(@(Inventory).Count){throw 'Native session present'};$next705=@{owner=$proof.owner;ownerKeyHash=(KeyHash $proof.ownerKey);leaseId=$proof.leaseId;activeRuntime=$proof.runId;workRoot=$RunRoot;project=$Project;profile=$profile;pid=$null;status='starting_gepard_serial_scene';reservedUtc=[DateTime]::UtcNow.ToString('o')};if($l.psobject.Properties.Name -contains 'userRuntimeHold'){$next705['userRuntimeHold']=$l.userRuntimeHold};return $next705}
$claimed=$true
try {
 $arguments=($nativeArgs|ForEach-Object {QuoteNative $_}) -join ' '
 $style='Hidden';if($Interactive){$style='Normal'}
 if(UserRuntimeHold705){throw 'User runtime hold active immediately before native launch; preserve claim'}
 $game=Start-Process -FilePath $exe -WorkingDirectory ([IO.Path]::GetDirectoryName($exe)) -ArgumentList $arguments -WindowStyle $style -PassThru
 $row=Get-CimInstance Win32_Process -Filter ('ProcessId='+$game.Id) -OperationTimeoutSec 5 -ErrorAction Stop
 if(!$row -or $row.ExecutablePath -ne $exe){throw 'Attachment uncertain; retain lease'}
 $owner=@{pid=$game.Id;exe=$exe;creationUtc=$row.CreationDate.ToUniversalTime().ToString('o');managedCreationUtc=$game.StartTime.ToUniversalTime().ToString('o');project=$Project;profile=$profile;args=$nativeArgs;leaseId=$proof.leaseId}
 Save 'owner.json' $owner
 SlotTransaction {param($l)AssertOwner $l;if(UserRuntimeHoldRecord705 $l){throw 'User runtime hold active at attachment; preserve native and lease'};$l.pid=$game.Id;$l.status='running_gepard_serial_scene';return $l};$attached=$true
 $until=[DateTime]::UtcNow.AddSeconds($Seconds);$closed='native-or-user-exit';$cancelled705=$false
 while(AssertIdentity){
  if(UserRuntimeHold705){throw 'User runtime hold active in owned loop; preserve process and lease'}
  if(CancelRequested705){$cancelled705=$true}
  if($cancelled705 -or (!$KeepOpen -and [DateTime]::UtcNow -ge $until)){
   $managed=Get-Process -Id $game.Id -ErrorAction Stop
   if($managed.StartTime.ToUniversalTime().Ticks -ne ([DateTime]$owner.managedCreationUtc).ToUniversalTime().Ticks){throw 'Managed birth differs'}
   if(UserRuntimeHold705){throw 'User runtime hold active before close; preserve owned process and lease'}
   [void]$managed.CloseMainWindow();$closed='owned-normal-window-close';if($cancelled705){$closed='owned-normal-window-close-cancelled'};$grace=[DateTime]::UtcNow.AddSeconds(15)
   while(AssertIdentity){if(UserRuntimeHold705){throw 'User runtime hold active during close wait; preserve uncertainty'};if(CancelRequested705){$cancelled705=$true};if([DateTime]::UtcNow -ge $grace){throw 'Normal close incomplete; preserve owned session and lease'};Start-Sleep -Milliseconds 250};break
  }
  Start-Sleep -Milliseconds 500
 }
 $game.WaitForExit()
 if(@(Inventory).Count){throw 'Unexpected native session remains; preserve lease'}
 SlotTransaction {param($l)AssertOwner $l;if(UserRuntimeHoldRecord705 $l){throw 'User runtime hold active at release; preserve lease for coordinator'};if($l.pid -ne $game.Id){throw 'Lease PID changed'};$l.pid=$null;$l.activeRuntime=$null;$l.status='released_gepard_serial_scene';$l|Add-Member releasedUtc ([DateTime]::UtcNow.ToString('o')) -Force;return $l};$claimed=$false
 Save 'exit.json' @{verifiedExited=$true;leaseReleased=$true;exitCode=$game.ExitCode;closeMethod=$closed;cancelRequested=($cancelled705 -or (CancelRequested705));utc=[DateTime]::UtcNow.ToString('o')}
 Get-Content -LiteralPath (Join-Path $RunRoot 'exit.json')
}catch{Save 'FAILURE.json' @{status='NOT_TESTED';error=$_.Exception.Message;leaseRetained=$claimed;attached=$attached;utc=[DateTime]::UtcNow.ToString('o')};throw}

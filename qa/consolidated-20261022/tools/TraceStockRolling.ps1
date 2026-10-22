param(
 [Parameter(Mandatory=$true)][string]$Project,
 [Parameter(Mandatory=$true)][ValidateSet("GEP033_TraceStockRollingPlugin")][string]$Plugin,
 [Parameter(Mandatory=$true)][string]$Manifest,
 [Parameter(Mandatory=$true)][string]$RunRoot,
 [ValidateRange(30,300)][int]$Seconds=120,
 [switch]$Execute,
 [switch]$Interactive,
 [switch]$KeepOpen
)
# Addon-owned exact-list texture cook gate, derived from the exercised serial scene wrapper. Existing native command, legacy lease and normal
# window-close paths only. No desktop input, force kill, or parallel admission.
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'


$slot='C:\Users\marco\Documents\Codex\2026-10-12\task\RUNTIME_SLOT.json'
$exe='D:\SteamLibrary\steamapps\common\Arma Reforger Tools\Workbench\ArmaReforgerWorkbenchSteamDiag.exe'
$proof=$null;$game=$null;$attached=$false;$claimed=$false;$owner=$null
function Save($name,$value){[IO.File]::WriteAllText((Join-Path $RunRoot $name),($value|ConvertTo-Json -Depth 20))}
function KeyHash([string]$s){[Convert]::ToHexString([Security.Cryptography.SHA256]::HashData([Text.Encoding]::UTF8.GetBytes($s))).ToLowerInvariant()}
function Inventory {@(Get-CimInstance Win32_Process -OperationTimeoutSec 5 -ErrorAction Stop|Where-Object {$_.Name -match '^(ArmaReforger|Workbench|workbenchApp)'})}
function HoldActive($l){return ($l.status -match '^user_runtime_hold' -or ($l.psobject.Properties.Name -contains 'userRuntimeHold' -and $l.userRuntimeHold.active -ne $false))}
function SlotTransaction([scriptblock]$change){
 $stream=[IO.File]::Open($slot,[IO.FileMode]::Open,[IO.FileAccess]::ReadWrite,[IO.FileShare]::None)
 try{$reader=[IO.StreamReader]::new($stream,[Text.Encoding]::UTF8,$true,1024,$true);try{$l=$reader.ReadToEnd()|ConvertFrom-Json}finally{$reader.Dispose()};if(HoldActive $l){throw 'User runtime hold active; preserve process/lease'};$next=& $change $l;if(($l.psobject.Properties.Name -contains 'userRuntimeHold') -and !($next.psobject.Properties.Name -contains 'userRuntimeHold')){if($next -is [hashtable]){$next['userRuntimeHold']=$l.userRuntimeHold}else{$next|Add-Member userRuntimeHold $l.userRuntimeHold -Force}};if(!$next){throw 'No lease result'};$bytes=[Text.Encoding]::UTF8.GetBytes(($next|ConvertTo-Json -Depth 20));$stream.Position=0;$stream.SetLength(0);$stream.Write($bytes,0,$bytes.Length);$stream.Flush($true)}finally{$stream.Dispose()}
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
Add-Type @'
using System;using System.Text;using System.Runtime.InteropServices;using System.Collections.Generic;
public class GEPProbeNormalClose {delegate bool Call(IntPtr w,IntPtr l);[DllImport("user32.dll")]static extern bool EnumWindows(Call c,IntPtr l);[DllImport("user32.dll")]static extern uint GetWindowThreadProcessId(IntPtr h,out uint p);[DllImport("user32.dll",CharSet=CharSet.Unicode)]static extern int GetWindowText(IntPtr h,StringBuilder b,int n);[DllImport("user32.dll",CharSet=CharSet.Unicode)]static extern int GetClassName(IntPtr h,StringBuilder b,int n);[DllImport("user32.dll")]static extern bool PostMessage(IntPtr h,uint m,IntPtr w,IntPtr l);public static int Close(uint wanted){var found=new List<IntPtr>();EnumWindows((h,l)=>{uint p;GetWindowThreadProcessId(h,out p);if(p==wanted){var t=new StringBuilder(1024);var c=new StringBuilder(256);GetWindowText(h,t,1024);GetClassName(h,c,256);if(c.ToString()=="Qt683QWindowIcon"&&t.ToString()=="Enfusion Workbench - Gepard stock rolling graph inspection")found.Add(h);}return true;},IntPtr.Zero);if(found.Count!=1)throw new InvalidOperationException("Expected exactly one owned project window");if(!PostMessage(found[0],0x10,IntPtr.Zero,IntPtr.Zero))throw new InvalidOperationException("Normal WM_CLOSE not posted");return found.Count;}}
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
if(!$Execute){@{status='NOT_TESTED';dryRun=$true;nativeStarted=$false;leaseMutated=$false;project=$Project;plugin=$Plugin;manifest=$Manifest;runRoot=$RunRoot;durableOutput='Fresh raw stock graph metadata console and exact owned normal-close receipts';requiredAdmission='User explicit overnight resume plus fresh atomic census/lease claim'}|ConvertTo-Json -Depth 6;exit 0}

if(HoldActive (Get-Content -LiteralPath $slot -Raw|ConvertFrom-Json)){throw 'User runtime hold active'}
if(Test-Path -LiteralPath $RunRoot){throw 'Fresh run root required'}
foreach($volume in Get-Volume -DriveLetter C,D){if($volume.SizeRemaining -lt 2GB){throw 'Below2GiB preflight'}}
$addon=[IO.Path]::GetDirectoryName($Project);$addons=[IO.Path]::GetDirectoryName($addon)
if(!(Test-Path -LiteralPath $Project -PathType Leaf)){throw 'Exact project missing'}
if($Project -cne 'D:\ArmaProjects\Gepard-Consolidated-20261022\native-audio-stage\stockRollTrace\GEP_StockRollTrace033.gproj' -or $Plugin -cne 'GEP033_TraceStockRollingPlugin'){throw 'Only private fixed stock rolling metadata probe is bound'}
$source=Get-Content -LiteralPath $Manifest -Raw|ConvertFrom-Json -AsHashtable
if(@(Get-ChildItem -LiteralPath $addon -File -Recurse).Count -ne $source.Count){throw 'Complete private trace source count differs'}
foreach($n in $source.Keys){if((Get-FileHash -LiteralPath (Join-Path $addon $n) -Algorithm SHA256).Hash.ToLowerInvariant() -ne $source[$n]){throw "Source pin differs: $n"}}
[IO.Directory]::CreateDirectory($RunRoot)|Out-Null
$profile=Join-Path $RunRoot 'profile';$logs=Join-Path $RunRoot 'logs';$downloads=Join-Path $RunRoot 'downloads'
foreach($dir in @($profile,$logs,$downloads)){[IO.Directory]::CreateDirectory($dir)|Out-Null}
$nativeArgs=@('-gproj',$Project,'-addonsDir','D:\ArmaProjects\Gepard-Consolidated-20261022\native-audio-stage,D:\SteamLibrary\steamapps\common\Arma Reforger\addons','-profile',$profile,'-logsDir',$logs,'-noBackend','-wbModule=ResourceManager',('-plugin='+$Plugin))
$nativeArgs+='-wbSilent'
$proof=@{owner='Gepard serial resource inspection coordinator';ownerKey=([Guid]::NewGuid().ToString('N')+[Guid]::NewGuid().ToString('N'));leaseId=[Guid]::NewGuid().ToString();runId=[Guid]::NewGuid().ToString();workRoot=$RunRoot;project=$Project}
Save 'owner-private.json' $proof
Save 'plan.json' @{exe=$exe;args=$nativeArgs;plugin=$Plugin;project=$Project;manifest=$Manifest;manifestSHA256=(Get-FileHash -LiteralPath $Manifest -Algorithm SHA256).Hash;seconds=$Seconds;keepOpen=[bool]$KeepOpen;physicalInput='NOT_TESTED'}
SlotTransaction {param($l)if($l.pid -or $l.activeRuntime -or $l.status -notmatch '^released' -or ($l.psobject.Properties.Name -contains 'parallelCaptures' -and $l.parallelCaptures)){throw 'Lease busy'};if(@(Inventory).Count){throw 'Native session present'};return @{owner=$proof.owner;ownerKeyHash=(KeyHash $proof.ownerKey);leaseId=$proof.leaseId;activeRuntime=$proof.runId;workRoot=$RunRoot;project=$Project;profile=$profile;pid=$null;status='starting_gepard_resource_probe';manualSession=[bool]($KeepOpen -and $Interactive);automaticTermination=$false;reservedUtc=[DateTime]::UtcNow.ToString('o')}}
$claimed=$true
try {
 $arguments=($nativeArgs|ForEach-Object {QuoteNative $_}) -join ' '
 $style='Hidden';if($Interactive){$style='Normal'}
 $game=Start-Process -FilePath $exe -WorkingDirectory ([IO.Path]::GetDirectoryName($exe)) -ArgumentList $arguments -WindowStyle $style -PassThru
 $row=Get-CimInstance Win32_Process -Filter ('ProcessId='+$game.Id) -OperationTimeoutSec 5 -ErrorAction Stop
 if(!$row -or $row.ExecutablePath -ne $exe){throw 'Attachment uncertain; retain lease'}
 $owner=@{pid=$game.Id;exe=$exe;creationUtc=$row.CreationDate.ToUniversalTime().ToString('o');managedCreationUtc=$game.StartTime.ToUniversalTime().ToString('o');project=$Project;profile=$profile;args=$nativeArgs;leaseId=$proof.leaseId}
 Save 'owner.json' $owner
 SlotTransaction {param($l)AssertOwner $l;$l.pid=$game.Id;$l.status='running_gepard_resource_probe';if($KeepOpen -and $Interactive){$l.status='running_gepard_marco_human_compare'};return $l};$attached=$true
 $until=[DateTime]::UtcNow.AddSeconds($Seconds);$closed='native-or-user-exit'
 while(AssertIdentity){
  if(HoldActive (Get-Content -LiteralPath $slot -Raw|ConvertFrom-Json)){throw 'User runtime hold active in owned loop; preserve'}
  $traceTerminal=$false;$traceComplete=$false
  $consolePath=Join-Path $logs 'console.log'
  if(Test-Path -LiteralPath $consolePath){
   $traceText=Get-Content -LiteralPath $consolePath -Raw
   $traceTerminal=$traceText.Contains('GEP033_ROLL_TRACE_END complete=')
   $traceComplete=$traceText.Contains('GEP033_ROLL_TRACE_END complete=1')
  }
  if(!$KeepOpen -and ([DateTime]::UtcNow -ge $until -or $traceTerminal)){
   $managed=Get-Process -Id $game.Id -ErrorAction Stop
   if($managed.StartTime.ToUniversalTime().Ticks -ne ([DateTime]$owner.managedCreationUtc).ToUniversalTime().Ticks){throw 'Managed birth differs'}
   if(HoldActive (Get-Content -LiteralPath $slot -Raw|ConvertFrom-Json)){throw 'User runtime hold active before close; preserve'}
   [void][GEPProbeNormalClose]::Close([uint32]$owner.pid);$closed='owned-normal-window-close';if($traceTerminal){$closed='owned-normal-window-close-after-stock-metadata-terminal'};$grace=[DateTime]::UtcNow.AddSeconds(15)
   while(AssertIdentity){if([DateTime]::UtcNow -ge $grace){throw 'Normal close incomplete; preserve owned session and lease'};Start-Sleep -Milliseconds 250};break
  }
  Start-Sleep -Milliseconds 500
 }
 $game.WaitForExit()
 if(@(Inventory).Count){throw 'Unexpected native session remains; preserve lease'}
 SlotTransaction {param($l)AssertOwner $l;if($l.pid -ne $game.Id){throw 'Lease PID changed'};$l.pid=$null;$l.activeRuntime=$null;$l.status='released_gepard_resource_probe';$l|Add-Member releasedUtc ([DateTime]::UtcNow.ToString('o')) -Force;return $l};$claimed=$false
 Save 'exit.json' @{verifiedExited=$true;leaseReleased=$true;exitCode=$game.ExitCode;closeMethod=$closed;stockMetadataComplete=$traceComplete;audibleAcceptance='NOT_TESTED';utc=[DateTime]::UtcNow.ToString('o')}
 Get-Content -LiteralPath (Join-Path $RunRoot 'exit.json')
}catch{Save 'FAILURE.json' @{status='NOT_TESTED';error=$_.Exception.Message;leaseRetained=$claimed;attached=$attached;utc=[DateTime]::UtcNow.ToString('o')};throw}

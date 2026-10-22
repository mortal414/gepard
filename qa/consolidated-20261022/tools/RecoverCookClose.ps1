param(
 [Parameter(Mandatory=$true)][string]$Project,
 [Parameter(Mandatory=$true)][ValidateSet("GEP_ArtV11RebuildFunctional")][string]$Plugin,
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
public class GEPProbeNormalClose {delegate bool Call(IntPtr w,IntPtr l);[DllImport("user32.dll")]static extern bool EnumWindows(Call c,IntPtr l);[DllImport("user32.dll")]static extern uint GetWindowThreadProcessId(IntPtr h,out uint p);[DllImport("user32.dll",CharSet=CharSet.Unicode)]static extern int GetWindowText(IntPtr h,StringBuilder b,int n);[DllImport("user32.dll",CharSet=CharSet.Unicode)]static extern int GetClassName(IntPtr h,StringBuilder b,int n);[DllImport("user32.dll")]static extern bool PostMessage(IntPtr h,uint m,IntPtr w,IntPtr l);public static int Close(uint wanted){var found=new List<IntPtr>();EnumWindows((h,l)=>{uint p;GetWindowThreadProcessId(h,out p);if(p==wanted){var t=new StringBuilder(1024);var c=new StringBuilder(256);GetWindowText(h,t,1024);GetClassName(h,c,256);if(c.ToString()=="Qt683QWindowIcon"&&t.ToString()=="Enfusion Workbench - PRIVATE Gepard V11 articulated import")found.Add(h);}return true;},IntPtr.Zero);if(found.Count!=1)throw new InvalidOperationException("Expected exactly one owned project window");if(!PostMessage(found[0],0x10,IntPtr.Zero,IntPtr.Zero))throw new InvalidOperationException("Normal WM_CLOSE not posted");return found.Count;}}
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

$proof=Get-Content -LiteralPath (Join-Path $RunRoot 'owner-private.json') -Raw|ConvertFrom-Json
$owner=Get-Content -LiteralPath (Join-Path $RunRoot 'owner.json') -Raw|ConvertFrom-Json
$nativeArgs=@($owner.args);$game=Get-Process -Id $owner.pid -ErrorAction Stop;$attached=$true;$claimed=$true
if(!(AssertIdentity)){throw 'Original importer already absent; inspect before reconciliation'}
if($game.StartTime.ToUniversalTime().Ticks -ne ([datetime]$owner.managedCreationUtc).ToUniversalTime().Ticks){throw 'Managed importer birth differs'}
if(HoldActive (Get-Content -LiteralPath $slot -Raw|ConvertFrom-Json)){throw 'User hold active; preserve'}
[void][GEPProbeNormalClose]::Close([uint32]$owner.pid)
$grace=[DateTime]::UtcNow.AddSeconds(20)
while(AssertIdentity){if([DateTime]::UtcNow -ge $grace){throw 'Normal recovered close incomplete; preserve original lease'};Start-Sleep -Milliseconds 250}
$game.WaitForExit()
if(@(Inventory).Count){throw 'Native process remains; preserve'}
SlotTransaction {param($l)AssertOwner $l;if($l.pid -ne $owner.pid){throw 'Original lease PID differs'};$l.pid=$null;$l.activeRuntime=$null;$l.status='released_gepard_resource_probe_recovered';$l|Add-Member releasedUtc ([DateTime]::UtcNow.ToString('o')) -Force;return $l}
Save 'exit-recovered.json' @{verifiedExited=$true;leaseReleased=$true;exitCode=$game.ExitCode;closeMethod='original-exact-owner-normal-WM_CLOSE-after-informational-warning';originalTimedWrapperFailurePreserved=$true;utc=[DateTime]::UtcNow.ToString('o')}
Get-Content -LiteralPath (Join-Path $RunRoot 'exit-recovered.json')

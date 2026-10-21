from pathlib import Path
p=Path(r'D:\ArmaProjects\Gepard-EveningSprint-20261021\audio-resolver-01\tools\ResolveAudio.ps1')
text=p.read_text(encoding='utf-8-sig').split('if(!$Execute)')[0]
text+=r'''
$proof=Get-Content -Raw -LiteralPath (Join-Path $RunRoot 'owner-private.json')|ConvertFrom-Json
$owner=Get-Content -Raw -LiteralPath (Join-Path $RunRoot 'owner.json')|ConvertFrom-Json
$plan=Get-Content -Raw -LiteralPath (Join-Path $RunRoot 'plan.json')|ConvertFrom-Json
$nativeArgs=@($plan.args);$game=Get-Process -Id $owner.pid -ErrorAction Stop;$attached=$true
if(!(AssertIdentity)){throw 'Owned process absent; no window action'}
Add-Type @'
using System;using System.Text;using System.Runtime.InteropServices;using System.Collections.Generic;
public class GEPProbeNormalClose {delegate bool Call(IntPtr w,IntPtr l);[DllImport("user32.dll")]static extern bool EnumWindows(Call c,IntPtr l);[DllImport("user32.dll")]static extern uint GetWindowThreadProcessId(IntPtr h,out uint p);[DllImport("user32.dll",CharSet=CharSet.Unicode)]static extern int GetWindowText(IntPtr h,StringBuilder b,int n);[DllImport("user32.dll",CharSet=CharSet.Unicode)]static extern int GetClassName(IntPtr h,StringBuilder b,int n);[DllImport("user32.dll")]static extern bool PostMessage(IntPtr h,uint m,IntPtr w,IntPtr l);public static int Close(uint wanted){var found=new List<IntPtr>();EnumWindows((h,l)=>{uint p;GetWindowThreadProcessId(h,out p);if(p==wanted){var t=new StringBuilder(1024);var c=new StringBuilder(256);GetWindowText(h,t,1024);GetClassName(h,c,256);if(c.ToString()=="Qt683QWindowIcon"&&t.ToString()=="Enfusion Workbench - Gepard 35 mm SPAA (WIP)")found.Add(h);}return true;},IntPtr.Zero);if(found.Count!=1)throw new InvalidOperationException("Expected exactly one owned project window");if(!PostMessage(found[0],0x10,IntPtr.Zero,IntPtr.Zero))throw new InvalidOperationException("Normal WM_CLOSE not posted");return found.Count;}}
'@
[void][GEPProbeNormalClose]::Close([uint32]$owner.pid)
$deadline=[DateTime]::UtcNow.AddSeconds(15)
while(AssertIdentity){if([DateTime]::UtcNow -ge $deadline){throw 'Owned hidden-window normal close incomplete; preserve lease'};Start-Sleep -Milliseconds 250}
if(@(Inventory).Count){throw 'Unexpected native session remains; preserve lease'}
SlotTransaction {param($l)AssertOwner $l;if($l.pid -ne $owner.pid){throw 'Lease PID changed'};$l.pid=$null;$l.activeRuntime=$null;$l.status='released_gepard_resource_probe_normal_recovery';$l|Add-Member releasedUtc ([DateTime]::UtcNow.ToString('o')) -Force;return $l}
Save 'RECOVERY.json' @{verifiedExited=$true;leaseReleased=$true;closeMethod='owned-hidden-project-window-normal-WM_CLOSE';originalFailureRetained=$true;utc=[DateTime]::UtcNow.ToString('o')}
Get-Content -LiteralPath (Join-Path $RunRoot 'RECOVERY.json')
'''
Path(__file__).with_name('Recover-OwnedProbe.ps1').write_text(text,encoding='utf-8',newline='\n')

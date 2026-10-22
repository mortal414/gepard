param([Parameter(Mandatory=$true)][string]$Project,[Parameter(Mandatory=$true)][string]$RunRoot,[switch]$Execute)
# Single-owner wrapper around the maintained validator; no concurrency extension.
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$slot='C:\Users\marco\Documents\Codex\2026-10-12\task\RUNTIME_SLOT.json'
$proof=$null;$claimed=$false
function KeyHash([string]$value){$h=[Security.Cryptography.SHA256]::Create();try{([BitConverter]::ToString($h.ComputeHash([Text.Encoding]::UTF8.GetBytes($value)))).Replace('-','').ToLowerInvariant()}finally{$h.Dispose()}}
function SlotTransaction([scriptblock]$change){
 $s=[IO.File]::Open($slot,[IO.FileMode]::Open,[IO.FileAccess]::ReadWrite,[IO.FileShare]::None)
 try{$reader=[IO.StreamReader]::new($s,[Text.Encoding]::UTF8,$true,1024,$true);try{$l=$reader.ReadToEnd()|ConvertFrom-Json}finally{$reader.Dispose()};if(HoldActive $l){throw 'User runtime hold active; preserve native and lease'};$next=& $change $l;if(($l.psobject.Properties.Name -contains 'userRuntimeHold') -and !($next.psobject.Properties.Name -contains 'userRuntimeHold')){if($next -is [hashtable]){$next['userRuntimeHold']=$l.userRuntimeHold}else{$next|Add-Member userRuntimeHold $l.userRuntimeHold -Force}};if(!$next){throw 'No lease result'};$b=[Text.Encoding]::UTF8.GetBytes(($next|ConvertTo-Json -Depth 15));$s.Position=0;$s.SetLength(0);$s.Write($b,0,$b.Length);$s.Flush($true)}finally{$s.Dispose()}
}
function NativeInventory { @(Get-CimInstance Win32_Process -OperationTimeoutSec 5 -ErrorAction Stop|Where-Object {$_.Name -match '^(ArmaReforger|Workbench|workbenchApp)'}) }
function HoldActive($l){return ($l.status -match '^user_runtime_hold' -or ($l.psobject.Properties.Name -contains 'userRuntimeHold' -and $l.userRuntimeHold.active -ne $false))}
function AssertOwner($l){if(!$proof -or $l.leaseId -ne $proof.leaseId -or $l.activeRuntime -ne $proof.runId -or $l.ownerKeyHash -ne (KeyHash $proof.ownerKey) -or $l.workRoot -ne $RunRoot){throw 'Owner identity differs; preserve'}}

function AssertCompleteSources705 {
 $closures=Get-Content -LiteralPath 'D:\ArmaProjects\Gepard-Consolidated-20261022\qa\consolidated-20261022\SOURCE-CLOSURES.json' -Raw | ConvertFrom-Json
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

AssertCompleteSources705
if(!$Execute){@{status='SOURCE_PINS_PASS_DRY_RUN';nativeStarted=$false;project=$Project}|ConvertTo-Json;exit 0}
if(HoldActive (Get-Content -LiteralPath $slot -Raw|ConvertFrom-Json)){throw 'User runtime hold active'}
if(Test-Path -LiteralPath $RunRoot){throw 'Fresh run root required'}
if(!(Test-Path -LiteralPath $Project -PathType Leaf)){throw 'Project missing'}
foreach($v in Get-Volume -DriveLetter C,D){if($v.SizeRemaining -lt 2GB){throw 'Preflight below2GiB'}}
[IO.Directory]::CreateDirectory($RunRoot)|Out-Null
$proof=@{owner='Gepard serial native coordinator';ownerKey=([Guid]::NewGuid().ToString('N')+[Guid]::NewGuid().ToString('N'));leaseId=[Guid]::NewGuid().ToString();runId=[Guid]::NewGuid().ToString();workRoot=$RunRoot;project=$Project}
[IO.File]::WriteAllText((Join-Path $RunRoot 'owner-private.json'),($proof|ConvertTo-Json))
SlotTransaction {param($l)
 if($l.pid -or $l.activeRuntime -or $l.status -notmatch '^released' -or ($l.psobject.Properties.Name -contains 'parallelCaptures' -and $l.parallelCaptures)){throw 'Lease busy'}
 if(@(NativeInventory).Count){throw 'Native session present'}
 return @{owner=$proof.owner;ownerKeyHash=(KeyHash $proof.ownerKey);leaseId=$proof.leaseId;activeRuntime=$proof.runId;workRoot=$RunRoot;project=$Project;profile=$null;pid=$null;status='reserved_gepard_serial_compile';reservedUtc=[DateTime]::UtcNow.ToString('o')}
};$claimed=$true
try {
 $nativeCompileArgs=@('-gproj',('"'+$Project+'"'),'-addonsDir','"D:\ArmaProjects\Gepard-Consolidated-20261022\native-stage,D:\SteamLibrary\steamapps\common\Arma Reforger\addons"','-logsDir',('"'+(Join-Path $RunRoot 'logs\validate')+'"'),'-profile',('"'+(Join-Path $RunRoot 'profile')+'"'),'-wbSilent','-wbModule=ScriptEditor','-validate','PC')
 $compilerProcess=Start-Process -FilePath 'D:\SteamLibrary\steamapps\common\Arma Reforger Tools\Workbench\ArmaReforgerWorkbenchSteamDiag.exe' -WorkingDirectory 'D:\SteamLibrary\steamapps\common\Arma Reforger Tools\Workbench' -ArgumentList $nativeCompileArgs -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $RunRoot 'launcher.stdout.txt') -RedirectStandardError (Join-Path $RunRoot 'launcher.stderr.txt')
 $compilerRow=Get-CimInstance Win32_Process -Filter ('ProcessId='+$compilerProcess.Id)
 if(!$compilerRow -or $compilerRow.ExecutablePath -ne 'D:\SteamLibrary\steamapps\common\Arma Reforger Tools\Workbench\ArmaReforgerWorkbenchSteamDiag.exe' -or !$compilerRow.CommandLine.Contains($RunRoot)){throw 'Compiler attachment uncertain; preserve'}
 [IO.File]::WriteAllText((Join-Path $RunRoot 'compiler-identity.json'),($compilerRow|Select-Object ProcessId,CreationDate,ExecutablePath,CommandLine|ConvertTo-Json))
 SlotTransaction {param($l)AssertOwner $l;$l.pid=$compilerProcess.Id;$l.status='running_gepard_serial_compile';return $l}
 if(!$compilerProcess.WaitForExit(60000)){throw 'Compiler deadline reached; preserve owned process for exact normal recovery'}
 $compilerProcess.Refresh();$LASTEXITCODE=$compilerProcess.ExitCode
 $code=$LASTEXITCODE
 [IO.File]::WriteAllText((Join-Path $RunRoot 'launcher-exit.json'),(@{exitCode=$code;utc=[DateTime]::UtcNow.ToString('o')}|ConvertTo-Json))
 if(@(NativeInventory).Count){throw 'Native process remains; preserve lease and inspect exact identity'}
 SlotTransaction {param($l)AssertOwner $l;$l.pid=$null;$l.activeRuntime=$null;$l.status='released_gepard_serial_compile';$l|Add-Member releasedUtc ([DateTime]::UtcNow.ToString('o')) -Force;return $l};$claimed=$false
 $console=Join-Path $RunRoot 'logs\validate\console.log';$text=Get-Content -LiteralPath $console -Raw
 $scriptErrors=@($text -split '\r?\n'|Where-Object {$_ -match 'SCRIPT.*\(E\)|Cannot compile|error:'})
 $passed=$code -eq 0 -and $text.Contains('Script validation successful.') -and $scriptErrors.Count -eq 0
 $result=@{status=$(if($passed){'SCRIPT_COMPILE_PASS'}else{'SCRIPT_COMPILE_FAIL'});exitCode=$code;scriptErrors=$scriptErrors;console=$console;nativeAbsent=$true;leaseReleased=$true;nativeGameplay='NOT_TESTED';shutdownErrorsRetained=$true}
 [IO.File]::WriteAllText((Join-Path $RunRoot 'RESULT.json'),($result|ConvertTo-Json -Depth 8));$result|ConvertTo-Json -Depth 8
 if(!$passed){exit 2}
} catch {
 [IO.File]::WriteAllText((Join-Path $RunRoot 'FAILURE.json'),(@{status='NOT_TESTED';error=$_.Exception.Message;leaseRetained=$claimed;utc=[DateTime]::UtcNow.ToString('o')}|ConvertTo-Json));throw
}

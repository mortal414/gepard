param([switch]$PlanOnly,[switch]$Probe)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$original='C:\Users\marco\Documents\Codex\2026-10-21\task\GEP-Private-Demo-Pack09-Smoke-Preparation-20261022'
$seal=Get-Content -LiteralPath (Join-Path $PSScriptRoot 'SOURCE-MANIFEST.json') -Raw|ConvertFrom-Json -AsHashtable
foreach($name in @('Launch-Gepard-Private-Demo09.ps1','PackageOnlyScene09.ps1')){if((Get-FileHash -LiteralPath (Join-Path $PSScriptRoot $name) -Algorithm SHA256).Hash.ToLowerInvariant() -cne $seal.files[$name]){throw 'Frozen launcher/parser successor differs'}}
foreach($entry in $seal.originalPins.GetEnumerator()){if((Get-FileHash -LiteralPath $entry.Key -Algorithm SHA256).Hash.ToLowerInvariant() -cne $entry.Value){throw 'Original Package09 proof/planner source differs'}}
$tools=Get-Content -LiteralPath 'C:\Users\marco\Documents\Codex\2026-10-21\task\GEP-Private-Demo-v3-Pack04-Preparation-20261022\TOOL-PINS.json' -Raw|ConvertFrom-Json -AsHashtable
foreach($p in @($tools.node,$tools.tsxLoader)){if((Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash.ToLowerInvariant() -cne $tools.files[$p]){throw 'Pinned Node/loader differs'}}
$uri=([Uri]::new([IO.Path]::GetFullPath($tools.tsxLoader))).AbsoluteUri
if(([Uri]$uri).LocalPath -cne [IO.Path]::GetFullPath($tools.tsxLoader)){throw 'Loader URI roundtrip differs'}
$mode='manual';if($Probe){$mode='probe'}
$json=& $tools.node --import $uri (Join-Path $original 'package_launch.mjs') plan $mode
if($LASTEXITCODE -ne 0){throw 'Original same-build Package09 plan refused'}
$plan=$json|ConvertFrom-Json
if($plan.status -cne 'PACKAGE_ONLY_PLAN_READY_NATIVE_PENDING' -or (Get-FileHash -LiteralPath $plan.plan -Algorithm SHA256).Hash.ToLowerInvariant() -cne $plan.sha256){throw 'Fresh plan identity differs'}
if($PlanOnly){$json;exit 0}
$pwsh=Join-Path $PSHOME 'pwsh.exe'
if(!(Test-Path -LiteralPath $pwsh -PathType Leaf)){throw 'Use PowerShell7'}
if($Probe){& $pwsh -NoProfile -File (Join-Path $PSScriptRoot 'PackageOnlyScene09.ps1') -PlanFile $plan.plan -PlanSHA256 $plan.sha256 -ProbeSeconds 75 -Execute}
else{& $pwsh -NoProfile -File (Join-Path $PSScriptRoot 'PackageOnlyScene09.ps1') -PlanFile $plan.plan -PlanSHA256 $plan.sha256 -Interactive -Execute}
exit $LASTEXITCODE

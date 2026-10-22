param([switch]$PlanOnly,[switch]$Probe)
# Root may install this unchanged file in the consolidated R root. Default is an
# untimed human session. Each invocation gets its own UUID4 profile/run/plan.
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$bundle='C:\Users\marco\Documents\Codex\2026-10-21\task\GEP-Private-Demo-v6-Pack08-ObservedTransfer-Preparation-20261022'
$original='C:\Users\marco\Documents\Codex\2026-10-21\task\GEP-Private-Demo-v3-Pack04-Preparation-20261022'
$manifest=Get-Content -LiteralPath (Join-Path $bundle 'DELIVERABLE-MANIFEST.json') -Raw|ConvertFrom-Json -AsHashtable
if((Get-FileHash -LiteralPath $PSCommandPath -Algorithm SHA256).Hash.ToLowerInvariant() -cne $manifest.files['Launch-Gepard-Private-Demo.ps1']){throw 'Installed launcher differs from sealed original'}
foreach($name in @('package_launch.mjs','PackageOnlyScene08.ps1')){if((Get-FileHash -LiteralPath (Join-Path $bundle $name) -Algorithm SHA256).Hash.ToLowerInvariant() -cne $manifest.files[$name]){throw 'Frozen launch source pin differs'}}
$tools=Get-Content -LiteralPath (Join-Path $original 'TOOL-PINS.json') -Raw|ConvertFrom-Json -AsHashtable
foreach($p in @($tools.node,$tools.tsxLoader)){if((Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash.ToLowerInvariant() -cne $tools.files[$p]){throw 'Pinned Node/loader bytes differ'}}
$loaderURI=([Uri]::new([IO.Path]::GetFullPath($tools.tsxLoader))).AbsoluteUri
if(([Uri]$loaderURI).LocalPath -cne [IO.Path]::GetFullPath($tools.tsxLoader)){throw 'Exact loader URI round-trip differs'}
$mode='manual';if($Probe){$mode='probe'}
$json=& $tools.node --import $loaderURI (Join-Path $bundle 'package_launch.mjs') plan $mode
if($LASTEXITCODE -ne 0){throw 'Verified package session planning failed; no native started'}
$plan=$json|ConvertFrom-Json
if($plan.status -cne 'PACKAGE_ONLY_PLAN_READY_NATIVE_PENDING'){throw 'Unexpected native session plan status'}
if((Get-FileHash -LiteralPath $plan.plan -Algorithm SHA256).Hash.ToLowerInvariant() -cne $plan.sha256){throw 'Fresh session plan pin differs'}
if($PlanOnly){$json;exit 0}
# Fresh PowerShell process preserves repeatability of the existing native argv
# parser Add-Type. It does not reuse an already-defined GEPSceneArgs type.
$pwsh=Join-Path $PSHOME 'pwsh.exe'
if(!(Test-Path -LiteralPath $pwsh -PathType Leaf)){throw 'Use PowerShell7 for this launcher'}
if($Probe){& $pwsh -NoProfile -File (Join-Path $bundle 'PackageOnlyScene08.ps1') -PlanFile $plan.plan -PlanSHA256 $plan.sha256 -ProbeSeconds 75 -Execute}
else{& $pwsh -NoProfile -File (Join-Path $bundle 'PackageOnlyScene08.ps1') -PlanFile $plan.plan -PlanSHA256 $plan.sha256 -Interactive -Execute}
exit $LASTEXITCODE

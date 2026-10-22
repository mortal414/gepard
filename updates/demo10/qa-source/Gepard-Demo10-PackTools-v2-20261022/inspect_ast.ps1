$ErrorActionPreference='Stop'
$rows=@()
foreach($name in @('SerialValidatePack10.ps1','PackageOnlyScene10.ps1','Launch-Gepard-Private-Demo10.ps1','NativeTailClassifier10.ps1','NativeTailClassifier10-v2.ps1')){
 $p=Join-Path $PSScriptRoot $name;$tokens=$null;$errors=$null
 $null=[Management.Automation.Language.Parser]::ParseFile($p,[ref]$tokens,[ref]$errors)
 $rows+=@{path=$name;sha256=(Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash.ToLowerInvariant();pass=($errors.Count -eq 0);errors=@($errors|ForEach-Object {$_.Message})}
 if($errors.Count){throw "AST failed $name"}
}
$rows|ConvertTo-Json -Depth 6|Set-Content -LiteralPath (Join-Path $PSScriptRoot 'PS-AST-CHECKS.json') -Encoding utf8
Write-Output 'PASS five PowerShell ASTs; no native execution'

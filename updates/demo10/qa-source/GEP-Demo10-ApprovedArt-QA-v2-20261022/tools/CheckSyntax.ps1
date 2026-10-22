$ErrorActionPreference='Stop'
$qaPackage=Split-Path $PSScriptRoot -Parent
$rows=@()
foreach($name in @('Scene.ps1','Compile.ps1')) {
 $qaTokens=$null;$qaErrors=$null
 [System.Management.Automation.Language.Parser]::ParseFile((Join-Path $PSScriptRoot $name),[ref]$qaTokens,[ref]$qaErrors)|Out-Null
 if($qaErrors.Count){throw ($qaErrors|Out-String)}
 $rows+=@{file=$name;parseErrors=0;executed=$false}
}
@{status='POWERSHELL_PARSE_ONLY_PASS';rows=$rows;nativeStarted=$false;leaseRead=$false;DWritten=$false}|ConvertTo-Json -Depth 8|Set-Content -LiteralPath (Join-Path $qaPackage 'WRAPPER-SYNTAX.json') -Encoding utf8

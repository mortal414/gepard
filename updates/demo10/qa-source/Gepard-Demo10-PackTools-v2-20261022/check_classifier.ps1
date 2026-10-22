$ErrorActionPreference='Stop'
$r=$PSScriptRoot
. "$r\NativeTailClassifier10-v2.ps1"
$baseline="$r\NATIVE-TAIL-BASELINE-v2.json"
$b=Get-Content $baseline -Raw|ConvertFrom-Json
$text=Get-Content $b.actualDemo10Compile.console -Raw
if((Get-FileHash $b.actualDemo10Compile.console -Algorithm SHA256).Hash.ToLowerInvariant() -cne $b.actualDemo10Compile.consoleSHA256){throw 'Actual console changed'}
$rows=@()
function Check([string]$Name,[string]$Text,[bool]$Expected,[int]$Exit=0,[string]$Step='validate',[bool]$Accepted=$false){
 $x=ClassifyKnownNativeTail -Text $Text -StepName $Step -ExitCode $Exit -BaselinePath $baseline -ValidationWasAccepted $Accepted
 if($x.operationalPass -ne $Expected){throw "$Name expected=$Expected actual=$($x.operationalPass) reason=$($x.reason)"}
 if($x.rawNativeErrorCount -gt 0 -and $x.strictNativeVerdict -cne 'FAIL'){throw 'Strict FAIL lost'}
 $script:rows+=@{name=$Name;expectedOperationalPass=$Expected;actual=$x}
}
Check 'actual27_exact_pair' $text $true
$lines=@($text -split '\r?\n');$w=$lines[101];$sp=$lines[103]
Check 'repeat_space' ($text.Replace($sp,"$sp`n$sp")) $false
Check 'space_only' ($text.Replace($w,'')) $false
Check 'novel_runtime_conf' ($text.Replace('wcs_spacecore_localization.st','wcs_spacecore_localization.de_de.conf')) $false
Check 'reverse_pair' ($text.Replace($w,'SWAP_WARNING').Replace($sp,$w).Replace('SWAP_WARNING',$sp)) $false
Check 'known_wcs_only' ($text.Replace($sp,'')) $true
Check 'known_tail_only' ($text.Replace($sp,'').Replace($w,'')) $true
Check 'nonzero_exit' $text $false 1
Check 'missing_success' ($text.Replace('Script validation successful.','Script validation incomplete.')) $false
Check 'novel_tail' ($text.Replace('==== Resource leaks ====','==== Novel failure ====')) $false
Check 'late_extra_line' ($text+"`n13:12:08.600 RESOURCES (E): novel") $false
$pack=$text.Replace('SCRIPT       : Script validation successful.','RESOURCES    : Packaging project successful')
# Replace flexible spacing using the same exact native success body semantics.
$pack=[regex]::Replace($text,'SCRIPT\s+: Script validation successful\.','RESOURCES : Packaging project successful')
Check 'pack_without_validation' $pack $false 0 'pack' $false
Check 'pack_samebatch_validation' $pack $true 0 'pack' $true
$rows|ConvertTo-Json -Depth 12|Set-Content -LiteralPath "$r\CLASSIFIER-CHECKS.json" -Encoding utf8
Write-Output "PASS $($rows.Count) finite boundary checks; actual raw27 strictFAIL retained"

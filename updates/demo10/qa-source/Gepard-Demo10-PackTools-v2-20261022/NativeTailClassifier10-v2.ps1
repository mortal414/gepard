# Addon-owned finite classifier; pure text/data only. Not a native lifecycle controller.
function ClassifyKnownNativeTail {
 param([Parameter(Mandatory=$true)][string]$Text,[Parameter(Mandatory=$true)][ValidateSet('validate','pack')][string]$StepName,[int]$ExitCode,[Parameter(Mandatory=$true)][string]$BaselinePath,[bool]$ValidationWasAccepted=$false)
 $b=Get-Content -LiteralPath $BaselinePath -Raw|ConvertFrom-Json
 if($b.schema -ne 1 -or $b.signatureCount -ne 25 -or $b.signatures.Count -ne 25){throw 'Finite tail baseline malformed'}
 $lines=@($Text -split '\r?\n');$errors=@();$success=@();$destroyed=@();$parsed=@{}
 for($i=0;$i -lt $lines.Count;$i++){
  $line=$lines[$i]
  if($line -match '^([0-9]{2}):([0-9]{2}):([0-9]{2})\.([0-9]{3})\s+(.*)$'){
   $hh=[int]$Matches[1];$mm=[int]$Matches[2];$ss=[int]$Matches[3];$ms=[int]$Matches[4];$body=$Matches[5]
   if($hh -le 23 -and $mm -le 59 -and $ss -le 59){$parsed[$i]=@{body=$body;ms=((($hh*60+$mm)*60+$ss)*1000+$ms)}}
  }
  if($line -match '\b[A-Z][A-Z0-9_]*\s*\([EF]\)|Cannot compile|error:'){$errors+=@{line=$i+1;text=$line;index=$i}}
  if($parsed.ContainsKey($i)){
   $body=$parsed[$i].body
   if(($StepName -eq 'validate' -and $body -cmatch '^SCRIPT\s+: Script validation successful\.$') -or ($StepName -eq 'pack' -and $body -cmatch '^RESOURCES\s+: Packaging project successful$')){$success+=$i}
   if($body -cmatch '^ENGINE\s+: Game destroyed\.$'){$destroyed+=$i}
  }
 }
 $result=[ordered]@{schema=1;operationalPass=$false;operationalVerdict='FAIL';strictNativeVerdict=$(if($errors.Count){'FAIL'}else{'PASS'});rawNativeErrorCount=$errors.Count;knownBaselineTeardownCount=0;knownDependencyAuthoringWarningCount=0;operationalErrorCount=$errors.Count;errors=$errors;classification='UNQUALIFIED';reason='';successLine=$null;shutdownLines=@();baselineSignaturesSHA256=$b.signaturesSHA256;step=$StepName;exitCode=$ExitCode;validationWasAccepted=$ValidationWasAccepted}
 if($ExitCode -ne 0){$result.reason='Nonzero actual native exit';return [pscustomobject]$result}
 if($success.Count -ne 1){$result.reason='Missing or repeated exact step success marker';return [pscustomobject]$result}
 if($StepName -eq 'pack' -and !$ValidationWasAccepted){$result.reason='Pack lacks accepted same-batch preceding native validation';return [pscustomobject]$result}
 $result.successLine=$success[0]+1
 if(!$errors.Count){$result.operationalPass=$true;$result.operationalVerdict='PASS';$result.operationalErrorCount=0;$result.classification='NO_NATIVE_ERRORS';return [pscustomobject]$result}
 if($b.dependencyAuthoringWarning.addonId -cne '629B2BA37EFFD577' -or $b.dependencyAuthoringWarning.version -cne '8.4.0' -or $b.dependencyAuthoringWarnings.Count -ne 2 -or $b.dependencyAuthoringWarnings[1].addonId -cne '5E389BB9F58B79A6'){$result.reason='Dependency source/version mismatch';return [pscustomobject]$result}
 $sourceWarnings=@($errors|Where-Object {$parsed.ContainsKey($_.index) -and $parsed[$_.index].body -ceq $b.dependencyAuthoringWarnings[0].signature})
 $spaceWarnings=@($errors|Where-Object {$parsed.ContainsKey($_.index) -and $parsed[$_.index].body -ceq $b.dependencyAuthoringWarnings[1].signature})
 if($sourceWarnings.Count -gt 1 -or $spaceWarnings.Count -gt 1 -or ($spaceWarnings.Count -eq 1 -and $sourceWarnings.Count -ne 1)){$result.reason='Repeated/unsupported dependency authoring warning combination';return [pscustomobject]$result}
 $authorCount=$sourceWarnings.Count+$spaceWarnings.Count
 if($authorCount){
  if($errors.Count -ne (25+$authorCount) -or $errors[0].index -ne $sourceWarnings[0].index -or $sourceWarnings[0].index -ge $success[0]){$result.reason='Dependency warning position/count differs';return [pscustomobject]$result}
  if($spaceWarnings.Count -eq 1 -and ($errors[1].index -ne $spaceWarnings[0].index -or $spaceWarnings[0].index -le $sourceWarnings[0].index -or $spaceWarnings[0].index -ge $success[0])){$result.reason='Exact observed WCS then SpaceCore warning order differs';return [pscustomobject]$result}
  $result.knownDependencyAuthoringWarningCount=$authorCount
 }
 # Every raw error remains in result.errors and strictFAIL. Only exact source-bound
 # authoring diagnostics are separated from the unchanged finite25 shutdown tail.
 $warningIndices=@($sourceWarnings|ForEach-Object {$_.index})+@($spaceWarnings|ForEach-Object {$_.index})
 $tailErrors=@($errors|Where-Object {$_.index -notin $warningIndices})
 if($tailErrors.Count -ne 25){$result.reason='Native tail/error count differs from finite25-row baseline';return [pscustomobject]$result}
 if($destroyed.Count -ne 2 -or $destroyed[0] -le $success[0] -or $destroyed[1] -ne ($destroyed[0]+1)){$result.reason='Exact ordered adjacent double Game destroyed boundary absent';return [pscustomobject]$result}
 $result.shutdownLines=@($destroyed|ForEach-Object {$_+1})
 $first=$tailErrors[0].index
 if($first -ne ($destroyed[1]+1)){$result.reason='Error tail not immediately after native shutdown';return [pscustomobject]$result}
 $tail=@();for($i=$first;$i -lt $lines.Count;$i++){if($lines[$i] -ne ''){$tail+=$i}}
 if($tail.Count -ne 25){$result.reason='Meaningful data follows or interrupts finite EOF tail';return [pscustomobject]$result}
 $lastMs=$parsed[$destroyed[1]].ms
 if($lastMs -lt $parsed[$success[0]].ms){$result.reason='Shutdown timestamp predates step success';return [pscustomobject]$result}
 for($n=0;$n -lt 25;$n++){
  $i=$tail[$n]
  if($i -ne $tailErrors[$n].index -or !$parsed.ContainsKey($i)){$result.reason='Unlocated/untimestamped error or non-error in tail';return [pscustomobject]$result}
  if($parsed[$i].body -cne $b.signatures[$n]){$result.reason='Novel/reordered/changed exact diagnostic signature';return [pscustomobject]$result}
  if($parsed[$i].ms -lt $lastMs -or ($parsed[$i].ms-$parsed[$destroyed[1]].ms) -gt 1000){$result.reason='Tail timestamp outside ordered one-second shutdown window';return [pscustomobject]$result}
  $lastMs=$parsed[$i].ms
 }
 $result.operationalPass=$true;$result.operationalVerdict='PASS_WITH_EXACT_BASELINE_TEARDOWN';$result.knownBaselineTeardownCount=25;$result.operationalErrorCount=0;$result.classification=$(if($result.knownDependencyAuthoringWarningCount -eq 2){'EXACT_KNOWN_WCS_SPACECORE_AUTHORING_AND_NATIVE_TEARDOWN_RETAINED'}elseif($result.knownDependencyAuthoringWarningCount -eq 1){'EXACT_KNOWN_WCS_AUTHORING_AND_NATIVE_TEARDOWN_RETAINED'}else{'EXACT_BASELINE_NATIVE_TEARDOWN_RETAINED'});$result.reason='Actual step success/normalexit and exact known source-bound diagnostic categories; every raw row and strictFAIL retained, no clean-health/gameplay claim'
 return [pscustomobject]$result
}

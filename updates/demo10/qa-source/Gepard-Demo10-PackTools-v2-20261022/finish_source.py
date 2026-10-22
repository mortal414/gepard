from pathlib import Path
import json,hashlib,re,difflib
r=Path(__file__).resolve().parent
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def read(p):return json.loads(Path(p).read_text(encoding='utf-8-sig'))
def save(p,d):Path(p).write_text(json.dumps(d,indent=2)+'\n',encoding='utf-8',newline='\n')
old=r/'NativeTailClassifier10.ps1';s=old.read_text()
start=s.index(" if($b.dependencyAuthoringWarning.addonId")
end=s.index(" if($tailErrors.Count -ne 25)",start)
s=s[:start]+''' if($b.dependencyAuthoringWarning.addonId -cne '629B2BA37EFFD577' -or $b.dependencyAuthoringWarning.version -cne '8.4.0' -or $b.dependencyAuthoringWarnings.Count -ne 2 -or $b.dependencyAuthoringWarnings[1].addonId -cne '5E389BB9F58B79A6'){$result.reason='Dependency source/version mismatch';return [pscustomobject]$result}
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
 $tailErrors=@($errors|Where-Object {$_.index -notin @($sourceWarnings.index+$spaceWarnings.index)})
'''+s[end:]
s=s.replace("if($result.knownDependencyAuthoringWarningCount -eq 1){'EXACT_KNOWN_WCS_AUTHORING_AND_NATIVE_TEARDOWN_RETAINED'}else", "if($result.knownDependencyAuthoringWarningCount -eq 2){'EXACT_KNOWN_WCS_SPACECORE_AUTHORING_AND_NATIVE_TEARDOWN_RETAINED'}elseif($result.knownDependencyAuthoringWarningCount -eq 1){'EXACT_KNOWN_WCS_AUTHORING_AND_NATIVE_TEARDOWN_RETAINED'}else")
(r/'NativeTailClassifier10-v2.ps1').write_text(s,encoding='utf-8',newline='\n')
b=read(r/'NATIVE-TAIL-BASELINE.json');evidence=read(r/'DEPENDENCY-LOCALIZATION-INDEX.json')
b['classifierRevision']='DEMO10_EXACT_WCS_THEN_SPACECORE_PAIR_V2'
b['dependencyAuthoringWarnings']=[b['dependencyAuthoringWarning'],{'signature':'RESOURCES (E): Wrong GUID/name for resource @"{84270E09F7A4A7A0}Language/wcs_spacecore_localization.st" in property "StringTableSource"','addonId':'5E389BB9F58B79A6','projectSHA256':evidence['results'][1]['projectSHA256'],'pakSHA256':evidence['results'][1]['pakSHA256'],'scope':'Exact observed editor source warning, index-only evidence; runtime localization behavior not claimed'}]
b['dependencyIndexEvidence']={'path':str(r/'DEPENDENCY-LOCALIZATION-INDEX.json'),'sha256':sha(r/'DEPENDENCY-LOCALIZATION-INDEX.json')}
b['actualDemo10Compile']={'console':r'D:\ArmaProjects\Gepard-Demo10-TrackArt-20261022\native-runs\compile-demo10-core-01\logs\validate\console.log','consoleSHA256':'056847878a0f8d1fb00b1ba35baac1ad117ce50276210af01cc4647c14c1785b','warningLines':[102,104],'successLine':267,'shutdownLines':[287,288],'tailLines':[289,313],'strictNativeVerdict':'FAIL','rawErrorCount':27}
save(r/'NATIVE-TAIL-BASELINE-v2.json',b)
p=r/'make_plan10.mjs';s=p.read_text().replace('NativeTailClassifier10.ps1','NativeTailClassifier10-v2.ps1').replace('NATIVE-TAIL-BASELINE.json','NATIVE-TAIL-BASELINE-v2.json');p.write_text(s,encoding='utf-8',newline='\n')
p=r/'package_launch.mjs';s=p.read_text().replace('[25,26].includes(raw.length)','[25,26,27].includes(raw.length)').replace('[0,1].includes(authorCount)','[0,1,2].includes(authorCount)')
needle="   if(authorCount)insist(normalized[0]===baseline.dependencyAuthoringWarning.signature&&d.classification==='EXACT_KNOWN_WCS_AUTHORING_AND_NATIVE_TEARDOWN_RETAINED','Dependency warning not exact/source-bound');"
assert needle in s
s=s.replace(needle,"   if(authorCount)insist(normalized[0]===baseline.dependencyAuthoringWarnings[0].signature,'WCS warning not exact/source-bound');\n   if(authorCount===1)insist(d.classification==='EXACT_KNOWN_WCS_AUTHORING_AND_NATIVE_TEARDOWN_RETAINED','Single warning classification differs');\n   if(authorCount===2)insist(normalized[1]===baseline.dependencyAuthoringWarnings[1].signature&&d.classification==='EXACT_KNOWN_WCS_SPACECORE_AUTHORING_AND_NATIVE_TEARDOWN_RETAINED','Exact ordered WCS/SpaceCore pair differs');")
p.write_text(s,encoding='utf-8',newline='\n')
p=r/'check_static.py';s=p.read_text().replace('ownershipFunctionsByteExact','ownershipFunctionsTextExact').replace('startupBoardSpaceParserByteExact','startupBoardSpaceParserTextExact').replace("'finalSourcesAndCompile':'PENDING_ROOT'","'finalSourcesAndCompile':'ACTUAL_ROOT_ADOPTED_SIX_CLOSURES' ");p.write_text(s,encoding='utf-8',newline='\n')
print('Versioned exact-pair rule prepared; originals preserved')

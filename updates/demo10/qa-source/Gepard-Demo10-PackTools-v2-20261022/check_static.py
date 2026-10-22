from pathlib import Path
import json,re,ast,hashlib
r=Path(__file__).resolve().parent;t=r.parent
old=t/'GEP-Private-Demo-Pack09-Smoke-Preparation-20261022'
launcher=Path(r'D:\ArmaProjects\Gepard-Consolidated-20261022\updates\demo09\launcher')
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def read(p):return json.loads(Path(p).read_text(encoding='utf-8-sig'))
def functions(s):
 matches=list(re.finditer(r'(?m)^function ([A-Za-z0-9_]+)\b',s));out={}
 for i,m in enumerate(matches):
  e=matches[i+1].start() if i+1<len(matches) else len(s)
  # All ownership helpers precede Add-Type or a next function.
  text=s[m.start():e];text=text.split("Add-Type -TypeDefinition @'",1)[0]
  out[m[1]]=text.rstrip()
 return out
guards=['KeyHash','Inventory','OpenExclusiveLeaseWriter','SlotTransaction','AssertOwner','QuoteNative','AssertIdentity','UserRuntimeHoldRecord705','UserRuntimeHold705','CancelRequested705']
rows=[]
for candidate,origin in [('SerialValidatePack10.ps1',old/'SerialValidatePack09.ps1'),('PackageOnlyScene10.ps1',launcher/'PackageOnlyScene09.ps1')]:
 a=functions((r/candidate).read_text());b=functions(origin.read_text(encoding='utf-8-sig'))
 for name in guards:assert a[name]==b[name],(candidate,name)
 rows.append({'file':candidate,'ownershipFunctionsTextExact':guards})
a=functions((r/'PackageOnlyScene10.ps1').read_text())['ReadWorldStartup10'].replace('ReadWorldStartup10','ReadWorldStartup09')
b=functions((launcher/'PackageOnlyScene09.ps1').read_text())['ReadWorldStartup09'];assert a==b
assert sha(r/'NativeTailClassifier10.ps1')==sha(old/'NativeTailClassifier09.ps1')
assert sha(r/'NATIVE-TAIL-BASELINE.json')==sha(old/'NATIVE-TAIL-BASELINE.json')
serial=(r/'SerialValidatePack10.ps1').read_text();assert "@('integration:validate','integration:pack','provingGround:validate','provingGround:pack')" in serial
assert '$expectedSupportCount=1152' in serial and '$expectedSupportCount=14' in serial
scene=(r/'PackageOnlyScene10.ps1').read_text();assert '$files.Count -ne 12' in scene and '$wcsClosure.files.Count -ne 6' in scene
assert "'-addons','D561AD08932E7B40,7E0150C026101500,5E51C5D8F04FDB80,A76A420022000001,5E389BB9F58B79A6,629B2BA37EFFD577'" in scene
for p in r.glob('*.py'):ast.parse(p.read_text())
for x in read(r/'PS-AST-CHECKS.json'):assert x['pass'] and not x['errors'] and sha(r/x['path'])==x['sha256']
for name,role,count in [('CORE08-TRIO-REUSE-INPUT.json','gepard',1152),('SMOKE09-TRIO-REUSE-INPUT.json','wcsSmoke',14)]:
 d=read(r/name);assert d['role']==role and len(d['files'])==3 and sha(d['originalSameBuildProof']['proofPath'])==d['originalSameBuildProof']['proofSHA256']
 for n,h in d['files'].items():assert sha(Path(d['root'])/n)==h
 assert len(read(d['currentSourceManifest']))==count and sha(d['currentSourceManifest'])==d['currentSourceManifestSHA256']
dep=read(r/'SEPARATE-DEPENDENCIES.json');assert len(dep['files'])==6 and len(dep['closures'])==2
for n,h in dep['files'].items():assert sha(Path(dep['root'])/n)==h
pending=read(r/'PENDING-GATE-CHECK.json');assert pending['status']=='PASS_PENDING_SOURCE_REFUSAL_AND_RESOURCE_GUARDS'
report={'status':'PASS_STATIC_DERIVATION_AST_SOURCE_AND_PENDING_GATE_ONLY','ownership':rows,'startupBoardSpaceParserTextExact':True,'finiteTailClassifierAndBaselineByteExact':True,'reuseOwnTrios':2,'supportingSourceCounts':[1152,14],'dependencyCounts':[3,3],'newPackedRoles':['integration','provingGround'],'exactFourNativeStepsBound':True,'ownInstalledFilesBound':12,'separateDependencyFilesBound':6,'noBradleyDirectoryOrDependencyEdgeAllowed':True,'nativeStarted':False,'packaged':False,'installed':False,'finalSourcesAndCompile':'ACTUAL_ROOT_ADOPTED_SIX_CLOSURES' }
(r/'STATIC-CHECKS.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8',newline='\n');print(json.dumps(report,indent=2))

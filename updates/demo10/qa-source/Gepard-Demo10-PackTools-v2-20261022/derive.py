from pathlib import Path
import json,hashlib,difflib,re
r=Path(__file__).resolve().parent;t=r.parent
old=t/'GEP-Private-Demo-Pack09-Smoke-Preparation-20261022'
launcher=Path(r'D:\ArmaProjects\Gepard-Consolidated-20261022\updates\demo09\launcher')
R=Path(r'D:\ArmaProjects\Gepard-Demo10-TrackArt-20261022');stage=R/'private-demo-source10'
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def read(p):return json.loads(Path(p).read_text(encoding='utf-8-sig'))
def write(n,v):
 p=r/n;assert not p.exists(),str(p);p.parent.mkdir(parents=True,exist_ok=True);p.write_text(v if isinstance(v,str) else json.dumps(v,indent=2)+'\n',encoding='utf-8',newline='\n')
assert not (r/'SOURCE-MANIFEST.json').exists()
inputs={};derivations=[]
def source(p):inputs[str(p)]=sha(p);return p.read_text(encoding='utf-8-sig')
def derived(name,p,after):
 before=source(p);write(name,after)
 write(name+'.patch',''.join(difflib.unified_diff(before.splitlines(True),after.splitlines(True),fromfile=str(p),tofile='Demo10/'+name)))
 derivations.append({'source':str(p),'sourceSHA256':sha(p),'target':name,'targetSHA256':sha(r/name)})
def replace(s,a,b):assert s.count(a)==1,(a,s.count(a));return s.replace(a,b)
support=(r/'supporting.fragment.mjs').read_text();reuse=(r/'reuse.fragment.mjs').read_text()
s=source(old/'pack_tools09.mjs')
a=s.index('function supporting(plan){');b=s.index('export function sourceCheck',a);s=s[:a]+support+s[b:]
s=s.replace("{wcsSmoke:'7E0150C026101500',provingGround:'A76A420022000001'}","{integration:'5E51C5D8F04FDB80',provingGround:'A76A420022000001'}")
s=s.replace("project.role==='wcsSmoke'","project.role==='integration'").replace("role==='wcsSmoke'","role==='integration'").replace("['wcsSmoke','provingGround']","['integration','provingGround']").replace('pack_tools09.mjs','pack_tools10.mjs')
s=replace(s," supporting(c);\n insist(c.schema===1&&c.status==='ROOT_FINAL_SOURCE_READY_FOR_PRIVATE_PACK','Final source staging/pins still pending root');"," insist(c.schema===1&&c.status==='ROOT_FINAL_SOURCE_READY_FOR_PRIVATE_PACK','Final source staging/pins still pending root');\n admission(c);supporting(c);")
s=replace(s," const ordered=['integration','provingGround'].map(role=>c.projects.find(p=>p.role===role));"," for(const f of [c.actualCompileReceipt,c.rootReview])tools.files[f]=sha(f);\n const actualAdmission=read(c.actualCompileReceipt);for(const f of [actualAdmission.resultPath,actualAdmission.consolePath,path.join(path.dirname(actualAdmission.resultPath),'owner-private.json')])tools.files[f]=sha(f);\n for(const role of ['gepard','wcsSmoke']){const input=c.reusedTrioInputs[role],p=read(input.path);for(const f of [input.path,p.currentSourceManifest,p.originalSameBuildProof.proofPath,...Object.keys(p.files).map(n=>path.join(p.root,n)),...Object.values(read(p.originalSameBuildProof.proofPath).trio).map(x=>x.path)])tools.files[f]=sha(f);}\n const ordered=['integration','provingGround'].map(role=>c.projects.find(p=>p.role===role));")
s=replace(s," const index=await readPakIndex(pak);const resources=", " const index=await readPakIndex(pak);const ownedSource=new Set(Object.keys(read(p.sourceManifest)).map(n=>n.toLowerCase()));for(const entry of index.entries)insist(ownedSource.has(entry.resource.toLowerCase()),'Foreign/dependency resource included in owned PAK: '+entry.resource);const resources=")
derived('pack_tools10.mjs',old/'pack_tools09.mjs',s)
s=source(old/'make_plan09.mjs').replace('pack_tools09','pack_tools10').replace('SerialValidatePack09','SerialValidatePack10').replace('NativeTailClassifier09','NativeTailClassifier10').replace('ROOT-PACK-PLAN-09','ROOT-PACK-PLAN-10').replace('PLAN09-RESULT','PLAN10-RESULT').replace('PACK09','DEMO10').replace('CORE08-TRIO-REUSE-INPUT.json\'','CORE08-TRIO-REUSE-INPUT.json\',\'SMOKE09-TRIO-REUSE-INPUT.json\'')
derived('make_plan10.mjs',old/'make_plan09.mjs',s)
s=source(old/'SerialValidatePack09.ps1').replace("@('wcsSmoke:validate','wcsSmoke:pack','provingGround:validate','provingGround:pack')","@('integration:validate','integration:pack','provingGround:validate','provingGround:pack')")
a="  if($corePins.Count -ne 1152 -or @(Get-ChildItem -LiteralPath $support.root -Recurse -File).Count -ne 1152){throw 'Exact qualified1152 source required'}"
b="  $expectedSupportCount=0;if($support.role -ceq 'gepard'){$expectedSupportCount=1152}elseif($support.role -ceq 'wcsSmoke'){$expectedSupportCount=14}else{throw 'Unexpected supporting source role'}\n  if($corePins.Count -ne $expectedSupportCount -or @(Get-ChildItem -LiteralPath $support.root -Recurse -File).Count -ne $expectedSupportCount){throw 'Exact qualified supporting source count required'}"
s=replace(s,a,b);derived('SerialValidatePack10.ps1',old/'SerialValidatePack09.ps1',s)
for name,newname in [('NativeTailClassifier09.ps1','NativeTailClassifier10.ps1'),('NATIVE-TAIL-BASELINE.json','NATIVE-TAIL-BASELINE.json')]:
 source(old/name);assert not (r/newname).exists();(r/newname).write_bytes((old/name).read_bytes())
s=source(old/'package_launch.mjs').replace('pack_tools09','pack_tools10')
s=replace(s,"const R='D:\\\\ArmaProjects\\\\Gepard-Consolidated-20261022';","const R='D:\\\\ArmaProjects\\\\Gepard-Demo10-TrackArt-20261022';")
s=s.replace('private-installed-demo-09','private-installed-demo-10').replace('private-pack-09','private-pack-10').replace('private-package-build-09','private-package-build-10').replace('private-demo-09-sessions','private-demo-10-sessions').replace('ROOT-PACK-PLAN-09','ROOT-PACK-PLAN-10').replace('PACKAGE09-WORLD-CONTRACT','PACKAGE10-WORLD-CONTRACT').replace('PackageOnlyScene09','PackageOnlyScene10')
s=replace(s,"const expected={gepard:'D561AD08932E7B40',wcsSmoke:'7E0150C026101500',provingGround:'A76A420022000001'};","const expected={gepard:'D561AD08932E7B40',wcsSmoke:'7E0150C026101500',integration:'5E51C5D8F04FDB80',provingGround:'A76A420022000001'};")
s=replace(s,"const wcs='629B2BA37EFFD577',wcsRoot=path.join(R,'private-smoke-demo-source-v2','dependencies');","const wcs='629B2BA37EFFD577',space='5E389BB9F58B79A6',dependencyRoot=path.join(R,'private-demo-source10','dependencies');")
a=s.index('function dependency(){');b=s.index('function qualifyPackDiagnostics',a);s=s[:a]+reuse+s[b:]
s=s.replace('rows=[reusedCore()]','rows=[reused(\'gepard\'),reused(\'wcsSmoke\')]').replace("['wcsSmoke','provingGround']","['integration','provingGround']").replace('wcsSmoke:validate,wcsSmoke:pack','integration:validate,integration:pack')
s=s.replace('length===9','length===12').replace('nine own addon files','twelve own addon files').replace('three own addon GUIDs','four own addon GUIDs').replace('Only exact three','Only exact four').replace('OWN_THREE_TRIOS_INSTALLED_WCS_SEPARATE_NO_NATIVE','OWN_FOUR_TRIOS_INSTALLED_TWO_DEPS_SEPARATE_NO_NATIVE')
s=s.replace('verifyInstalledAddons([...Object.values(expected),wcs],[installed,wcsRoot]);','verifyInstalledAddons([...Object.values(expected),space,wcs],[installed,dependencyRoot]);')
s=s.replace('addonIds:[expected.gepard,expected.wcsSmoke,expected.provingGround,wcs],addonsDirs:[installed,wcsRoot,plan.coreAddonRoot]','addonIds:[expected.gepard,expected.wcsSmoke,expected.integration,expected.provingGround,space,wcs],addonsDirs:[installed,dependencyRoot,plan.coreAddonRoot]')
s=s.replace('WCSCopiedIntoOwnPayload:false','WCSCopiedIntoOwnPayload:false,SpaceCoreCopiedIntoOwnPayload:false')
derived('package_launch.mjs',old/'package_launch.mjs',s)
s=source(launcher/'PackageOnlyScene09.ps1')
s=s.replace('PackageOnlyScene09','PackageOnlyScene10').replace('ReadWorldStartup09','ReadWorldStartup10').replace(str(old),str(r)).replace('PACKAGE09-WORLD-CONTRACT','PACKAGE10-WORLD-CONTRACT')
s=s.replace('D:\\ArmaProjects\\Gepard-Consolidated-20261022','D:\\ArmaProjects\\Gepard-Demo10-TrackArt-20261022').replace('private-installed-demo-09','private-installed-demo-10').replace('private-demo-09-sessions','private-demo-10-sessions').replace('private-smoke-demo-source-v2','private-demo-source10')
s=s.replace('$files.Count -ne 9 -or $pins.files.Count -ne 9','$files.Count -ne 12 -or $pins.files.Count -ne 12').replace('Exact three own installed trios required','Exact four own installed trios required')
s=s.replace('$wcsClosure.files.Count -ne 3 -or @(Get-ChildItem -LiteralPath $wcsClosure.root -Recurse -File).Count -ne 3','$wcsClosure.files.Count -ne 6 -or @(Get-ChildItem -LiteralPath $wcsClosure.root -Recurse -File).Count -ne 6')
s=s.replace('Only exact separate installed WCS trio permitted','Only exact separate SpaceCore and WCS trios permitted')
needle=" foreach($relativeWCS in $wcsClosure.files.Keys)"
assert s.count(needle)==1
s=s.replace(needle," if((@(Get-ChildItem -LiteralPath $wcsClosure.root -Directory | ForEach-Object {$_.Name} | Sort-Object) -join ',') -cne 'SpaceCore_5E389BB9F58B79A6,WCS_Armaments_629B2BA37EFFD577'){throw 'Unexpected/Bradley dependency directory'}\n"+needle)
s=s.replace("'-addons','D561AD08932E7B40,7E0150C026101500,A76A420022000001,629B2BA37EFFD577'","'-addons','D561AD08932E7B40,7E0150C026101500,5E51C5D8F04FDB80,A76A420022000001,5E389BB9F58B79A6,629B2BA37EFFD577'")
derived('PackageOnlyScene10.ps1',launcher/'PackageOnlyScene09.ps1',s)
s=source(launcher/'Launch-Gepard-Private-Demo09.ps1').replace('Launch-Gepard-Private-Demo09','Launch-Gepard-Private-Demo10').replace('PackageOnlyScene09','PackageOnlyScene10').replace('Package09','Demo10')
s=replace(s,"$original='"+str(old)+"'","$original=$PSScriptRoot")
derived('Launch-Gepard-Private-Demo10.ps1',launcher/'Launch-Gepard-Private-Demo09.ps1',s)
# Exact reused trio lineage; no copying installed payloads or protected content.
core=read(old/'CORE08-TRIO-REUSE-INPUT.json');core['status']='EXACT_INSTALLED_TRIO_MATCHES_ORIGINAL_SAME_BUILD_PROOF_NO_COPY';core['role']='gepard'
write('CORE08-TRIO-REUSE-INPUT.json',core)
proofpath=Path(r'D:\ArmaProjects\Gepard-Consolidated-20261022\native-runs\private-pack-09\wcsSmoke-pack\package-proof.json');proof=read(proofpath)
smoke={'status':'EXACT_INSTALLED_TRIO_MATCHES_ORIGINAL_SAME_BUILD_PROOF_NO_COPY','role':'wcsSmoke','root':r'D:\ArmaProjects\Gepard-Consolidated-20261022\private-installed-demo-09\addons','files':{},'originalSameBuildProof':{'role':'wcsSmoke','guid':'7E0150C026101500','proofPath':str(proofpath),'proofSHA256':sha(proofpath)},'currentSourceManifest':str(old/'adopted/CurrentSourceManifest-wcsSmoke.json'),'currentSourceManifestSHA256':sha(old/'adopted/CurrentSourceManifest-wcsSmoke.json'),'copied':False,'nativeStarted':False}
for n,row in proof['trio'].items():
 filename='addon.gproj' if n.endswith('.gproj') else n;smoke['files']['7E0150C026101500/'+filename]=row['sha256'];assert sha(Path(smoke['root'])/'7E0150C026101500'/filename)==row['sha256']==sha(row['path'])
write('SMOKE09-TRIO-REUSE-INPUT.json',smoke)
for p in [t/'Gepard-Demo10-Integration-20261022/PLAN.json',t/'Gepard-Demo10-Integration-20261022/DEMO10-INITIAL-SOURCE.json']:inputs[str(p)]=sha(p)
write('SOURCE-INPUT-PINS.json',{'status':'DRAFT_PARENT_CONTEXT_AND_EXACT_REUSE_INPUTS_FINAL_SOURCE_PENDING','inputPins':inputs,'finalSourceManifests':'PENDING_ROOT','actualCompile':'PENDING_ROOT','nativeStarted':False})
write('SCRIPT-DERIVATION.json',{'status':'NEW_C_ONLY_DEMO10_PACK_TOOLS_TEMPLATE_FINAL_SOURCE_PENDING','files':derivations,'originalInputPins':inputs,'DWritten':False,'nativeStarted':False,'packageExecuted':False})
write('ORIGINAL-PINS.json',inputs)
print(json.dumps({'status':'C_ONLY_DERIVED_CODE_FINAL_BINDINGS_PENDING','files':len(derivations),'DWritten':False,'nativeStarted':False},indent=2))

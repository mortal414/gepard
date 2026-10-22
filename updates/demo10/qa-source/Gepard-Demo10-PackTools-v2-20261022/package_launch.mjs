// Original bounded Pack08 install/plan glue adapted for reusedcore08+two newtrios.
// No native spawn or publication. Requires a real root contract and pack receipts.
import fs from 'node:fs';import path from 'node:path';
import {fileURLToPath} from 'node:url';import {randomUUID} from 'node:crypto';
import {read,sha,save,insist,guidOf,verifySources} from './pack_tools10.mjs';
import {makeClientPlan,verifyInstalledAddons} from 'file:///C:/Users/marco/Desktop/reforger-agent-harness/src/client-cli.ts';
const HERE=path.dirname(fileURLToPath(import.meta.url));
const R='D:\\ArmaProjects\\Gepard-Demo10-TrackArt-20261022';
const installed=path.join(R,'private-installed-demo-10b','addons');
const expected={gepard:'D561AD08932E7B40',wcsSmoke:'7E0150C026101500',integration:'5E51C5D8F04FDB80',provingGround:'A76A420022000001'};
const wcs='629B2BA37EFFD577',space='5E389BB9F58B79A6',dependencyRoot=path.join(R,'private-demo-source10','dependencies');
const planPath=path.join(HERE,'ROOT-PACK-PLAN-10.json');
function complete(root,pins){
 const actual=[];for(const dir of fs.readdirSync(root,{withFileTypes:true})){insist(dir.isDirectory()&&!dir.isSymbolicLink(),'Only declared addon directories');for(const n of fs.readdirSync(path.join(root,dir.name)))actual.push(dir.name+'/'+n);}
 insist(actual.length===Object.keys(pins).length&&actual.every(n=>Object.hasOwn(pins,n)&&sha(path.join(root,n))===pins[n]),'Complete installed/dependency closure differs');
}
function dependency(){
 const file=path.join(HERE,'SEPARATE-DEPENDENCIES.json'),d=read(file);
 const expectedFolders=['SpaceCore_5E389BB9F58B79A6','WCS_Armaments_629B2BA37EFFD577'];
 insist(d.root===dependencyRoot&&Object.keys(d.files).length===6&&d.closures.length===2,'Exact two separate dependency trios required');
 insist(fs.readdirSync(d.root).sort().join(',')===expectedFolders.sort().join(','),'No Bradley/extra dependency folder');complete(d.root,d.files);
 const ids={SpaceCore_5E389BB9F58B79A6:'5E389BB9F58B79A6',WCS_Armaments_629B2BA37EFFD577:'629B2BA37EFFD577'};
 for(const [folder,id]of Object.entries(ids)){
  insist(guidOf(fs.readFileSync(path.join(d.root,folder,'addon.gproj'),'utf8'))===id,'Dependency GUID differs');
  const row=d.closures.find(x=>x.role===folder);insist(row&&row.root===path.join(d.root,folder)&&sha(row.manifest)===row.manifestSHA256,'Separate dependency source proof differs');
  const pins=read(row.manifest);insist(Object.keys(pins).length===3&&Object.keys(pins).every(n=>d.files[folder+'/'+n]===pins[n]),'Dependency per-trio pins differ');
 }
 return {file,d};
}
function reused(role){
 const name=role==='gepard'?'CORE08-TRIO-REUSE-INPUT.json':'SMOKE09-TRIO-REUSE-INPUT.json';
 const p=read(path.join(HERE,name)),row=p.originalSameBuildProof;
 insist(p.status==='EXACT_INSTALLED_TRIO_MATCHES_ORIGINAL_SAME_BUILD_PROOF_NO_COPY'&&p.role===role&&Object.keys(p.files).length===3,'Exact reuse input status/role differs');
 insist(sha(row.proofPath)===row.proofSHA256&&sha(p.currentSourceManifest)===p.currentSourceManifestSHA256,'Original same-build/source proof changed');
 const proof=read(row.proofPath);insist(proof.status==='SAME_BUILD_TRIO_AND_CONTENTS_VERIFIED'&&proof.role===role&&proof.guid===expected[role]&&proof.privateQAExcludedFromProduction===true,'Original same-build/QA exclusion proof differs');
 if(role==='wcsSmoke')insist(proof.automatedQAExcluded===true,'Original optional09 automated QA exclusion proof absent');
 const contract=read(read(planPath).contract),support=contract.supportingSourceClosures.find(x=>x.role===role);
 insist(support&&JSON.stringify(Object.entries(read(support.manifest)).sort())===JSON.stringify(Object.entries(read(p.currentSourceManifest)).sort()),'Demo10 supporting source is not the reused same-build source');
 const trio=[];for(const [n,h]of Object.entries(p.files)){
  insist(n.startsWith(expected[role]+'/')&&sha(path.join(p.root,n))===h,'Previously installed reuse bytes changed');
  const original=Object.values(proof.trio).find(f=>(/\.gproj$/i.test(f.path)?'addon.gproj':path.basename(f.path))===path.basename(n));
  insist(original&&original.sha256===h&&sha(original.path)===h,'Reuse bytes differ from original same-build trio');
  trio.push({path:path.join(p.root,n),bytes:fs.statSync(path.join(p.root,n)).size,sha256:h});
 }
 return {role,guid:expected[role],reused:true,proofPath:row.proofPath,proofSHA256:row.proofSHA256,trio};
}
function qualifyPackDiagnostics(plan,result){
 insist(result.operationalVerdict==='PASS'&&result.baselineTeardownDiagnosticsRetained===true,'Actual operational result/strict diagnostics absent');
 const baseline=read(plan.nativeTailBaselinePath);let sum=0;const rows=[];
 for(const s of result.steps){
  const step=plan.steps.find(x=>x.role===s.role&&x.name===s.step);insist(step,'Unknown actual step');
  const root=path.join(plan.runRoot,s.role+'-'+s.step),consolePath=path.join(step.logsDir,'console.log'),diagPath=path.join(root,'native-diagnostics.json');
  insist(s.stepRoot===root&&s.console===consolePath&&s.diagnosticsReport===diagPath&&sha(diagPath)===s.diagnosticsReportSHA256,'Step diagnostics identity differs');
  const d=read(diagPath),lines=fs.readFileSync(consolePath,'utf8').split(/\r?\n/);
  const raw=lines.flatMap((text,i)=>/\b[A-Z][A-Z0-9_]*\s*\([EF]\)|Cannot compile|error:/i.test(text)?[{line:i+1,text}]:[]);
  insist(d.operationalPass===true&&d.operationalErrorCount===0&&d.step===s.step&&d.exitCode===0,'Step success/artifact diagnostics failed');
  insist(d.rawNativeErrorCount===raw.length&&s.nativeErrorCount===raw.length&&s.strictNativeVerdict===d.strictNativeVerdict,'Strict all-channel count mismatch');
  insist(JSON.stringify(d.errors.map(x=>({line:x.line,text:x.text})))===JSON.stringify(raw),'Actual raw E/F rows differ from receipt');
  insist(JSON.stringify(read(path.join(root,'raw-errors.json')))===JSON.stringify(raw.map(x=>x.text)),'Raw E/F artifact missing or changed');
  insist(d.baselineSignaturesSHA256===baseline.signaturesSHA256,'Finite tail source differs');
  if(raw.length){
   insist(d.strictNativeVerdict==='FAIL'&&d.knownBaselineTeardownCount===25&&[25,26,27].includes(raw.length),'Strict known diagnostics counts differ');
   const authorCount=d.knownDependencyAuthoringWarningCount??0;
   insist(raw.length===25+authorCount&&[0,1,2].includes(authorCount),'Unexpected authoring-warning multiplicity');
   const normalized=raw.map(x=>x.text.replace(/^\d{2}:\d{2}:\d{2}\.\d{3}\s+/,''));
   if(authorCount)insist(normalized[0]===baseline.dependencyAuthoringWarnings[0].signature,'WCS warning not exact/source-bound');
   if(authorCount===1)insist(d.classification==='EXACT_KNOWN_WCS_AUTHORING_AND_NATIVE_TEARDOWN_RETAINED','Single warning classification differs');
   if(authorCount===2)insist(normalized[1]===baseline.dependencyAuthoringWarnings[1].signature&&d.classification==='EXACT_KNOWN_WCS_SPACECORE_AUTHORING_AND_NATIVE_TEARDOWN_RETAINED','Exact ordered WCS/SpaceCore pair differs');
   insist(normalized.slice(authorCount).every((text,i)=>text===baseline.signatures[i]),'Novel/reordered diagnostic signature');
  }else insist(d.strictNativeVerdict==='PASS'&&d.classification==='NO_NATIVE_ERRORS','Clean strict classification differs');
  sum+=raw.length;rows.push({role:s.role,step:s.step,rawErrorCount:raw.length,strictNativeVerdict:d.strictNativeVerdict,operationalVerdict:d.operationalVerdict,consoleSHA256:sha(consolePath),diagnosticsReportSHA256:sha(diagPath)});
 }
 insist(result.nativeErrorCount===sum&&result.strictNativeVerdict===(sum?'FAIL':'PASS'),'Strict aggregate differs');
 return {operationalVerdict:'PASS',strictNativeVerdict:sum?'FAIL':'PASS',rawNativeErrorCount:sum,rows,meaning:'Actual validation/package success and closed ownership; known errors remain strictFAIL, no clean-health/gameplay claim'};
}
export function qualifyPackages(){
 const plan=read(planPath);verifySources(plan);dependency();
 insist(plan.runRoot===path.join(R,'native-runs','private-pack-10b')&&plan.outputRoot===path.join(R,'private-package-build-10b'),'Only exact09 pack bound');
 const result=read(path.join(plan.runRoot,'RESULT.json'));
 insist(result.status==='PRIVATE_VALIDATE_PACK_SEQUENCE_OPERATIONAL_PASS'&&result.nativeAbsent===true&&result.leaseReleased===true&&result.published===false,'Actual complete closed private pack required');
 insist(result.steps.length===4&&result.steps.every(s=>s.exitCode===0&&s.verifiedExited===true),'Four actual natural exits required');
 insist(result.steps.map(s=>s.role+':'+s.step).join(',')==='integration:validate,integration:pack,provingGround:validate,provingGround:pack','Actual four-step order differs');
 const diagnostics=qualifyPackDiagnostics(plan,result),rows=[reused('gepard'),reused('wcsSmoke')];
 for(const role of ['integration','provingGround']){
  const guid=expected[role],proofPath=path.join(plan.runRoot,role+'-pack','package-proof.json'),proof=read(proofPath);
  insist(proof.status==='SAME_BUILD_TRIO_AND_CONTENTS_VERIFIED'&&proof.role===role&&proof.guid===guid&&proof.automatedQAExcluded===true,'Fresh same-build optional/world proof differs');
  const trio=Object.values(proof.trio);insist(trio.length===3,'Exact fresh same-build trio required');
  const project=trio.filter(f=>/\.gproj$/i.test(f.path));insist(project.length===1,'One actual generated project required');
  for(const f of trio)insist(path.dirname(f.path)===path.join(plan.outputRoot,role)&&fs.statSync(f.path).size===f.bytes&&sha(f.path)===f.sha256,'Fresh package trio changed');
  insist(guidOf(fs.readFileSync(project[0].path,'utf8'))===guid,'Fresh package GUID differs');
  rows.push({role,guid,reused:false,proofPath,proofSHA256:sha(proofPath),trio});
 }
 return {plan,rows,diagnostics};
}
export function install(){
 const {rows,diagnostics}=qualifyPackages();insist(!fs.existsSync(path.dirname(installed)),'Fresh private09 install root required');
 fs.mkdirSync(installed,{recursive:true});const pins={};
 for(const row of rows){const dest=path.join(installed,row.guid);fs.mkdirSync(dest);for(const f of row.trio){const name=/\.gproj$/i.test(f.path)?'addon.gproj':path.basename(f.path);insist(['addon.gproj','resourceDatabase.rdb','data.pak'].includes(name),'Unexpected own trio entry');const target=path.join(dest,name);fs.copyFileSync(f.path,target,fs.constants.COPYFILE_EXCL);insist(sha(target)===f.sha256,'Own install bytes changed');pins[path.relative(installed,target).replaceAll('\\','/')]=f.sha256;}}
 insist(Object.keys(pins).length===12,'Exactly twelve own addon files');
 save(path.join(HERE,'INSTALLED-PACKAGE-CLOSURE.json'),{schema:1,root:installed,files:pins,packPlanSHA256:sha(planPath),packages:rows,packDiagnostics:diagnostics,WCSCopiedIntoOwnPayload:false,SpaceCoreCopiedIntoOwnPayload:false,nativeStarted:false,metadataBytesChanged:false,privateQAAdded:false});return pins;
}
export function manualPlan(mode='manual'){
 insist(mode==='manual'||mode==='probe','Only exact manual/probe mode');
 const {plan}=qualifyPackages(),closure=read(path.join(HERE,'INSTALLED-PACKAGE-CLOSURE.json')),dep=dependency();
 insist(closure.root===installed&&closure.packPlanSHA256===sha(planPath)&&Object.keys(closure.files).length===12,'Installed09 origin differs');complete(installed,closure.files);
 insist(fs.readdirSync(installed).sort().join(',')===Object.values(expected).sort().join(','),'Only exact four own addon GUIDs');
 verifyInstalledAddons([...Object.values(expected),space,wcs],[installed,dependencyRoot]);
 const sessionId=randomUUID(),run=path.join(R,'native-runs','private-demo-10b-sessions',mode+'-'+sessionId),sessionRoot=path.join(HERE,'session-plans',mode+'-'+sessionId);
 insist(!fs.existsSync(run),'Fresh UUID native profile required');
 const exe='D:\\SteamLibrary\\steamapps\\common\\Arma Reforger\\ArmaReforgerSteamDiag.exe';
 const client=makeClientPlan({addonIds:[expected.gepard,expected.wcsSmoke,expected.integration,expected.provingGround,space,wcs],addonsDirs:[installed,dependencyRoot,plan.coreAddonRoot],world:'Worlds/GPG_Arland.ent',connectLocal:false,forceUpdate:true,durationSeconds:600,keepOpen:true,expectGame:true,profile:path.join(run,'profile')},exe,run);
 client.args.push('-worldSystemsConfig','{C50579A2EC48E8EF}Configs/Systems/MissionSystems.conf','-noBackend');
 const layout=fs.readFileSync(path.join(plan.sourceAddonRoot,'provingGround','scripts','Game','GPG_Layout.c'),'utf8');
 const hashes=[...layout.matchAll(/static string (RegistryHash|RuntimeHash)\(\) \{return "([0-9a-f]{64})";/g)];insist(hashes.length===2,'Exact current world hash methods required');
 const worldContract=path.join(HERE,'PACKAGE10-WORLD-CONTRACT.json'),wc=read(worldContract);insist(wc.expectedOwnedObjects===34&&wc.expectedBoards===24&&hashes.find(x=>x[1]==='RegistryHash')[2]===wc.registryHash&&hashes.find(x=>x[1]==='RuntimeHash')[2]===wc.runtimeHash,'Exact V2 world startup contract differs');
 const nativeToolPins={};for(const f of [exe,'C:\\Users\\marco\\Desktop\\reforger-agent-harness\\src\\client-cli.ts',path.join(HERE,'package_launch.mjs'),path.join(HERE,'PackageOnlyScene10.ps1')])nativeToolPins[f]=sha(f);
 const planFile=path.join(sessionRoot,mode==='manual'?'MANUAL-UNTIMED-PLAN.json':'PACKAGE-PROBE-PLAN.json');
 const out={schema:1,type:'private-package-only-ordinary-scene',mode,sessionId,planFile,...client,runRoot:run,project:path.join(installed,expected.provingGround,'addon.gproj'),installedClosure:path.join(HERE,'INSTALLED-PACKAGE-CLOSURE.json'),installedClosureSHA256:sha(path.join(HERE,'INSTALLED-PACKAGE-CLOSURE.json')),dependencyClosure:dep.file,dependencyClosureSHA256:sha(dep.file),worldContract,worldContractSHA256:sha(worldContract),expectedRegistry:hashes.find(x=>x[1]==='RegistryHash')[2],expectedRuntime:hashes.find(x=>x[1]==='RuntimeHash')[2],nativeToolPins,closePolicy:mode==='manual'?'Human natural exit; untimed':'Explicit75s then exact-owned normal close',proofScope:'Private package-only current ordinary world readiness; all smoke/HUD/input/service/art judgments remain separate'};
 fs.mkdirSync(sessionRoot,{recursive:true});save(planFile,out);return out;
}
if(process.argv[1]&&path.resolve(process.argv[1])===fileURLToPath(import.meta.url)){
 if(process.argv[2]==='install'){insist(process.argv[3]==='--execute','Explicit root install required');install();console.log(JSON.stringify({status:'OWN_FOUR_TRIOS_INSTALLED_TWO_DEPS_SEPARATE_NO_NATIVE',installed}));}
 else if(process.argv[2]==='plan'){const p=manualPlan(process.argv[3]??'manual');console.log(JSON.stringify({status:'PACKAGE_ONLY_PLAN_READY_NATIVE_PENDING',plan:p.planFile,sha256:sha(p.planFile),runRoot:p.runRoot,mode:p.mode}));}
 else if(process.argv[2]==='check'){qualifyPackages();console.log(JSON.stringify({status:'ACTUAL_CLOSED_PACK09_ARTIFACTS_REUSEDCORE_WITH_STRICT_DIAGNOSTICS'}));}
 else throw Error('Use install --execute | plan manual|probe | check');
}

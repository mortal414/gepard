// Private addon-owned proof glue. Reuses maintained planning and PAK verification.
// No native child processes, file copying, metadata rewriting or publication.
import fs from 'node:fs';
import path from 'node:path';
import crypto from 'node:crypto';
import { pathToFileURL, fileURLToPath } from 'node:url';
import { makeWorkbenchPlan, verifyPackResult } from 'file:///C:/Users/marco/Desktop/reforger-agent-harness/src/workbench-cli.ts';
import { readPakIndex, readPakText } from 'file:///C:/Users/marco/Desktop/reforger-agent-harness/src/pak-source.ts';
export const HERE=path.dirname(fileURLToPath(import.meta.url));
export const sha=p=>crypto.createHash('sha256').update(fs.readFileSync(p)).digest('hex');
export const read=p=>JSON.parse(fs.readFileSync(p,'utf8').replace(/^\uFEFF/,''));
export const insist=(ok,message)=>{if(!ok)throw new Error(message);};
export const save=(p,data)=>fs.writeFileSync(p,JSON.stringify(data,null,2)+'\n',{flag:'wx'});
const expectedGuids={integration:'5E51C5D8F04FDB80',provingGround:'A76A420022000001'};
const forbidden=/(^|\/)(?:qa|checks|native-runs|hudqa|coreqa|artqa|tools|proof)(\/|$)|GEP_(?:Approved|ASCII_RENDER|DamageQA|CoreQA|FCAtomicRangeProbe)|GPG_/i;
export const forbiddenPrivateResource=p=>forbidden.test(p.replaceAll('\\','/'));
const classDeny=/\bclass\s+(?:GEP_(?:Approved|ASCII|DamageQA|CoreQA|FCAtomicRangeProbe)|GPG_)/;
const automatedQA=/(^|\/)(?:qa|checks|native-runs|hudqa|coreqa|artqa|tools|proof)(\/|$)|WorldResetQA|QA20\d\d|GEP_(?:Approved|ASCII_RENDER|DamageQA|CoreQA|FCAtomicRangeProbe)/i;
const automatedQACode=/\b(?:GEP|GPG)_[A-Za-z0-9_]*QA[A-Za-z0-9_]*\b|["']-?(?:gep|gpg)[A-Za-z0-9_-]*(?:qa|autotest)[A-Za-z0-9_-]*["']/i;
export function guidOf(text){const m=[...text.matchAll(/^\s*GUID\s+"([0-9A-Fa-f]{16})"/gm)];insist(m.length===1,'Exactly one project GUID required');return m[0][1].toUpperCase();}
const under=(root,p)=>{const rel=path.relative(root,p);return !path.isAbsolute(rel)&&rel!==''&&!rel.startsWith('..'+path.sep)&&rel!=='..';};
function tree(root){const result={};for(const e of fs.readdirSync(root,{withFileTypes:true})){const p=path.join(root,e.name);insist(!e.isSymbolicLink(),'Reparse/symlink source unsupported');if(e.isDirectory()){for(const [rel,h]of Object.entries(tree(p)))result[e.name+'/'+rel]=h;}else if(e.isFile())result[e.name]=sha(p);else throw new Error('Unsupported source entry');}return result;}
function supporting(plan){
 const fixed='D:\\ArmaProjects\\Gepard-Demo10-TrackArt-20261022\\private-demo-source10';
 insist(plan.sourceAddonRoot===path.join(fixed,'single')&&JSON.stringify(plan.dependencyAddonRoots)===JSON.stringify([path.join(fixed,'dependencies')]),'Only exact Demo10 owned/supporting and dependency roots');
 insist(plan.supportingSourceClosures.length===2&&plan.dependencyClosures.length===2,'Exact core1152+smoke14 and two separate dependency3 closures required');
 const supportCounts={gepard:1152,wcsSmoke:14},depIds={SpaceCore_5E389BB9F58B79A6:'5E389BB9F58B79A6',WCS_Armaments_629B2BA37EFFD577:'629B2BA37EFFD577'};
 insist(plan.supportingSourceClosures.map(x=>x.role).sort().join(',')==='gepard,wcsSmoke','Exact supporting roles required');
 insist(plan.dependencyClosures.map(x=>x.role).sort().join(',')===Object.keys(depIds).sort().join(','),'Exact dependency roles; no Bradley/other addon');
 insist(fs.readdirSync(plan.sourceAddonRoot).sort().join(',')==='gepard,integration,provingGround,wcsSmoke','Exact four owned/supporting source directories');
 insist(fs.readdirSync(path.join(fixed,'dependencies')).sort().join(',')===Object.keys(depIds).sort().join(','),'Exact two dependency directories only');
 for(const row of [...plan.supportingSourceClosures,...plan.dependencyClosures]){
  insist(sha(row.manifest)===row.manifestSHA256,'Supporting/dependency manifest differs');const pins=read(row.manifest),actual=tree(row.root);
  const count=supportCounts[row.role]??3;
  insist(Object.keys(actual).length===count&&Object.keys(pins).length===count&&Object.keys(pins).every(n=>actual[n]===pins[n]),'Complete immutable supporting/dependency bytes differ');
  if(Object.hasOwn(supportCounts,row.role))insist(row.root===path.join(fixed,'single',row.role),'Exact supporting source role root');
  else insist(row.root===path.join(fixed,'dependencies',row.role)&&guidOf(fs.readFileSync(path.join(row.root,'addon.gproj'),'utf8'))===depIds[row.role],'Exact separate dependency GUID/root');
 }
 const allowed=new Set(['5614BBCCBB55ED1C','58D0FB3206B6F859','D561AD08932E7B40','7E0150C026101500','5E51C5D8F04FDB80','A76A420022000001',...Object.values(depIds)]);
 for(const folder of ['gepard','wcsSmoke','integration','provingGround']){
  const dir=path.join(plan.sourceAddonRoot,folder),gprojects=fs.readdirSync(dir).filter(x=>/\.gproj$/i.test(x));insist(gprojects.length===1,'Exactly one owned source project');
  const text=fs.readFileSync(path.join(dir,gprojects[0]),'utf8'),matches=[...text.matchAll(/\bDependencies\s*\{([^}]+)\}/g)];insist(matches.length===1,'Exactly one dependency graph declaration');
  const ids=[...matches[0][1].matchAll(/"([0-9A-Fa-f]{16})"/g)].map(x=>x[1].toUpperCase());insist(ids.length>0&&ids.every(x=>allowed.has(x)),'Unapproved/Bradley dependency edge');
 }
}
function admission(c){
 insist(sha(c.actualCompileReceipt)===c.actualCompileReceiptSHA256&&sha(c.rootReview)===c.rootReviewSHA256,'Actual compile/root review pins required');
 const review=read(c.rootReview),a=read(c.actualCompileReceipt);
 insist(review.status==='ROOT_PRIVATE_DEMO10_SOURCE_ADMITTED'&&review.explicitPack10Requested===true&&review.strictNativeFailuresPreserved===true&&review.dependenciesWCSAndSpaceCoreConfirmed===true&&review.noBradley===true&&review.noPublication===true,'Explicit root Demo10 review missing');
 insist(a.status==='ROOT_DEMO10_COMPILE_ADMITTED_CURRENT_CLOSURES'&&a.project===c.projects.find(x=>x.role==='provingGround').project,'Actual current world compile adoption required');
 insist(sha(a.resultPath)===a.resultSHA256&&sha(a.consolePath)===a.consoleSHA256,'Actual compiler result/console changed');
 const result=read(a.resultPath),owner=read(path.join(path.dirname(a.resultPath),'owner-private.json'));
 insist(result.status==='SCRIPT_COMPILE_PASS'&&result.exitCode===0&&result.nativeAbsent===true&&result.leaseReleased===true&&owner.project===a.project,'Actual compile not PASS/normal0/absent/released/currentproject');
 insist(result.console===a.consolePath&&fs.readFileSync(a.consolePath,'utf8').includes('Script validation successful.'),'Actual compile success missing');
 const rows=[...c.projects.map(x=>({role:x.role,root:path.dirname(x.project),manifest:x.sourceManifest,manifestSHA256:x.sourceManifestSHA256})),...c.supportingSourceClosures,...c.dependencyClosures];
 insist(a.sourceClosures.length===6&&new Set(a.sourceClosures.map(x=>x.role)).size===6,'Exact six actual compiled source closures required');
 for(const row of rows){const actual=a.sourceClosures.find(x=>x.role===row.role);insist(actual&&actual.root===row.root&&actual.manifest===row.manifest&&actual.manifestSHA256===row.manifestSHA256,'Compiled/current source closure identity differs');}
 insist(Object.keys(c.reusedTrioInputs).sort().join(',')==='gepard,wcsSmoke','Exact two reused own addons required');
 for(const role of ['gepard','wcsSmoke']){
  const input=c.reusedTrioInputs[role];insist(sha(input.path)===input.sha256,'Reuse descriptor changed');const p=read(input.path),proof=read(p.originalSameBuildProof.proofPath);
  insist(p.role===role&&p.status==='EXACT_INSTALLED_TRIO_MATCHES_ORIGINAL_SAME_BUILD_PROOF_NO_COPY'&&Object.keys(p.files).length===3,'Reuse descriptor role/count differs');
  insist(sha(p.originalSameBuildProof.proofPath)===p.originalSameBuildProof.proofSHA256&&proof.status==='SAME_BUILD_TRIO_AND_CONTENTS_VERIFIED'&&proof.role===role&&proof.privateQAExcludedFromProduction===true,'Original reuse same-build/QA exclusion proof differs');
  insist(sha(p.currentSourceManifest)===p.currentSourceManifestSHA256,'Original reuse source manifest changed');
  const support=c.supportingSourceClosures.find(x=>x.role===role);insist(JSON.stringify(Object.entries(read(support.manifest)).sort())===JSON.stringify(Object.entries(read(p.currentSourceManifest)).sort()),'Supporting source differs from reused addon build');
  for(const [n,h]of Object.entries(p.files)){
   insist(sha(path.join(p.root,n))===h,'Previously installed reuse bytes changed');
   const original=Object.values(proof.trio).find(f=>(/\.gproj$/i.test(f.path)?'addon.gproj':path.basename(f.path))===path.basename(n));insist(original&&sha(original.path)===h&&original.sha256===h,'Reuse trio not original same-build bytes');
  }
 }
}
export function sourceCheck(project,{allowGeneratedRDB=false}={}){
 insist(sha(project.sourceManifest)===project.sourceManifestSHA256,'Complete manifest pin differs');
 const pins=read(project.sourceManifest),actual=tree(path.dirname(project.project));
 const cache='resourceDatabase.rdb';let generatedRDBdelta=null;
 if(allowGeneratedRDB){
  const original=Object.keys(pins).filter(k=>k!==cache),now=Object.keys(actual).filter(k=>k!==cache);
  insist(original.length===now.length&&original.every(k=>actual[k]===pins[k]),'Non-RDB source file set/bytes differ');
  insist(!Object.hasOwn(pins,cache)||Object.hasOwn(actual,cache),'Original RDB was removed');
  if(Object.hasOwn(actual,cache))insist(fs.statSync(path.join(path.dirname(project.project),cache)).size>0,'Generated RDB empty');
  if(pins[cache]!==actual[cache])generatedRDBdelta={relative:cache,originalSHA256:pins[cache]??null,currentSHA256:actual[cache]??null};
 }else insist(Object.keys(actual).length===Object.keys(pins).length&&Object.keys(pins).every(k=>actual[k]===pins[k]),'Complete source file set/bytes differ');
 insist(guidOf(fs.readFileSync(project.project,'utf8'))===expectedGuids[project.role],'Original project GUID differs');
 if(project.role==='integration')for(const rel of Object.keys(pins)){
  insist(!forbiddenPrivateResource(rel),'Private QA/proof resource in production source: '+rel);
  if(/\.c$/i.test(rel))insist(!classDeny.test(fs.readFileSync(path.join(path.dirname(project.project),rel),'utf8')),'Private QA/proof class in production source: '+rel);
 }
 for(const rel of Object.keys(actual)){
  insist(!automatedQA.test(rel),'Automated/private QA resource in final addon: '+rel);
  if(/\.c$/i.test(rel))insist(!automatedQACode.test(fs.readFileSync(path.join(path.dirname(project.project),rel),'utf8')),'Automated QA class/hook/opt-in flag in final addon: '+rel);
 }
 for(const rel of project.requiredPackageResources)insist(Object.hasOwn(pins,rel),'Required PAK resource absent from source closure: '+rel);
 return {role:project.role,project:project.project,guid:expectedGuids[project.role],manifestSHA256:project.sourceManifestSHA256,fileCount:Object.keys(actual).length,projectSHA256:sha(project.project),generatedRDBdelta};
}
export function buildPlan(contractPath){
 const c=read(contractPath);
 insist(c.schema===1&&c.status==='ROOT_FINAL_SOURCE_READY_FOR_PRIVATE_PACK','Final source staging/pins still pending root');
 admission(c);supporting(c);
 insist(Array.isArray(c.projects)&&c.projects.length===2&&new Set(c.projects.map(p=>p.role)).size===2,'Exactly separate Gepard/GPG projects required');
 for(const role of Object.keys(expectedGuids))insist(c.projects.some(p=>p.role===role),'Missing role '+role);
 for(const key of ['sourceAddonRoot','coreAddonRoot','runRoot','outputRoot'])insist(path.isAbsolute(c[key]),'Explicit absolute '+key+' required');
 insist(!fs.existsSync(c.runRoot)&&!fs.existsSync(c.outputRoot),'Fresh output/run roots required');
 insist(c.sourceAddonRoot!==c.coreAddonRoot,'Private source root/core discovery collide');
 for(const discovery of [c.sourceAddonRoot,c.coreAddonRoot])for(const target of [c.runRoot,c.outputRoot])insist(target!==discovery&&!under(discovery,target),'Run/package output must stay outside addon discovery roots');
 const tools=read("C:\\Users\\marco\\Documents\\Codex\\2026-10-21\\task\\GEP-Private-Demo-v3-Pack04-Preparation-20261022\\TOOL-PINS.json");
 for(const [p,h]of Object.entries(tools.files))insist(sha(p)===h,'Maintained tool pin differs: '+p);
 for(const f of [c.actualCompileReceipt,c.rootReview])tools.files[f]=sha(f);
 const actualAdmission=read(c.actualCompileReceipt);for(const f of [actualAdmission.resultPath,actualAdmission.consolePath,path.join(path.dirname(actualAdmission.resultPath),'owner-private.json')])tools.files[f]=sha(f);
 for(const role of ['gepard','wcsSmoke']){const input=c.reusedTrioInputs[role],p=read(input.path);for(const f of [input.path,p.currentSourceManifest,p.originalSameBuildProof.proofPath,...Object.keys(p.files).map(n=>path.join(p.root,n)),...Object.values(read(p.originalSameBuildProof.proofPath).trio).map(x=>x.path)])tools.files[f]=sha(f);}
 const ordered=['integration','provingGround'].map(role=>c.projects.find(p=>p.role===role));
 const sources=[];const steps=[];
 for(const p of ordered){
  insist(path.isAbsolute(p.project)&&path.isAbsolute(p.sourceManifest),'Explicit project/manifest paths required');
  insist(under(c.sourceAddonRoot,p.project),'Both addon projects must be contained in one explicit private source discovery root');
  insist(Array.isArray(p.requiredPackageResources)&&p.requiredPackageResources.length>0,'Actual expected package resources required');
  insist(/^[A-Za-z0-9_-]+$/.test(p.outputName),'Unsafe output name');
  const output=path.join(c.outputRoot,p.outputName);
  insist(!under(path.dirname(p.project),c.outputRoot)&&!under(c.outputRoot,path.dirname(p.project))&&c.outputRoot!==path.dirname(p.project),'Package/source roots overlap');
  sources.push(sourceCheck(p));
  const planned=makeWorkbenchPlan({action:'pack',project:p.project,output,configuration:'PC',execute:false},tools.workbench,path.join(c.runRoot,p.role,'logs'),[c.sourceAddonRoot,...c.dependencyAddonRoots,c.coreAddonRoot].join(','));
  for(const step of planned)steps.push({...step,role:p.role,project:p.project,output,guid:expectedGuids[p.role],deadlineSeconds:step.name==='validate'?90:180});
 }
 insist(!steps.some(s=>s.args.some(a=>/publish|workshop|changenote|password|token/i.test(a))),'Forbidden non-private argument');
 return {schema:1,type:'private-serial-validate-pack',contract:path.resolve(contractPath),contractSHA256:sha(contractPath),runRoot:c.runRoot,outputRoot:c.outputRoot,sourceAddonRoot:c.sourceAddonRoot,coreAddonRoot:c.coreAddonRoot,dependencyAddonRoots:c.dependencyAddonRoots,dependencyClosures:c.dependencyClosures,supportingSourceClosures:c.supportingSourceClosures,allowGeneratedResourceDatabaseOnly:c.allowGeneratedResourceDatabaseOnly===true,sources,projects:ordered,steps,toolPins:tools.files,tools:{node:tools.node,tsxLoader:tools.tsxLoader,helper:path.join(HERE,'pack_tools10.mjs')},proofScope:'Compile and package only. Gameplay, visual judgment, multiplayer, publication NOT_TESTED.'};
}
export function verifySources(plan){for(const[p,h]of Object.entries(plan.toolPins))insist(sha(p)===h,'Maintained tool pin changed');insist(sha(plan.contract)===plan.contractSHA256,'Root contract changed');supporting(plan);return plan.projects.map(p=>sourceCheck(p,{allowGeneratedRDB:plan.allowGeneratedResourceDatabaseOnly===true}));}
export async function verifyPackage(plan,role,startedAtMs){
 const sourceProof=verifySources(plan);const p=plan.projects.find(p=>p.role===role),step=plan.steps.find(s=>s.role===role&&s.name==='pack');
 insist(p&&step,'Unknown package role');
 const problems=verifyPackResult(step.output,step.logsDir,startedAtMs);insist(problems.length===0,problems.join('\n'));
 const projects=fs.readdirSync(step.output).filter(n=>/\.gproj$/i.test(n));insist(projects.length===1,'Expected one actual generated .gproj, not a renamed project');
 const gp=path.join(step.output,projects[0]),db=path.join(step.output,'resourceDatabase.rdb'),pak=path.join(step.output,'data.pak');
 insist(fs.statSync(db).isFile()&&fs.statSync(db).size>0,'Missing/empty same-pack resourceDatabase.rdb');
 insist(guidOf(fs.readFileSync(gp,'utf8'))===expectedGuids[role],'Packed GUID differs');
 insist(sha(gp)===sha(p.project),'Packed project metadata bytes differ; preserve and review rather than rewrite');
 const index=await readPakIndex(pak);const ownedSource=new Set(Object.keys(read(p.sourceManifest)).map(n=>n.toLowerCase()));for(const entry of index.entries)insist(ownedSource.has(entry.resource.toLowerCase()),'Foreign/dependency resource included in owned PAK: '+entry.resource);const resources=new Map(index.entries.map(e=>[e.resource.toLowerCase(),e]));
 for(const rel of p.requiredPackageResources)insist(resources.has(rel.toLowerCase()),'Required resource missing from actual PAK: '+rel);
 for(const e of index.entries)insist(!automatedQA.test(e.resource),'Private/automated QA in final PAK: '+e.resource);
 for(const e of index.entries)if(/\.c$/i.test(e.resource))insist(!automatedQACode.test((await readPakText(index,e)).toString('utf8')),'Automated QA class/hook/flag in final PAK: '+e.resource);
 if(role==='integration')for(const e of index.entries){
  insist(!forbiddenPrivateResource(e.resource),'Private QA/proof resource in production PAK: '+e.resource);
  if(/\.c$/i.test(e.resource)){const t=(await readPakText(index,e)).toString('utf8');insist(!classDeny.test(t),'Private QA class in production packed source: '+e.resource);}
 }
 const trio={};for(const f of [gp,db,pak])trio[path.basename(f)]={path:f,bytes:fs.statSync(f).size,sha256:sha(f)};
 return {status:'SAME_BUILD_TRIO_AND_CONTENTS_VERIFIED',role,guid:expectedGuids[role],trio,indexSHA256:index.indexSha256,indexBytes:index.indexBytes,packageEntryCount:index.entries.length,privateQAExcludedFromProduction:role==='integration',automatedQAExcluded:true,sourceProof,nativeGameplay:'NOT_TESTED',published:false};
}
if(process.argv[1]&&path.resolve(process.argv[1])===fileURLToPath(import.meta.url)){
 try{
  const[action,...args]=process.argv.slice(2);
  if(action==='plan'){const [contract,out]=args;const plan=buildPlan(contract);save(out,plan);process.stdout.write(JSON.stringify({status:'SOURCE_BOUND_PRIVATE_PLAN_READY',plan:out,sha256:sha(out),nativeStarted:false})+'\n');}
  else if(action==='sources'){const [plan]=args;process.stdout.write(JSON.stringify({status:'SOURCE_PINS_PASS',sources:verifySources(read(plan)),nativeStarted:false})+'\n');}
  else if(action==='package'){const[plan,role,startedAtMs,out]=args;const report=await verifyPackage(read(plan),role,Number(startedAtMs));save(out,report);process.stdout.write(JSON.stringify({status:report.status,role})+'\n');}
  else throw new Error('Use plan CONTRACT OUT | sources PLAN | package PLAN ROLE STARTED_MS OUT');
 }catch(e){process.stderr.write(String(e.stack??e)+'\n');process.exitCode=2;}
}

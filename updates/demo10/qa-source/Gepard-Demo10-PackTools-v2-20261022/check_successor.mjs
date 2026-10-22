// Read-only proof against the preserved first PAK, not a new package receipt.
import fs from 'node:fs';import path from 'node:path';import crypto from 'node:crypto';import {fileURLToPath} from 'node:url';
import {readPakIndex} from 'file:///C:/Users/marco/Desktop/reforger-agent-harness/dist/src/pak-source.js';
const here=path.dirname(fileURLToPath(import.meta.url)),old=path.join(path.dirname(here),'Gepard-Demo10-PackTools-20261022');
const read=p=>JSON.parse(fs.readFileSync(p,'utf8').replace(/^\uFEFF/,'')),sha=p=>crypto.createHash('sha256').update(fs.readFileSync(p)).digest('hex');
const before=read(path.join(old,'ROOT-COMPILED-SOURCE-CONTRACT.json')),after=read(path.join(here,'ROOT-COMPILED-SOURCE-CONTRACT.json'));
const inverse=structuredClone(after);inverse.runRoot=inverse.runRoot.slice(0,-1);inverse.outputRoot=inverse.outputRoot.slice(0,-1);
for(const k of ['actualCompileReceipt','rootReview'])inverse[k]=before[k];inverse.reusedTrioInputs=before.reusedTrioInputs;
const integration=inverse.projects.find(x=>x.role==='integration');integration.requiredPackageResources=before.projects.find(x=>x.role==='integration').requiredPackageResources;
if(JSON.stringify(inverse)!==JSON.stringify(before))throw Error('Unexpected source contract delta');
const delta=read(path.join(here,'SUCCESSOR-DELTA.json'));
for(const file of ['package_launch.mjs','PackageOnlyScene10.ps1']){
 let text=fs.readFileSync(path.join(here,file),'utf8');for(const [a,b] of Object.entries(delta.allowedRuntimePathRenames))text=text.replaceAll(b,a);
 if(text.replaceAll('\r\n','\n')!==fs.readFileSync(path.join(old,file),'utf8').replaceAll('\r\n','\n'))throw Error('Unexpected runtime guard delta '+file);
}
const unchanged=['SerialValidatePack10.ps1','pack_tools10.mjs','make_plan10.mjs','NativeTailClassifier10-v2.ps1','NATIVE-TAIL-BASELINE-v2.json','Launch-Gepard-Private-Demo10.ps1','ACTUAL-COMPILE-ADMISSION.json','ROOT-10-ADMISSION.json','PACKAGE10-WORLD-CONTRACT.json','CORE08-TRIO-REUSE-INPUT.json','SMOKE09-TRIO-REUSE-INPUT.json'];
for(const file of unchanged)if(sha(path.join(here,file))!==sha(path.join(old,file)))throw Error('Unchanged source differs '+file);
const review=read(path.join(here,'TXO-PACK-REVIEW.json')),row=review.rows.find(x=>x.role==='failed-proof-integration10'),ix=await readPakIndex(row.pak);
if(sha(row.pak)!==row.pakSHA256||ix.indexSha256!==row.indexSHA256)throw Error('Preserved actual PAK changed');
const pins=read(after.projects.find(x=>x.role==='integration').sourceManifest),required=after.projects.find(x=>x.role==='integration').requiredPackageResources;
const entries=new Map(ix.entries.map(e=>[e.resource.toLowerCase(),e]));
for(const n of required){const e=entries.get(n.toLowerCase());if(!e||e.rawBytes<=0||e.storedBytes<=0)throw Error('Missing/empty runtime requirement '+n);}
for(const e of ix.entries)if(!Object.hasOwn(pins,e.resource))throw Error('Foreign resource '+e.resource);
if(required.length!==25||required.filter(n=>n.endsWith('.xob')).length!==8||required.some(n=>n.endsWith('.txo')))throw Error('Requirement count/type changed');
const report={status:'PASS_MINIMAL_CONTRACT_AND_PATH_SUCCESSOR_WITH_ACTUAL_PRIOR_PAK_RUNTIME_RESOURCES_PRESENT',contractInverseExactAfterDeclaredEightTXORemovalAndPathRebinding:true,guardSourceTextInverseExactAfterPathRenames:true,unchangedRuntimeAndAdmissionFiles:unchanged,preservedActualPAK:{path:row.pak,sha256:row.pakSHA256,indexSHA256:ix.indexSha256,required25PresentNonempty:true,foreignOwnedResources:0},TXORetainedInExactSourceManifest:true,newNativeBatch:'NOT_RUN',nativeStarted:false,DWritten:false};
fs.writeFileSync(path.join(here,'SUCCESSOR-CHECKS.json'),JSON.stringify(report,null,2)+'\n');console.log(JSON.stringify(report));

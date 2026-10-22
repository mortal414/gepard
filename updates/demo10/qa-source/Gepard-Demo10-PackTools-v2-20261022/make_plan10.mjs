// Existing maintained pure planner/source proof, URI-correct and exact source pins.
import fs from 'node:fs';import path from 'node:path';import {fileURLToPath,pathToFileURL} from 'node:url';
import {buildPlan,verifySources,sha,save} from './pack_tools10.mjs';
const HERE=path.dirname(fileURLToPath(import.meta.url));
const contract=path.join(HERE,'ROOT-COMPILED-SOURCE-CONTRACT.json'),plan=buildPlan(contract);
const uri=pathToFileURL(path.resolve(plan.tools.tsxLoader)).href;
if(fileURLToPath(uri)!==path.resolve(plan.tools.tsxLoader)||sha(fileURLToPath(uri))!==plan.toolPins[plan.tools.tsxLoader])throw Error('Pinned loader URI roundtrip mismatch');
plan.tools.tsxLoader=uri;plan.nativeTailClassifierPath=path.join(HERE,'NativeTailClassifier10-v2.ps1');plan.nativeTailBaselinePath=path.join(HERE,'NATIVE-TAIL-BASELINE-v2.json');
for(const name of ['SerialValidatePack10.ps1','NativeTailClassifier10-v2.ps1','NATIVE-TAIL-BASELINE-v2.json','pack_tools10.mjs','SOURCE-INPUT-PINS.json','CORE08-TRIO-REUSE-INPUT.json','SMOKE09-TRIO-REUSE-INPUT.json'])plan.toolPins[path.join(HERE,name)]=sha(path.join(HERE,name));
plan.diagnosticsPolicy='Every E/F retained as raw strictFAIL; exact dependency authoringwarning and finiteEOFtail categorized separately from actual native step success/artifacts/normalexit. No clean-health or gameplay claim.';
verifySources(plan);const out=path.join(HERE,'ROOT-PACK-PLAN-10.json');save(out,plan);
save(path.join(HERE,'PLAN10-RESULT.json'),{status:'ACTUAL_CURRENT_COMPILE_BOUND_FOURSTEP_OPTIONAL_WORLD_PLAN_NO_NATIVE',plan:out,planSHA256:sha(out),sourceVerification:verifySources(plan),steps:plan.steps.map(s=>s.role+':'+s.name),coreRebuilt:false,nativeStarted:false});
console.log(JSON.stringify({status:'SOURCE_BOUND_PRIVATE_DEMO10_PLAN_READY',plan:out,sha256:sha(out),nativeStarted:false}));

// Pure source gate checks. No planner success claim and no native action.
import fs from 'node:fs';import path from 'node:path';import {fileURLToPath} from 'node:url';
import {buildPlan,forbiddenPrivateResource} from './pack_tools10.mjs';
const here=path.dirname(fileURLToPath(import.meta.url));let refusal;
try{buildPlan(path.join(here,'ROOT-COMPILED-SOURCE-CONTRACT.template.json'));throw Error('Draft contract unexpectedly admitted');}
catch(e){refusal=String(e.message);if(refusal!=='Final source staging/pins still pending root')throw e;}
if(!forbiddenPrivateResource('scripts/Game/QA/Probe.c'))throw Error('Private QA resource guard missing');
if(forbiddenPrivateResource('Prefabs/Demo10/Camo/GEP_MobileCarrierDemo10.et'))throw Error('Original vehicle config misclassified');
const out={status:'PASS_PENDING_SOURCE_REFUSAL_AND_RESOURCE_GUARDS',draftRefusal:refusal,nativeStarted:false,packaged:false,installed:false,sourceBindingsFinal:false};
fs.writeFileSync(path.join(here,'PENDING-GATE-CHECK.json'),JSON.stringify(out,null,2)+'\n',{flag:'wx'});console.log(JSON.stringify(out));

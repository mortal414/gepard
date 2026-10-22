// Original adapter proposal. Game probe, fixtures and native assets live elsewhere.
import { createHash, randomBytes, randomUUID } from "node:crypto";
import { createRequire } from "node:module";
import { readFile, writeFile, rename, lstat, readdir } from "node:fs/promises";
import path from "node:path";
import { pathToFileURL, fileURLToPath } from "node:url";
import { setTimeout as delay } from "node:timers/promises";

const capture = "C:/Users/marco/Desktop/reforger-agent-harness";
const existingRequire = createRequire(path.join(capture,"package.json"));
const { z } = existingRequire("zod");
const { makeClientPlan, classifyFatalClientLog } = await import(pathToFileURL(path.join(capture,"src/client-cli.ts")).href);
const { requireStorageSpace } = await import(pathToFileURL(path.join(capture,"src/storage-preflight.ts")).href);
const named = z.string().regex(/^[a-z0-9][a-z0-9-]{0,47}$/u);
const hex = z.string().regex(/^[0-9a-f]{64}$/u);
const resource = z.string().regex(/^(?:\{[0-9A-Fa-f]{16}\})?[A-Za-z0-9_./-]+\.et$/u);
const vector = z.tuple([z.number().finite(),z.number().finite(),z.number().finite()]);
const vectorString=z.string().max(96).refine((v:string)=>{
 const a=v.trim().split(/\s+/u);return a.length===3&&a.every(s=>/^-?\d+(?:\.\d+)?$/u.test(s)&&Number.isFinite(Number(s))&&Math.abs(Number(s))<=1000000);
},"Three bounded decimal vector components required");
const status = z.enum(["PASS","FAIL","NOT_TESTED"]);
export const caseSchema683 = z.object({
 schema:z.literal(1),id:named,settleSeconds:z.number().min(1).max(30),sampleSeconds:z.number().min(.1).max(30),
 maxGameSeconds:z.number().int().min(10).max(180),requestScriptedEgress:z.boolean(),
 stimulus:z.enum(["none","native-projectile","native-blast"]),resource:z.union([resource,z.literal("")]),
 position:vectorString,targetLocal:vectorString,incoming:vectorString,
 range:z.number().min(.5).max(1000),standoff:z.number().min(.1).max(100),
 shotCount:z.number().int().min(1).max(12),shotInterval:z.number().min(.1).max(5),
 crewRole:z.enum(["none","gunner","driver"]),seedRequest:z.number().int().min(-1).max(2147483647),capture:z.boolean(),
}).strict().superRefine((v:any,c:any)=>{
 if(v.settleSeconds+v.sampleSeconds+(v.shotCount-1)*v.shotInterval+10>v.maxGameSeconds)c.addIssue({code:"custom",message:"Game budget too short for settling/shots/sampling/terminal cleanup"});
 if(v.stimulus!=="none"&&!v.resource)c.addIssue({code:"custom",message:"Actual native stimulus resource required"});
 if(v.requestScriptedEgress&&v.crewRole!=="gunner")c.addIssue({code:"custom",message:"Scripted egress requires genuine occupied gunner setup"});
 if(v.seedRequest!==-1)c.addIssue({code:"custom",message:"Native seed control is NOT_SUPPORTED; use seedRequest=-1 without a deterministic-damage claim"});
 const incoming=v.incoming.trim().split(/\s+/u).map(Number);if(Math.hypot(...incoming)<.01)c.addIssue({code:"custom",message:"Nonzero outward incoming vector required"});
});
export const vehicleSchema683=z.object({schema:z.literal(1),id:named,prefab:resource}).strict();
export const configSchema683=z.object({
 schema:z.literal(1),fixtureReady:z.boolean(),gameExe:z.string().min(1),workRoot:z.string().min(1),addonRoot:z.string().min(1),
 manifestPath:z.string().min(1),manifestSha256:hex,fileCount:z.number().int().positive(),addonId:z.string().regex(/^[0-9A-Fa-f]{16}$/u),
 world:z.literal("Worlds/GEP_GunRange.ent"),worldSystemsConfig:z.string().min(1),
}).strict();
const checkSchema=z.object({name:z.string().regex(/^[A-Za-z][A-Za-z0-9_.-]{0,95}$/u),status,actual:z.union([z.number().finite(),z.boolean(),z.string()]).optional()}).strict();
export const nativeResultSchema683=z.object({
 schema:z.literal(1),runId:z.string().uuid(),nonce:hex,caseId:named,vehicleId:named,status,
 enteredPlay:z.literal(true),nativeStimulusObserved:z.boolean(),healthSettersUsed:z.literal(false),
 scriptedEgressCompleted:z.boolean().nullable(),physicalAcceptance:z.literal(false).optional(),
 checks:z.array(checkSchema).min(1).max(128),measurements:z.union([z.record(z.string(),z.unknown()),z.array(z.unknown())]),reason:z.string().max(2048).optional(),
}).strict();
const sha=(b:Buffer|string)=>createHash("sha256").update(b).digest("hex");
const parseJson683=(text:string)=>JSON.parse(text.replace(/^\uFEFF/u,""));
async function json(file:string){return parseJson683(await readFile(file,"utf8"));}
async function absent(file:string){try{await lstat(file);return false;}catch(e){if((e as NodeJS.ErrnoException).code==="ENOENT")return true;throw e;}}
async function fileCount683(dir:string):Promise<number>{let n=0;for(const ent of await readdir(dir,{withFileTypes:true})){if(ent.isSymbolicLink())throw new Error("Addon symlink refused");n+=ent.isDirectory()?await fileCount683(path.join(dir,ent.name)):1;}return n;}
function within(root:string,file:string){const r=path.resolve(root).toLowerCase()+path.sep;return path.resolve(file).toLowerCase().startsWith(r);}
function cleanPath(value:string){if(!path.isAbsolute(value)||/["\x00-\x1f]/u.test(value))throw new Error("Absolute clean Windows path required");return path.resolve(value);}
export async function makeDamagePlan683(configFile:string,caseFile:string,vehicleFile:string,selectedCase:string,selectedVehicle:string,execute:boolean){
 const configBytes=await readFile(configFile);const cfg=configSchema683.parse(parseJson683(configBytes.toString("utf8")));
 const c=caseSchema683.parse(await json(caseFile));const v=vehicleSchema683.parse(await json(vehicleFile));
 if(c.id!==selectedCase||v.id!==selectedVehicle)throw new Error("Selected named case/vehicle identity differs");
 for(const key of ["gameExe","workRoot","addonRoot","manifestPath"])cfg[key]=cleanPath(cfg[key]);
 if(!within(cfg.workRoot,cfg.addonRoot)||!within(cfg.workRoot,cfg.manifestPath))throw new Error("Frozen addon/manifest must be inside selected work root");
 if(!/^ArmaReforgerSteam(?:Diag)?\.exe$/iu.test(path.basename(cfg.gameExe)))throw new Error("Native Game executable required; Workbench/server is not a trial");
 const exeBytes=await readFile(cfg.gameExe);const manifestBytes=await readFile(cfg.manifestPath);
 if(sha(manifestBytes)!==cfg.manifestSha256)throw new Error("Pinned manifest hash differs");
 const manifest=parseJson683(manifestBytes.toString("utf8"));const files=manifest.manifest??manifest;
 if(!files||Array.isArray(files)||typeof files!=="object"||Object.keys(files).length!==cfg.fileCount)throw new Error("Frozen manifest count/schema differs");
 for(const [name,digest] of Object.entries(files)){
  const target=path.resolve(cfg.addonRoot,name);if(!within(cfg.addonRoot,target)||typeof digest!=="string"||!hex.safeParse(digest).success)throw new Error("Unsafe manifest entry");
  if(sha(await readFile(target))!==digest)throw new Error("Frozen addon differs: "+name);
 }
 // Count actual source files, as the existing frozen681 launcher does.
 if(await fileCount683(cfg.addonRoot)!==cfg.fileCount)throw new Error("Unexpected extra addon files");
 const runId=randomUUID();const runDir=path.join(cfg.workRoot,"damage-runs",`${c.id}-${v.id}-${runId}`);
 if(!await absent(runDir))throw new Error("Fresh run directory required");
 const native=makeClientPlan({addonIds:[cfg.addonId],addonsDirs:[path.dirname(cfg.addonRoot)],world:cfg.world,forceUpdate:true,connectLocal:false,expectGame:true,durationSeconds:c.maxGameSeconds,keepOpen:false,execute},cfg.gameExe,runDir);
 native.downloadDir=path.join(runDir,"downloads");
 const replaceArg=(flag:string,value:string)=>{const i=native.args.indexOf(flag);if(i<0)throw new Error("Required existing plan flag missing "+flag);native.args[i+1]=value;};
 replaceArg("-addonDownloadDir",native.downloadDir);replaceArg("-maxFPS","30");
 native.args.push("-worldSystemsConfig",cfg.worldSystemsConfig,"-noFocus","-noBackend");
 const profileData=path.join(native.profile,"profile");const nonce=randomBytes(32).toString("hex");
 const runnerSources=[];
 for(const file of [fileURLToPath(import.meta.url),fileURLToPath(new URL("./Damage.ps1",import.meta.url)),path.join(capture,"src/client-cli.ts"),path.join(capture,"src/storage-preflight.ts")])runnerSources.push({file,sha256:sha(await readFile(file))});
 const job={schema:1,runId,nonce,caseId:c.id,vehicleId:v.id,prefab:v.prefab,stimulus:c.stimulus,resource:c.resource,
  position:c.position,targetLocal:c.targetLocal,incoming:c.incoming,range:c.range,standoff:c.standoff,
  shotCount:c.shotCount,shotInterval:c.shotInterval,crewRole:c.crewRole,seedRequest:c.seedRequest,capture:c.capture,
  settleSeconds:c.settleSeconds,sampleSeconds:c.sampleSeconds,maxGameSeconds:c.maxGameSeconds,
  requestScriptedEgress:c.requestScriptedEgress,physicalAcceptance:false};
 if(execute){if(!cfg.fixtureReady)throw new Error("Fixture not yet qualified/adopted; source-only config cannot execute");await requireStorageSpace([runDir,native.profile,native.logsDir,native.downloadDir]);}
 return {schema:1,kind:"native-game-damage-regression",status:"NOT_TESTED",execute,fixtureReady:cfg.fixtureReady,runId,nonce,runDir,profileData,
  native,job,configPath:path.resolve(configFile),casePath:path.resolve(caseFile),vehiclePath:path.resolve(vehicleFile),
  configSha256:sha(configBytes),caseSha256:sha(await readFile(caseFile)),vehicleSha256:sha(await readFile(vehicleFile)),
  manifestPath:cfg.manifestPath,manifestSha256:cfg.manifestSha256,addonRoot:cfg.addonRoot,files,gameSha256:sha(exeBytes),runnerSources,
  jobPath:path.join(profileData,"DamageHarness-job683.json"),nativeResultPath:path.join(profileData,"DamageHarness-result683.json"),
  controlPath:path.join(profileData,"DamageHarness-control683.json"),readyMarker:`GEP_DAMAGE683_READY|${runId}|${nonce}`,doneMarker:`GEP_DAMAGE683_DONE|${runId}|${nonce}`,
  physicalAcceptance:false,scriptedEgressIsPhysicalAcceptance:false};
}
async function atomic(file:string,value:unknown){const tmp=file+"."+randomUUID()+".tmp";await writeFile(tmp,JSON.stringify(value,null,2)+"\n",{flag:"wx"});await rename(tmp,file);}
async function requestClose(plan:any,reason:string){await atomic(plan.controlPath,{schema:1,runId:plan.runId,nonce:plan.nonce,command:"close",reason});}
async function boundedLog(file:string){try{const info=await lstat(file);if(info.size>64*1024**2)throw new Error("Log exceeds64MiB cap");return await readFile(file,"utf8");}catch(e){if((e as NodeJS.ErrnoException).code==="ENOENT")return "";throw e;}}
function fatalLog683(log:string){return classifyFatalClientLog(log,false)??(/SCRIPT\s+\(E\)/u.test(log)?"native script error (inspect full console)":undefined);}
export async function watchDamage683(plan:any){
 const deadline=performance.now()+plan.job.maxGameSeconds*1000;let reason="watchdog-timeout";let interrupted=false;
 const onInterrupt=()=>{interrupted=true;};process.on("SIGINT",onInterrupt);process.on("SIGTERM",onInterrupt);
 try{while(performance.now()<deadline){
  const cancelFile=process.env.REFORGER_QA_CANCEL_PATH;
  if(cancelFile && !await absent(cancelFile)){reason="workflow-cancelled";break;}
  const log=await boundedLog(path.join(plan.native.logsDir,"console.log"));
  const fatal=fatalLog683(log);if(fatal){reason="fatal-log: "+fatal;break;}
  if(log.includes(plan.doneMarker)){reason="terminal-marker";break;}
  if(interrupted){reason="runner-interrupted";break;}
  await delay(250);
 }}catch(e){reason="watch-error: "+String(e);}finally{process.off("SIGINT",onInterrupt);process.off("SIGTERM",onInterrupt);}
 await requestClose(plan,reason);
 const result={schema:1,reason,runId:plan.runId,normalCloseRequested:true,physicalAcceptance:false};
 await atomic(path.join(plan.runDir,"watch.json"),result);return result;
}
export async function collectDamage683(plan:any,exit:any){
 const consoleFile=path.join(plan.native.logsDir,"console.log");const log=await boundedLog(consoleFile);
 const fatal=fatalLog683(log);const gameSeen=/SCR_BaseGameMode::OnGameStateChanged\s*=\s*GAME(?:\s|$)/u.test(log);
 const readySeen=log.includes(plan.readyMarker);const doneSeen=log.includes(plan.doneMarker);let nativeResult:any;let error="";
 try{if((await lstat(plan.nativeResultPath)).size>8*1024**2)throw new Error("Native result exceeds8MiB cap");nativeResult=nativeResultSchema683.parse(await json(plan.nativeResultPath));
  for(const [key,value] of Object.entries({runId:plan.runId,nonce:plan.nonce,caseId:plan.job.caseId,vehicleId:plan.job.vehicleId}))if(nativeResult[key]!==value)throw new Error("Native result identity differs: "+key);
  if(nativeResult.status==="PASS"&&(nativeResult.checks.some((c:any)=>c.status==="FAIL")||!nativeResult.checks.some((c:any)=>c.status==="PASS")))throw new Error("PASS without passing fixture checks or with failed checks");
  if(nativeResult.status==="PASS"&&["gameplay","damageFixture"].some(name=>!nativeResult.checks.some((c:any)=>c.name===name&&c.status==="PASS")))throw new Error("Required gameplay/damageFixture PASS checks absent");
  if(nativeResult.status==="PASS"&&plan.job.stimulus!=="none"&&!nativeResult.nativeStimulusObserved)throw new Error("No actual native stimulus observed");
  if(plan.job.requestScriptedEgress&&nativeResult.status==="PASS"&&nativeResult.scriptedEgressCompleted!==true)throw new Error("Scripted egress incomplete");
 }catch(e){error=String(e);nativeResult=null;}
 let unchanged=true;const changed:string[]=[];
 for(const [name,digest] of Object.entries(plan.files)){try{if(sha(await readFile(path.join(plan.addonRoot,name)))!==digest){unchanged=false;changed.push(name);}}catch{unchanged=false;changed.push(name);}}
 try{if(await fileCount683(plan.addonRoot)!==Object.keys(plan.files).length){unchanged=false;changed.push("unexpected-addon-file-count");}}catch{unchanged=false;changed.push("addon-enumeration-failed");}
 for(const [file,digest] of [[plan.configPath,plan.configSha256],[plan.casePath,plan.caseSha256],[plan.vehiclePath,plan.vehicleSha256],[plan.manifestPath,plan.manifestSha256],[plan.native.executable,plan.gameSha256]]){try{if(sha(await readFile(file))!==digest){unchanged=false;changed.push(file);}}catch{unchanged=false;changed.push(file);}}
 for(const entry of plan.runnerSources??[]){try{if(sha(await readFile(entry.file))!==entry.sha256){unchanged=false;changed.push(entry.file);}}catch{unchanged=false;changed.push(entry.file);}}
 const seedRequestSupported=plan.job.seedRequest===-1;
 // A no-gamemode layer need not emit SCR_BaseGameMode's marker.
 // Exact owned Game executable/arguments plus actual probe InPlayMode remain mandatory.
 const nativePlayConfirmed=exit.gameIdentityVerified===true&&nativeResult?.enteredPlay===true;
 const captureEvidence=[];
 for(const phase of ["before","after"]){
  const file=path.join(plan.profileData,`DamageHarness-${phase}.bmp`);
  try{const b=await readFile(file);const validBmp=b.length>54&&b.subarray(0,2).toString("ascii")==="BM"&&b.readInt32LE(18)===1280&&Math.abs(b.readInt32LE(22))===720;
   captureEvidence.push({phase,file,bytes:b.length,sha256:sha(b),validBmp});
  }catch(e){captureEvidence.push({phase,file,validBmp:false,error:String(e)});}
 }
 const capturesPresent=!plan.job.capture||captureEvidence.every(frame=>frame.validBmp);
 const infrastructurePass=nativePlayConfirmed&&readySeen&&doneSeen&&!fatal&&unchanged&&capturesPresent&&exit.verifiedExited===true&&exit.exitCode===0&&exit.leaseReleased===true;
 const aggregate=infrastructurePass&&nativeResult?nativeResult.status:"NOT_TESTED";
 const artifacts=[];for(const name of ["console.log","script.log","error.log"]){const file=path.join(plan.native.logsDir,name);try{const b=await readFile(file);artifacts.push({file,bytes:b.length,sha256:sha(b)});}catch(e){if((e as NodeJS.ErrnoException).code!=="ENOENT")throw e;}}
 for(const name of ["DamageHarness-job683.json","DamageHarness-result683.json","DamageHarness-control683.json"]){const file=path.join(plan.profileData,name);try{const b=await readFile(file);artifacts.push({file,bytes:b.length,sha256:sha(b)});}catch(e){if((e as NodeJS.ErrnoException).code!=="ENOENT")throw e;}}
 const result={schema:1,kind:plan.kind,status:aggregate,caseId:plan.job.caseId,vehicleId:plan.job.vehicleId,runId:plan.runId,
  nativeStimulus:plan.job.stimulus,seedSupport:"NOT_SUPPORTED",deterministicDamageClaimed:false,scriptedEgressRequested:plan.job.requestScriptedEgress,physicalAcceptance:false,
  provenance:{manifestPath:plan.manifestPath,manifestSha256:plan.manifestSha256,sourceFileCount:Object.keys(plan.files).length,gameExecutable:plan.native.executable,gameSha256:plan.gameSha256,configSha256:plan.configSha256,caseSha256:plan.caseSha256,vehicleSha256:plan.vehicleSha256,runnerSources:plan.runnerSources},
  infrastructure:{passed:infrastructurePass,nativePlayConfirmed,baseGameModeGameMarker:gameSeen,readySeen,doneSeen,fatal,unchanged,changed,seedRequestSupported,capturesPresent,captureEvidence,exit},nativeResult,error,artifacts,
  observedUtc:new Date().toISOString(),runDir:plan.runDir};
 await atomic(path.join(plan.runDir,"result.json"),result);return result;
}
function args683(argv:string[]){
 const allowed:Record<string,string[]>={plan:["--config","--case-file","--vehicle-file","--case","--vehicle","--execute"],watch:["--plan"],collect:["--plan","--exit"]};
 if(!allowed[argv[0]])throw new Error("Unknown adapter command");const x:Record<string,string>={};
 for(let i=1;i<argv.length;i+=2){if(!allowed[argv[0]].includes(argv[i])||!argv[i+1]||x[argv[i]])throw new Error("Malformed/unknown/duplicate adapter argument");x[argv[i]]=argv[i+1];}
 if(allowed[argv[0]].some(k=>!x[k]))throw new Error("Missing adapter argument");
 if(argv[0]==="plan"&&!['true','false'].includes(x['--execute']))throw new Error("Explicit execute Boolean required");return x;
}
async function main(){const argv=process.argv.slice(2);const a=args683(argv);let result:any;
 if(argv[0]==="plan")result=await makeDamagePlan683(a["--config"],a["--case-file"],a["--vehicle-file"],a["--case"],a["--vehicle"],a["--execute"]==="true");
 else if(argv[0]==="watch")result=await watchDamage683(await json(a["--plan"]));
 else if(argv[0]==="collect")result=await collectDamage683(await json(a["--plan"]),await json(a["--exit"]));
 else throw new Error("Use plan|watch|collect; native launch/lease is owned solely by Damage.ps1");
 process.stdout.write(JSON.stringify(result,null,2)+"\n");}
if(process.argv[1]&&import.meta.url===pathToFileURL(path.resolve(process.argv[1])).href)main().catch(e=>{process.stderr.write(String(e)+"\n");process.exitCode=2;});

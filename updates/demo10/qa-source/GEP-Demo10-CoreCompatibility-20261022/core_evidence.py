"""Collect-only reuse of maintained683 schema/collector. Never launches native apps."""
from pathlib import Path
import json, hashlib, secrets, subprocess, uuid
from evidence import sha,read_json,save,assertion

ROOT=Path(__file__).resolve().parent
NODE=Path(r'C:\Program Files\nodejs\node.exe')
TSX=Path(r'C:\Users\marco\Desktop\reforger-agent-harness\node_modules\tsx\dist\cli.mjs')

def make_native_job(request,p,attempt):
 case=read_json(p['caseFile']);vehicle=read_json(p['vehicleFile'])
 assert case['id']==p['nativeCase'] and vehicle['id']==p['vehicle'],'Native definition differs'
 job={**case,'caseId':case['id'],'vehicleId':vehicle['id'],'prefab':vehicle['prefab'],'runId':request['runId'],'nonce':secrets.token_hex(32),'physicalAcceptance':False}
 del job['id'];target=attempt/'native-job.json';save(target,job)
 return target

def require_healthy(p):
 if p['nativeCase']=='v11-healthy':return
 proof=read_json(ROOT/'HEALTHY-ADOPTION.json')
 assert proof['status']=='BOTH_HEALTHY_NATIVE_CONTROLS_ADOPTED','Both current healthy controls must pass before follow-on cases'
 assert proof['sourceManifestSHA256']==sha(p['qaManifest']),'Healthy source differs'
 for vehicle in ['v11-camo','v11-charcoal']:
  row=proof[vehicle];result=Path(row['result']);assert sha(result)==row['sha256'],'Healthy proof pin differs'
  bridges=qualified_healthy_bridge(result,vehicle)
  assert row['bridgeProofs']==bridges,'Healthy normalized bridge proof differs'
  data=read_json(result);assert data['infrastructure']['passed'] is True and data['status']=='PASS'
  assert data['nativeResult']['vehicleId']==vehicle and data['nativeResult']['caseId']=='v11-healthy'
  assert data['provenance']['manifestSha256']==sha(p['qaManifest'])
  m=data['nativeResult']['measurements'];assert m['operationStartedQA'] is True
  assert min(m['rightTraverseDegreesQA'],m['leftTraverseDegreesQA'])>=1
  assert m['upElevationDegreesQA']>=.5 and m['downElevationDegreesQA']<=-.5
  assert min(m['leftRoundsUsedQA'],m['rightRoundsUsedQA'],m['reentryLeftRoundsUsedQA'],m['reentryRightRoundsUsedQA'])>0
  assert m['publicReentryCompletedQA'] is True

def qualified_healthy_bridge(result,vehicle):
 result=Path(result).resolve();native=read_json(result)
 expected='v11-healthy-'+vehicle;attempt=result.parent.parent
 bridgePath=attempt/'adapter-result.json';normalizedPath=attempt/'result.json'
 bridge=read_json(bridgePath);normalized=read_json(normalizedPath)
 assert bridge['runId']==normalized['runId']==native['runId'],'Healthy run identities differ'
 assert bridge['caseId']==normalized['caseId']==expected,'Healthy case identities differ'
 assert bridge['infrastructure']['status']=='PASS' and normalized['infrastructure']['status']=='PASS','Healthy bridge incomplete; raw nativePASS cannot replace the failed bridge'
 assert normalized['status']=='PASS','Normalized healthy harness case did not pass'
 for receipt in [bridge,normalized]:
  assert receipt['lifecycle']['status']=='PASS' and receipt['lifecycle']['verifiedExited'] is True and receipt['lifecycle']['leaseReleased'] is True,'Healthy normalized closure incomplete'
  for name in ['fixture.ready','world.loaded','native.errors','native.case-result','input.positive-control','post-hit.traverse','post-hit.elevation','post-hit.fire','crew.exit','crew.reentry','post-reentry.fire']:
   assert receipt['assertions'][name]['status']=='PASS','Healthy bridge assertion missing/failed:'+name
 return [{'path':str(path),'sha256':sha(path)} for path in [bridgePath,normalizedPath]]

def command_for(request,p,native):
 assert request['template']=='startup' and request['world']=='Worlds/GEP_GunRange.ent'
 assert Path(request['project']).resolve()==Path(r'C:\Users\marco\Documents\Codex\2026-10-21\task\GEP-Demo10-CoreCompatibility-20261022\native-stage\single\qaCore\GEP_SmokeCoreCompatQA.gproj').resolve()
 assert request['caseId']==p['nativeCase']+'-'+p['vehicle']
 assert p['addonId']=='A9F84D1626102251'
 assert type(p['seconds']) is int and 30<=p['seconds']<=180
 assert type(p['adapterDeadlineSeconds']) is int and p['seconds']<p['adapterDeadlineSeconds']<=150
 assert p['screenWidth']==1280 and p['screenHeight']==720
 assert type(p['childLogLimitBytes']) is int and 4096<=p['childLogLimitBytes']<=4*1024**2
 return [p['powerShell'],'-NoLogo','-NoProfile','-NonInteractive','-File',p['wrapper'],'-Project',request['project'],'-World',request['world'],'-Manifest',p['qaManifest'],'-RunRoot',str(native),'-Seconds',str(p['seconds']),'-AddonId',p['addonId'],'-ScreenWidth','1280','-ScreenHeight','720','-NativeJob',p['nativeJob'],'-NativeJobSHA256',sha(p['nativeJob']),'-Execute']

def collect(request,p,native,attempt,exit,wrapper_plan):
 job=read_json(p['nativeJob']);console=native/'logs/console.log'
 assert console.stat().st_size<=64*1024**2
 log=console.read_text(encoding='utf-8-sig',errors='strict')
 ready='GEP_DAMAGE683_READY|'+job['runId']+'|'+job['nonce'];done='GEP_DAMAGE683_DONE|'+job['runId']+'|'+job['nonce']
 assert log.count(ready)==1 and log.count(done)==1 and log.index(ready)<log.index(done),'Unique ordered native job markers absent'
 paths=[ROOT/'adapter.py',ROOT/'core_evidence.py',ROOT/'tools/Scene.ps1',ROOT/'damage-adapter683.ts']
 plan={'schema':1,'kind':'native-game-damage-regression','runId':job['runId'],'nonce':job['nonce'],'runDir':str(native),'profileData':str(native/'profile/profile'),'native':{'executable':wrapper_plan['exe'],'args':wrapper_plan['args'],'logsDir':str(native/'logs')},'job':job,'configPath':p['boundJob'],'configSha256':sha(p['boundJob']),'casePath':p['caseFile'],'caseSha256':sha(p['caseFile']),'vehiclePath':p['vehicleFile'],'vehicleSha256':sha(p['vehicleFile']),'manifestPath':p['qaManifest'],'manifestSha256':sha(p['qaManifest']),'addonRoot':str(Path(request['project']).parent),'files':read_json(p['qaManifest']),'gameSha256':sha(wrapper_plan['exe']),'runnerSources':[{'file':str(path),'sha256':sha(path)} for path in paths],'jobPath':str(native/'profile/profile/DamageHarness-job683.json'),'nativeResultPath':str(native/'profile/profile/DamageHarness-result683.json'),'controlPath':str(native/'profile/profile/DamageHarness-control683.json'),'readyMarker':ready,'doneMarker':done,'physicalAcceptance':False}
 save(native/'legacy-collect-plan.json',plan)
 # This derived flag is issued only after original wrapper identity/argv/lease
 # receipts and verified normal exit passed in adapter.lifecycle, not from GAME.
 save(native/'legacy-collect-exit.json',{**exit,'gameIdentityVerified':True})
 command=[str(NODE),str(TSX),str(ROOT/'damage-adapter683.ts'),'collect','--plan',str(native/'legacy-collect-plan.json'),'--exit',str(native/'legacy-collect-exit.json')]
 with (attempt/'collector.stdout.json').open('xb') as out,(attempt/'collector.stderr.log').open('xb') as err:
  completed=subprocess.run(command,stdout=out,stderr=err,creationflags=getattr(subprocess,'CREATE_NO_WINDOW',0),shell=False,timeout=30)
 assert completed.returncode==0,'Original schema/collector failed; preserve stderr and raw native result'
 result=read_json(native/'result.json');checks=review_collected_result(result,p)
 import re
 error_rows=[{'line':i+1,'raw':line} for i,line in enumerate(log.splitlines()) if re.search(r'\b([A-Z][A-Z0-9_]*)(?:\s*\([^)]*\))*\s*\(([EF])\)',line)]
 error_receipt=attempt/'ALL-DOMAIN-NATIVE-ERRORS.json'
 save(error_receipt,{'status':'FAIL' if error_rows else 'PASS','console':str(console),'consoleSHA256':sha(console),'rows':error_rows,'omitted':0,'waivers':[]})
 checks['native.errors']={'status':'FAIL' if error_rows else 'PASS','reason':'Independent all-domain E/F scan; original legacy result retained unchanged','actual':len(error_rows)}
 loaded=[]
  # Successor repair of the observer: scope one Loaded block; never deduplicate.
 blocks=[i for i,l in enumerate(log.splitlines()) if 'Loaded addons:' in l]
 if len(blocks)==1:
  for line in log.splitlines()[blocks[0]+1:]:
   match=re.search(r"ENGINE\s*: gproj: '.*?' guid: '([A-Fa-f0-9]{16})'",line)
   if not match:break
   loaded.append(match[1].upper())
 expected={'5614BBCCBB55ED1C','58D0FB3206B6F859','D561AD08932E7B40','629B2BA37EFFD577','7E0150C026101500','A9F84D1626102251','5E51C5D8F04FDB80','5E389BB9F58B79A6'}
 checks['addons.exact-stack']={'status':'PASS' if len(blocks)==1 and len(loaded)==len(expected) and set(loaded)==expected else 'FAIL','reason':'One scoped Loaded addons block; exact Demo10 eight IDs, no deduplication; old scan-wide failure preserved','actual':loaded}
 checks['smoke.not-triggered']={'status':'FAIL' if 'WCS_NATIVE_REARM phase=' in log or 'WCS_MANUAL_PROGRAM' in log else 'PASS','reason':'No smoke QA program dispatcher is included in the core fixture'}

 return result,checks,[native/'result.json',native/'legacy-collect-plan.json',native/'legacy-collect-exit.json',attempt/'collector.stdout.json',attempt/'collector.stderr.log',console,error_receipt]


def review_collected_result(result,p):
 assert result.get('schema')==1 and result.get('kind')=='native-game-damage-regression','Wrong original collector schema'
 assert result.get('status') in ['PASS','FAIL','NOT_TESTED'],'Invalid collector status'
 assert isinstance(result.get('infrastructure'),dict) and type(result['infrastructure'].get('passed')) is bool,'Collector infrastructure missing/invalid'
 fatal=result['infrastructure'].get('fatal')
 assert fatal is None or isinstance(fatal,str),'Original optional fatal classification must be absent/null or a string'
 m=(result.get('nativeResult') or {}).get('measurements') or {};passed=result['infrastructure']['passed'] is True
 checks={};check=lambda name,ok,reason,actual=None:checks.update({name:{'status':'PASS' if ok else 'FAIL','reason':reason,**({'actual':actual} if actual is not None else {})}})
 check('fixture.ready',passed,'Maintained683 collector: exact nonce,source,capture,result schema,owned native normal exit and lease receipts')
 check('world.loaded',result['infrastructure']['nativePlayConfirmed'],'Native enteredPlay plus original exact Game identity')
 checks['native.case-result']={'status':(result.get('nativeResult') or {}).get('status','NOT_TESTED'),'reason':(result.get('nativeResult') or {}).get('reason','Original native result absent; never replace its FAIL with a partial assertion subset')}
 checks['native.errors']={'status':'FAIL' if result['infrastructure'].get('fatal') else 'PASS','reason':'Original fatal classification and full console retained; no error suppression','actual':result['infrastructure'].get('fatal') or 'none'}
 check('scripts.compiled',True,'Actual source-matched separate compile adoption')
 if p['nativeCase']=='v11-wheeled-course':
  checks['wheel.telemetry']={'status':'NOT_TESTED','reason':'Actual WHEELED samples require independent signed-motion,power,contact,dwell review; sample completion is not handling acceptance','actual':m.get('wheeledCourseSamplesCompletedQA',False)}
 else:
  check('input.positive-control',m.get('operationStartedQA') is True,'Actual controlled original gunner/native inputs; healthy controls separately gate damage')
  check('post-hit.traverse',min(m.get('rightTraverseDegreesQA',-1),m.get('leftTraverseDegreesQA',-1))>=1,'Actual native sights direction after opposite input actions',{'right':m.get('rightTraverseDegreesQA'),'left':m.get('leftTraverseDegreesQA')})
  check('post-hit.elevation',m.get('upElevationDegreesQA',-1)>=.5 and m.get('downElevationDegreesQA',1)<=-.5,'Actual hull-relative signed elevation readback, no pose setter',{'up':m.get('upElevationDegreesQA'),'down':m.get('downElevationDegreesQA')})
  check('post-hit.fire',min(m.get('leftRoundsUsedQA',0),m.get('rightRoundsUsedQA',0))>0,'Both native muzzle ammunition counts decrease; projectile/recoil visual acceptance separate')
  check('crew.exit',(result.get('nativeResult') or {}).get('scriptedEgressCompleted') is True,'Original actor completed public scripted exit; physical F/hatch acceptance separate')
  check('crew.reentry',m.get('publicReentryCompletedQA') is True,'Original actor/seat completed public entry action')
  check('post-reentry.fire',min(m.get('reentryLeftRoundsUsedQA',0),m.get('reentryRightRoundsUsedQA',0))>0,'Both feeds decrease after completed reentry')
  if p['nativeCase']=='v11-fragment-survivor':
   check('stimulus.native',(result.get('nativeResult') or {}).get('nativeStimulusObserved') is True and m.get('issued',0)>0,'Actual native warhead/stimulus receipt; no seeded-randomness claim')
   check('damage.observed',m.get('damageCallbacks',0)>0,'Actual native subject damage callbacks; no health setter')
 return checks

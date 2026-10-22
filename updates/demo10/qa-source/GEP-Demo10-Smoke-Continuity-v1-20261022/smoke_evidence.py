"""MechanismControl collector; original analyzer reused unchanged, no native actions."""
from pathlib import Path
import re,struct,math
from evidence import read_json,sha
from analyze_wcs_smoke_supplier import analyze
IDS={'7E0150C026106500','7E0150C026101500','D561AD08932E7B40','629B2BA37EFFD577','58D0FB3206B6F859','5614BBCCBB55ED1C','5E51C5D8F04FDB80','5E389BB9F58B79A6'}
SCOPE='WCS_DEPENDENCY_ENGINEERING_DIAGNOSTIC'
def untested_acceptance():
 return {name:{'status':'NOT_TESTED','reason':reason} for name,reason in {
  'smoke.visible-cloud':'Capture files/telemetry do not establish visible cloud',
  'smoke.impact':'Native movement does not prove impact/fuse deployment',
  'smoke.coverage':'Useful screening, gaps, placement and duration need pixel and user review',
  'smoke.manual-input':'Program callback is scripted; ordinary roles/keybinding not exercised',
  'smoke.rearm-cooldown-roles':'No manual supply accounting or denial/cooldown case is bound',
  'smoke.mp-jip':'No multiplayer/dedicated/JIP acceptance in this case',
  'art.v11-smoke-registration':'Exact resource/public-bank currentV11 case only; actual eight tube fit/nonzero hull or turret/visual acceptance remain untested',
  'package.trio':'Dependency trio pins are identity only, not optional-addon publication/package acceptance',
  'visual.approved':'No pixel/cloud/geometry judgment in this collector'
 }.items()}
def combine(analysis,names,reason):
 items={n:analysis['deterministicTelemetry']['checks'][n] for n in names}
 states=[v['status'] for v in items.values()]
 return {'status':'FAIL' if 'FAIL' in states else 'NOT_TESTED' if 'NOT_TESTED' in states else 'PASS','reason':reason,'actual':items}
def scope_console(text,world):
 lines=text.splitlines();assert len(lines)<=500000,'Console exceeds line bound'
 def unique(pattern):
  hits=[(i,re.search(pattern,line)) for i,line in enumerate(lines) if re.search(pattern,line)]
  assert len(hits)==1,'Missing/duplicate '+pattern;return hits[0]
 loaded_at,_=unique(r'ENGINE\s*: Loaded addons:\s*$');loaded=[]
 for line in lines[loaded_at+1:]:
  m=re.search(r"ENGINE\s*: gproj: '.*?' guid: '([0-9a-fA-F]{16})'",line)
  if not m:break
  loaded.append(m[1].upper())
 assert len(loaded)==8 and len(set(loaded))==8 and set(loaded)==IDS,'Wrong/missing/duplicate addon set'
 world_at,_=unique(r"WORLD\s*:.*Entities load '\{?[A-Fa-f0-9]*\}?"+re.escape(world)+r"'")
 game_at,_=unique(r'SCR_BaseGameMode::OnGameStateChanged = GAME\b')
 assert loaded_at<world_at<game_at,'Loaded/world/GAME out of order'
 program=[i for i,line in enumerate(lines) if 'GEP215_WCS_PROGRAM ' in line]
 preflight=[i for i,line in enumerate(lines) if 'GEP215_WCS_PREFLIGHT ' in line]
 terminal=[i for i,line in enumerate(lines) if 'GEP215_WCS_TERMINAL ' in line]
 # Missing/failing mechanism evidence is classified by the original analyzer.
 if len(preflight)==len(program)==len(terminal)==1:assert game_at<preflight[0]<program[0]<terminal[0],'Mechanism markers out of order'
 modes=[re.search(r'GEP215_WCS_CAPTURE_MODE enabled=(false|true|0|1)\s*$',line) for line in lines if 'GEP215_WCS_CAPTURE_MODE ' in line]
 assert len(modes)==1 and modes[0] and modes[0][1] in ('false','0'),'Camera-off mode marker absent/duplicated/enabled'
 forbidden=['GEP215_WCS_CAPTURE_SETUP ','GEP215_WCS_CAPTURE_UNAVAILABLE ','GEP215_WCS_CAPTURE pair=','GEP215_WCS_FRAME ','GEP215_WCS_BASELINE ']
 assert not any(token in line for token in forbidden for line in lines),'Unexpected custom-camera/capture branch in camera-off case'
 errors=[{'line':i+1,'text':line} for i,line in enumerate(lines) if re.search(r'\b[A-Z]+\s*\([EF]\)|Cannot compile|Can.t compile|Unhandled exception|Fatal error',line,re.I)]
 return {'loadedAddonIds':loaded,'worldLine':world_at+1,'gameLine':game_at+1,'strictNativeErrors':errors}
def image(path,started,width,height):
 assert 54<=path.stat().st_size<=64*1024**2,'Image missing/oversize'
 assert path.stat().st_mtime>=started-2,'Stale capture'
 with path.open('rb') as f:h=f.read(54)
 assert h[:2]==b'BM','Only actual native BMP files are bound'
 w,y=struct.unpack_from('<ii',h,18);bits=struct.unpack_from('<H',h,28)[0];compression=struct.unpack_from('<I',h,30)[0];offset=struct.unpack_from('<I',h,10)[0]
 assert w==width and abs(y)==height and bits in (24,32) and compression in (0,3),'Capture dimensions/encoding differ'
 assert offset>=54 and path.stat().st_size>=offset+((w*bits+31)//32)*4*abs(y),'Incomplete pixel payload'
 return {'path':str(path),'sha256':sha(path),'bytes':path.stat().st_size,'width':w,'height':abs(y),'pixelsReviewed':False}
def collect(native,started,world,parameters):
 native=Path(native);console=native/'logs/console.log'
 assert console.is_file() and console.stat().st_size<=64*1024**2 and console.stat().st_mtime>=started-2,'Missing/stale/oversize exact console'
 text=console.read_bytes().decode('utf-8-sig');assert str(native/'profile').casefold() in text.casefold(),'Exact owned profile binding absent'
 scope=scope_console(text,world)
 candidates=[p for p in [native/'profile',native/'profile/profile'] if (p/'GEP215-wcs-result.json').is_file()]
 assert len(candidates)==1,'Missing/ambiguous actual $profile terminal root'
 profile=candidates[0];jsons=[profile/'GEP215-wcs-result.json',*sorted(profile.glob('GEP215-wcs-life-*.json'))]
 assert 1<len(jsons)<=1024,'Missing/excessive lifecycle samples'
 inputs=[]
 for path in jsons:
  assert path.stat().st_mtime>=started-2,'Stale JSON telemetry'
  value=read_json(path);assert isinstance(value,dict),'Non-object telemetry'
  assert value.get('scope')==SCOPE,'Wrong native telemetry scope'
  inputs.append({'path':str(path),'sha256':sha(path)})
 assert sum(p.stat().st_size for p in jsons)<=8*1024**2,'Telemetry total bound exceeded'
 terminal=read_json(jsons[0]);assert terminal.get('vehiclePrefab')==parameters['expectedVehiclePrefab'] and terminal.get('fixtureYaw')==0,'Wrong fixture/variant scope'
 assert terminal.get('captureEnabled') is False and terminal.get('captureUnavailable') is False,'Camera-off terminal scope differs'
 assert terminal.get('screenshotsRequested')==0 and terminal.get('pairedViewRequests')==0 and terminal.get('captureFailureReason')=='','Unexpected capture request/failure in camera-off terminal'
 assert not any(p.is_file() and p.suffix.lower() in ('.bmp','.png','.jpg','.jpeg') for p in profile.glob('GEP215-wcs-*')),'Unexpected screenshot files in camera-off case'
 analysis=analyze(native,'full',console,profile)
 current_rows={}
 for line in text.splitlines():
  match=re.search(r'\bGEP215_CURRENT_(RESOURCE|BANK|BANK_TOTAL|HUD|POSE)\s+(.+)$',line)
  if not match:continue
  fields={m[1]:m[2].strip() for m in re.finditer(r'\b([A-Za-z][A-Za-z0-9]*)=(.*?)(?=\s+[A-Za-z][A-Za-z0-9]*=|$)',match[2])}
  current_rows.setdefault(match[1],[]).append(fields)
 def flag(value):return value in ('1','true')
 resource_rows=current_rows.get('RESOURCE',[]);totals=current_rows.get('BANK_TOTAL',[]);bank_rows=current_rows.get('BANK',[]);hud_rows=current_rows.get('HUD',[])
 resource_ok=len(resource_rows)==2 and {r.get('phase') for r in resource_rows}=={'preflight','terminal'} and all(r.get('actual')==r.get('requested')==parameters['expectedVehiclePrefab'] and flag(r.get('matches')) and flag(r.get('approvedVariant')) and float(r.get('authoredHullYaw','nan'))==0 for r in resource_rows)
 resource_ok=resource_ok and all(re.fullmatch(r'0x[0-9A-Fa-f]{16} \{\}',r.get(key,'')) for r in resource_rows for key in ('hull','turret')) and len({(r.get('hull'),r.get('turret')) for r in resource_rows})==1
 bank_ok=len(totals)==2 and len(bank_rows)==16
 identities={}
 for phase,current in [('preflight',8),('terminal',0)]:
  phase_totals=[r for r in totals if r.get('phase')==phase];phase_banks=[r for r in bank_rows if r.get('phase')==phase]
  bank_ok=bank_ok and len(phase_totals)==1 and len(phase_banks)==8
  if phase_totals:bank_ok=bank_ok and phase_totals[0].get('count')=='8' and phase_totals[0].get('capacity')=='8' and phase_totals[0].get('current')==str(current) and flag(phase_totals[0].get('valid'))
  groups={}
  for row in phase_banks:groups.setdefault((row.get('owner'),row.get('ownerPrefab')),[]).append(row)
  expected={'{7E0150C026101503}Prefabs/Weapons/GEP_WCSSmokeBankLeft215.et','{7E0150C026101504}Prefabs/Weapons/GEP_WCSSmokeBankRight215.et'}
  bank_ok=bank_ok and len(groups)==2 and len({k[0] for k in groups})==2 and {k[1] for k in groups}==expected
  for (owner,prefab),rows in groups.items():
   bank_ok=bank_ok and bool(re.fullmatch(r'0x[0-9A-Fa-f]{16} \{\}',owner or '')) and len(rows)==4 and {r.get('id') for r in rows}=={'0','1','2','3'} and all(r.get('capacity')=='1' and r.get('current')==str(current//8) for r in rows)
  identities[phase]={(r.get('owner'),r.get('id'),r.get('ownerPrefab')) for r in phase_banks}
 bank_ok=bank_ok and len(identities.get('preflight',set()))==8 and identities.get('preflight')==identities.get('terminal')
 hud_ok=len(hud_rows)==2 and {r.get('phase') for r in hud_rows}=={'preflight','terminal'} and all(flag(r.get('workspace')) and r.get('observationOnly')=='1' for r in hud_rows)
 captures=[];capture_status='NOT_TESTED'
 checks={
  'smoke.current-v11-identity':{'status':'PASS' if resource_ok else 'FAIL','reason':'Actual requested/instantiated exact currentV11 skin resource and full native hull/turret IDs, preflight/terminal; no visual fit acceptance','actual':resource_rows},
  'smoke.current-v11-banks':{'status':'PASS' if bank_ok else 'FAIL','reason':'Actual eight public dispenser IDs, count/capacity/current8to0 under actual owned V11 hierarchy; bank ENTITY poses are not tube fire matrices','actual':{'totals':totals,'banks':bank_rows}},
  'smoke.current-hud-observations':{'status':'PASS' if hud_ok else 'NOT_TESTED','reason':'Read-only current widget presence/visibility/ADS observations only; no HUD usability/lifecycle acceptance','actual':hud_rows},
  'world.loaded':{'status':'PASS','reason':'Exact fresh owned console loaded only approved eight addons and bound world/GAME'},
  'fixture.ready':combine(analysis,['settledCrewInitialEight','terminalSettledCrew'],'Actual probe settled-crew capacity8 preflight and terminal state; scripted fixture only'),
  'smoke.program-payload':combine(analysis,['inputSchemaValid','oneAuthorizedProgramRequest','terminalProgramRecorded','terminalScopeMatches','oneSuccessfulDiagnosticTerminal','terminalAmmoMatches','uniqueOrderedSamples','ammoReachesAndRemainsExpected','ammoNeverIncreases','exactUniqueNativeLaunches','allKnownCountsBounded','readbackIdentityMatchesNativeLaunches','oneEmptyProgramAfterInitial','emptyProgramRecorded','continuedEmptyObservation','emptyAddsNoObservedAmmoOrPayload'],'Original analyzer: one scripted full program, ammo8->0, exactly eight observed launched payloads and one empty negative request and bounded native readbacks'),
  'smoke.empty-negative':combine(analysis,['oneEmptyProgramAfterInitial','emptyProgramRecorded','continuedEmptyObservation','emptyAddsNoObservedAmmoOrPayload'],'One explicit empty request after initial program; continued zero-ammo/eight-known-payload readbacks, no ninth observed native launch'),
  'smoke.flight-telemetry':combine(analysis,['launchOriginsNearFixture','allObservedPositionsNearFixture','actualMotionForEveryPayload'],'Observed native positions/motion only; no impact, clearance or cloud claim'),
  'smoke.capture-files':{'status':'NOT_TESTED','reason':'CaptureEnabled0 deliberately disables all custom-camera/screenshot branches; no capture/pixel acceptance','actual':{'captureEnabled':False,'screenshotsRequested':0,'pairedViewRequests':0}},
  'native.errors':{'status':'FAIL' if scope['strictNativeErrors'] else 'PASS','reason':'All native engine/script errors retained; no baseline whitelist','actual':scope['strictNativeErrors']}
 }
 return {'scope':scope,'profile':str(profile),'consoleSHA256':sha(console),'analysis':analysis,'assertions':checks,'inputs':inputs,'captureFiles':captures,'currentV11Observations':current_rows,'acceptance':untested_acceptance(),'nativeNonce':'Probe has no nonce; exact fresh owner/profile/argv/source association must be qualified'}

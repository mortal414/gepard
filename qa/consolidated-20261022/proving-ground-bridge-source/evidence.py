"""Pure GPG source-bound startup/survey parser; never controls a process."""
from pathlib import Path
import hashlib,json,re,math
def sha(p):
 h=hashlib.sha256()
 with Path(p).open('rb') as f:
  for b in iter(lambda:f.read(1024*1024),b''):h.update(b)
 return h.hexdigest()
def read_json(p):
 p=Path(p);assert p.stat().st_size<=4*1024**2,'JSON exceeds bound'
 def pairs(rows):
  out={}
  for k,v in rows:assert k not in out,'Duplicate JSON key';out[k]=v
  return out
 def invalid(v):raise ValueError('Nonfinite JSON: '+v)
 def finite(v):
  n=float(v);assert math.isfinite(n),'Nonfinite JSON float';return n
 return json.loads(p.read_bytes().decode('utf-8-sig'),object_pairs_hook=pairs,parse_constant=invalid,parse_float=finite)
def save(p,value):
 with Path(p).open('x',encoding='utf-8') as f:json.dump(value,f,indent=2,allow_nan=False);f.write('\n')
def assertion(ok,reason,actual=None):
 row={'status':'PASS' if ok else 'NOT_TESTED','reason':reason}
 if actual is not None:row['actual']=actual
 return row
def untested_acceptance():
 return {name:{'status':'NOT_TESTED','reason':reason} for name,reason in {
  'scripts.compiled':'Startup marker nativeCompiled=NOT_TESTED is not compiler proof; actual separate compile adoption required',
  'package.trio':'No GPG pack/hash .gproj/.rdb/.pak qualification implemented in this startup bridge',
  'input.physical':'No ordinary human input/boarding/ADS performed by this case',
  'player.usable':'GPG_READY does not measure alive/interact/menu/loading or ordinary control usability',
  'visual.approved':'No screenshot/pixel or human station/art/HUD judgment performed',
  'glyphs.reviewed':'No native glyph review performed',
  'drive.accepted':'Route markers/terrain grade are not measured driving or accepted handling',
  'terrain.usable':'Reported terrain samples do not establish clearance, surface contact or navigability',
  'range.visibility':'Terrain-only TraceMove fraction is not complete LOS or a qualified usable firing lane',
  'range.impact':'No weapon firing, measured hit/impact or ballistic correction tested',
  'service.accepted':'Supply initialization is not transfer, repair, rearm or resource-cost evidence',
  'damage.accepted':'Object startup is not native damage or surviving crew/vehicle behavior',
  'art.new-bound':'NEW-CAMO and NEW-CHARCOAL remain unbound',
  'hostile.qualified':'Hostiles remain off; containment and AI are not qualified',
  'smoke.accepted':'Optional smoke excluded','audio.accepted':'Optional track audio excluded'
 }.items()}

NUMBER=r'[-+]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][-+]?\d+)?'
def number(s):
 assert re.fullmatch(NUMBER,s.strip()),'Malformed/nonfinite number';n=float(s);assert math.isfinite(n),'Nonfinite number';return n
def vector(s):
 s=s.strip()
 if s and s[0] in '<(\"':
  assert s[-1]=={'<':'>','(':')','"':'"'}[s[0]],'Mismatched vector delimiters';s=s[1:-1]
 parts=re.split(r'\s*,\s*|\s+',s.strip());assert len(parts)==3,'Vector must have three coordinates';return list(map(number,parts))
def horizontal(actual,pad):
 assert all(abs(actual[i]-pad[i])<=0.05 for i in (0,2)),'Reported x/z differs from pinned source'
def review_file(console,started,profile,world,contract):
 console=Path(console);assert console.is_file(),'Missing exact owned console'
 assert console.stat().st_mtime>=started-2,'Stale console'
 assert console.stat().st_size<=64*1024**2,'Console exceeds bound'
 text=console.read_bytes().decode('utf-8-sig')
 assert profile.casefold() in text.casefold(),'Fresh exact profile binding absent'
 return review_console(text,world,contract)
def review_console(text,world,contract):
 assert len(text.encode('utf-8'))<=64*1024**2,'Console exceeds bound'
 lines=text.splitlines();assert len(lines)<=500000,'Too many console lines'
 for token in ['GPG_STAGED ','GPG_READY ']:assert sum(token in line for line in lines)==1,'Missing/duplicate/malformed startup marker'
 def unique(pattern):
  hits=[(i,re.search(pattern,line)) for i,line in enumerate(lines) if re.search(pattern,line)]
  assert len(hits)==1,'Expected one '+pattern+'; got '+str(len(hits));return hits[0]
 def rows(prefix,pattern):
  result={}
  for i,line in enumerate(lines):
   if prefix not in line:continue
   m=re.search(pattern,line);assert m,'Malformed '+prefix
   assert m[1] not in result,'Duplicate '+prefix+' '+m[1];result[m[1]]=(i,m)
  return result
 loaded_at,_=unique(r'ENGINE\s*: Loaded addons:\s*$');loaded=[]
 for line in lines[loaded_at+1:]:
  match=re.search(r"ENGINE\s*: gproj: '.*?' guid: '([A-Fa-f0-9]{16})'",line)
  if not match:break
  loaded.append(match[1].upper())
 assert len(loaded)==len(set(loaded))==4 and set(loaded)==set(contract['loadedAddonIds']),'Unapproved/missing/duplicate loaded addon identity'
 world_at,_=unique(r"WORLD\s*:.*Entities load '\{?[A-Fa-f0-9]*\}?"+re.escape(world)+r"'")
 game_at,_=unique(r'SCR_BaseGameMode::OnGameStateChanged = GAME\b')
 staged_at,staged=unique(r'GPG_STAGED registry=([a-f0-9]{64}) source=([a-f0-9]{64}) nativeCompiled=NOT_TESTED\s*$')
 ready_at,ready=unique(r'GPG_READY staged=(true|false|1|0) resourceFailure=(true|false|1|0) objects=(\d+) nativeBehaviorAcceptance=NOT_TESTED\s*$')
 assert loaded_at<world_at<game_at<staged_at<ready_at,'Out-of-order loaded/world/GAME/staged/ready markers'
 assert staged[1]==contract['registrySHA256'] and staged[2]==contract['runtimeSHA256'],'Wrong registry/runtime source hash'
 assert ready[1] in ('true','1') and ready[2] in ('false','0'),'Startup reports staging/resource failure'
 assert int(ready[3])==len(contract['objects']),'Wrong object count'
 forbidden=['GPG_RESOURCE_FAIL','GPG_SPAWN_FAIL','GPG_COMMAND','GPG_RESET','GPG_RESPAWN','GPG_HOSTILE']
 assert not any(token in line for token in forbidden for line in lines),'Resource/spawn failure or unrequested command/reset/respawn/hostile operation'
 objects=rows('GPG_OBJECT ',r'GPG_OBJECT id=(.+?) station=(\S+) epoch=(\d+) prefab=(\S+) origin=(.+?)\s*$')
 assert set(objects)==set(contract['objects']),'Object identity set differs, including unbound art'
 for id,(at,m) in objects.items():
  expected=contract['objects'][id];assert game_at<at<staged_at,'Object outside initial staging window'
  assert m[2]==expected['station'] and int(m[3])==0 and m[4]==expected['prefab'],'Object station/epoch/prefab differs';horizontal(vector(m[5]),expected['pad'])
 stations=rows('GPG_STATION ',r'GPG_STATION id=(.+?) board=(.+?) title=(.+?)\s*$')
 assert set(stations)==set(contract['stations']),'Station identity set differs'
 for id,(at,m) in stations.items():assert game_at<at<staged_at,'Station outside staging';horizontal(vector(m[2]),contract['stations'][id])
 gates=rows('GPG_GATE ',r'GPG_GATE id=(.+?) origin=(.+?) gradeDegrees=(\S+) surface=NOT_TESTED\s*$')
 assert set(gates)==set(contract['gates']),'Gate identity set differs'
 grades={}
 for id,(at,m) in gates.items():
  assert game_at<at<staged_at,'Gate outside staging';horizontal(vector(m[2]),contract['gates'][id]);grade=number(m[3]);assert -90<=grade<=90,'Invalid signed terrain grade';grades[id]=grade
 stocks=rows('GPG_STOCK ',r'GPG_STOCK source=(\S+) supplies=(\S+) initializationOnly=1\s*$')
 assert set(stocks)==set(contract['stocks']),'Missing/extra stock initialization marker'
 for at,m in stocks.values():assert game_at<at<staged_at and abs(number(m[2])-1000)<.01,'Wrong initial stock/time'
 surveys=rows('GPG_SURVEY ',r'GPG_SURVEY target=(\S+) nominalHorizontal=(\S+) slant=(\S+) terrainTraceFraction=(\S+) line=(.+?) aim=(.+?) nativeVisibilityReview=NOT_TESTED\s*$')
 assert set(surveys)=={t['id'] for t in contract['targets']},'Missing/extra target survey'
 measurements=[]
 for target in contract['targets']:
  at,m=surveys[target['id']];assert game_at<at<staged_at,'Survey outside staging'
  nominal,slant,fraction=map(number,m.group(2,3,4));line=vector(m[5]);aim=vector(m[6])
  assert nominal==target['nominal'],'Wrong nominal horizontal target label'
  horizontal(line,[900,0,2450]);horizontal(aim,target['pad'])
  h=math.hypot(aim[0]-line[0],aim[2]-line[2]);distance=math.dist(line,aim)
  assert abs(h-nominal)<=.1,'Horizontal target radius inconsistent'
  assert abs(distance-slant)<=max(.15,distance*.0001),'Slant inconsistent with reported endpoints'
  assert slant>=h-.15 and 0<=fraction<=1,'Invalid slant/fraction'
  measurements.append({'target':target['id'],'nominalHorizontal':nominal,'reportedSlant':slant,'reportedTerrainFraction':fraction,'line':line,'aim':aim,'visibilityAcceptance':'NOT_TESTED'})
 errors=[{'line':i+1,'text':line} for i,line in enumerate(lines) if re.search(r'\b(?:SCRIPT|ENGINE|WORLD|ENTITY|RESOURCES|RPL|NETWORK)\s+\(E\)',line)]
 return {'status':'STARTUP_MARKERS_AND_TELEMETRY_CONSISTENT_ONLY','markers':{'loaded':loaded_at+1,'world':world_at+1,'game':game_at+1,'staged':staged_at+1,'ready':ready_at+1},'loadedAddonIds':loaded,'objectCount':len(objects),'stationCount':len(stations),'gateGrades':grades,'survey':measurements,'diagnostics':{'errors':errors},'acceptance':untested_acceptance(),'nativeNonce':'Not emitted; fresh owner/argv/profile/source linkage requires separate native qualification.'}

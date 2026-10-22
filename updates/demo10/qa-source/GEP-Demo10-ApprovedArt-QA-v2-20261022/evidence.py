"""Source-bound native art artifact collection; never controls any process."""
from pathlib import Path
import hashlib,json,re,math
from types import SimpleNamespace
import review_run

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
  'visual.approved':'BMP artifact completeness does not judge unobstructed framing, geometry, textures or Marco appearance acceptance',
  'materials.gpu':'Prefab config/resource loads are not effective GPU material bindings; deleted tire roles unresolved',
  'bones.parents':'Reviewed public API does not expose skeleton parent identity',
  'skinning.dynamic':'Native bone readout and still images do not test moving skinning',
  'input.physical':'Only private QA camera actor control; no ordinary human vehicle input',
  'drive.accepted':'No vehicle control, braking or motion actions',
  'art.new-bound':'NEW pads remain unbound by this bridge',
  'package.trio':'No packaged addon qualification',
  'smoke.accepted':'Separate smoke lane', 'audio.accepted':'Separate audio lane'
 }.items()}

def review_console(text,world,contract):
 assert len(text.encode('utf-8'))<=64*1024**2,'Console exceeds bound'
 lines=text.splitlines();assert len(lines)<=500000,'Too many console lines'
 def unique(pattern):
  hits=[(i,re.search(pattern,line)) for i,line in enumerate(lines) if re.search(pattern,line)]
  assert len(hits)==1,'Missing/duplicate source-bound marker: '+pattern
  return hits[0]
 loaded_at,_=unique(r'ENGINE\s*: Loaded addons:\s*$');loaded=[]
 for line in lines[loaded_at+1:]:
  match=re.search(r"ENGINE\s*: gproj: '.*?' guid: '([A-Fa-f0-9]{16})'",line)
  if not match:break
  loaded.append(match[1].upper())
 assert len(loaded)==len(set(loaded))==len(contract['loadedAddonIds']) and set(loaded)==set(contract['loadedAddonIds']),'Unapproved/missing/duplicate loaded addon identity'
 world_at,_=unique(r"WORLD\s*:.*Entities load '\{?"+re.escape(contract['worldGuid'])+r"\}?"+re.escape(world)+r"'")
 game_at,_=unique(r'SCR_BaseGameMode::OnGameStateChanged = GAME\b')
 begin_at,_=unique(r'GEP_ART_SCENE begin=1 actualFunctionalSubjects=2 plannedViews=14 staticDisplays=0 vehicleInputs=0\s*$')
 assert loaded_at<world_at<game_at<begin_at,'Out-of-order native scene startup'
 return {'loadedAddonIds':loaded,'markers':{'loaded':loaded_at+1,'world':world_at+1,'game':game_at+1,'sceneBegin':begin_at+1}}

def review_file(console,started,profile,world,contract):
 console=Path(console);assert console.is_file(),'Missing exact owned console'
 assert console.stat().st_mtime>=started-2,'Stale console'
 assert console.stat().st_size<=64*1024**2,'Console exceeds bound'
 text=console.read_bytes().decode('utf-8-sig');lines=text.splitlines()
 errors=[{'line':i+1,'text':line} for i,line in enumerate(lines) if re.search(r'\([EF]\)|Cannot compile|Can.t compile|Unhandled exception|Fatal error|\berror:',line,re.I)]
 warnings=[{'line':i+1,'text':line} for i,line in enumerate(lines) if re.search(r'\(W\)',line)]
 startup={};failures=[]
 try:
  assert profile.casefold() in text.casefold(),'Exact fresh profile binding absent'
  startup=review_console(text,world,contract)
 except Exception as error:failures.append(type(error).__name__+': '+str(error))
 artifact_path=console.parent.parent/'artifacts-review.json'
 code=review_run.main(SimpleNamespace(log=console,profile=Path(profile),output=artifact_path))
 artifact=read_json(artifact_path)
 if code:failures += artifact['failures']
 return {'status':'READOUT_AND_IMAGE_FILES_PRESENT_VISUAL_REVIEW_PENDING' if not failures else 'INCOMPLETE_OR_FAILED_NATIVE_EVIDENCE',
  'startup':startup,'failures':failures,'artifactsReport':str(artifact_path),'artifactsReportSHA256':sha(artifact_path),
  'images':artifact['images'],'diagnostics':{'errors':errors,'warnings':warnings,'suppression':'NONE'},'acceptance':untested_acceptance()}

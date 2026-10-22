"""Existing HUD705 console/file contract. Reads only; no native control."""
from pathlib import Path
import hashlib,json,re,math,struct
def sha(p):
 h=hashlib.sha256()
 with Path(p).open('rb') as f:
  for b in iter(lambda:f.read(1024*1024),b''):h.update(b)
 return h.hexdigest()
def read_json(p):
 p=Path(p);assert p.stat().st_size<=4*1024**2,'JSON too large'
 def pairs(rows):
  out={}
  for k,v in rows:assert k not in out,'Duplicate JSON key';out[k]=v
  return out
 def invalid(v):raise ValueError('Nonfinite JSON: '+v)
 return json.loads(p.read_bytes().decode('utf-8-sig'),object_pairs_hook=pairs,parse_constant=invalid)
def save(p,value):
 with Path(p).open('x',encoding='utf-8') as f:json.dump(value,f,indent=2,allow_nan=False);f.write('\n')
def assertion(ok,reason,actual=None):
 row={'status':'PASS' if ok else 'NOT_TESTED','reason':reason}
 if actual is not None:row['actual']=actual
 return row
def review_console(text,world):
 assert len(text.encode('utf-8'))<=64*1024**2,'Console exceeds bound'
 lines=text.splitlines();assert len(lines)<=500000,'Console line count exceeds bound'
 def unique(pattern):
  hits=[(i,re.search(pattern,line)) for i,line in enumerate(lines) if re.search(pattern,line)]
  assert len(hits)==1,'Expected one '+pattern+'; got '+str(len(hits))
  return hits[0]
 positions={}
 loaded_at,_=unique(r'ENGINE\s*: Loaded addons:\s*$');loaded=[]
 for line in lines[loaded_at+1:]:
  match=re.search(r"ENGINE\s*: gproj: '.*?' guid: '([A-Fa-f0-9]{16})'",line)
  if not match:break
  loaded.append(match[1].upper())
 assert len(loaded)==4 and set(loaded)=={'7E0050C026102500','D561AD08932E7B40','58D0FB3206B6F859','5614BBCCBB55ED1C'},'Loaded addon identities missing/duplicated/unapproved'
 positions['world'],_=unique(r"WORLD\s*:.*Entities load '\{?[A-Fa-f0-9]*\}?"+re.escape(world)+r"'")
 positions['game'],_=unique(r'SCR_BaseGameMode::OnGameStateChanged = GAME\b')
 positions['scene'],_=unique(r'GEP_UPDATE101_SCENE_STAGED baseline=1 candidate=1 automaticBoarding=0 inputWrites=0 timeLimit=none\s*$')
 positions['player'],_=unique(r'GEP_UPDATE101_SCENE_PLAYER ready=1 alive=1 canInteract=1 loading=0 menu=0 playerCamera=1 renderEnabled=1 physicalInput=NOT_TESTED\s*$')
 positions['boarding'],_=unique(r'GEP_APPROVED_RENDER705 boarding=1 case=0 style=705 physicalInput=0\s*$')
 positions['ads'],_=unique(r'GEP_APPROVED_RENDER705 native_ready=1 case=0 style=705 seated=1 ADS=1 visible=1 physicalInput=0\s*$')
 positions['data'],data=unique(r'GEP_APPROVED_RENDER705 data group=(-?\d+) left=(-?\d+)/(-?\d+) right=(-?\d+)/(-?\d+) rangeState=(\d+) rangeMetres=([-+\d.eE]+) rangeAge=([-+\d.eE]+) magnification=([-+\d.eE]+)\s*$')
 positions['widgets'],widgets=unique(r'GEP_APPROVED_RENDER705 widgets left=(.*?) right=(.*?) range=(.*?) zoom=(.*?)\s*$')
 positions['capture'],_=unique(r'GEP_APPROVED_RENDER705 capture=1 case=0 style=705 range=.* zoom=.* physicalInput=0\s*$')
 positions['terminal'],_=unique(r'GEP_APPROVED_RENDER705 terminal=CAPTURED cases=1 renderedInspection=PENDING physicalInput=0 accuracy=NOT_TESTED\s*$')
 assert list(positions.values())==sorted(positions.values()) and len(set(positions.values()))==len(positions),'Out-of-order native markers'
 assert not any('GEP_APPROVED_RENDER705 terminal=NOT_TESTED' in x for x in lines),'Native probe failed'
 assert sum('GEP_APPROVED_RENDER705 capture=' in x for x in lines)==1,'Duplicate or failed capture marker'
 nums=list(map(float,data.groups()));assert all(math.isfinite(n) for n in nums),'Nonfinite telemetry'
 group,left,leftcap,right,rightcap,state,metres,age,zoom=nums
 assert group in (0,1,2) and state in (0,1,2,3) and 2<=zoom<=8,'Unknown data scope/state/zoom'
 assert left>=-1 and right>=-1 and (leftcap==-1 or leftcap>0) and (rightcap==-1 or rightcap>0),'Invalid ammo telemetry'
 expected_left=str(int(left)) if left>=0 else '--';expected_right=str(int(right)) if right>=0 else '--'
 assert widgets[1]==expected_left and widgets[2]==expected_right,'Widget count does not match native data'
 expected_zoom=f'{zoom:.1f}'.replace('.',',')+'×';assert widgets[4]==expected_zoom,'Widget zoom mismatch'
 if state==1:assert metres>0 and age>=0 and widgets[3]==str(int(math.floor(metres+.5)))+' m','Invalid valid-range binding'
 elif state==2:assert widgets[3]=='KEIN MESSWERT','Wrong no-return label'
 elif state==3:assert widgets[3]=='MESSUNG NICHT VERFÜGBAR','Wrong unavailable label'
 else:assert widgets[3]=='-- m','Wrong unmeasured label'
 # Do not promote known engine errors to harmless without separate baseline proof.
 errors=[{'line':i+1,'text':line} for i,line in enumerate(lines) if re.search(r'\b(?:SCRIPT|ENGINE|WORLD|ENTITY|RESOURCES|RPL|NETWORK)\s+\(E\)',line)]
 return {'markers':{k:v+1 for k,v in positions.items()},'loadedAddonIds':loaded,'measurements':{'group':int(group),'left':int(left),'leftCapacity':int(leftcap),'right':int(right),'rightCapacity':int(rightcap),'rangeState':int(state),'rangeMetres':metres,'rangeAge':age,'magnification':zoom},'widgets':dict(zip(['left','right','range','zoom'],widgets.groups())),'diagnostics':{'beforeCapture':[e for e in errors if e['line']<=positions['terminal']+1],'afterCapture':[e for e in errors if e['line']>positions['terminal']+1]},'lineCount':len(lines),'nativeNonce':'Not emitted by current probe; fresh exact-owned profile/argv/source closure is required externally.'}
def screenshot(native_dir,started,width,height):
 candidates=[native_dir/'profile/approved705-render-case-0.bmp',native_dir/'profile/profile/approved705-render-case-0.bmp']
 found=[p for p in candidates if p.is_file()];assert len(found)==1,'Missing/ambiguous exact native screenshot'
 p=found[0];assert 54<=p.stat().st_size<=64*1024**2,'BMP size outside bound'
 assert p.stat().st_mtime>=started-2,'Stale screenshot timestamp'
 with p.open('rb') as f:header=f.read(54)
 assert header[:2]==b'BM','Native screenshot is not BMP'
 w,h=struct.unpack_from('<ii',header,18);assert w==width and abs(h)==height,'Screenshot dimensions mismatch'
 offset=struct.unpack_from('<I',header,10)[0];bits=struct.unpack_from('<H',header,28)[0];compression=struct.unpack_from('<I',header,30)[0]
 assert bits in (24,32) and compression in (0,3),'Unsupported native BMP encoding'
 assert p.stat().st_size>=offset+((w*bits+31)//32)*4*abs(h),'Incomplete screenshot pixel payload'
 return {'path':str(p),'sha256':sha(p),'bytes':p.stat().st_size,'width':w,'height':abs(h),'contentInspected':False,'glyphAcceptance':'NOT_TESTED'}

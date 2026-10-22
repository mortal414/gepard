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

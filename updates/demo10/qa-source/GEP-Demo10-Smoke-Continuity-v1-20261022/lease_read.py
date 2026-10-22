"""Bounded read-only retry for the one shared lease; no permission changes."""
from pathlib import Path
import json,math,time
SLOT=Path(r'C:\Users\marco\Documents\Codex\2026-10-12\task\RUNTIME_SLOT.json')
class LeaseReadUncertain(RuntimeError):pass
def decode_lease(raw):
 assert len(raw)<=4*1024**2,'Lease exceeds bound'
 def pairs(rows):
  out={}
  for k,v in rows:assert k not in out,'Duplicate lease key';out[k]=v
  return out
 def invalid(v):raise ValueError('Nonfinite lease JSON: '+v)
 def finite(v):
  value=float(v);assert math.isfinite(value),'Nonfinite lease float';return value
 lease=json.loads(raw.decode('utf-8-sig'),object_pairs_hook=pairs,parse_constant=invalid,parse_float=finite)
 assert isinstance(lease,dict),'Lease is not an object'
 assert isinstance(lease.get('status'),str) and lease['status'].strip(),'Missing/malformed lease status'
 if 'pid' in lease:assert lease['pid'] is None or type(lease['pid']) is int and lease['pid']>0,'Malformed lease PID'
 if 'activeRuntime' in lease:assert lease['activeRuntime'] is None or isinstance(lease['activeRuntime'],str) and lease['activeRuntime'],'Malformed activeRuntime'
 if 'userRuntimeHold' in lease:assert isinstance(lease['userRuntimeHold'],dict) and type(lease['userRuntimeHold'].get('active')) is bool,'Malformed hold record'
 return lease
def read_shared_lease(path=SLOT,*,reader=None,clock=time.monotonic,sleeper=time.sleep,timeout=1.0):
 assert Path(path).resolve()==SLOT.resolve(),'Only the exact shared lease has retry semantics'
 assert 0<timeout<=1.0,'Lease read retry capped at1second'
 reader=reader or (lambda p:Path(p).read_bytes())
 deadline=clock()+timeout;attempts=0;last=None
 while True:
  attempts+=1
  try:
   raw=reader(SLOT)
   if not raw.strip():raise EOFError('Empty atomic-write lease snapshot')
   return decode_lease(raw)
  except PermissionError as error:last=error
  except EOFError as error:last=error
  except json.JSONDecodeError as error:
   # Retry only a plausible unfinished snapshot; arbitrary malformed JSON fails now.
   if error.pos<len(error.doc.rstrip())-1 and not error.msg.startswith('Unterminated string'):raise
   last=error
  except UnicodeDecodeError as error:
   if error.end!=len(error.object):raise
   last=error
  left=deadline-clock()
  if left<=0 or attempts>=21:raise LeaseReadUncertain('Shared lease unreadable after bounded read-only retry; preserve ownership') from last
  sleeper(min(.05,left))

def observe_owned_wrapper(child,seconds,*,clock=time.monotonic,sleeper=time.sleep):
 """Observe/drain owner only; never signal, close, kill or release it."""
 assert 0<=seconds<=120,'Observation bound exceeds existing adapter deadline'
 deadline=clock()+seconds
 while child.poll() is None:
  if clock()>=deadline:return False
  sleeper(min(.1,max(0,deadline-clock())))
 return True

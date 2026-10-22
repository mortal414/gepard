from pathlib import Path
import re,json,math,sys,statistics,hashlib
p=Path(sys.argv[1]);text=(p/'logs/console.log').read_text(encoding='utf-8-sig',errors='replace')
def rows(marker):
 out=[]
 for line in text.splitlines():
  if marker+' ' not in line:continue
  body=line.split(marker+' ',1)[1]
  out.append(dict(re.findall(r'(\w+)=(<[^>]*>|.*?)(?= \w+=|$)',body)))
 return out
motion={r['sample']:r for r in rows('GEP_CORE_WHEEL_MOTION')}
phases={}
for state in rows('GEP_CORE_WHEEL_STATE'):
 sample=state['sample'];m=motion.get(sample)
 if not m:continue
 velocity=[float(x) for x in m['velocity'].strip('<>').split(',')]
 speed=math.sqrt(sum(x*x for x in velocity))
 phases.setdefault(state['phase'],[]).append({'sample':int(sample),'worldMs':float(state['worldMs']),'speed':speed,'controllerOn':state['controllerOn'],'simOn':state['simOn'],'position':m['position']})
summary={k:{'count':len(v),'speedMin':min(x['speed'] for x in v),'speedMax':max(x['speed'] for x in v),'last':v[-1]} for k,v in phases.items()}
out={'run':str(p),'consoleSHA256':hashlib.sha256((p/'logs/console.log').read_bytes()).hexdigest(),'done':rows('GEP_CORE_WHEEL_DONE'),'bindings':rows('GEP033_AUDIO_BIND_SLOT'),'phases':summary,'parentPool':rows('GEP033_AUDIO_PARENT_POOL'),'surfaces':sorted(set(r['surfaceSignal'] for r in rows('GEP033_AUDIO_SURFACE'))),'pivots':list({r['pivot']+':'+r['bone'] for r in rows('GEP033_AUDIO_PIVOT')}),'signals':rows('GEP033_AUDIO_SIGNAL_NAME'),'rawErrorCount':sum(bool(re.search(r'\((E|F)\)',x)) for x in text.splitlines()),'readbackRows':len(rows('GEP033_AUDIO_OBSERVE')),'limits':['Parent-pool index/playing is not private wheel-pool evidence','TNodeId hashes may be negative; only -1 means missing','Source/effective binding and movement do not prove audible wheel output']}
target=Path(sys.argv[2]) if len(sys.argv)>2 else None
if target:target.parent.mkdir(parents=True,exist_ok=True);target.write_text(json.dumps(out,indent=2)+'\n')
print(json.dumps(out,indent=2))

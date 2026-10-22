"""Review actual telemetry; never infer human sound quality from a waveform."""
from pathlib import Path
import argparse,hashlib,json,math,re
p=argparse.ArgumentParser();p.add_argument('--console',type=Path,required=True);p.add_argument('--view',choices=['exterior','cabin'],required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args()
assert not a.output.exists(),'Keep all earlier reviews'
text=a.console.read_text(encoding='utf-8-sig',errors='replace')
def rows(marker):
 out=[]
 for line in text.splitlines():
  if marker+' ' in line:out.append(dict(re.findall(r'(\w+)=(<[^>]*>|.*?)(?= \w+=|$)',line.split(marker+' ',1)[1])))
 return out
def by_sample(marker):return {r['sample']:r for r in rows(marker)}
def vec(s):return tuple(float(x) for x in s.strip('<>').split(','))
def dot(x,y):return sum(a*b for a,b in zip(x,y))
def sub(x,y):return tuple(a-b for a,b in zip(x,y))
def length(x):return math.sqrt(dot(x,x))
states=by_sample('GEP_CORE_WHEEL_STATE');motion=by_sample('GEP_CORE_WHEEL_MOTION');power=by_sample('GEP_CORE_WHEEL_POWER');control=by_sample('GEP_CORE_WHEEL_CONTROL');bound=by_sample('GEP033_BOUND_MOTION');hand=by_sample('GEP033_BOUND_HANDBRAKE')
assertions={};metrics={}
def result(name,ok,reason,available=True):assertions[name]={'status':('PASS' if ok else 'FAIL') if available else 'NOT_TESTED','reason':reason}
ids=set(states)&set(motion)&set(power)&set(control)&set(bound)&set(hand)
samples=[dict(state=states[i],motion=motion[i],power=power[i],control=control[i],bound=bound[i],hand=hand[i]) for i in sorted(ids,key=int)]
result('telemetry.join',bool(samples) and len(samples)==len(states),'All source markers paired by actual sample ID',bool(states))
result('clock.native',bool(samples) and all(float(s['state']['timeScale'])==1 for s in samples) and all(0<float(y['state']['worldMs'])-float(x['state']['worldMs'])<=1000 for x,y in zip(samples,samples[1:])),'Native world time must increase at scale1 without >1s gaps',bool(samples))
reverse=[s for s in samples if s['state']['phase']=='reverse_default']
good=[s for s in reverse if s['state']['simOn']=='1' and float(s['state']['rpm'])>=500 and float(s['power']['load'])>0.05 and float(s['control']['actionBrake'])>0.05 and s['bound']['contact']=='1' and float(s['bound']['signedSpeed'])<-.25 and s['bound']['carContext']=='1']
best=0;span=0;segment=[];good_ids={s['state']['sample'] for s in good}
for s in reverse:
 if s['state']['sample'] not in good_ids:
  segment=[]
  continue
 if segment and (float(s['state']['worldMs'])-float(segment[-1]['state']['worldMs'])>1000 or dot(vec(s['motion']['forward']),vec(segment[0]['motion']['forward']))<math.cos(math.radians(15))):segment=[]
 segment.append(s)
 delta=sub(vec(s['motion']['position']),vec(segment[0]['motion']['position']))
 progress=-dot(delta,vec(segment[0]['motion']['forward']))
 if progress>best:best=progress;span=(float(s['state']['worldMs'])-float(segment[0]['state']['worldMs']))/1000
metrics.update(reverseSamples=len(reverse),poweredReverseSamples=len(good),bestContinuousReverseProgressMeters=best,continuousReverseSpanSeconds=span)
result('reverse.powered',len(reverse)>=6 and best>=1 and span>=.5,'>=1m actual backward progress during continuous powered/contact/native reverse input overlap',bool(reverse))
stopping=[s for s in samples if s['state']['phase']=='reverse_neutral_handbrake']
result('reverse.neutral-handbrake',bool(stopping) and all(float(s['hand']['actionHand'])>.5 and float(s['hand']['actionThrust'])==0 and float(s['hand']['actionBrake'])==0 and s['hand']['nativeHand']=='1' for s in stopping),'Native momentary handbrake readback; both propulsion actions zero',bool(stopping))
rest=[s for s in samples if s['state']['phase']=='public_stop_observe']
if rest:
 final=rest[-1];end=float(final['state']['worldMs']);dwell=[s for s in rest if end-float(s['state']['worldMs'])<=2000]
 drift=max((length(sub(vec(s['motion']['position']),vec(dwell[0]['motion']['position']))) for s in dwell),default=999)
 grounded=all(s['bound']['contact']=='1' and length(vec(s['motion']['velocity']))<=.15 and s['state']['simOn']=='0' for s in dwell)
 duration=(end-float(dwell[0]['state']['worldMs']))/1000
 metrics.update(finalRestSeconds=duration,finalRestDriftMeters=drift)
 result('stop.engine-off-dwell',duration>=1.5 and drift<=.1 and grounded,'Final >=1.5s engine-off grounded3D rest, drift<=.1m')
else:result('stop.engine-off-dwell',False,'Missing actual engine-off rest',False)
cameras=rows('GEP033_AUDIO_NATIVE_CAMERA');listeners=rows('GEP033_AUDIO_LISTENER')
result('camera.native-view',bool(cameras) and all(r['handlerReady']=='1' and r['thirdPerson']==('1' if a.view=='exterior' else '0') for r in cameras) and all(r['selected']=='1' for r in listeners),'Actual native player camera and requested view; requested distance remains0',bool(cameras))
events=rows('GEP033_LIFECYCLE');names=[r['event'] for r in events]
result('crew.exit-reentry',names==['exit_complete','reentry_requested','reentry_complete'] and events[1].get('accepted')=='1','One actual exit, accepted native entry request, completed occupied-seat return',bool(events))
done=rows('GEP033_BOUND_DONE')
result('sequence.terminal',len(done)==1,'Exactly one terminal observation marker; no audibility claim',bool(done))
tail=rows('GEP033_TAIL');result('tail.stationary',bool(tail) and all(float(r['speed'])<=.15 for r in tail),'Actual stationary stop/exit/reentry tail samples',bool(tail))
errors=[line for line in text.splitlines() if re.search(r'\((E|F)\)',line)]
result('native.errors',not errors,'All raw native E/F rows retained; no audio-specific suppression')
for gate in ['audio.human-quality','audio.cabin-balance','audio.stop-exit-tail-judgment','audio.delete-reset','audio.near-far30-100','audio.cannon-balance','input.physical','MP.JIP']:result(gate,False,'Requires separate actual native/manual acceptance; not implied by telemetry',False)
status='FAIL' if any(r['status']=='FAIL' for r in assertions.values()) else 'NOT_TESTED'
out={'status':status,'console':str(a.console.resolve()),'consoleSHA256':hashlib.sha256(a.console.read_bytes()).hexdigest(),'view':a.view,'assertions':assertions,'metrics':metrics,'nativeErrorRows':errors,'humanAcceptedTimbre':'Inherited parent acceptance of candidate02; this review does not reinterpret it','lifecycle':'Require original root wrapper normal-exit/lease receipts separately','recording':'Require finalized original continuous WASAPI WAV and ledger; no trim/gain/normalization'}
a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(out,indent=2)+'\n');print(a.output)

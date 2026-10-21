"""Offline GEP033 preservation/fixture/signal contracts, not native compilation or listening."""
from pathlib import Path
import hashlib, json, math, random, re

QA=Path(__file__).resolve().parent
ROOT=QA.parents[1]
PROBE=QA/'signal-probe'
def sha(path): return hashlib.sha256(path.read_bytes()).hexdigest()

# Production preservation is the important gate: all released files, not a few audio snippets.
manifest=json.loads((ROOT/'SOURCE_MANIFEST.json').read_text(encoding='utf-8-sig'))
preserved={path:sha(ROOT/path)==expected for path,expected in manifest['files'].items()}
assert all(preserved.values()), [p for p,v in preserved.items() if not v]
expected_addon={p for p in manifest['files'] if p.startswith('addon/')}
actual_addon={p.relative_to(ROOT).as_posix() for p in (ROOT/'addon').rglob('*') if p.is_file()}
assert actual_addon==expected_addon, {'extra':sorted(actual_addon-expected_addon),'missing':sorted(expected_addon-actual_addon)}

source=(PROBE/'scripts/Game/GEP033/GEP_TrackMovementSignals102.c').read_text()
child=(PROBE/'Prefabs/Vehicles/GEP033_SignalProbe102.et').read_text()
project=(PROBE/'GEP033SignalProbe.gproj').read_text()
assert '{D561AD08932E5480}Prefabs/Vehicles/GEP_MobileCarrier.et' in child
assert re.search(r'Dependencies\s*\{\s*"58D0FB3206B6F859"\s*"D561AD08932E7B40"\s*\}',project)
assert 'GEP_TrackMovementSignals102' in source and 'GEP_TrackMovementSignals102' in child
for forbidden in ('SoundEvent(', 'PlayStr(', 'SetThrottle(', 'SetVelocity(', 'EngineIsOn(', 'EngineGetRPM(', 'SetGear('):
    assert forbidden not in source, forbidden
for required in ('GetVelocity()', 'GetAngularVelocity()', 'GetTrackLevel()', 'GetRollingSpeed()', 'GetTrackPitch()'):
    assert required in source, required
assert not [p for p in PROBE.rglob('*') if p.suffix.lower() in ('.wav','.acp','.sig','.ogg')]
contract=json.loads((QA/'AUDIO-GRAPH-CONTRACT.json').read_text())
assert contract['bank']['stockGuid'] is None
assert contract['status']=='INERT_AUTHORING_CONTRACT_NOT_A_COMPILED_GRAPH'

# A mathematical model exercises intended signal invariants. It does not execute Enforce.
def update(state,forward,yaw,dt,destroyed=False):
    level,speed=state
    rolling=min(22,(abs(forward+yaw*1.46)+abs(forward-yaw*1.46))/2)
    if destroyed: rolling=0
    gate=max(0,min(1,(rolling-.15)/.5)); gate=gate*gate*(3-2*gate)
    target=gate*(.18+.27*min(1,rolling/14))
    tau=.12 if target>level else .22
    level+=(target-level)*(dt/(tau+dt))
    if target==0 and level<.001: level=0
    speed+=(rolling-speed)*(dt/(.18+dt))
    return (level,speed),-3+8*min(1,speed/14)
def run(v,y=0,state=(0,0),seconds=3,dt=1/30,destroyed=False):
    for _ in range(round(seconds/dt)): state,pitch=update(state,v,y,dt,destroyed)
    return state,pitch
cases=[]
for name,v,y in [('idle',0,0),('creep',.1,0),('slow',1,0),('fast',14,0),('reverse',-4,0),('turn',0,.5),('overspeed',1000,100)]:
    state,pitch=run(v,y)
    assert 0<=state[0]<=.45 and -3<=pitch<=5 and 0<=state[1]<=22
    cases.append(dict(name=name,gain=state[0],rollingSpeed=state[1],pitch=pitch))
assert run(0)[0][0]==run(.1)[0][0]==0
assert run(4)==run(-4) and run(4,1)==run(-4,-1)
assert run(1)[0][0]<run(14)[0][0]
assert run(0,.5)[0][0]>0
fast=run(14)[0]
assert run(0,state=fast)[0][0]==0
assert run(14,state=fast,destroyed=True)[0][0]==0
random.seed(33)
state=(0,0)
for _ in range(10000):
    v,y,dt=random.uniform(-80,80),random.uniform(-5,5),random.uniform(1/120,.1)
    state,pitch=update(state,v,y,dt)
    assert all(math.isfinite(x) for x in (*state,pitch))
    assert 0<=state[0]<=.45 and 0<=state[1]<=22 and -3<=pitch<=5
result={
 'status':'PASS_SOURCE_STATIC_ONLY','gepId':'GEP-033','gepStatus':'Blocked',
 'baseline':'699f245afaf3e912b9ce9827b0f3f5f7dd32f3e7',
 'productionFilesPreserved':len(preserved),'productionChanges':0,
 'optionalProbe':'rebased onto released GEP_MobileCarrier; no playback/physics/input setters',
 'mathCases':cases,'randomizedBoundChecks':10000,
 'nativeCompile':'NOT_TESTED','nativeGuidResolution':'PENDING','nativeAudition':'NOT_TESTED',
 'audibleImplementation':False,'nativeAppsLaunched':False,
 'blocker':'Pending supported native resource resolution and original Audio Editor graph authoring; qualified native slot held by Isfahan.',
 'sourceFiles':{p.relative_to(QA).as_posix():sha(p) for p in sorted(PROBE.rglob('*')) if p.is_file()}
}
(QA/'STATIC-RESULT.json').write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8',newline='\n')
print(json.dumps(result,indent=2))

from pathlib import Path
import hashlib,json,math,subprocess
r=Path(r'D:\ArmaProjects\Gepard-EveningSprint-20261021');q=r/'diagnosis/qa/evening-followup-20261021'
q.mkdir(exist_ok=True)
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def save(p,o):p.write_bytes((json.dumps(o,indent=2)+'\n').encode())
rows=[]
for name in ['smoke-stock-drop-01','smoke-lifecycle-control-01','smoke-lifecycle-enabled-01','smoke-lifecycle-upward-01','smoke-physics-drop-01','smoke-character-throw-01','smoke-character-cloud-01']:
 run=r/'native-runs'/name
 if not (run/'exit.json').exists():continue
 fs=list(run.glob('profile/**/GEP015-auto-result.json'));traces=[]
 for p in run.glob('profile/**/GEP015-life-*.json'):
  traces.append((p,json.loads(p.read_bytes())))
 traces.sort(key=lambda v:v[1]['sequence'])
 launch=next((v for _,v in traces if v['phase']=='post_launch'),{})
 velocity=launch.get('projectileVelocity')
 rows.append({'run':name,'acceptance':'ENGINEERING_CONTROL_ONLY_CLOUD_PENDING_REVIEW' if 'character' in name else 'FAIL_FLIGHT_CLOUD_NOT_QUALIFIED','exit':json.loads((run/'exit.json').read_bytes()),'result':json.loads(fs[0].read_bytes()) if fs else None,'traceCount':len(traces),'postLaunchVelocity':velocity,'postLaunchPitchDegrees':math.degrees(math.atan2(velocity[1],math.hypot(velocity[0],velocity[2]))) if velocity else None,'firstZeroOrigin':next((v for _,v in traces if v.get('origin')==[0,0,0]),None),'artifacts':{p.relative_to(run).as_posix():sha(p) for p in sorted(run.rglob('*')) if p.is_file() and (p.suffix.lower() in {'.json','.log','.bmp'}) and 'owner-private' not in p.name}})
save(q/'NATIVE-SMOKE-FOLLOWUP.json',{'scope':'Actual isolated native probes; no gameplay acceptance inferred','rows':rows,'retainedOriginalCrash':'native-runs/smoke-diagnostic-01','ordinaryHumanStockThrow':'NOT_TESTED; separate native character weapon-manager engineering control observed motion/fuse','newCompletions':0})
baseline=Path(r'D:\ArmaProjects\Gepard-GM-Hotfix-1.0.2-20261021\public-source\addon')
def files(p):return {f.relative_to(p).as_posix():sha(f) for f in p.rglob('*') if f.is_file()}
b=files(baseline);c=files(r/'diagnosis/addon');changes=[n for n in sorted(set(b)|set(c)) if b.get(n)!=c.get(n)]
allowed=['scripts/Game/GEP/GEP_CrewActions579.c','scripts/Game/GEP/GEP_DriverStation.c','scripts/Game/GEP/GEP_RadarSystem.c']
save(q/'PRODUCTION-REGRESSION.json',{'status':'PASS' if changes==allowed else 'FAIL','baseline':'699f245afaf3e912b9ce9827b0f3f5f7dd32f3e7','preservedManualCandidate':'cc5d0989376ed1d20ada109ce7c1ada5d96aac7e','baselineFiles':len(b),'candidateFiles':len(c),'changed':changes,'bytePreserved':len(set(b)&set(c))-len(changes),'allOtherProductionBytesPreserved':changes==allowed,'commanderWording':'Get in commander restored for both exterior and internal actions','hiddenPeriscopeReturn':'unchanged; physical F exit remains needs user test'})
print(json.dumps({'smokeCases':len(rows),'productionChanges':changes}))

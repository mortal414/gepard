"""Repeatable offline checks only. Does not compile, spawn or launch anything."""
from pathlib import Path
import argparse,hashlib,json,re,sys
qa=Path(__file__).resolve().parent
ap=argparse.ArgumentParser();ap.add_argument('--output',type=Path);args=ap.parse_args()
checks=[]
def check(name,value): checks.append({'name':name,'pass':bool(value)})
manifest=json.loads((qa/'SOURCE_MANIFEST.json').read_text(encoding='utf-8'))
for name,expected in manifest.items():
    p=qa/name;check('sha256:'+name,p.is_file() and hashlib.sha256(p.read_bytes()).hexdigest()==expected)
code=(qa/'control-source-candidate/scripts/Game/GEP015Stock/GEP_StockSmoke015.c').read_text(encoding='utf-8')
probe=(qa/'diagnostic-overlay/scripts/Game/GEP015Probe/GEP_OneRoundProbe015.c').read_text(encoding='utf-8')
drop=(qa/'diagnostic-overlay/scripts/Game/GEP015Probe/GEP_StockDropControl015.c').read_text(encoding='utf-8')
control=(qa/'control-source-candidate/Prefabs/Weapons/GEP_VehicleTurret_StockSmoke015.et').read_text(encoding='utf-8')
enabled=(qa/'enable-overrides/Prefabs/Weapons/GEP_VehicleTurret_StockSmoke015.et').read_text(encoding='utf-8')
check('only-enabled-prefab-flag-diff',control.replace('GEP_StockSmoke015 "{B3495C8E7A0D2010}" {}','GEP_StockSmoke015 "{B3495C8E7A0D2010}" { m_bEnableNativeSimulation015 1 }')==enabled)
up=(qa/'upward-overrides/scripts/Game/GEP015Stock/GEP_StockSmoke015.c').read_text(encoding='utf-8')
check('upward-only-pitch-diff',code.replace('Vector(sign * (10 + 10 * ordinal), -30, 0)','Vector(sign * (10 + 10 * ordinal), 30, 0)')==up)
check('null-weapon-repair-present','GetRootParent(), null, null);' in code)
check('EnableSimulation-gated-before-Launch',code.index('if (m_bEnableNativeSimulation015) move.EnableSimulation(round);')<code.index('move.Launch('))
check('arming-time-unchanged','fuse.SetLive();' in code and not re.search(r'CallLater\([^\n]*SetLive',code))
check('no-timer-velocity-effect-or-grenade-pose-setter',not re.search(r'SetTimer\(|SetVelocity\(|SetOrigin\(|SpawnParticleEffect\(',code+probe+drop))
check('one-action-callback',probe.count('selected.PerformAction(')==1)
check('terminal-frame-mask-cleared','ClearEventMask(EntityEvent.FRAME);' in probe and 'if(Finished)' not in probe)
check('early-spawn-observation',code.index('TraceLifecycle015("post_spawn"')<code.index('move.Launch('))
check('drop-control-no-launch','.Launch(' not in drop and '.EnableSimulation(Round)' in drop and 'CallLater(ArmStockControl015,1000,false)' in drop)
check('probe-native-parameter-names','EOnFrame(IEntity owner,float timeSlice)' in probe and 'IEntity ProbeTurret' in probe)
check('diagnostic-position-invalidity-retained','record.WriteValue("nearRequestedMuzzle",distance<200)' in probe and 'if(distance<200)' in probe)
for name,content in [('component',code),('probe',probe),('drop',drop)]:
    stripped=re.sub(r'//[^\n]*|"[^"\n]*"','',content);check('balanced-braces:'+name,stripped.count('{')==stripped.count('}'))
result={'status':'PASS' if all(c['pass'] for c in checks) else 'FAIL','scope':'OFFLINE_SOURCE_CONTRACT_AND_IDENTITY_ONLY','nativeCompile':'NOT_TESTED','gameplay':'NOT_TESTED','checks':checks}
if args.output:args.output.write_bytes((json.dumps(result,indent=2)+'\n').encode('utf-8'))
print(json.dumps({'status':result['status'],'checks':len(checks),'failed':[c['name'] for c in checks if not c['pass']],'native':'NOT_TESTED'},indent=2))
sys.exit(0 if result['status']=='PASS' else 1)

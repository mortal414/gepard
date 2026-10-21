"""Offline recovery/inheritance/preservation checks; never starts native software."""
from pathlib import Path
import argparse, hashlib, json, re, subprocess, sys
BASE='699f245afaf3e912b9ce9827b0f3f5f7dd32f3e7'
qa=Path(__file__).resolve().parent
root=qa.parents[1]
ap=argparse.ArgumentParser()
ap.add_argument('--output',type=Path)
ap.add_argument('--allow-existing',action='append',default=[],help='Explicit integration-reviewed existing addon path allowed to differ')
args=ap.parse_args()
def sha(p): return hashlib.sha256(p.read_bytes()).hexdigest()
def git(*a): return subprocess.check_output(['git','-C',str(root),*a],text=True).strip()
rec=json.loads((qa/'RECOVERY.json').read_text())
checks=[]
def check(name,ok,details=None): checks.append(dict(name=name,pass_=bool(ok),details=details))
for item in rec['sourceResources']+rec['privateFixtureOverlay']:
    p=root/item['path']
    check('recovered-exact:'+item['path'],p.is_file() and sha(p)==item['sha256'])
baseline_paths=git('ls-tree','-r','--name-only',BASE,'--','addon').splitlines()
changed=[]
proc=subprocess.Popen(['git','-C',str(root),'cat-file','--batch'],stdin=subprocess.PIPE,stdout=subprocess.PIPE)
for path in baseline_paths:
    proc.stdin.write((BASE+':'+path+'\n').encode()); proc.stdin.flush()
    header=proc.stdout.readline().split()
    size=int(header[2]); original=proc.stdout.read(size); proc.stdout.read(1)
    p=root/path
    if original.startswith(b'version https://git-lfs.github.com/spec/v1\n'):
        oid=re.search(rb'oid sha256:([0-9a-f]{64})',original)
        same=p.is_file() and oid and sha(p)==oid.group(1).decode()
    else:
        same=p.is_file() and p.read_bytes()==original
    if not same: changed.append(path)
proc.stdin.close(); proc.wait()
check('existing-addon-preservation',not(set(changed)-set(args.allow_existing)),{'files':len(baseline_paths),'changed':changed,'explicitlyAllowed':args.allow_existing})
candidate_paths={i['path'] for i in rec['sourceResources']}
unknown=[str(p.relative_to(root)).replace('\\','/') for p in (root/'addon').rglob('*') if p.is_file() and str(p.relative_to(root)).replace('\\','/') not in set(baseline_paths)|candidate_paths]
check('only-six-optional-addon-resources',not unknown,unknown)
script=(root/'addon/scripts/Game/GEP015Stock/GEP_StockSmoke015.c').read_text()
carrier=(root/'addon/Prefabs/Vehicles/GEP_MobileCarrier_StockSmoke015.et').read_text()
turret=(root/'addon/Prefabs/Weapons/GEP_VehicleTurret_StockSmoke015.et').read_text()
check('corrected-native-action-parameter-names',script.count('PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)')==2)
check('stock-grenade-direct-reference','{9DB69176CEF0EE97}Prefabs/Weapons/Grenades/Smoke_ANM8HC.et' in script)
check('inherits-current-released-carrier','Vehicle : "{D561AD08932E5480}Prefabs/Vehicles/GEP_MobileCarrier.et"' in carrier)
check('inherits-current-released-turret','Turret : "{D561AD08932E5450}Prefabs/Weapons/GEP_VehicleTurret.et"' in turret)
check('no-simulation-audio-or-geometry-overrides',not re.search(r'VehicleWheeledSimulation|Gearbox|BrakeTorque|SoundComponent|MeshObject|RigidBody',carrier+turret))
check('base-game-dependency-only',re.findall(r'Dependencies\s*\{([^}]+)\}',(root/'addon/Gepard.gproj').read_text())==[' "58D0FB3206B6F859" '])
check('no-custom-rpc-or-timer-override',not re.search(r'\b(Rpc|RplRpc|SetTimer)\s*\(',script))
new_guids=[]
for item in rec['sourceResources']:
    if item['path'].endswith('.meta'):
        match=re.search(r'Name\s+"\{([0-9A-F]+)\}',(root/item['path']).read_text())
        new_guids.append(match.group(1))
check('unique-optional-resource-guids',len(new_guids)==len(set(new_guids))==3)
fixture=(qa/'native-overlay/scripts/Game/GEPDemo/GEP_DemoScene.c').read_text()
check('manual-fixture-no-automated-smoke-dispatch',not re.search(r'\.Dispatch\(|\.BeginReload\(|\.Launch\(|\.SetLive\(',fixture))
check('manual-fixture-single-player-only','RplSession.Mode()!=RplMode.None' in fixture)
result={'status':'PASS' if all(c['pass_'] for c in checks) else 'FAIL','scope':'OFFLINE_SOURCE_RECOVERY_AND_PRESERVATION_ONLY','baseline':BASE,'nativeCompileOfRebasedCandidate':'NOT_TESTED','gameplay':'NOT_TESTED','checks':checks}
encoded=json.dumps(result,indent=2)+'\n'
if args.output: args.output.write_text(encoded,encoding='utf-8')
print(encoded)
sys.exit(0 if result['status']=='PASS' else 1)

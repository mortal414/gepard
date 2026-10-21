"""Source contracts and release-preservation checks; does not compile or run Reforger."""
from pathlib import Path
import argparse, hashlib, json, subprocess
ROOT=Path(__file__).resolve().parents[2]
BASE='699f245afaf3e912b9ce9827b0f3f5f7dd32f3e7'
S='addon/scripts/Game/GEP/'
ALLOWED={S+'GEP_CrewActions579.c',S+'GEP_DriverStation.c',S+'GEP_RadarSystem.c'}
def git(*args):
    return subprocess.check_output(['git','-C',str(ROOT),*args])
def source(name):return (ROOT/S/name).read_text(encoding='utf-8')
def base(name):return git('show',BASE+':'+S+name).decode().replace('\r\n','\n')
checks={}
changed=set(git('diff','--name-only',BASE,'--','addon').decode().splitlines())
checks['runtime_changes_within_three_script_allowlist']=changed==ALLOWED
driver=source('GEP_DriverStation.c')
checks['intentional_hidden_periscope_prompt_exact']=driver[driver.index('// Presentation only. The native interaction handler still owns and dispatches'):]==base('GEP_DriverStation.c')[base('GEP_DriverStation.c').index('// Presentation only. The native interaction handler still owns and dispatches'):]
checks['driver_conscious_and_exact_occupant']='life.IsUnconscious()' in driver and 'seat.GetOccupant()==user' in driver
checks['crew_conscious_and_exact_occupant']='life.IsUnconscious()' in source('GEP_RadarSystem.c') and 'seat.GetOccupant()==user' in source('GEP_RadarSystem.c')
checks['station_lights_conscious']='life.IsUnconscious()' in source('GEP_CrewActions579.c')
checks['requested_commander_wording_preserved']=source('GEP_SwitchSeatAction.c')==base('GEP_SwitchSeatAction.c') and 'outName="Get in commander"' in source('GEP_CrewControls569.c')
for name in ['GEP_CrewControls569.c','GEP_GunStatus.c','GEP_Presentation680.c','GEP_OpticEvidence679.c','GEP_Range676.c','GEP_FireControl.c']:
    checks['baseline_exact_'+name]=source(name)==base(name)
for name in ['GEP_CrewActions579.c','GEP_DriverStation.c','GEP_RadarSystem.c']:
    # Verify no new input dispatch, physics, camera or replicated action calls.
    old=base(name)
    new=source(name)
    for token in ['AddActionListener(','.OpenDoor(','.CloseDoor(','.GetInVehicle(','.GetOutVehicle(','.SetInput(','.SetVelocity(','.SetTransform(','.SelectView(']:
        checks['no_added_dispatch_'+name+'_'+token]=new.count(token)<=old.count(token)
checks['native_exit_and_hatch_actions_untouched']=source('GEP_CrewControls569.c')==base('GEP_CrewControls569.c')
hashes={}
for rel in sorted(ALLOWED):hashes[rel]=hashlib.sha256((ROOT/rel).read_bytes()).hexdigest()
report={'status':'PASS' if all(checks.values()) else 'FAIL','baseline':BASE,'changedRuntimeFiles':sorted(changed),'checks':checks,'sourceSha256':hashes,'nativeCompile':'NOT_TESTED','gameplay':'NOT_TESTED','manualAcceptance':'NEEDS_USER_TEST','scope':'Source contracts only; no engine/session/lease action performed.'}
parser=argparse.ArgumentParser();parser.add_argument('--output',type=Path);args=parser.parse_args()
if args.output:args.output.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
print(json.dumps(report,indent=2))
raise SystemExit(0 if report['status']=='PASS' else 1)

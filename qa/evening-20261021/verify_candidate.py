"""Verify the integrated candidate stays inside its explicitly reviewed source slice."""
from pathlib import Path
import argparse, hashlib, json, subprocess

BASE='699f245afaf3e912b9ce9827b0f3f5f7dd32f3e7'
CHANGED={f'scripts/Game/GEP/{f}' for f in ['GEP_CrewActions579.c','GEP_DriverStation.c','GEP_RadarSystem.c','GEP_SwitchSeatAction.c']}
ADDED=set()  # Failed smoke source is retained only under qa/, outside production addon.

def inventory(root):
    return {p.relative_to(root).as_posix():hashlib.sha256(p.read_bytes()).hexdigest() for p in sorted(root.rglob('*')) if p.is_file()}

def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--repo',type=Path,required=True)
    ap.add_argument('--baseline',type=Path,required=True)
    ap.add_argument('--output',type=Path,required=True)
    a=ap.parse_args()
    baseline_head=subprocess.check_output(['git','-C',str(a.baseline),'rev-parse','HEAD'],text=True).strip()
    old=inventory(a.baseline/'addon'); new=inventory(a.repo/'addon')
    changed={p for p in old.keys() & new.keys() if old[p]!=new[p]}
    added=new.keys()-old.keys(); removed=old.keys()-new.keys()
    checks={'baselineIdentity':baseline_head==BASE,'exactChangedFiles':changed==CHANGED,'exactNewFiles':added==ADDED,'noRemovedFiles':not removed,
      'acceptedDrivingPrefabsAndPhysicsPreserved':not any(p.startswith('Prefabs/') for p in changed),
      'allReleasedArtPreserved':not any(p.startswith('Assets/') for p in changed),
      'projectDependenciesAndVersionPreserved':old['Gepard.gproj']==new['Gepard.gproj'],
      'stockAudioAndHUDFilesPreserved':not any('Sound' in p or any(x in p for x in ['GunStatus','Range676','Optical444','FireControl','GEPWheelDrive']) for p in changed)}
    # Keep the precise intended periscope presentation override unchanged.
    driver='scripts/Game/GEP/GEP_DriverStation.c'
    oldtext=(a.baseline/'addon'/driver).read_text();newtext=(a.repo/'addon'/driver).read_text()
    marker='modded class SCR_ActionMenuInteractionDisplay'
    checks['intentionalHiddenPeriscopePromptPreserved']=marker in oldtext and oldtext.split(marker,1)[1]==newtext.split(marker,1)[1]
    result={'schema':1,'status':'PASS' if all(checks.values()) else 'FAIL','baseline':BASE,
      'captureHeadCommit':subprocess.check_output(['git','-C',str(a.repo),'rev-parse','HEAD'],text=True).strip(),
      'productionManifestSHA256':hashlib.sha256(json.dumps(new,sort_keys=True).encode()).hexdigest(),
      'checks':checks,'baselineFiles':len(old),'preservedFiles':len(old)-len(changed)-len(removed),'changed':sorted(changed),'added':sorted(added),'removed':sorted(removed),
      'changedHashes':{p:{'before':old[p],'after':new[p]} for p in sorted(changed)},'addedHashes':{p:new[p] for p in sorted(added)},
      'nativeCompile':'Separate native evidence required','gameplay':'Separate native evidence required'}
    a.output.parent.mkdir(parents=True,exist_ok=True)
    a.output.write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8',newline='\n')
    print(json.dumps(result,indent=2))
    raise SystemExit(0 if result['status']=='PASS' else 2)

if __name__=='__main__':main()

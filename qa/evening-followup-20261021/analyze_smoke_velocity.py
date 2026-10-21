"""Analyze retained native launch velocity, without inventing motion or contacts."""
from pathlib import Path
import argparse,hashlib,json,math,re

def main():
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--console',type=Path,required=True);ap.add_argument('--output',type=Path,required=True);a=ap.parse_args()
    raw=a.console.read_bytes();text=raw.decode('utf-8',errors='replace')
    match=re.search(r'GEP015_STOCK_DISPATCH tube=(\d+) entity=(\d+) muzzle=<([^>]+)> velocity=<([^>]+)> timer=([0-9.eE+-]+) remaining=(\d+)',text)
    assert match,'No actual native dispatch velocity present'
    muzzle=[float(x) for x in match[3].split(',')];velocity=[float(x) for x in match[4].split(',')]
    horizontal=math.hypot(velocity[0],velocity[2]);speed=math.sqrt(sum(x*x for x in velocity));pitch=math.degrees(math.atan2(velocity[1],horizontal))
    observations=re.findall(r'GEP015_AUTO_ROUND id=(\d+) elapsed=([0-9.eE+-]+) position=<([^>]+)> velocity=<([^>]+)> timer=([0-9.eE+-]+) triggered=(\d+)',text)
    records=[{'id':int(row[0]),'elapsed':float(row[1]),'position':[float(x) for x in row[2].split(',')],'velocity':[float(x) for x in row[3].split(',')],'localTimer':float(row[4]),'triggered':row[5]=='1'} for row in observations]
    result={'scope':'Retained native velocity/position/timer observations only; no simulated flight or contact conclusion','source':str(a.console),'sourceSHA256':hashlib.sha256(raw).hexdigest(),'tube':int(match[1]),'entity':int(match[2]),'requestedMuzzle':muzzle,'actualNativeInitialVelocity':velocity,'speedMetresPerSecond':speed,'worldVelocityPitchDegrees':pitch,'downwardInitialVelocityProved':velocity[1]<0,'actualSpawnOriginBeforeLaunch':'NOT_MEASURED','firstObservedAfterSeconds':records[0]['elapsed'] if records else None,'observedRecords':len(records),'allObservedOriginsZero':bool(records) and all(row['position']==[0,0,0] for row in records),'allObservedLocalTimersSame':bool(records) and len({row['localTimer'] for row in records})==1,'triggeredEverObserved':any(row['triggered'] for row in records),'selfCollision':'HYPOTHESIS_NOT_PROVED','simulationPreparationMissing':'SOURCE_COMPARISON_FACT; causal effect requires isolated next native probe','nextIndependentChanges':['Explicit EnableSimulation before Launch, otherwise old trajectory and immediate arming unchanged','Separate +30 degree elevation variant after simulation-enabled case, preserving coordinates','Record actual origin/world matrix before/after spawn/enable/Launch/live and first0/33/100/250ms; never count zero-origin distance as flight']}
    a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8',newline='\n');print(json.dumps(result,indent=2))

if __name__=='__main__':main()

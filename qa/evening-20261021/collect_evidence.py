"""Summarize retained real evidence and recheck complete frozen native stages; no launches."""
from pathlib import Path
import argparse, hashlib, json

def digest(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def read(p):return json.loads(p.read_text(encoding='utf-8-sig'))

def main():
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--sprint-root',type=Path,required=True);ap.add_argument('--output',type=Path,required=True);a=ap.parse_args();r=a.sprint_root
    result={'scope':'Native measured cases and complete source-closure checks; no whole-task/human acceptance implied','compiles':{},'batches':{},'frozenStages':{},'smoke':{},'images':[]}
    for name in ['qa-compile-01','diagnostic-compile-01','diagnostic-compile-02','smoke-repair-compile-01','evening-final-compile-01']:
        p=r/'native-runs'/name/'RESULT.json'
        if p.exists():result['compiles'][name]={'result':read(p),'path':str(p),'sha256':digest(p)}
    for name in ['healthy-batch-01','fragment-batch-01','healthy-final-01']:
        p=r/'evidence'/name/'summary.json'
        if p.exists():
            summary=read(p);cases={}
            for case in summary['cases']:
                cp=p.parent/case['result'];native=read(cp)
                cases[case['id']]={'status':native['status'],'assertions':native['assertions'],'lifecycle':native['lifecycle'],'path':str(cp),'sha256':digest(cp)}
            result['batches'][name]={'status':summary['status'],'cases':cases,'path':str(p),'sha256':digest(p)}
    for name in ['batch-01','batch-02','batch-03','evening-final']:
        addon=r/name/'native-stage/gepard';mp=r/name/'checks/SOURCE_MANIFEST.json'
        if mp.exists():
            pins=read(mp);actual={p.relative_to(addon).as_posix():digest(p) for p in sorted(addon.rglob('*')) if p.is_file()}
            result['frozenStages'][name]={'status':'PASS' if pins==actual else 'FAIL','files':len(actual),'manifest':str(mp),'manifestSHA256':digest(mp),'changed':sorted(k for k in pins.keys()&actual.keys() if pins[k]!=actual[k]),'added':sorted(actual.keys()-pins.keys()),'removed':sorted(pins.keys()-actual.keys())}
    smoke=r/'native-runs/smoke-diagnostic-01'
    p=smoke/'profile/profile/GEP015-auto-result.json'
    if p.exists():result['smoke']['diagnostic']={'record':read(p),'path':str(p),'sha256':digest(p)}
    p=smoke/'exit.json'
    if p.exists():result['smoke']['lifecycle']={'record':read(p),'path':str(p),'sha256':digest(p)}
    for number in [1,2]:
        attempt=r/f'native-runs/smoke-diagnostic-{number:02d}'
        records={}
        for rel in ['profile/profile/GEP015-auto-result.json','exit.json']:
            p=attempt/rel
            if p.exists():records[rel]={'record':read(p),'path':str(p),'sha256':digest(p)}
        crash=attempt/'logs/crash.log'
        if crash.exists():records['crashLog']={'path':str(crash),'sha256':digest(crash)}
        result['smoke'][f'attempt{number}']=records
    for base in [r/'batch-01/damage-runs',r/'evening-final/damage-runs',smoke,r/'native-runs/smoke-diagnostic-02',r/'native-runs/evening-manual-ready-01']:
        if base.exists():
            for p in sorted(base.rglob('*')):
                if p.is_file() and p.suffix.lower() in ['.bmp','.png']:result['images'].append({'path':str(p),'sha256':digest(p),'bytes':p.stat().st_size,'interpretation':'Requires explicit visual inspection; not a gameplay pass by itself'})
    a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps({'compiles':{k:v['result']['status'] for k,v in result['compiles'].items()},'batches':{k:v['status'] for k,v in result['batches'].items()},'sourceClosure':{k:v['status'] for k,v in result['frozenStages'].items()},'images':len(result['images']),'smokeRecordPresent':'diagnostic' in result['smoke']}))

if __name__=='__main__':main()

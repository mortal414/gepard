"""Read stock package indices through the maintained harness; no payload or native app access."""
from pathlib import Path
import argparse,json,subprocess
p=argparse.ArgumentParser()
p.add_argument('--harness',type=Path,required=True)
p.add_argument('--node',type=Path,required=True)
p.add_argument('--stock-root',type=Path,required=True)
args=p.parse_args()
command=[str(args.node),str(args.harness/'node_modules/tsx/dist/cli.mjs'),str(args.harness/'src/pak-source-cli.ts'),'list','--root',str(args.stock_root),'--filter','Sounds/Vehicles/_SharedData/Tracks/','--limit','100']
result=subprocess.run(command,text=True,capture_output=True,check=True)
data=json.loads(result.stdout)
assert not data['truncated']
rows=data['resources']
assert len(rows)==4 and all(r['resource'].endswith('.wav') for r in rows)
record={
 'status':'PASS_INDEX_ONLY',
 'scannedPacks':data['scannedPacks'],
 'stockTrackGraphsFound':len([r for r in rows if r['resource'].endswith(('.acp','.sig'))]),
 'stockTrackRecordingsFound':len(rows),
 'payloadRead':False,'wavGuid':'NOT_EXPOSED_BY_PACKAGE_INDEX',
 'repeatCommand':'python inspect-stock.py --harness <maintained-harness> --node <node.exe> --stock-root <stock-addons-data>',
 'resources':[{k:r[k] for k in ('resource','rawBytes','method','indexSha256')} for r in rows]
}
(Path(__file__).resolve().parent/'STOCK-INDEX-RESULT.json').write_text(json.dumps(record,indent=2)+'\n')
print(json.dumps(record,indent=2))

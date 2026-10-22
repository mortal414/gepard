"""Bind a coordinator-created additive prefab. No asset copying or native launch."""
import argparse
import json
from pathlib import Path
from generate import ROOT, resource_file

parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--slot',required=True,choices=['OLD-CAMO','OLD-CHARCOAL','NEW-CAMO','NEW-CHARCOAL'])
parser.add_argument('--prefab',required=True)
parser.add_argument('--label',required=True)
args=parser.parse_args()
path=ROOT/'registry/variants.json'
data=json.loads(path.read_text())
resource_file(Path(data['sharedRuntime']['sourceRoot']),args.prefab)
if not args.prefab.startswith('{'):
 raise SystemExit('Use the actual GUID-prefixed ResourceName for a comparison subject')
slot=next(s for s in data['slots'] if s['id']==args.slot)
slot.update(prefab=args.prefab,label=args.label,status='BOUND_SOURCE_ONLY')
path.write_text(json.dumps(data,indent=2)+'\n')
print('Bound source only. Run tools/generate.py --check to pin the full current runtime and regenerate; native validation is still required.')

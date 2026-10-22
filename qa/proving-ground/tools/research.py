"""Reuse the installed harness selective reader; retain provenance, not stock source."""
import hashlib
import json
import re
import subprocess
from pathlib import Path

ROOT=Path(__file__).resolve().parents[1]
HARNESS=Path(r"C:\Users\marco\Desktop\reforger-agent-harness")
PACK=Path(r"D:\SteamLibrary\steamapps\common\Arma Reforger\addons\data\data007.pak")
NODE=Path(r"C:\Program Files\nodejs\node.exe")

resources=json.loads((ROOT/'registry/resources.json').read_text())
paths=[re.sub(r'^\{[0-9A-Fa-f]{16}\}', '', p) for key,p in resources.items() if key!='board']
paths += [
 'scripts/Game/FiringRange/SCR_FiringRangeTarget.c',
 'Prefabs/Props/Military/Arsenal/ArsenalBoxes/ArsenalBox_Base.et',
 'Prefabs/Structures/Signs/Military/Sign_FiringRange_US_01.et',
]
rows=[]
for path in paths:
 result=subprocess.run([str(NODE),'node_modules/tsx/dist/cli.mjs','src/pak-source-cli.ts','read',
                        '--pack',str(PACK),'--resource',path],cwd=HARNESS,capture_output=True)
 if result.returncode:
  rows.append({'resource':path,'status':'UNAVAILABLE','stderr':result.stderr.decode().strip()})
  continue
 provenance=json.loads(result.stderr.strip())
 assert provenance['sourceSha256']==hashlib.sha256(result.stdout).hexdigest()
 rows.append({'status':'READ_VERIFIED',**provenance})
(ROOT/'checks/native-resource-provenance.json').write_text(json.dumps({'reader':str(HARNESS/'src/pak-source-cli.ts'),
 'accessDateUTC':'2026-10-22','scope':'read-only installed native text; no extracted stock payload bundled','resources':rows},indent=2)+'\n')
print(json.dumps({'verified':sum(r['status']=='READ_VERIFIED' for r in rows),'unavailable':[r['resource'] for r in rows if r['status']!='READ_VERIFIED']}))

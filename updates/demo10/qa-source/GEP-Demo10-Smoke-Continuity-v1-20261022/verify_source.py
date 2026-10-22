"""Read-only final Demo10 inputs and minimal QA rebind; no native/staging."""
from pathlib import Path
import json
from evidence import read_json,sha
ROOT=Path(__file__).resolve().parent
def tree(root):return {p.relative_to(root).as_posix():sha(p) for p in root.rglob('*') if p.is_file()}
def verify(require_seal=True):
 pins=read_json(ROOT/'INPUT-PINS.json');adoption=Path(pins['demo10Adoption'])
 assert sha(adoption)==pins['demo10AdoptionSHA256']
 source=read_json(ROOT/'SOURCE-COPY-CLOSURES.json');assert len(source)==7 and sum(r['files'] for r in source)==1261
 assert sum(r['files'] for r in source if r['copyRequired'])==10 and sum(r['files'] for r in source if not r['copyRequired'])==1251
 for row in source:
  assert sha(row['manifest'])==row['sha256'];actual=tree(Path(row['root']));assert actual==read_json(row['manifest']) and len(actual)==row['files']
 oldroot=Path(r'D:\ArmaProjects\Gepard-Consolidated-20261022\native-smoke-current-v11-stage-v3\current\qaSmoke')
 old=tree(oldroot);new=tree(ROOT/'addon');assert set(old)==set(new) and len(new)==10
 changed=[n for n,h in old.items() if new[n]!=h];assert sorted(changed)==sorted(pins['qaChanges']) and len(changed)==4
 for n in changed:
  before=(oldroot/n).read_text();after=(ROOT/'addon'/n).read_text()
  for original,final in pins['finalResourceRebind'].items():after=after.replace(final,original)
  if n.endswith('.gproj'):after=after.replace('"7E0150C026101500" "5E51C5D8F04FDB80" }','"7E0150C026101500" }')
  assert before==after,'QA semantic change exceeds exact resources/dependency'
 oldtools=ROOT.parent/'GEP-Smoke-CurrentV11-Qualification-v3-20261022'
 assert sha(ROOT/'analyze_wcs_smoke_supplier.py')==sha(oldtools/'analyze_wcs_smoke_supplier.py'),'Mechanism analyzer must stay byte exact'
 if require_seal:
  actual={p.relative_to(ROOT).as_posix():sha(p) for p in ROOT.rglob('*') if p.is_file() and p.name!='SOURCE-MANIFEST.json' and not {'state','adopted','__pycache__'}.intersection(p.relative_to(ROOT).parts)}
  assert actual=={r['relative']:r['sha256'] for r in read_json(ROOT/'SOURCE-MANIFEST.json')['files']}
 return {'status':'FINAL_DEMO10_MINIMAL_QA_REBIND_SOURCE_PASS','productionFilesMountedExact':1251,'qaFilesCopied':10,'qaFilesChanged':4,'mechanismAnalyzerByteExact':True,'nativeStarted':False}
if __name__=='__main__':print(json.dumps(verify(),indent=2))

"""Root-only explicit fresh private-QA staging; default is a read-only plan."""
from pathlib import Path
import argparse,hashlib,json,shutil,sys
sys.dont_write_bytecode=True
O=Path(__file__).resolve().parent
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def read(p):return json.loads(Path(p).read_text(encoding='utf-8-sig'))
def verify():
 seal=read(O/'SOURCE-MANIFEST.json')
 assert all(sha(O/r)==h for r,h in seal['files'].items()),'Frozen C source changed'
 assert all(sha(p)==h for p,h in read(O/'INPUT-PINS.json').items()),'Recovered/native input changed'
 for c in read(O/'EXPECTED-STAGE-CLOSURES.json')[:-1]:
  pins=read(c['manifest']);assert sha(c['manifest'])==c['sha256']
  assert {p.relative_to(c['root']).as_posix():sha(p)for p in Path(c['root']).rglob('*')if p.is_file()}==pins
 return read(O/'BINDING-CONFIG.json')
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--execute',action='store_true');a=p.parse_args();c=verify();target=Path(c['qaRoot']).resolve()
 parent=Path(r'D:\ArmaProjects\Gepard-Demo10-TrackArt-20261022').resolve()
 assert target==parent/'native-artqa-new-stage-v2/artNativeQA'and target.is_relative_to(parent)
 assert not target.exists(),'Fresh QA root required; preserve existing stage'
 if a.execute:
  shutil.copytree(O/'source',target)
  assert {p.relative_to(target).as_posix():sha(p)for p in target.rglob('*')if p.is_file()}==read(O/'QA-EXPECTED-MANIFEST.json')
 print(json.dumps({'status':'PRIVATE_QA_SIX_SOURCE_FILES_STAGED_NATIVE_NOT_STARTED'if a.execute else'READ_ONLY_FRESH_STAGE_PLAN','target':str(target),'nativeStarted':False,'DWritten':a.execute,'productionRootsChanged':False}))

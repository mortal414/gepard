from pathlib import Path
import json,hashlib,re
P=Path(__file__).resolve().parent;T=Path(r'D:\ArmaProjects\Gepard-Demo10-TrackArt-20261022');D=T/'private-demo-source10/single/integration';A=Path(r'D:\ArmaProjects\Gepard-ArtApproved-20261022');R=T/'native-runs/approved-art-rebuild-02'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def load(p):return json.loads(p.read_text(encoding='utf-8-sig'))
old=load(P/'art-import-v2/IMPORT-TARGET-PINS.json');now={p.relative_to(D).as_posix():sha(p) for p in D.rglob('*') if p.is_file()};m=load(P/'ART-IMPORT-EXPECTATIONS.json');e=load(R/'exit.json');log=(R/'logs/console.log').read_text(errors='replace')
assert e['verifiedExited'] and e['leaseReleased'] and e['exitCode']==0 and e['closeMethod']=='owned-normal-window-close-after8-builds'
assert log.count('RESOURCES    : Build successful')==8 and 'GEP_DEMO10_APPROVED_8MODEL_BATCH_QUEUED' in log
assert not re.search(r'SCRIPT\s+\(E\)',log)
expectedChanged={'resourceDatabase.rdb'};added=set();models=[]
for row in m['models']:
 rel=row['new_model'].split('}',1)[1];meta=rel+'.meta';fbx=row['new_fbx'];txo=rel[:-4]+'.txo'
 assert sha(D/rel)!=row['seed_xob_sha256'] and sha(D/fbx)==row['new_fbx_sha256']
 token=lambda text:re.findall(r'"(?:\\.|[^"\\])*"|[{}]|[^\s{}"]+',text)
 assert token((D/meta).read_text())==token((A/'overlay/gepard'/meta).read_text()),('Native meta semantic delta',meta)
 assert (D/txo).stat().st_size>0
 assert row['new_model'] in (D/meta).read_text()
 expectedChanged.update([rel,meta]);added.add(txo);models.append({**row,'actualCookedSHA256':sha(D/rel),'actualNativeMetaSHA256':sha(D/meta),'nativeMetaTokensUnchanged':True,'txoSHA256':sha(D/txo)})
assert set(now)-set(old)==added and not(set(old)-set(now))
assert {p for p in old if old[p]!=now[p]}==expectedChanged
out={'status':'EIGHT_NATIVE_BUILDS_AND_CHANGED_COOKED_HASHES_VERIFIED','models':models,'sourceFBXsUnchanged':True,'nativeMetaTokensUnchanged':True,'newFiles':sorted(added),'exit':e,'console':str(R/'logs/console.log'),'consoleSHA256':sha(R/'logs/console.log'),'rawErrors':[x for x in log.splitlines() if re.search(r'\([EF]\):',x)],'nativeMaterialsRenderingClearanceCollisionLOD':'PENDING_SCENE_VALIDATION','scope':'Model import only; no gameplay acceptance or publication'}
(P/'ART-NATIVE-REBUILD-RESULT.json').write_text(json.dumps(out,indent=2)+'\n');print(out['status'])

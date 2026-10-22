"""Recover completed additive copy after the original receipt's wrong hardcoded count."""
from pathlib import Path
import json,hashlib,re
P=Path(__file__).resolve().parent;T=Path(r'D:\ArmaProjects\Gepard-Demo10-TrackArt-20261022');S=T/'private-demo-source10/single';A=Path(r'D:\ArmaProjects\Gepard-ArtApproved-20261022');D=S/'integration'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def inv(p):return {x.relative_to(p).as_posix():sha(x) for x in p.rglob('*') if x.is_file()}
def load(p):return json.loads(p.read_text(encoding='utf-8-sig'))
m=load(A/'APPROVED_ART_HANDOFF.json');assert sha(A/'APPROVED_ART_HANDOFF.json')=='e846d6aed7dba2277d5bab9da5ed715815934f3ed98e592156a7bdcd855a3ae3'
assert len(m['files'])==52 and len(m['models'])==8
rebases={};allowed={};seed=load(P/'DEMO10-INITIAL-SOURCE.json')
for rel,row in m['files'].items():
 src=A/'overlay'/rel;assert sha(src)==row['sha256'];inside=rel.split('/',1)[1];dst=D/inside
 if inside.endswith('/GEP_MobileCarrier.et'):
  skin='Charcoal' if '/Charcoal/' in inside else 'Camo';guid='A76A420022100004' if skin=='Charcoal' else 'A76A420022100002'
  old='{7E0150C02610300'+('4' if skin=='Charcoal' else '2')+'}Prefabs/ArtV11Smoke/'+skin+'/GEP_MobileCarrierV11Smoke.et';new='{'+guid+'}Prefabs/Demo10/'+skin+'/GEP_MobileCarrierDemo10.et'
  expected=src.read_text().replace(old,new);assert expected.count(new)==1 and dst.read_text()==expected
  rebases[skin]={'before':old,'after':new,'sha256':sha(dst)}
 else:assert sha(dst)==row['sha256'],inside
 allowed[inside]=sha(dst)
assert inv(S/'gepard')==load(Path(r'D:\ArmaProjects\Gepard-Consolidated-20261022\qa\consolidated-20261022\private-final-source-v6\CurrentSourceManifest-gepard.json'))
assert inv(S/'wcsSmoke')==inv(Path(r'D:\ArmaProjects\Gepard-Consolidated-20261022\private-smoke-demo-source-v2\single\wcsSmoke'))
assert len(inv(D))==59
for row in seed['wrappers']:assert sha(D/row['resource'].split('}',1)[1])==row['originalAudioOnlySHA256']
models=[]
for row in m['models']:
 assert sha(D/row['new_fbx'])==row['new_fbx_sha256'];path=D/row['new_model'].split('}',1)[1]
 assert sha(path)==row['seed_xob_sha256']
 models.append({k:row[k] for k in ['new_model','new_fbx','new_fbx_sha256','seed_xob_sha256']})
report={'status':'SOURCE_INTEGRATED_NATIVE_REBUILD_PENDING','sourceIntegrated':True,'manifest':str(A/'APPROVED_ART_HANDOFF.json'),'manifestSHA256':sha(A/'APPROVED_ART_HANDOFF.json'),'newFiles':allowed,'parentRebases':rebases,'models':models,'core1152AndSmoke14Unchanged':True,'baselineAudioWrapperUnchanged':True,'visualScope':m['visual_approval'],'inheritedBase':m['inherited_art_base'],'exclusions':m['exclusions'],'native':'NOT_TESTED','source':str(D),'recovery':'Copy and two exact rebases completed; old receipt assertion incorrectly assumed49 instead of actual sealed52 files. No copying repeated; every completed output independently verified.'}
(P/'ART-INTEGRATION-RESULT.json').write_text(json.dumps(report,indent=2)+'\n');(P/'ART-IMPORT-EXPECTATIONS.json').write_text(json.dumps({'models':models},indent=2)+'\n')
print(json.dumps({'additiveFiles':len(allowed),'models':len(models),'allHashesVerified':True,'copiedAgain':False}))

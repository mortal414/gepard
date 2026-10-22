"""Read-only art rebuild/adoption gate. Never imports, launches or binds."""
from pathlib import Path
import argparse,hashlib,json,sys
ROOT=Path(__file__).resolve().parent
SLOT=Path(r'C:\Users\marco\Documents\Codex\2026-10-12\task\RUNTIME_SLOT.json')
def read(p):return json.loads(p.read_text(encoding='utf-8-sig'))
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()

CORE_BASELINE_SHA256 = 'b2649c3ec7a11939853c87d3953cec5e65b2e1e55ada6fa64a778cc029a02da3'
CONTINUITY_SHA256 = '73ff69cf038b30cf8a82c54a65648e6edf34f0f14a0dcc407e167136618ea8cf'
EXACT_HUD_SUCCESSORS = {'scripts/Game/GEP/GEP_GunStatus.c': {'old': 'db84c6c9afc57be1a8b1406e0b40dbe8e0a047b2ee00074f093da436d3ce1966', 'new': '44f14a9ce04e7c4d18decf5ba49a4b0cfdbdc04db18b982e3ec12287c099b4db'}, 'scripts/Game/GEP/GEP_Presentation680.c': {'old': 'bb7dd41acefa9d3c6266eb22127fecd6241c9fa84c1f3eed621eb1fa62d85fca', 'new': '10add9409d473078c8f9ffe8c1ee349cd3d43763ac907b9f4a285cb7ae0146f1'}}
CORE_FILES_EXPECTED = 1112
def core_continuity_reasons(core, source_core, addon, continuity):
 """Exact two source-text transitions only; tested ART4 stage keeps old core pins."""
 try:
  assert sha(ROOT/'CORE-BEFORE.json')==CORE_BASELINE_SHA256, 'Baseline core manifest pin changed'
  assert sha(continuity)==CONTINUITY_SHA256, 'Explicit continuity receipt pin changed'
  proof=read(continuity)
  assert proof['status']=='PASS_EXACT_TWO_LITERAL_SUCCESSORS_ONLY_ALL_OTHER_CORE_UNCHANGED', 'Continuity status differs'
  assert proof['baselineCoreManifestSha256']==CORE_BASELINE_SHA256 and len(core)==CORE_FILES_EXPECTED, 'Continuity baseline/count differs'
  assert Path(proof['baselineCoreManifest']).resolve()==(ROOT/'CORE-BEFORE.json').resolve(), 'Baseline manifest path differs'
  assert Path(proof['currentSourceRoot']).resolve()==source_core.resolve() and Path(proof['art04NativeRoot']).resolve()==addon.resolve(), 'Continuity source/tested-stage path differs'
  rows=proof['allowedExactChanges']
  assert len(rows)==2 and {row['relative'] for row in rows}==set(EXACT_HUD_SUCCESSORS), 'Only exact two HUD source paths allowed'
  for row in rows:
   expected=EXACT_HUD_SUCCESSORS[row['relative']]
   assert row['oldSha256']==row['nativeArt04UsedSha256']==core[row['relative']]==expected['old'], 'Old tested HUD pin differs'
   assert row['newSha256']==expected['new'] and row['literalOnlySourceGuard'] is True, 'Successor HUD pin/scope differs'
  for name,old_hash in core.items():
   expected=EXACT_HUD_SUCCESSORS[name]['new'] if name in EXACT_HUD_SUCCESSORS else old_hash
   assert (source_core/name).is_file() and sha(source_core/name)==expected, 'Unlisted or mismatched source-core change: '+name
   if name!='resourceDatabase.rdb':
    assert (addon/name).is_file() and sha(addon/name)==old_hash, 'Actual ART4 old-stage core pin changed: '+name
  return []
 except (AssertionError,KeyError,TypeError,ValueError,OSError) as error:
  return ['Explicit core continuity refused: '+str(error)]

def inspect(addon,receipt,continuity=None):
 reasons=[];models=[];manifest=read(ROOT/'COMPARISON_OVERLAY_MANIFEST.json');core=read(ROOT/'CORE-BEFORE.json')
 source_core=ROOT.parents[2]/'addon'
 if continuity is None:
  if any(not (source_core/n).is_file() or sha(source_core/n)!=v for n,v in core.items()):reasons.append('Approved core source changed')
 else:
  reasons.extend(core_continuity_reasons(core,source_core,addon,continuity))
 if any(not (addon/n).is_file() or sha(addon/n)!=v for n,v in core.items() if n!='resourceDatabase.rdb'):reasons.append('Core runtime bytes changed outside generated resource database')
 for model in manifest['models']:
  relative=model['new_model'].split('}',1)[1];cooked=addon/relative;fbx=addon/model['fbx'];meta=addon/model['meta']
  changed=cooked.is_file() and sha(cooked)!=model['seed_xob_sha256']
  if not changed:reasons.append('Baseline registration seed not rebuilt: '+relative)
  if not fbx.is_file() or sha(fbx)!=model['fbx_sha256']:reasons.append('Authored FBX differs: '+model['fbx'])
  guid=model['new_model'].split('}',1)[0][1:]
  if not meta.is_file() or guid.lower() not in meta.read_text().lower():reasons.append('New model metadata/GUID differs: '+relative)
  models.append({'resource':model['new_model'],'currentSHA256':sha(cooked) if cooked.is_file() else None,'changedFromSeed':changed})
 adoption=read(receipt)
 if adoption.get('status')!='NATIVE_ART_IMPORT_REGISTRATION_REVIEWED':reasons.append('Authentic native import/adoption is pending')
 if not adoption.get('nativeRun'):reasons.append('Native run provenance absent')
 if any(adoption.get('registration',{}).get(k)!='PASS' for k in ('bones','materials','parts')):reasons.append('Bones/material/part registration review incomplete')
 for model,row in zip(manifest['models'],models):
  if adoption.get('rebuiltModelSHA256',{}).get(row['resource'])!=row['currentSHA256']:reasons.append('Adoption does not pin current cooked model: '+row['resource'])
  if adoption.get('sourceFBXSHA256',{}).get(model['fbx'])!=model['fbx_sha256']:reasons.append('Adoption does not pin authored FBX: '+model['fbx'])
 evidence=adoption.get('evidenceFiles',[])
 if not evidence:reasons.append('Native rebuild/registration raw evidence absent')
 for item in evidence:
  p=Path(item['path'])
  if not p.is_file() or sha(p)!=item['sha256']:reasons.append('Native evidence file missing/changed')
 lease=read(SLOT)
 if str(lease.get('status','')).startswith('user_runtime_hold') or lease.get('userRuntimeHold',{}).get('active') is not False:reasons.append('Explicit user runtime resume/admission pending')
 return {'status':'PASS_SOURCE_NATIVE_ADOPTION_GATE_ONLY' if not reasons else 'NOT_READY','reasons':reasons,'models':models,'userArtAcceptance':'PENDING_SEPARATE_JUDGMENT','nativeStarted':False}
if __name__=='__main__':
 parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--addon',type=Path,default=Path(r'D:\ArmaProjects\Gepard-Consolidated-20261022\native-stage\gepard'));parser.add_argument('--receipt',type=Path,default=ROOT/'NATIVE-ADOPTION-ART4-REVIEWED.json');parser.add_argument('--continuity',type=Path,default=ROOT/'CORE-CONTINUITY-ART4-HUD-LITERALS.json');args=parser.parse_args()
 report=inspect(args.addon,args.receipt,args.continuity);print(json.dumps(report,indent=2));sys.exit(0 if report['status'].startswith('PASS') else 2)

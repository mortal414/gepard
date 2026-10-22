"""Read-only art rebuild/adoption gate. Never imports, launches or binds."""
from pathlib import Path
import argparse,hashlib,json,sys
ROOT=Path(__file__).resolve().parent
SLOT=Path(r'C:\Users\marco\Documents\Codex\2026-10-12\task\RUNTIME_SLOT.json')
def read(p):return json.loads(p.read_text(encoding='utf-8-sig'))
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def inspect(addon,receipt):
 reasons=[];models=[];manifest=read(ROOT/'COMPARISON_OVERLAY_MANIFEST.json');core=read(ROOT/'CORE-BEFORE.json')
 source_core=ROOT.parents[2]/'addon'
 if any(not (source_core/n).is_file() or sha(source_core/n)!=v for n,v in core.items()):reasons.append('Approved core source changed')
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
 parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--addon',type=Path,default=Path(r'D:\ArmaProjects\Gepard-Consolidated-20261022\native-stage\gepard'));parser.add_argument('--receipt',type=Path,default=ROOT/'NATIVE-ADOPTION-PENDING.json');args=parser.parse_args()
 report=inspect(args.addon,args.receipt);print(json.dumps(report,indent=2));sys.exit(0 if report['status'].startswith('PASS') else 2)

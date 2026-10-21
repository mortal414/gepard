"""Create one exclusive private native fixture; never starts native software."""
from pathlib import Path
import argparse, hashlib, json, shutil
qa=Path(__file__).resolve().parent
root=qa.parents[1]
ap=argparse.ArgumentParser()
ap.add_argument('--output',type=Path,default=qa/'native-stage/gepard')
args=ap.parse_args()
output=args.output.resolve()
if output.exists(): raise SystemExit('Refusing to overwrite existing stage: '+str(output))
shutil.copytree(root/'addon',output)
shutil.copytree(qa/'native-overlay',output,dirs_exist_ok=True)
manifest={str(p.relative_to(output)).replace('\\','/'):hashlib.sha256(p.read_bytes()).hexdigest() for p in sorted(output.rglob('*')) if p.is_file()}
receipt={'status':'STAGED_NATIVE_NOT_TESTED','project':str(output/'Gepard.gproj'),'world':'{B7E96C96A1D02024}Worlds/GEP_Demo.ent','prefab':'{B3495C8E7A0D2001}Prefabs/Vehicles/GEP_MobileCarrier_StockSmoke015.et','files':len(manifest),'sourceManifest':manifest,'nativeLaunched':False,'ownership':'Native admission remains with coordinator; no lease operation performed'}
(output.parent/'STAGE.json').write_text(json.dumps(receipt,indent=2)+'\n',encoding='utf-8')
print(json.dumps({k:v for k,v in receipt.items() if k!='sourceManifest'},indent=2))

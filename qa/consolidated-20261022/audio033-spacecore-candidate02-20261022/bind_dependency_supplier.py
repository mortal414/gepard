from pathlib import Path
import json,hashlib
N=Path(__file__).resolve().parent;S=N.parent.parent/'task-5/stock-roll-candidate/spacecore-dependency-candidate-02'
manifest=json.loads((S/'MANIFEST.json').read_text());addon=S/'spaceCoreTracks'
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
assert sha(S/manifest['archive'])==manifest['archiveSha256']
pins={x['path']:x['sha256'] for x in manifest['files']}
assert len(pins)==3 and all(sha(addon/n)==h for n,h in pins.items())
target=N/'SPACECORE-SUPPLIER-BINDING.json';assert not target.exists()
target.write_text(json.dumps({'addonRoot':str(addon),'manifest':pins,'addonGUID':manifest['projectGuid'],'prefabResource':manifest['prefab'],'sourceManifest':str(S/'MANIFEST.json'),'sourceManifestSHA256':sha(S/'MANIFEST.json'),'archiveSHA256':manifest['archiveSha256'],'scope':manifest['changeScope']},indent=2)+'\n')
print(target)

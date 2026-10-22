from pathlib import Path
import hashlib,json,re
R=Path(r'D:\ArmaProjects\Gepard-Consolidated-20261022')
H=Path(r'D:\ArmaProjects\Gepard-GM-Hotfix-1.0.2-20261021')
roots=[R/'native-runs/startup-core-02']+[p for p in (H/'native-runs').iterdir() if p.name.startswith('gm-')]
for root in roots:
    for path in root.rglob('*'):
        if path.is_file() and 'console' in path.name.lower():
            raw=path.read_bytes();text=raw.decode('utf-8',errors='replace');lines=text.splitlines()
            hits=[(i+1,line) for i,line in enumerate(lines) if 'SlidingTrackMaterial' in line or ('WORLD' in line and ('(E)' in line or 'ERROR' in line))]
            print(json.dumps({'path':str(path),'bytes':len(raw),'sha256':hashlib.sha256(raw).hexdigest(),'hits':hits}))
            if root==roots[0]:
                for i,line in enumerate(lines):
                    if 'SlidingTrackMaterial' in line or ('Unknown' in line and 'Parent' in line):
                        print('\n'.join(f'{j+1}: {lines[j]}' for j in range(max(0,i-6),min(len(lines),i+5))))
for root in [R/'addon',R/'native-stage/gepard',H/'public-source/addon',H/'native-stage/gepard']:
    for path in root.rglob('*.et'):
        text=path.read_text(encoding='utf-8',errors='replace')
        if 'SlidingTrackMaterial' in text:
            print(json.dumps({'source':str(path),'sha256':hashlib.sha256(path.read_bytes()).hexdigest(),'matches':[(i+1,l) for i,l in enumerate(text.splitlines()) if 'SlidingTrackMaterial' in l or 'Parent ' in l]}))

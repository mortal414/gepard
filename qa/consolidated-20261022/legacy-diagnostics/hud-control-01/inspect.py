from pathlib import Path
import json,hashlib
R=Path(r'D:\ArmaProjects\Gepard-Consolidated-20261022')
p=R/'native-runs/hud-render-control-01/hud-render-startup/native-run/logs/console.log'
raw=p.read_bytes();lines=raw.decode('utf-8',errors='replace').splitlines()
print(json.dumps({'path':str(p),'bytes':len(raw),'sha256':hashlib.sha256(raw).hexdigest()}))
for i,line in enumerate(lines):
    if 'SCR_WidgetExportRuleRoot' in line or ('GUI' in line and '(E)' in line):
        print('\n'.join(f'{j+1}: {lines[j]}' for j in range(max(0,i-15),min(len(lines),i+8))))
for root in [R/'native-runs/hud-render-control-01',Path(r'D:\ArmaProjects\Gepard-EveningSprint-20261021/native-runs'),Path(r'D:\ArmaProjects\Gepard-DriveHud-Update-20261020/native-runs')]:
    print(json.dumps({'root':str(root),'children':[p.name for p in root.iterdir() if p.is_dir()]}))

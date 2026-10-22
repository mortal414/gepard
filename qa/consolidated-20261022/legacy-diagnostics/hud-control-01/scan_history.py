from pathlib import Path
import json,hashlib,re
O=Path(__file__).resolve().parent
roots=[Path(r'D:\ArmaProjects\Gepard-GM-Hotfix-1.0.2-20261021/native-runs'),Path(r'D:\ArmaProjects\Gepard-EveningSprint-20261021/native-runs'),Path(r'D:\ArmaProjects\Gepard-DriveHud-Update-20261020/native-runs')]
rows=[]
for root in roots:
    for path in root.rglob('*console*.log'):
        rel=path.relative_to(root).as_posix()
        if root==roots[0] and not rel.startswith('gm-'):continue
        if root!=roots[0] and not any(x in rel.lower() for x in ['hud','optic','human','release101-r2-healthy','release101-healthy']):continue
        raw=path.read_bytes();lines=raw.decode('utf-8',errors='replace').splitlines()
        hits=[];loads=[];guiErrors=[]
        for i,line in enumerate(lines):
            if 'SCR_WidgetExportRuleRoot' in line:
                hits.append({'line':i+1,'raw':line,'context':[{'line':j+1,'raw':lines[j]} for j in range(max(0,i-3),min(len(lines),i+3))]})
            if 'IntroSplashScreen.layout' in line:loads.append({'line':i+1,'raw':line})
            if 'GUI' in line and '(E)' in line:guiErrors.append({'line':i+1,'raw':line})
        row={'path':str(path),'bytes':len(raw),'sha256':hashlib.sha256(raw).hexdigest(),'hits':hits,'introLoads':loads,'guiErrors':guiErrors}
        rows.append(row)
        print(json.dumps({'path':str(path),'hits':[h['line'] for h in hits],'introLoadLines':[h['line'] for h in loads],'guiErrorCount':len(guiErrors)}))
(O/'HISTORY-SCAN.json').write_text(json.dumps({'scope':'Bounded gm-* hotfix consoles and HUD/optic/human/healthy-release native consoles in named historical roots','logs':rows},indent=2)+'\n',encoding='utf-8')

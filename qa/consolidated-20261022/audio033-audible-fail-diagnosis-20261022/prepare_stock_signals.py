from pathlib import Path
import hashlib,json,shutil
N=Path(__file__).resolve().parent;V=N/'stock-signal-read';B=V/'binding';STAGE=V/'native-stage'
S=N.parent.parent/'task-5/stock-roll-candidate/research/diagnosis-audible-fail-01'
def load(p):return json.loads(p.read_text(encoding='utf-8-sig'))
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def inv(p):return {x.relative_to(p).as_posix():sha(x) for x in p.rglob('*') if x.is_file()}
def save(p,d):p.parent.mkdir(parents=True,exist_ok=True);p.write_text(json.dumps(d,indent=2)+'\n')
assert not V.exists();seal=load(S/'SIGNAL-READ-SOURCE-SEAL.json');original=Path(seal['project']).parent
assert inv(original)==seal['manifest']
shutil.copytree(original,STAGE/'stockSignalRead');B.mkdir();project=STAGE/'stockSignalRead/GEP033_StockSignalRead.gproj'
manifest=B/'SIG-MANIFEST.json';save(manifest,inv(project.parent));save(B/'SOURCE-CLOSURES.json',[{'root':str(project.parent),'manifest':str(manifest)}]);save(B/'SOURCE-BEFORE-COMPILE.json',{str(project.parent):inv(project.parent)})
src=(N/'diagnostic-v3/binding/tools/Compile.ps1').read_text()
src=src.replace(str(N/'diagnostic-v3/binding'),str(B))
src=src.replace(str(N/'diagnostic-v3/native-stage')+','+str(N/'dependencies')+',',str(STAGE)+',')
(B/'Compile.ps1').write_text(src,newline='\n')
src=(N.parent/'sprint-tools/TraceStockRollingAuthorized.ps1').read_text()
src=src.replace('GEP033_TraceStockRollingPlugin','GEP033_InspectStockRollSignals')
src=src.replace('Enfusion Workbench - Gepard stock rolling graph inspection','Enfusion Workbench - Gepard stock rolling gate readback')
src=src.replace(r'D:\ArmaProjects\Gepard-Consolidated-20261022\native-audio-stage\stockRollTrace\GEP_StockRollTrace033.gproj',str(project))
src=src.replace(r'D:\ArmaProjects\Gepard-Consolidated-20261022\native-audio-stage',str(STAGE))
src=src.replace("'GEP033_ROLL_TRACE_END complete='","'GEP033_SIG_END loaded='").replace("'GEP033_ROLL_TRACE_END complete=1'","'GEP033_SIG_END loaded=1'")
(B/'Inspect.ps1').write_text(src,newline='\n')
save(V/'SOURCE-PROVENANCE.json',{'supplierSeal':seal,'copiedExact':inv(project.parent)==seal['manifest'],'project':str(project),'plugin':'GEP033_InspectStockRollSignals','scope':'Five fixed STOCK SIG read-only BaseContainerTools.LoadContainer; console walk only. No WCS payload, FileIO, Save, Set, event or gain changes.','security':'Do not approve a new unexpected security prompt; inspect and report exact scope. Prior pinned authoring approval is not blanket approval for new methods.'})
print(json.dumps({'stage':str(STAGE),'binding':str(B),'sourceCount':len(inv(project.parent))}))

"""Fresh successor after first plugin parse failure; keep failed source intact."""
from pathlib import Path
import json,hashlib
P=Path(__file__).resolve().parent;T=Path(r'D:\ArmaProjects\Gepard-Demo10-TrackArt-20261022');I=T/'private-demo-source10/single/integration';B=P/'art-import-v2/source/artImport'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
m=json.loads((P/'ART-IMPORT-EXPECTATIONS.json').read_text());assert not B.exists();B.mkdir(parents=True)
(B/'GEP_ApprovedArtImport.gproj').write_text((P/'native-import-stage/artImport/GEP_ApprovedArtImport.gproj').read_text())
p=B/'scripts/WorkbenchGame/GEP_Demo10ApprovedArtRebuild.c';p.parent.mkdir(parents=True)
files=',\n'.join('   "'+(I/x['new_fbx']).as_posix()+'"' for x in m['models'])
p.write_text('''[WorkbenchPluginAttribute(name:"Rebuild eight approved private articulated models",category:"Gepard private art",wbModules:{"ResourceManager"})]
class GEP_Demo10ApprovedArtRebuild : ResourceManagerPlugin
{
 override void RunCommandline()
 {
  ResourceManager manager = Workbench.GetModule(ResourceManager);
  if (!manager)
  {
   return;
  }
  array<string> files = {
'''+files+'''
  };
  manager.RebuildResourceFiles(files, "PC");
  Print("GEP_DEMO10_APPROVED_8MODEL_BATCH_QUEUED");
 }
}
''')
pins={x.relative_to(B).as_posix():sha(x) for x in B.rglob('*') if x.is_file()};(P/'art-import-v2/IMPORT-SOURCE-PINS.json').write_text(json.dumps(pins,indent=2)+'\n')
s=(P/'CookApprovedArt.ps1').read_text();assert s.count(str(P/'native-import-stage'))==1;s=s.replace(str(P/'native-import-stage'),str(B.parent))
(P/'art-import-v2/CookApprovedArt.ps1').write_text(s,newline='\n')
print('Fresh explicit-brace/multiline plugin ready; first failed source unchanged; no native launched.')

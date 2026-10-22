from pathlib import Path
import json,hashlib
P=Path(__file__).resolve().parent;T=Path(r'D:\ArmaProjects\Gepard-Demo10-TrackArt-20261022');I=T/'private-demo-source10/single/integration';B=P/'native-import-stage/artImport'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def save(p,d):p.parent.mkdir(parents=True,exist_ok=True);p.write_text(json.dumps(d,indent=2)+'\n')
m=json.loads((P/'ART-IMPORT-EXPECTATIONS.json').read_text());assert len(m['models'])==8 and not B.exists();B.mkdir(parents=True)
(B/'GEP_ApprovedArtImport.gproj').write_text('GameProject { ID "GepardApprovedArtNativeImport" GUID "A76A420022110001" TITLE "PRIVATE Gepard Demo10 approved art import" Dependencies { "58D0FB3206B6F859" "D561AD08932E7B40" "7E0150C026101500" "5E51C5D8F04FDB80" } Configurations { GameProjectConfig PC {} GameProjectConfig HEADLESS : PC {} } }\n')
plugin=B/'scripts/WorkbenchGame/GEP_Demo10ApprovedArtRebuild.c';plugin.parent.mkdir(parents=True)
files=','.join('"'+(I/x['new_fbx']).as_posix()+'"' for x in m['models'])
plugin.write_text('[WorkbenchPluginAttribute(name:"Rebuild eight approved private articulated models",category:"Gepard private art",wbModules:{"ResourceManager"})]\nclass GEP_Demo10ApprovedArtRebuild : ResourceManagerPlugin { override void RunCommandline() { ResourceManager manager=Workbench.GetModule(ResourceManager);if(!manager)return; array<string> files={'+files+'}; manager.RebuildResourceFiles(files,"PC");Print("GEP_DEMO10_APPROVED_8MODEL_BATCH_QUEUED"); } }\n')
save(P/'IMPORT-SOURCE-PINS.json',{p.relative_to(B).as_posix():sha(p) for p in B.rglob('*') if p.is_file()})
source=Path(r'D:\ArmaProjects\Gepard-Consolidated-20261022\qa\consolidated-20261022\tools\CookV11.ps1').read_text()
source=source.replace('GEP_ArtV11RebuildFunctional','GEP_Demo10ApprovedArtRebuild').replace('PRIVATE Gepard V11 articulated import','PRIVATE Gepard Demo10 approved art import')
old=r'D:\ArmaProjects\Gepard-Consolidated-20261022\native-import-stage,D:\ArmaProjects\Gepard-Consolidated-20261022\native-stage,D:\SteamLibrary\steamapps\common\Arma Reforger\addons'
new=','.join([str(B.parent),str(T/'private-demo-source10/single'),str(T/'private-demo-source10/dependencies'),r'D:\SteamLibrary\steamapps\common\Arma Reforger\addons']);assert source.count(old)==1;source=source.replace(old,new)
source=source.replace(r'D:\ArmaProjects\Gepard-Consolidated-20261022\qa\consolidated-20261022\art-v11\COMPARISON_OVERLAY_MANIFEST.json',str(P/'ART-IMPORT-EXPECTATIONS.json')).replace(r'D:\ArmaProjects\Gepard-Consolidated-20261022\native-stage\gepard',str(I))
source=source.replace('$successCount -ge 5','$successCount -ge 8').replace('GEP_ARTV11_FUNCTIONAL_5MODEL_BATCH_QUEUED','GEP_DEMO10_APPROVED_8MODEL_BATCH_QUEUED').replace('after5-builds','after8-builds').replace('five changed cooked hashes','eight changed cooked hashes')
preflight={'integration':{p.relative_to(I).as_posix():sha(p) for p in I.rglob('*') if p.is_file()}}
save(P/'IMPORT-TARGET-PINS.json',preflight['integration'])
needle="[IO.Directory]::CreateDirectory($RunRoot)|Out-Null"
guard="$targetPins=Get-Content -LiteralPath '"+str(P/'IMPORT-TARGET-PINS.json')+"' -Raw|ConvertFrom-Json -AsHashtable\n$targetRoot='"+str(I)+"'\n$targetFiles=@(Get-ChildItem -LiteralPath $targetRoot -Recurse -File)\nif($targetFiles.Count -ne $targetPins.Count){throw 'Pre-import complete target closure count differs'}\nforeach($rel in $targetPins.Keys){if((Get-FileHash -LiteralPath (Join-Path $targetRoot $rel) -Algorithm SHA256).Hash.ToLowerInvariant() -ne $targetPins[$rel]){throw ('Pre-import target pin differs: '+$rel)}}\n"
assert source.count(needle)==1;source=source.replace(needle,guard+needle)
(P/'CookApprovedArt.ps1').write_text(source,newline='\n')
save(P/'IMPORT-BINDING.json',{'project':str(B/'GEP_ApprovedArtImport.gproj'),'plugin':'GEP_Demo10ApprovedArtRebuild','manifest':str(P/'IMPORT-SOURCE-PINS.json'),'runRoot':str(T/'native-runs/approved-art-rebuild-01'),'wrapper':str(P/'CookApprovedArt.ps1'),'inputManifest':str(P/'IMPORT-TARGET-PINS.json'),'commandScope':'Eight exact approved additive FBXs; normal owned close only; no stock/current/release source changes','gameplay':'NOT_TESTED'})
print('Prepared exact eight-model rebuild binding; not executed.')

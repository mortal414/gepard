"""Rebind the exercised serial compiler to the exact final Demo10 closures."""
from pathlib import Path
import json,hashlib
P=Path(__file__).resolve().parent;T=Path(r'D:\ArmaProjects\Gepard-Demo10-TrackArt-20261022');S=T/'private-demo-source10';A=P.parent/'GEP033-BoundedQualification-20261022'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def inv(p):return {x.relative_to(p).as_posix():sha(x) for x in p.rglob('*') if x.is_file()}
def save(p,d):p.parent.mkdir(parents=True,exist_ok=True);p.write_text(json.dumps(d,indent=2)+'\n')
assert not (P/'compile/Compile.ps1').exists()
closures=[]
for r in ['single/gepard','single/wcsSmoke','single/integration','single/provingGround','dependencies/SpaceCore_5E389BB9F58B79A6','dependencies/WCS_Armaments_629B2BA37EFFD577']:
 root=S/r;name=root.name;manifest=P/f'compile/SourceManifest-{name}.json';save(manifest,inv(root));closures.append({'root':str(root),'manifest':str(manifest),'manifestSHA256':sha(manifest)})
save(P/'compile/SOURCE-CLOSURES.json',closures)
script=(A/'binding/tools/Compile.ps1').read_text();old=str(A/'binding/SOURCE-CLOSURES.json');assert script.count(old)==1;script=script.replace(old,str(P/'compile/SOURCE-CLOSURES.json'))
old=str(A/'native-stage')+','+str(P.parent/'GEP033-WCS-Reference-Diagnosis-20261022/dependencies')+r',D:\SteamLibrary\steamapps\common\Arma Reforger\addons'
new=','.join([str(S/'single'),str(S/'dependencies'),r'D:\SteamLibrary\steamapps\common\Arma Reforger\addons']);assert script.count(old)==1;script=script.replace(old,new)
(P/'compile/Compile.ps1').write_text(script,newline='\n')
save(P/'compile/BINDING.json',{'project':str(S/'single/provingGround/GepardProvingGround.gproj'),'run':str(T/'native-runs/compile-demo10-core-01'),'sourceClosures':closures,'native':'NOT_TESTED'})
print('Final-source serial compile prepared; no native launch.')

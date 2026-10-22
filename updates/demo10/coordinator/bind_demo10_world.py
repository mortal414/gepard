"""Bind the existing proving ground to the additive successor, without new objects."""
from pathlib import Path
import json, hashlib, re
P=Path(__file__).resolve().parent
T=Path(r'D:\ArmaProjects\Gepard-Demo10-TrackArt-20261022')
S=T/'private-demo-source10/single'
def sha(p): return hashlib.sha256(p.read_bytes()).hexdigest()
def inv(p): return {f.relative_to(p).as_posix():sha(f) for f in sorted(p.rglob('*')) if f.is_file() and f.name!='resourceDatabase.rdb'}
def digest(d): return hashlib.sha256(json.dumps(d,sort_keys=True,separators=(',',':')).encode()).hexdigest()
src=Path(r'D:\ArmaProjects\Gepard-Consolidated-20261022\updates\demo09\source\provingGround\Scripts\Game\GPG_Layout.c')
p=S/'provingGround/Scripts/Game/GPG_Layout.c'; text=src.read_text()
bindings={
 'OLD-CAMO':'{D561AD08932E5480}Prefabs/Vehicles/GEP_MobileCarrier.et',
 'OLD-CHARCOAL':'{BEF5FEB0ECCBC01D}Prefabs/Variants/Charcoal018/Prefabs/Vehicles/GEP_MobileCarrier.et',
 'NEW-CAMO':'{A76A420022100002}Prefabs/Demo10/Camo/GEP_MobileCarrierDemo10.et',
 'NEW-CHARCOAL':'{A76A420022100004}Prefabs/Demo10/Charcoal/GEP_MobileCarrierDemo10.et',
 'ROUTE-SUBJECT':'{A76A420022100002}Prefabs/Demo10/Camo/GEP_MobileCarrierDemo10.et'}
art=P/'ART-INTEGRATION-RESULT.json'
artReady=art.exists() and json.loads(art.read_text()).get('sourceIntegrated') is True
if artReady:
 bindings['NEW-CAMO']='{2150FC6F1E081188}Prefabs/ArtApproved20261022/Camo/GEP_MobileCarrier.et'
 bindings['NEW-CHARCOAL']='{8095474EC9806D0D}Prefabs/ArtApproved20261022/Charcoal/GEP_MobileCarrier.et'
 bindings['ROUTE-SUBJECT']=bindings['NEW-CAMO']
artLabel='approved cumulative exterior' if artReady else 'V11 exterior'
labels={
 'OLD-CAMO':'OLD camo: original exterior, original rolling sound, shared approved optic',
 'OLD-CHARCOAL':'OLD charcoal: original exterior, original rolling sound, shared approved optic',
 'NEW-CAMO':f'NEW camo: {artLabel}, SpaceCore track sound, eight-round WCS smoke',
 'NEW-CHARCOAL':f'NEW charcoal: {artLabel}, SpaceCore track sound, eight-round WCS smoke'}
for key,resource in bindings.items():
 pattern=r'(world\.Add\("'+key+r'","[^"]+",)"[^"]+"'
 text,n=re.subn(pattern,lambda m:m[1]+'"'+resource+'"',text);assert n==1,key
for key,label in labels.items():
 pattern=r'(world\.Station\("pad-'+key+r'","'+key+r'",)"[^"]+"'
 replacement=label+'. Compare at this pad; reset only when every occupant has exited and the pad is clear.'
 text,n=re.subn(pattern,lambda m:m[1]+'"'+replacement+'"',text);assert n==1,key
start=text.index('  return "\\nOLD-CAMO:');end=text.index('\n }',start)
text=text[:start]+'  return "'+''.join('\\n'+k+': '+v for k,v in labels.items())+'";'+text[end:]
text=text.replace('OLD/NEW exterior cases share the coordinator\'s functional scripts and approved optic. Read each numbered pad identity.', 'Demo10 OLD/NEW share current functional scripts and approved optic. OLD keeps original rolling audio; NEW and route vehicle use the accepted SpaceCore track-sound direction. Read each numbered pad identity.')
text=text.replace('Experimental smoke service for the two NEW pads.', 'Finite smoke service for the two NEW pads and route vehicle.')
text=text.replace('Use one listening test at a time, stay clear of the moving route and record actual distance.', 'The route vehicle uses SpaceCore tracks. Listen to motion, stopped engine and engine-off tails; use OLD pads for original sound. Stand clear of the route. Distances are marked horizontal offsets; record actual distance.')
runtime={role:inv(S/role) for role in ('gepard','wcsSmoke','integration')}
runtimeHash=digest(runtime)
registry={'bindings':bindings,'objects':34,'boards':24,'layoutTemplateSHA256':sha(src),'runtimeSHA256':runtimeHash,'labels':labels}
registryHash=digest(registry)
text=re.sub(r'(static string RegistryHash\(\) \{return ")[^"]+',lambda m:m[1]+registryHash,text)
text=re.sub(r'(static string RuntimeHash\(\) \{return ")[^"]+',lambda m:m[1]+runtimeHash,text)
assert text.count('world.Station(')==src.read_text().count('world.Station(')
assert len(re.findall(r'world\.(?:Add|Spawn|Target)\(',text))==len(re.findall(r'world\.(?:Add|Spawn|Target)\(',src.read_text()))
p.write_text(text,newline='\n')
out={'status':'FINAL_WORLD_SOURCE_BOUND_PENDING_NATIVE','sourceIntegratedArt':artReady,'registrySHA256':registryHash,'runtimeSHA256':runtimeHash,'registry':registry,'runtimeClosure':runtime,'worldLayoutSHA256':sha(p),'objectCount':34,'boardCount':24,'gameplay':'NOT_TESTED'}
(P/'WORLD-BINDING.json').write_text(json.dumps(out,indent=2)+'\n')
print(json.dumps({k:v for k,v in out.items() if k!='runtimeClosure'}))

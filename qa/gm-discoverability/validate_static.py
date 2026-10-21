"""Static release contract only. Passing is never a Game Master/runtime pass."""
from pathlib import Path
import hashlib, json, re, subprocess, sys

repo=Path(sys.argv[1]).resolve() if len(sys.argv)>1 else Path(__file__).resolve().parents[2]
addon=repo/'addon'
baseline='62daa9ba7986289b6032b48761150ec12763ad03'
def git_blob(path):
 return subprocess.check_output(['git','-C',str(repo),'show',baseline+':'+path])
def digest(data): return hashlib.sha256(data).hexdigest()
old=json.loads(git_blob('SOURCE_MANIFEST.json'))['files']
now={'addon/'+p.relative_to(addon).as_posix():digest(p.read_bytes()) for p in addon.rglob('*') if p.is_file()}
changed=sorted(k for k in old if now.get(k)!=old[k])
added=sorted(set(now)-set(old))
allowed={
 'addon/Gepard.gproj',
 'addon/resourceDatabase.rdb',
 'addon/Configs/EntityCatalog/GEP_Vehicle.conf',
 'addon/Configs/EntityCatalog/GEP_CharcoalVehicle018.conf',
 'addon/Prefabs/Vehicles/GEP_MobileCarrier.et',
 'addon/Prefabs/Variants/Charcoal018/Prefabs/Vehicles/GEP_MobileCarrier.et',
}
checks=[]
def check(name,value):
 checks.append({'name':name,'pass':bool(value)})
 if not value: raise AssertionError(name)
check('only five authored metadata files plus native generated resource database changed; no deleted files',set(changed)==allowed and set(old)<=set(now))
check('eight resources added',len(added)==8 and len(now)==1112)
check('all other release files exact, including gameplay scripts and artwork',all(now[k]==v for k,v in old.items() if k not in allowed))
def text(path): return (repo/path).read_text()
def original(path): return git_blob(path).decode().replace('\r\n','\n')
base='addon/Prefabs/Vehicles/GEP_MobileCarrier.et'
char='addon/Prefabs/Variants/Charcoal018/Prefabs/Vehicles/GEP_MobileCarrier.et'
for path,key in [(base,'GEP102-DefaultName'),(char,'GEP102-CharcoalName')]:
 oldText=original(path)
 current=text(path)
 oldName=re.search(r'\bName\s+"([^"]*Gepard[^"]*)"',oldText).group(1)
 current=current.replace('Name "#'+key+'"','Name "'+oldName+'"',1)
 if path==base:
  current=current.replace('FACTION_US TRAIT_ARMOR TRAIT_ARMED CONTENT_MODDED','FACTION_US TRAIT_ARMOR TRAIT_ARMED',1)
 check('vehicle behavior unchanged: '+path,current==oldText)
for path in ['addon/Configs/EntityCatalog/GEP_Vehicle.conf','addon/Configs/EntityCatalog/GEP_CharcoalVehicle018.conf']:
 restored=text(path).replace('{ m_eValidEditorModes 0 }','{}',1)
 check('catalog spawner/data unchanged; editor duplication disabled: '+path,restored==original(path) and text(path).count('m_eValidEditorModes 0')==1)
gp=text('addon/Gepard.gproj')
start=gp.index('GameProjectConfig PC {');brace=gp.index('{',start)
depth=0;quoted=False;escape=False;end=None
for i in range(brace,len(gp)):
 c=gp[i]
 if escape: escape=False; continue
 if c=='\\' and quoted: escape=True; continue
 if c=='"': quoted=not quoted
 if not quoted:
  if c=='{':depth+=1
  elif c=='}':
   depth-=1
   if depth==0:end=i+1;break
check('only project PC localization configuration changed',gp[:start]+'GameProjectConfig PC {}'+gp[end:]==original('addon/Gepard.gproj'))
registry=text('addon/Configs/Editor/PlaceableEntities/GEP_Vehicles102.conf')
check('exactly two canonical root prefab references',re.findall(r'"(\{[A-F0-9]{16}\}[^"\n]+\.et)"',registry)==[
 '{D561AD08932E5480}Prefabs/Vehicles/GEP_MobileCarrier.et',
 '{BEF5FEB0ECCBC01D}Prefabs/Variants/Charcoal018/Prefabs/Vehicles/GEP_MobileCarrier.et'])
check('registry explicitly exposed','m_bExposed 1' in registry)
edit=text('addon/Prefabs/Editor/Modes/EditorModeEdit.et')
check('native editor root and placing component identities retained','{E56F54E533ACE527}Prefabs/Editor/Modes/EditorModeBase.et' in edit and 'ID "54F0A77282A3AA3B"' in edit and 'SCR_PlacingEditorComponent "{5145484399BAA2E7}"' in edit)
check('editor override is additive delta, no replacement stock lists','SCR_PlaceableEntitiesRegistryFromCatalog' not in edit and edit.count('SCR_PlaceableEntitiesRegistry "')==1)
metaGuids={}
for p in addon.rglob('*.meta'):
 m=re.search(r'Name\s+"\{([A-Fa-f0-9]{16})\}([^"\r\n]+)"',p.read_text(errors='replace'))
 if not m:continue
 guid,rel=m.groups()
 check('unique resource GUID: '+guid,guid not in metaGuids)
 metaGuids[guid]=rel
for path in added:
 if path.endswith('.meta'):continue
 p=repo/path
 m=re.search(r'Name\s+"\{([A-Fa-f0-9]{16})\}([^"\r\n]+)"',Path(str(p)+'.meta').read_text())
 check('added resource metadata path matches: '+path,m and m.group(2)==p.relative_to(addon).as_posix())
 for guid,rel in re.findall(r'"\{([A-Fa-f0-9]{16})\}([^"\r\n]+)"',p.read_text()):
  if rel=='Prefabs/Editor/Modes/EditorModeBase.et':continue
  check('local reference resolves: '+rel,(addon/rel).is_file() and metaGuids.get(guid)==rel)
st=text('addon/Language/GEP_GM102_localization.st');rt=text('addon/Language/GEP_GM102_localization.en_us.conf')
for key in ['GEP102-DefaultName','GEP102-CharcoalName']:
 check('display key defined in source/runtime: '+key,'Id "'+key+'"' in st and '"'+key+'"' in rt)
check('runtime display text contains searchable name',rt.count('Gepard 35 mm SPAA')==2)
manifest=json.loads((repo/'SOURCE_MANIFEST.json').read_text())
check('current source fingerprint exact',manifest['files']==now)
check('tested release metadata pins correct new package',manifest['proposedVersion']=='1.0.2' and manifest['packageSHA256']=='0f0289014bbc78d2eb0bc1bf0697150a7672b7fb599f415f49696b7c8fa052d6' and manifest['nativeValidation']=='PACKAGED_GM_BROWSER_PLACEMENT_PASS')
print(json.dumps({'status':'STATIC_CONTRACT_PASS','checkCount':len(checks),'baseline':baseline,'publicFiles':len(now),'added':added,'changed':changed,'unchangedExisting':len(old)-len(changed),'nativeCompile':'see validation-1.0.2.json','ordinaryBrowserAndPlacement':'see validation-1.0.2.json','packageSHA256':manifest['packageSHA256']},indent=2))

"""C-only source preparation; no wrapper, adapter main or native execution."""
from pathlib import Path
import json,hashlib,re,difflib
ROOT=Path(__file__).resolve().parent
UP=ROOT.parent/'GEP-Consolidated-QA-20261022'
STAGE=Path(r'D:\ArmaProjects\Gepard-Consolidated-20261022')
WORLD=STAGE/'qa/proving-ground';NATIVE=STAGE/'native-stage';ADDON=NATIVE/'provingGround';GEP=NATIVE/'gepard'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def load(p):return json.loads(p.read_text(encoding='utf-8-sig'))
def write(p,v):
 p.parent.mkdir(parents=True,exist_ok=True)
 with p.open('x',encoding='utf-8',newline='\n') as f:f.write(v)
def js(p,v):write(p,json.dumps(v,indent=2)+'\n')
def replace(s,a,b):
 assert s.count(a)==1,repr(a);return s.replace(a,b,1)

# Verify the sealed upstream bridge before reusing it. Never mutate that package.
sealed=load(UP/'DELIVERABLE-MANIFEST.json')
assert sha(UP/'DELIVERABLE-MANIFEST.json')=='70aa4f87166665b7c57c91c1ab9ee36c4ddafbcca125a58893750795e694d1ed'
for row in sealed['files']:assert sha(UP/row['relative'])==row['sha256'],row['relative']

# Exact original pins, independently checked against the integrated source trees.
lock=load(WORLD/'checks/variant-lock.json');pins=lock['sharedRuntime']['files']
assert len(pins)==1112
assert set(pins)=={p.relative_to(GEP).as_posix() for p in GEP.rglob('*') if p.is_file()}
assert all(sha(GEP/p)==h for p,h in pins.items())
worldpins={n.removeprefix('addon/'):h for n,h in load(WORLD/'checks/world-source-manifest.json')['files'].items()}
assert len(worldpins)==10
assert set(worldpins)=={p.relative_to(ADDON).as_posix() for p in ADDON.rglob('*') if p.is_file()}
assert all(sha(ADDON/p)==h for p,h in worldpins.items())
js(ROOT/'GEPARD_SOURCE_MANIFEST.json',pins);js(ROOT/'QA_SOURCE_MANIFEST.json',worldpins)
closures=[{'root':str(ADDON),'manifest':str(ROOT/'QA_SOURCE_MANIFEST.json')},{'root':str(GEP),'manifest':str(ROOT/'GEPARD_SOURCE_MANIFEST.json')}]
js(ROOT/'SOURCE-CLOSURES.json',closures)

# Recover exact expected calls from the pinned generated layout and known director.
layout=(ADDON/'scripts/Game/GPG_Layout.c').read_text();director=(ADDON/'scripts/Game/GPG_Director.c').read_text()
resources=dict(re.findall(r'static ResourceName (\w+)\(\) \{return "([^"]+)";', (ADDON/'scripts/Game/GPG_Resources.c').read_text()))
objects={}
def obj(id,station,prefab,pad):
 assert id not in objects;objects[id]={'station':station,'prefab':prefab,'pad':list(map(float,pad.split())) if isinstance(pad,str) else pad}
for id,station,prefab,pad in re.findall(r'world.Add\("([^"]+)","([^"]+)","([^"]+)","([^"]+)"',layout):obj(id,station,prefab,pad)
targets=[]
for id,pad,prefab,nominal in re.findall(r'world.Target\("([^"]+)","([^"]+)","([^"]+)",(\d+)\)',layout):
 obj(id,'range',prefab,pad);targets.append({'id':id,'pad':list(map(float,pad.split())),'nominal':int(nominal)})
for id,station,res,pad in re.findall(r'Add\("([^"]+)","([^"]+)",GPG_Resources\.(\w+)\(\),"([^"]+)"',director):obj(id,station,resources[res],pad)
damage=re.search(r'static ResourceName DamagePrefab\(\) \{return "([^"]+)";',layout)[1]
obj('D1','damage',damage,'600 0 1800');obj('D2','damage',damage,'620 0 1800')
for n in range(4):
 for prefix,res,z in [('M72_','Launcher',1843),('RifleAmmo_','RifleAmmo',1845),('MGAmmo_','MachinegunAmmo',1847)]:obj(prefix+str(n),'weapons',resources[res],[585+n,0,z])
assert len(objects)==32,len(objects)
stations={id:list(map(float,board.split())) for id,board in re.findall(r'world.Station\("([^"]+)"[^\n]*?,"([-\d. ]+)"\);',layout)}
assert len(stations)==28,len(stations)
gates={id:list(map(float,pad.split())) for id,pad in re.findall(r'world.Gate\("([^"]+)","([^"]+)"\)',layout)}
gates['DAMAGE FIRE SOUTH']=[610,0,1840];assert len(gates)==12
variants=load(WORLD/'registry/variants.json')
assert [r['id'] for r in variants['slots'] if not r['prefab']]==['NEW-CAMO','NEW-CHARCOAL']
contract={'schema':1,'projectGuid':'A76A420022000001','worldGuid':'A76A420022000002','world':'Worlds/GPG_Arland.ent','loadedAddonIds':['A76A420022000001','D561AD08932E7B40','58D0FB3206B6F859','5614BBCCBB55ED1C'],'registrySHA256':lock['registrySha256'],'runtimeSHA256':lock['sharedRuntime']['sha256'],'objects':objects,'stations':stations,'gates':gates,'targets':targets,'stocks':['GPG_Ammo','GPG_Repair','GPG_DamageAmmo'],'unboundArt':['NEW-CAMO','NEW-CHARCOAL'],'acceptance':'STARTUP_AND_TELEMETRY_CONSISTENCY_ONLY; all station/gameplay/visual acceptance NOT_TESTED'}
js(ROOT/'MARKER-CONTRACT.json',contract)

# Minimal adaptation of the sealed original lifecycle wrapper, without invocation.
original=(UP/'tools/Scene.ps1').read_text(encoding='utf-8-sig');s=original
s=replace(s,r'D:\ArmaProjects\Gepard-OpticApproved-20261022\qa\optic-approved-20261022\SOURCE-CLOSURES.json',str(ROOT/'SOURCE-CLOSURES.json'))
roots=[str(NATIVE),r'D:\SteamLibrary\steamapps\common\Arma Reforger\addons']
addonspath=','.join(roots)
s=replace(s,'$addon=[IO.Path]::GetDirectoryName($Project);$addons=[IO.Path]::GetDirectoryName($addon)',"$addon=[IO.Path]::GetDirectoryName($Project)\nif($Project -cne '"+str(ADDON/'GepardProvingGround.gproj')+"' -or $World -cne 'Worlds/GPG_Arland.ent' -or $AddonId -cne 'A76A420022000001'){throw 'Only frozen proving-ground startup project/world/addon is bound'}\n$addonRootsGPG=@("+','.join("'"+r+"'" for r in roots)+")\nforeach($rootGPG in $addonRootsGPG){if(!(Test-Path -LiteralPath $rootGPG -PathType Container)){throw 'Explicit addon discovery root missing'}}\n$addons=$addonRootsGPG -join ','")
s=replace(s,"requiredAdmission='Art user session retains slot; explicit handback and fresh native admission required'","requiredAdmission='Explicit user resume, fresh ownership admission and actual compile adoption required'")
s=replace(s,' $game=Start-Process -FilePath $exe'," if(UserRuntimeHold705){throw 'User runtime hold active immediately before native launch; preserve claim'}\n $game=Start-Process -FilePath $exe")
s=replace(s,"SlotTransaction {param($l)AssertOwner $l;$l.pid=$game.Id;", "SlotTransaction {param($l)AssertOwner $l;if(UserRuntimeHoldRecord705 $l){throw 'User runtime hold active at attachment; preserve native and lease'};$l.pid=$game.Id;")
write(ROOT/'tools/Scene.ps1',s)
write(ROOT/'tools/Scene-proving-ground.patch',''.join(difflib.unified_diff(original.splitlines(True),s.splitlines(True),fromfile='a/sealed-bridge/Scene.ps1',tofile='b/proving-ground/Scene.ps1')))

# Keep the sealed bridge's admission, cancellation, output bounds and lifecycle code.
s=(UP/'adapter.py').read_text()
s=replace(s,'sha,read_json,save,assertion,review_console,screenshot','sha,read_json,save,assertion,review_console,review_file,untested_acceptance')
s=replace(s,"request['caseId']=='hud-render-startup'","request['caseId']=='gpg-startup-survey'")
s=replace(s,"request['world']=='Worlds/GEP_ApprovedRender705.ent','Only existing HUD render world is bound'","request['world']=='Worlds/GPG_Arland.ent','Only frozen proving-ground world is bound'")
s=replace(s,"Path(request['project']).name=='GEP_ApprovedHUD705_QA.gproj'","Path(request['project']).name=='GepardProvingGround.gproj'")
s=replace(s,"p['addonId']=='7E0050C026102500'","p['addonId']=='A76A420022000001'")
s=replace(s," for name,reason in [('visual.approved','No human pixel/concept judgment in automated bridge'),('glyphs.reviewed','BMP dimensions/marker do not establish Unicode glyph appearance'),('input.physical','Existing probe uses scripted boarding/ADS; ordinary controls remain untested')]:checks[name]={'status':'NOT_TESTED','reason':reason}"," checks.update(untested_acceptance())")
s=replace(s,'Existing native probe fixed case0; exact unique profile/process argv/source receipts bind this request.','GPG emits no native nonce; exact fresh owner/profile/argv/source receipts bind this request; native association unqualified.')
s=replace(s,"  assert request['world'] in plan['args'] and p['addonId'] in plan['args'],'Plan world/addon absent'","  assert request['world'] in plan['args'] and p['addonId'] in plan['args'],'Plan world/addon absent'\n  assert plan['args'][plan['args'].index('-addonsDir')+1]==p['addonSearchPath'],'Explicit addon search path differs'")
start=s.index("  console=native/'logs/console.log';")
end=s.index("  assert not hold_active(read_json(SLOT))",start)
s=s[:start]+"  console=native/'logs/console.log'\n  assert sha(p['markerContract'])==p['markerContractSHA256'],'Marker contract pin differs'\n  review=review_file(console,started_at,str(native/'profile'),request['world'],read_json(p['markerContract']))\n  review.update({'runId':request['runId'],'caseId':request['caseId'],'consoleSHA256':sha(console),'sources':closures_after})\n  report=attempt/'native-review.json';save(report,review);evidence += [console,report]\n"+s[end:]
s=replace(s,"  for name in ['fixture.ready','world.loaded','hud.scripted-ads','hud.capture-file','hud.actual-data']:checks[name]=assertion(True,'Fresh ordered native world/GAME/player/ADS/data/capture evidence; see native-review.json')","  for name in ['fixture.ready','world.loaded','gpg.startup','gpg.telemetry-consistent']:checks[name]=assertion(True,'Fresh source-bound bounded startup and survey telemetry only; no physical station acceptance; see native-review.json')")
s=replace(s,"review['diagnostics']['beforeCapture']+review['diagnostics']['afterCapture']","review['diagnostics']['errors']")
s=s.replace('Qualified source/compile, fresh fixed-case capture and exact owned closure; visual/physical acceptance remains separate','Qualified source/compile, fresh bounded startup/survey markers and exact owned closure; all physical/visual acceptance remains NOT_TESTED')
s=s.replace('late capture','late markers')
write(ROOT/'adapter.py',s)

project=ADDON/'GepardProvingGround.gproj'
js(ROOT/'COMPILE-ADOPTION-PENDING.json',{'schema':1,'status':'NOT_TESTED','project':str(project),'compileRun':None,'compilerResultSHA256':None,'compilerConsoleSHA256':None,'closures':[],'reason':'No native action allowed under explicit user runtime hold. Exact GPG+shared runtime compile/adoption pending; old HUD compile is not transferable.'})
js(ROOT/'WRAPPER-ADAPTATION.json',{'status':'SOURCE_ONLY_NATIVE_NOT_TESTED','upstream':str(UP/'tools/Scene.ps1'),'upstreamSHA256':sha(UP/'tools/Scene.ps1'),'patchedSHA256':sha(ROOT/'tools/Scene.ps1'),'changes':['Complete closure descriptor moved to this C package','Execute path restricts exact frozen GPG project/world/addon','Narrow native-stage parent plus installed base joined by comma, following existing compiler convention; game discovery native qualification pending','Extra hold recheck immediately before native launch and inside atomic attachment prevents attachment from overwriting status-only hold; failure preserves claim/native','Dry-run admission text corrected'],'preserved':['All shared hold guards before admission/claim/loops/sole normal close/release','Exact owned PID/executable/birth/argv and atomic lease','Cancel uses existing exact owned normal close only while hold inactive','No force kill or force release; uncertainty retained'],'nativeAddonSearchPath':addonspath,'nativeDiscoveryQualification':'NOT_TESTED'})
required=[ROOT/n for n in ['adapter.py','evidence.py','tools/Scene.ps1','COMPILE-ADOPTION-PENDING.json','SOURCE-CLOSURES.json','GEPARD_SOURCE_MANIFEST.json','QA_SOURCE_MANIFEST.json','MARKER-CONTRACT.json']]
required += [WORLD/'checks/variant-lock.json',WORLD/'checks/world-source-manifest.json',WORLD/'registry/variants.json',WORLD/'registry/stations.json',WORLD/'registry/resources.json',Path(r'C:\Users\marco\.cache\codex-runtimes\codex-primary-runtime\dependencies\native\powershell\pwsh.exe'),Path(r'D:\SteamLibrary\steamapps\common\Arma Reforger\ArmaReforgerSteamDiag.exe')]
assert all(p.exists() for p in required)
p={'wrapper':str(ROOT/'tools/Scene.ps1'),'wrapperSHA256':sha(ROOT/'tools/Scene.ps1'),'powerShell':str(required[-2]),'closures':str(ROOT/'SOURCE-CLOSURES.json'),'closurePinSHA256':sha(ROOT/'SOURCE-CLOSURES.json'),'completeManifestPins':{r['manifest']:sha(Path(r['manifest'])) for r in closures},'compileAdoption':str(ROOT/'COMPILE-ADOPTION-PENDING.json'),'markerContract':str(ROOT/'MARKER-CONTRACT.json'),'markerContractSHA256':sha(ROOT/'MARKER-CONTRACT.json'),'addonId':'A76A420022000001','addonSearchPath':addonspath,'seconds':60,'screenWidth':1280,'screenHeight':720,'adapterDeadlineSeconds':100,'childLogLimitBytes':4194304}
job={'schema':1,'template':'startup','project':str(project),'world':'Worlds/GPG_Arland.ent','sourceRoots':[str(ADDON),str(GEP)],'addonRoots':roots,'requiredFiles':[{'path':str(p),'sha256':sha(p)} for p in required],'requiredClasses':['GPG_Director','GPG_Layout','GPG_Resources','GPG_BoardComponent','GPG_ManualSpawnLogic','GEP_GunStatus'],'capabilities':['isolated-reset','owned-lifecycle','native-load'],'adapter':{'executable':r'C:\Users\marco\AppData\Local\Programs\Python\Python311\python.exe','script':str(ROOT/'adapter.py'),'args':['{script}','{request}','{result}','{cancel}']},'cases':[{'id':'gpg-startup-survey','parameters':p,'assertions':['fixture.ready','world.loaded','gpg.startup','gpg.telemetry-consistent','native.errors']}],'timeoutSeconds':130,'closeGraceSeconds':35,'notes':'SOURCE ONLY under explicit user hold. One bounded GPG startup/telemetry-consistency case. No automated driving, service, damage, terrain/LOS usability, capture, HUD/input or visual acceptance. NEW art unbound; smoke/audio excluded. Actual compile adoption + explicit user resume and ownership admission required.'}
js(ROOT/'gpg-startup-job.json',job)
js(ROOT/'bindings.proposed.json',{'schema':1,'bindings':{'gepard-proving-ground-startup':{'job':str(ROOT/'gpg-startup-job.json'),'description':'Source-only bounded GPG startup and emitted survey consistency; all native qualification pending.','outputRoot':str(ROOT/'future-evidence')}}})
js(ROOT/'SOURCE-RECEIPT.json',{'status':'COMPLETE_SOURCE_BYTES_MATCH_FROZEN_LOCKS_NATIVE_NOT_TESTED','upstreamBridgeManifestSHA256':sha(UP/'DELIVERABLE-MANIFEST.json'),'worldFiles':len(worldpins),'sharedFiles':len(pins),'worldSourceManifestFileSHA256':sha(WORLD/'checks/world-source-manifest.json'),'variantLockFileSHA256':sha(WORLD/'checks/variant-lock.json'),'integrationReceiptSHA256':sha(STAGE/'qa/consolidated-20261022/PROVING-GROUND-INTEGRATION.json'),'hold':'UNCHANGED','nativeInvoked':False,'dWrites':False,'unboundArt':contract['unboundArt']})
print(json.dumps({'prepared':str(ROOT),'objects':len(objects),'stations':len(stations),'gates':len(gates),'nativeInvoked':False}))

"""Read-only native-log/source comparison. Writes only beside this script on C:."""
from pathlib import Path
from collections import Counter
import hashlib,json,re,datetime

OUT=Path(__file__).resolve().parent
R=Path(r'D:\ArmaProjects\Gepard-Consolidated-20261022')
H=Path(r'D:\ArmaProjects\Gepard-GM-Hotfix-1.0.2-20261021')
Q=Path(r'C:\Users\marco\Documents\Codex\2026-10-21\task\GEP-ProvingGround-Startup-Retry-QA-20261022')
START=R/'native-runs/startup-core-02/gpg-startup-survey/native-run/logs/console.log'
BASES={n:H/f'native-runs/{n}/logs/console.log' for n in ['gm-baseline-01','gm-source-01','gm-source-02','gm-package-01','gm-uploaded-01','gm-catalogless-01']}
def digest(raw):return hashlib.sha256(raw).hexdigest()
def pin(path):
    raw=path.read_bytes();return {'path':str(path),'bytes':len(raw),'sha256':digest(raw)}
pins={}
def read(path):
    raw=path.read_bytes();pins[str(path)]={'path':str(path),'bytes':len(raw),'sha256':digest(raw)}
    return raw
pattern=re.compile(r"Unknown (keyword/data|class) '([^']+)' at offset (\d+)\((0x[0-9a-fA-F]+)\)")
def scan(path):
    raw=read(path);lines=raw.decode('utf-8',errors='replace').splitlines();errors=[];resource=None
    for i,line in enumerate(lines):
        if 'Entity prefab load @' in line:
            resource=line.split('Entity prefab load @',1)[1].strip().strip('"')
        if 'WORLD' not in line or '(E)' not in line:continue
        m=pattern.search(line)
        error={'line':i+1,'raw':line,'enclosingPrefab':resource,'context':[{'line':j+1,'raw':lines[j]} for j in range(max(0,i-3),min(len(lines),i+2))]}
        if m:error.update({'kind':m[1],'keyword':m[2],'offsetDecimal':int(m[3]),'offsetHex':m[4]})
        error['signature']=[error.get('kind'),error.get('keyword'),error.get('offsetDecimal'),resource]
        errors.append(error)
    metadata=[]
    for i,line in enumerate(lines):
        if any(t in line for t in ['Initializing engine, version','Current working directory:','Adding package ', 'GameProject load','CORE TEST','GPG_STAGE_READY']):metadata.append({'line':i+1,'raw':line})
    return {'console':pins[str(path)],'worldErrorCount':len(errors),'errors':errors,'metadata':metadata}
startup=scan(START)
baselines={n:scan(p) for n,p in BASES.items()}
for error in startup['errors']:
    error['matchingBaselines']={n:[e['line'] for e in b['errors'] if e['signature']==error['signature']] for n,b in baselines.items()}
    error['classificationProposal']='LEGACY_EXACT_BASELINE_MATCH' if all(error['matchingBaselines'][n] for n in ['gm-source-01','gm-source-02','gm-package-01','gm-uploaded-01']) else 'NEW_OR_UNEXPLAINED'

manifestPath=Q/'GEPARD_SOURCE_MANIFEST.json'
runManifest=json.loads(read(manifestPath))
releaseManifestPath=H/'public-source/SOURCE_MANIFEST.json'
releaseManifest=json.loads(read(releaseManifestPath))['files']
released=H/'public-source/addon';stage=R/'native-stage/gepard';addon=R/'addon'
originalET=[];changedET=[];missingET=[];fieldHits=[]
for original in sorted(released.rglob('*.et')):
    rel=original.relative_to(released).as_posix();releasedHash=digest(original.read_bytes())
    row={'relativePath':rel,'releaseSha256':releasedHash,'releaseManifestSha256':releaseManifest.get('addon/'+rel),'startupPinnedSha256':runManifest.get(rel)}
    for label,root in [('stage',stage),('productionSource',addon)]:
        row[label+'Sha256']=digest((root/rel).read_bytes()) if (root/rel).exists() else None
    row['releaseManifestMatches']=row['releaseManifestSha256']==releasedHash
    row['exactAtStartup']=row['startupPinnedSha256']==releasedHash
    row['exactInCurrentStage']=row['stageSha256']==releasedHash
    row['exactInCurrentProductionSource']=row['productionSourceSha256']==releasedHash
    originalET.append(row)
    if not all(row[k] for k in ['releaseManifestMatches','exactAtStartup','exactInCurrentStage','exactInCurrentProductionSource']):changedET.append(row)
    for label,path in [('release',original),('stage',stage/rel),('productionSource',addon/rel)]:
        if not path.exists():missingET.append(str(path));continue
        for i,line in enumerate(path.read_text(encoding='utf-8',errors='replace').splitlines()):
            if re.search(r'\bSlidingTrackMaterial\b|\bParent\b',line):fieldHits.append({'scope':label,'path':str(path),'line':i+1,'raw':line})

coreComparisons=[]
for rel,originalHash in releaseManifest.items():
    if not rel.startswith('addon/'):continue
    rel=rel[6:]
    if rel not in runManifest:continue
    if originalHash!=runManifest[rel]:coreComparisons.append({'path':rel,'releaseSha256':originalHash,'startupSha256':runManifest[rel]})
coreCommon=sum(1 for rel in releaseManifest if rel.startswith('addon/') and rel[6:] in runManifest)
recipeFiles=[Q/'SOURCE-CLOSURES.json',Q/'COMPILE-ADOPTION.json',R/'native-runs/startup-core-02/plan.json',R/'native-runs/startup-core-02/summary.json',R/'native-runs/startup-core-02/gpg-startup-survey/result.json',H/'checks/FINAL_RESULT.json',H/'native-runs/gm-uploaded-01/plan.json']
recipes={str(p):json.loads(read(p)) for p in recipeFiles}
strictSummary=recipes[str(R/'native-runs/startup-core-02/summary.json')]['status']
osPins={}
for p in [Path(r'D:\SteamLibrary\steamapps\common\Arma Reforger\addons\core\core.gproj'),Path(r'D:\SteamLibrary\steamapps\common\Arma Reforger\addons\data\ArmaReforger.gproj')]:osPins[str(p)]=pin(p)

result={
 'auditUtc':datetime.datetime.now(datetime.timezone.utc).isoformat(),
 'scope':'C-only read-only comparison of retained logs and source manifests; no native, collector, whitelist or production change',
 'startup':startup,'baselines':baselines,
 'classificationProposal':'7_LEGACY_EXACT_BASELINE_MATCHES_NO_NEW_WORLD_ERROR_SIGNATURE',
 'strictRecordedStatusUnchanged':strictSummary,
 'sourceComparison':{'originalETCount':len(originalET),'nonidenticalET':changedET,'missingET':missingET,'originalETRows':originalET,'matchingKeywordFieldsInGepardET':fieldHits,'commonCoreFilesAtStartup':coreCommon,'nonidenticalCommonCoreFilesAtStartup':coreComparisons},
 'sourceRecipe':{'startupGepardRoot':str(stage),'startupGepardManifest':pins[str(manifestPath)],'releaseSourceRoot':str(released),'releaseSourceManifest':pins[str(releaseManifestPath)],'recipePins':[pins[str(p)] for p in recipeFiles],'stockCurrentGprojPins':osPins},
 'limits':[
  'The seven parser diagnostics remain actual WORLD(E) errors; baseline matching does not prove harmlessness or gameplay acceptance.',
  'Stock-prefab attribution is direct enclosing resource-load evidence plus identical keyword/offset/resource signatures across retained runs, not a decoded stock PAK field inspection.',
  'No retained old stock PAK/executable byte hash was found in the compared launch plans; engine version192142 and stock GUID/path/package counts match, but historical full stock binary identity is not claimed.',
  'Original Gepard ET bytes and startup-manifest pins are checked directly; this cannot establish unseen stock ET payload contents.',
  'Legacy m_bShowDebugShape and catalogless unknown-class diagnostics are retained separately and are not included in this seven-signature proposal.',
  'Any changed keyword, offset, enclosing resource or extra WORLD(E) line remains NEW_OR_UNEXPLAINED; no broad Parent/SlidingTrackMaterial exemption is authorized.'
 ]
}
assert len(startup['errors'])==7
assert all(e['classificationProposal']=='LEGACY_EXACT_BASELINE_MATCH' for e in startup['errors'])
(OUT/'COMPARISON.json').write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8')
(OUT/'INPUT-PINS.json').write_text(json.dumps(pins,indent=2)+'\n',encoding='utf-8')
print(json.dumps({'startupWorldErrors':len(startup['errors']),'allSevenExactBaselineMatches':True,'originalETCount':len(originalET),'nonidenticalETCount':len(changedET),'ETKeywordHits':len(fieldHits),'commonCoreFiles':coreCommon,'changedCommonCoreCount':len(coreComparisons),'changedCommonCoreFiles':[r['path'] for r in coreComparisons],'strictStatus':strictSummary,'comparison':str(OUT/'COMPARISON.json')},indent=2))

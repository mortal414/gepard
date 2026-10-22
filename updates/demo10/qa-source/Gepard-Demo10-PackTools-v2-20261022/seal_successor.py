from pathlib import Path
import json,hashlib,difflib
r=Path(__file__).resolve().parent;old=r.parent/'Gepard-Demo10-PackTools-20261022'
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def read(p):return json.loads(Path(p).read_text(encoding='utf-8-sig'))
def save(p,d):Path(p).write_text(json.dumps(d,indent=2)+'\n',encoding='utf-8',newline='\n')
assert not (r/'SOURCE-MANIFEST.json').exists()
plan=read(r/'ROOT-PACK-PLAN-10.json');assert sha(r/'ROOT-PACK-PLAN-10.json')=='de70dc9fca8a08ccf1e6fc9b2fe181a81407f4970bdb9bfc51fa577c1ad79518'
for p,h in plan['toolPins'].items():assert sha(p)==h,p
assert read(r/'SUCCESSOR-CHECKS.json')['guardSourceTextInverseExactAfterPathRenames']
s=(r/'README.md').read_text(encoding='utf-8-sig').replace('Gepard-Demo10-PackTools-20261022','Gepard-Demo10-PackTools-v2-20261022').replace('f09ecfd278207ecc83a60a8ca62aa5e1d3f6abdb3fd89a2c940bca39f8184311',sha(r/'ROOT-PACK-PLAN-10.json')).replace('eight XOB/eight TXO runtime resources','eight XOB runtime resources').replace('FBX authoring inputs need not be packaged.','FBX/TXO authoring inputs need not be packaged; all source files remain pinned.')
s='This is the frozen10b successor. The original10 tools/seal/native failure remain preserved. TXO-PACK-DIAGNOSIS.md explains the evidence-backed eight-entry PAK-requirement correction. Runtime/source/strict-diagnostic guards are retained; run/output/install/session paths are fresh10b.\n\n'+s
(r/'README.md').write_text(s,encoding='utf-8',newline='\n')
binding=read(r/'ACTUAL-ROOT-BINDING.json');binding['requiredIntegrationResources']=25;binding['TXORequiredInPAK']=False;binding['TXORetainedInSourceManifest']=True;binding['contractSHA256']=sha(r/'ROOT-COMPILED-SOURCE-CONTRACT.json');save(r/'ACTUAL-ROOT-BINDING.json',binding)
final=read(r/'FINAL-SOURCE-CHECKS.json');final['planSHA256']=sha(r/'ROOT-PACK-PLAN-10.json');final['integrationRequiredRuntimeResources']=25;final['requiredTXOResources']=0;final['predecessorNativeBatchFailedAndPreserved']=True;save(r/'FINAL-SOURCE-CHECKS.json',final)
d=read(r/'SCRIPT-DERIVATION.json');d['status']='VERSIONED_TXO_REQUIREMENT_AND_FRESH_PATH_SUCCESSOR_FROZEN'
for row in d['files']:
 row['targetSHA256']=sha(r/row['target'])
 (r/(row['target']+'.patch')).write_text(''.join(difflib.unified_diff(Path(row['source']).read_text(encoding='utf-8-sig').splitlines(keepends=True),(r/row['target']).read_text(encoding='utf-8-sig').splitlines(keepends=True),fromfile=row['source'],tofile=row['target'])),encoding='utf-8',newline='\n')
save(r/'SCRIPT-DERIVATION.json',d)
pre=read(old/'SOURCE-MANIFEST.json')
for n,h in pre['files'].items():assert sha(old/n)==h,('Predecessor changed',n)
original=pre['originalPins'].copy();original[str(old/'SOURCE-MANIFEST.json')]=sha(old/'SOURCE-MANIFEST.json')
files={p.name:sha(p) for p in sorted(r.iterdir()) if p.is_file() and p.name not in ['SOURCE-MANIFEST.json','INSTALLED-PACKAGE-CLOSURE.json']}
out={'schema':1,'status':'FROZEN_DEMO10B_TWOWRAPPER_PATH_ONLY_AND_EIGHT_TXO_CONTRACT_CORRECTION','files':files,'originalPins':original,'contextPinsHistoricalOnly':pre['contextPinsHistoricalOnly'],'predecessorSeal':{'path':str(old/'SOURCE-MANIFEST.json'),'sha256':sha(old/'SOURCE-MANIFEST.json')},'planSHA256':sha(r/'ROOT-PACK-PLAN-10.json'),'nativeStartedByLane':False,'DWrittenByLane':False,'scope':'Source/PAK-index/provenance/AST/pure plan only; first native failure retained, no new package/install/native/gameplay proof.'}
save(r/'SOURCE-MANIFEST.json',out);print(json.dumps({'sealSHA256':sha(r/'SOURCE-MANIFEST.json'),'planSHA256':out['planSHA256'],'files':len(files)}))

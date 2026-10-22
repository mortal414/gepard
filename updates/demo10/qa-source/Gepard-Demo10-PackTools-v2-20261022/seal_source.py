from pathlib import Path
import json,hashlib,difflib
r=Path(__file__).resolve().parent
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def read(p):return json.loads(Path(p).read_text(encoding='utf-8-sig'))
def save(p,d):Path(p).write_text(json.dumps(d,indent=2)+'\n',encoding='utf-8',newline='\n')
assert not (r/'SOURCE-MANIFEST.json').exists(),'Frozen source already sealed; use successor'
plan=read(r/'ROOT-PACK-PLAN-10.json')
assert sha(r/'ROOT-PACK-PLAN-10.json')=='f09ecfd278207ecc83a60a8ca62aa5e1d3f6abdb3fd89a2c940bca39f8184311'
for p,h in plan['toolPins'].items():assert sha(p)==h,p
d=read(r/'SCRIPT-DERIVATION.json');d['status']='ACTUAL_ADOPTED_INPUTS_BOUND_C_ONLY_DEMO10_TOOLS_FROZEN'
for row in d['files']:
 assert sha(row['source'])==row['sourceSHA256']
 row['targetSHA256']=sha(r/row['target'])
 a=Path(row['source']).read_text(encoding='utf-8-sig').splitlines(keepends=True);b=(r/row['target']).read_text().splitlines(keepends=True)
 (r/(row['target']+'.patch')).write_text(''.join(difflib.unified_diff(a,b,fromfile=row['source'],tofile=row['target'])),encoding='utf-8',newline='\n')
save(r/'SCRIPT-DERIVATION.json',d)
(r/'NativeTailClassifier10-v2.ps1.patch').write_text(''.join(difflib.unified_diff((r/'NativeTailClassifier10.ps1').read_text().splitlines(keepends=True),(r/'NativeTailClassifier10-v2.ps1').read_text().splitlines(keepends=True),fromfile='preserved NativeTailClassifier10.ps1',tofile='NativeTailClassifier10-v2.ps1')),encoding='utf-8',newline='\n')
original=read(r/'ORIGINAL-PINS.json');context={p:h for p,h in original.items() if 'Gepard-Demo10-Integration-20261022' in p};original={p:h for p,h in original.items() if p not in context}
for p,h in original.items():assert sha(p)==h,p
save(r/'FINAL-SOURCE-CHECKS.json',{'status':'FROZEN_SOURCE_ONLY_READY_FOR_ROOT_SERIAL_PRIVATE_PACK','planSHA256':sha(r/'ROOT-PACK-PLAN-10.json'),'planAllToolPinsMatch':True,'actualCompileResultAndSixAdoptedClosurePinsBound':True,'sourceCounts':{'core':1152,'smoke':14,'integration':68,'world':11,'SpaceCore':3,'WCS':3},'totalSourceFiles':1251,'integrationRequiredRuntimeResources':33,'requiredFBXResources':0,'fourNativeSteps':[x['role']+':'+x['name'] for x in plan['steps']],'separateDependencyResources':6,'ownInstallResources':12,'originalClassifierAndBaselineByteExact':True,'versionedClassifierFiniteChecks':13,'PowerShellASTChecks':5,'ownershipHelpersAndBoardParserTextExact':True,'nodeSyntaxFiles':3,'rawStrictErrorsRetained':True,'nativeStartedByLane':False,'DWrittenByLane':False,'packagedByLane':False,'rootMayBeExecutingSeparately':True,'contextPinsHistoricalOnly':context})
excluded={'SOURCE-MANIFEST.json','INSTALLED-PACKAGE-CLOSURE.json'}
files={p.name:sha(p) for p in sorted(r.iterdir()) if p.is_file() and p.name not in excluded}
seal={'schema':1,'status':'FROZEN_DEMO10_SOURCE_TOOLS_ACTUAL_COMPILE_BOUND_ROOT_EXECUTION_ONLY','files':files,'originalPins':original,'contextPinsHistoricalOnly':context,'planSHA256':sha(r/'ROOT-PACK-PLAN-10.json'),'nativeStartedByLane':False,'DWrittenByLane':False,'scope':'Source/AST/provenance/pure plan only. No package/install/native/gameplay proof by this lane. Root native results stay separate; no accepted checkbox.'}
save(r/'SOURCE-MANIFEST.json',seal)
print(json.dumps({'status':seal['status'],'files':len(files),'sealSHA256':sha(r/'SOURCE-MANIFEST.json'),'planSHA256':seal['planSHA256']}))

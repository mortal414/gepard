"""C-only final source check/seal. Does not invoke runtime or stage commands."""
from pathlib import Path
import ast,json,hashlib
O=Path(__file__).resolve().parent
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def read(p):return json.loads(Path(p).read_text(encoding='utf-8-sig'))
for p in O.glob('*.py'):ast.parse(p.read_text(encoding='utf-8-sig'))
obs=(O/'source/scripts/Game/GEPArtNativeQA/GEP_ArtNativeQualification20261022.c').read_text()
scene=(O/'source/scripts/Game/GEPArtNativeSceneQA/GEP_ArtNativeSceneQA20261022.c').read_text()
ev=(O/'evidence.py').read_text();rev=(O/'review_run.py').read_text();wrap=(O/'tools/Scene.ps1').read_text();models=read(O/'MARKER-CONTRACT.json')['modelResources']
checks={
 'pythonSyntax':True,
 'eightRegistrySevenNewPlusCurrentStorage':len(models)==8 and 'C6D9B89642E424E3'in models[2] and sum('ArtApproved20261022'in p for p in models)==7,
 'signedNodeGettersRetained':'bone.matrixRead = entity.GetBoneMatrix(bone.index, matrix);'in obs and 'bone.localMatrixRead = entity.GetBoneLocalMatrix(bone.index, localMatrix);'in obs and 'bone.index >= 0'not in obs,
 'threeSkeletonOwnersGuardRetained':'row.modelIndex <= 2 && names.IsEmpty()'in obs,
 'noVehicleStateTuning':all(s not in obs+scene for s in ['SetAiming','SetVelocity','SetHealth','SetAngularVelocity']),
 'fourteenSameCameraViews':all(s in scene for s in ['m_Case / 7','m_Case % 7','m_Case == 14','offset = "28 0 -40"','if (now - m_ViewStartMs < 5000) return;']),
 'eightLoadedIDs':len(read(O/'MARKER-CONTRACT.json')['loadedAddonIds'])==8,
 'collectorExactResourcesAndEF':'exact approved model owner resources differ'in rev and r'\([EF]\)'in ev,
 'wrapperSharingBoundRetained':all(s in wrap for s in ['leaseOpenAttempt -lt 21','0x20,0x21','leaseOpenClock.ElapsedMilliseconds -ge 900']),
 'fixed180Wrapper':'Seconds -ne 180'in wrap,
 'privateQASixFiles':len(read(O/'QA-EXPECTED-MANIFEST.json'))==6,
 'precompile1257':sum(r['files']for r in read(O/'EXPECTED-STAGE-CLOSURES.json'))==1257,
 'wrapperParseOnlyPassed':read(O/'WRAPPER-SYNTAX.json')['status']=='POWERSHELL_PARSE_ONLY_PASS',
}
assert all(checks.values()),checks
(O/'SOURCE-CHECKS.json').write_text(json.dumps({'status':'STATIC_RECOVERY_AND_BINDING_CHECKS_PASS_NATIVE_NOT_TESTED','checks':checks,'nativeStarted':False,'DWritten':False},indent=2)+'\n',encoding='utf-8')
files={p.relative_to(O).as_posix():sha(p)for p in O.rglob('*')if p.is_file()and p.name not in ['SOURCE-MANIFEST.json','READY.json']and '__pycache__'not in p.parts and 'adopted'not in p.relative_to(O).parts}
(O/'SOURCE-MANIFEST.json').write_text(json.dumps({'status':'FROZEN_SOURCE_ONLY_ART4_DEMO10_REBIND_ACTUAL_QA_COMPILE_REQUIRED','files':files},indent=2)+'\n',encoding='utf-8')
print(json.dumps({'sourceSeal':sha(O/'SOURCE-MANIFEST.json'),'files':len(files),'checks':checks},indent=2))

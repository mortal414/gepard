"""Save owned source/automation and public receipts in the isolated branch; no native action."""
from pathlib import Path
import shutil,json,hashlib
P=Path(__file__).resolve().parent;C=P.parent;T=Path(r'D:\ArmaProjects\Gepard-Demo10-TrackArt-20261022');U=T/'updates/demo10'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def copy(src,dst):
 assert dst.resolve().is_relative_to(T.resolve());dst.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(src,dst)
assert not U.exists(),'Keep partial source checkpoint; diagnose before retry'
protected=json.loads((P/'DEMO10-INITIAL-SOURCE.json').read_text())['demo09Protected'];assert all(sha(Path(p))==h for p,h in protected.items())
for role in ['integration','provingGround']:
 root=T/'private-demo-source10/single'/role
 for f in root.rglob('*'):
  if f.is_file() and f.name!='resourceDatabase.rdb':copy(f,U/'source'/role/f.relative_to(root))
launcher=C/'Gepard-Demo10-Launcher-v3-20261022'
for name in ['Launch-Gepard-Private-Demo10.ps1','PackageOnlyScene10.ps1','SOURCE-MANIFEST.json']:copy(launcher/name,U/'launcher'/name)
packages=['Gepard-Demo10-PackTools-v2-20261022','Gepard-Demo10-Launcher-v3-20261022','Gepard-Demo10-Launcher-v3-Checks-20261022','GEP-Demo10-CoreCompatibility-20261022','GEP-Demo10-Smoke-Continuity-v1-20261022','GEP-Demo10-ApprovedArt-QA-v2-20261022','GEP-Demo10-DriveContinuity-20261022']
skip={'recordings','__pycache__','node_modules','adopted','session-plans','continuous'}
allow={'.py','.ps1','.json','.mjs','.md','.ts','.c','.gproj','.ent','.layer','.et','.meta','.txt','.patch'}
for name in packages:
 root=C/name
 for f in root.rglob('*'):
  if not f.is_file() or f.suffix.lower() not in allow or any(v in skip for v in f.relative_to(root).parts) or f.name.lower()=='owner-private.json':continue
  copy(f,U/'qa-source'/name/f.relative_to(root))
for f in P.iterdir():
 if f.is_file() and f.suffix.lower() in allow:copy(f,U/'coordinator'/f.name)
for name in ['Scene.ps1','REPAIR-RECEIPT.json']:
 copy(P/'drive-binding-v2'/name,U/'coordinator/drive-binding-v2'/name)
evidence={
 'core-pair':C/'GEP-Demo10-CoreCompatibility-20261022/paired-native-audit-01',
 'smoke-pair':C/'GEP-Demo10-Smoke-Paired-Audit-20261022',
 'art-static':C/'GEP-Demo10-ApprovedArt-Static-IndependentReview-20261022',
 'audio-bounded':C/'GEP033-BoundedQualification-20261022/paired-audit-01'}
for name,root in evidence.items():
 for f in root.rglob('*'):
  if f.is_file() and f.suffix.lower() in {'.json','.md'}:copy(f,U/'evidence'/name/f.relative_to(root))
copy(P/'PACKAGE-ONLY-PROBE.png',U/'evidence/PACKAGE-ONLY-PROBE.png')
cmd='@echo off\r\n"C:\\Users\\marco\\.cache\\codex-runtimes\\codex-primary-runtime\\dependencies\\native\\powershell\\pwsh.exe" -NoProfile -File "%~dp0updates\\demo10\\launcher\\Launch-Gepard-Private-Demo10.ps1" %*\r\n'
(T/'Launch-Gepard-Demo10.cmd').write_bytes(cmd.encode())
pins={f.relative_to(U).as_posix():sha(f) for f in U.rglob('*') if f.is_file()}
(U/'OWNED-CHECKPOINT-MANIFEST.json').write_text(json.dumps({'status':'OWNED_SOURCE_AND_AUTOMATION_SAVED_FINAL_RUNTIME_LEDGER_PENDING','files':pins,'sourceRoles':['integration','provingGround'],'separatelyLoadedDependencies':['5E389BB9F58B79A6','629B2BA37EFFD577'],'thirdPartyPayloadCopiedIntoCheckpoint':False,'demo09Protected':protected,'nativeLaunched':False,'published':False},indent=2)+'\n')
print(json.dumps({'files':len(pins),'entrypoint':str(T/'Launch-Gepard-Demo10.cmd'),'nativeStarted':False,'thirdPartyPayloadIncluded':False}))

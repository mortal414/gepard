from pathlib import Path
import json,hashlib,difflib
task=Path(__file__).resolve().parent.parent;checks=Path(__file__).resolve().parent
old=task/'Gepard-Demo10-PackTools-v2-20261022';new=task/'Gepard-Demo10-Launcher-v3-20261022'
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def read(p):return json.loads(Path(p).read_text(encoding='utf-8-sig'))
def save(p,d):Path(p).write_text(json.dumps(d,indent=2)+'\n',encoding='utf-8',newline='\n')
assert not new.exists();new.mkdir()
oldseal=read(old/'SOURCE-MANIFEST.json');assert sha(old/'SOURCE-MANIFEST.json')=='9292a10a250c5a892360370de0dab0a3e02432394f62da38304b5b09d03882b0'
for n,h in oldseal['files'].items():assert sha(old/n)==h,n
oldpath=str(task/'Gepard-Demo10-PackTools-20261022'/'PACKAGE10-WORLD-CONTRACT.json');newpath=str(old/'PACKAGE10-WORLD-CONTRACT.json')
scene=(old/'PackageOnlyScene10.ps1').read_text(encoding='utf-8-sig');assert scene.count(oldpath)==1
scene=scene.replace(oldpath,newpath);assert 'Gepard-Demo10-PackTools-20261022' not in scene
(new/'PackageOnlyScene10.ps1').write_text(scene,encoding='utf-8',newline='\n')
launcher=(old/'Launch-Gepard-Private-Demo10.ps1').read_text(encoding='utf-8-sig');assert launcher.count('$original=$PSScriptRoot')==1
launcher=launcher.replace('$original=$PSScriptRoot',"$original='"+str(old)+"'")
(new/'Launch-Gepard-Private-Demo10.ps1').write_text(launcher,encoding='utf-8',newline='\n')
assert scene.replace(newpath,oldpath)==(old/'PackageOnlyScene10.ps1').read_text(encoding='utf-8-sig')
assert launcher.replace("$original='"+str(old)+"'",'$original=$PSScriptRoot')==(old/'Launch-Gepard-Private-Demo10.ps1').read_text(encoding='utf-8-sig')
for n in ['Launch-Gepard-Private-Demo10.ps1','PackageOnlyScene10.ps1']:
 (checks/(n+'.patch')).write_text(''.join(difflib.unified_diff((old/n).read_text(encoding='utf-8-sig').splitlines(keepends=True),(new/n).read_text(encoding='utf-8-sig').splitlines(keepends=True),fromfile=str(old/n),tofile=str(new/n))),encoding='utf-8',newline='\n')
proof=read(old/'INSTALLED-PACKAGE-CLOSURE.json');assert len(proof['files'])==12
for n,h in proof['files'].items():assert sha(Path(proof['root'])/n)==h,n
pins={str(old/n):sha(old/n) for n in ['SOURCE-MANIFEST.json','package_launch.mjs','pack_tools10.mjs','ROOT-PACK-PLAN-10.json','ROOT-COMPILED-SOURCE-CONTRACT.json','PACKAGE10-WORLD-CONTRACT.json','SEPARATE-DEPENDENCIES.json','CORE08-TRIO-REUSE-INPUT.json','SMOKE09-TRIO-REUSE-INPUT.json','NativeTailClassifier10-v2.ps1','NATIVE-TAIL-BASELINE-v2.json','INSTALLED-PACKAGE-CLOSURE.json','PackageOnlyScene10.ps1','Launch-Gepard-Private-Demo10.ps1']}
plan=read(old/'ROOT-PACK-PLAN-10.json');result=Path(plan['runRoot'])/'RESULT.json';pins[str(result)]=sha(result)
for row in proof['packages']:pins[row['proofPath']]=row['proofSHA256'];assert sha(row['proofPath'])==row['proofSHA256']
previous=old/'session-plans'/'probe-4f1df565-7ee0-40cd-8bbb-ece7e85f5e56'/'PACKAGE-PROBE-PLAN.json';prior=read(previous)
assert not Path(prior['runRoot']).exists(),'Prior attempt outcome no longer preexecution/absent; inspect first'
save(checks/'PREEXECUTION-FAILURE-PRESERVED.json',{'status':'ROOT_REPORTED_EXACT_CONTRACT_PATH_REFUSAL_BEFORE_NATIVE','failureTarget':str(old/'PackageOnlyScene10.ps1')+':106','reason':'Old fixed v1 worldContract path differs from actual v2 planner path','failedPlan':{'path':str(previous),'sha256':sha(previous),'runRoot':prior['runRoot'],'runRootExistsAtReview':False},'oldWorldContractSHA256':sha(oldpath),'v2WorldContractSHA256':sha(newpath),'nativeNotStartedReportedByParent':True,'noPriorOutcomeRewrite':True})
readme=f'''Launcher-only Demo10 successor. Existing installed10b twelve files, complete four-step packaging and all sealed v2 tools remain unchanged. This folder contains only two PowerShell scripts, their source seal and this README. No repack or reinstall is required.

PackageOnlyScene10.ps1 changes exactly one fixed contract path from the v1 tools folder to the actual v2 planner folder. Launch-Gepard-Private-Demo10.ps1 changes exactly its planner-root assignment to that immutable v2 folder, while executing its own sealed Scene from PSScriptRoot. Owner/PID/birth/argv, hold/cancel, parser, strict errors and normal-close guards are text-identical after those two substitutions. Original planner/helpers/package proofs are pinned by SOURCE-MANIFEST.json. The previous refusal and absent native run root are recorded separately at {checks}.

Root-only commands (PS7):

```powershell
$launcher='{new}\\Launch-Gepard-Private-Demo10.ps1'
& $launcher -PlanOnly
& $launcher -Probe
# After root reviews actual package-only startup/render:
& $launcher
```

PlanOnly creates a fresh UUID plan through the exact existing v2 planner and starts no native process. Probe is75seconds with normal exact-owned close; default is untimed human exit. Root may copy these two scripts plus SOURCE-MANIFEST.json unchanged to the designated D updates/demo10/launcher folder; the original v2 planner path remains fixed. This lane performs no D writes/native launch, and package/art/gameplay acceptance remains separate from these source checks.
'''
(new/'README.md').write_text(readme,encoding='utf-8',newline='\n')
seal={'schema':1,'status':'FROZEN_LAUNCHER_ONLY_DEMO10_V3_EXACT_WORLD_CONTRACT_PATH_FIX','files':{n:sha(new/n) for n in ['Launch-Gepard-Private-Demo10.ps1','PackageOnlyScene10.ps1','README.md']},'originalPins':pins,'unchangedInstalledClosure':{'path':str(old/'INSTALLED-PACKAGE-CLOSURE.json'),'sha256':sha(old/'INSTALLED-PACKAGE-CLOSURE.json')},'noRepack':True,'noReinstall':True,'nativeStartedByLane':False,'DWrittenByLane':False}
save(new/'SOURCE-MANIFEST.json',seal)
save(checks/'SOURCE-INVERSE-CHECKS.json',{'status':'PASS_TWO_EXACT_SUBSTITUTIONS_ONLY','sceneSingleWorldContractPathSubstitution':True,'launcherSinglePlannerRootSubstitution':True,'otherStaleV1FixedScenePaths':0,'originalV2SealAndAllFilesVerified':True,'installedTwelveFilesExact':True,'allOriginalProofPinsVerified':True,'newLauncherSealSHA256':sha(new/'SOURCE-MANIFEST.json'),'files':seal['files'],'nativeStarted':False,'DWritten':False})
print(json.dumps({'folder':str(new),'sealSHA256':sha(new/'SOURCE-MANIFEST.json'),'files':seal['files']}))

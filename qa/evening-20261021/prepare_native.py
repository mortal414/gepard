"""Prepare a new private 1.0.2 QA fixture. Never launches native software."""
from pathlib import Path
import argparse, hashlib, json, shutil, subprocess

def sha(p):
    return hashlib.sha256(p.read_bytes()).hexdigest()

def manifest(root):
    return {p.relative_to(root).as_posix(): sha(p) for p in sorted(root.rglob('*')) if p.is_file()}

def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--repo', type=Path, required=True)
    ap.add_argument('--output', type=Path, required=True)
    ap.add_argument('--prior-qa', type=Path, required=True)
    ap.add_argument('--gm-tools', type=Path, required=True)
    ap.add_argument('--harness', type=Path, required=True)
    ap.add_argument('--python', type=Path, required=True)
    ap.add_argument('--refresh-manifest', action='store_true')
    ap.add_argument('--include-smoke', action='store_true', help='Engineering diagnostic only: include blocked experimental smoke source and observers')
    ap.add_argument('--adopt-compile', type=Path, help='Successful exact-stage compile run directory, required to authorize the prepared adapter')
    a = ap.parse_args()
    root = a.output.resolve()
    addon = root/'native-stage/gepard'
    if a.refresh_manifest:
        assert (addon/'Gepard.gproj').is_file()
        pin = root/'checks/SOURCE_MANIFEST.json'
        pin.write_text(json.dumps(manifest(addon), indent=2)+'\n')
        config_path=root/'harness/config683.json'
        config=json.loads(config_path.read_text())
        config.update(manifestSha256=sha(pin),fileCount=len(manifest(addon)))
        if a.adopt_compile:
            proof=json.loads((a.adopt_compile/'RESULT.json').read_text(encoding='utf-8-sig'))
            owner=json.loads((a.adopt_compile/'owner-private.json').read_text(encoding='utf-8-sig'))
            assert proof['status']=='SCRIPT_COMPILE_PASS' and proof['leaseReleased'] and proof['nativeAbsent']
            assert Path(owner['project']).resolve()==(addon/'Gepard.gproj').resolve(), 'Compile project differs'
            config['fixtureReady']=True
            (root/'checks/ADOPTION.json').write_text(json.dumps({'compileRun':str(a.adopt_compile),'resultSHA256':sha(a.adopt_compile/'RESULT.json'),'scope':'Native script compilation only; each behavior and ordinary input acceptance remain separate'},indent=2)+'\n')
        config_path.write_text(json.dumps(config,indent=2)+'\n')
        print(json.dumps({'manifest':str(pin),'sha256':sha(pin),'files':config['fileCount'],'nativeLaunched':False}))
        return
    assert not root.exists(), 'Fresh output directory required'
    assert (a.repo/'addon/Gepard.gproj').is_file()
    root.mkdir(parents=True)
    shutil.copytree(a.repo/'addon',addon)
    # Recover the previously compiled original QA fixtures; never alter their archive.
    shutil.copytree(a.prior_qa/'source-fixture',addon,dirs_exist_ok=True)
    scene=addon/'scripts/Game/GEPUpdate101/GEP_Update101Scene.c'
    scene.write_text(scene.read_text().replace('Gepard 1.0.1','Gepard 1.0.2 evening'),encoding='utf-8')
    passive=a.repo/'qa/evening-range-20261021/fixture/GEP_EveningRangeProbe.c'
    if passive.exists():shutil.copy2(passive,addon/'scripts/Game/GEP/GEP_EveningRangeProbe.c')
    if a.include_smoke:
        smoke=a.repo/'qa/evening-smoke-20261021'
        for sub in ['source-candidate','native-overlay','auto-overlay']:shutil.copytree(smoke/sub,addon,dirs_exist_ok=True)
    shutil.copytree(a.prior_qa/'harness',root/'harness')
    shutil.copy2(a.prior_qa/'adapter.py',root/'adapter.py')
    (root/'checks').mkdir()
    pin=root/'checks/SOURCE_MANIFEST.json'
    pin.write_text(json.dumps(manifest(addon),indent=2)+'\n')
    config_path=root/'harness/config683.json'
    config=json.loads(config_path.read_text(encoding='utf-8-sig'))
    config.update(fixtureReady=False,workRoot=str(root),addonRoot=str(addon),manifestPath=str(pin),manifestSha256=sha(pin),fileCount=len(manifest(addon)))
    config_path.write_text(json.dumps(config,indent=2)+'\n')
    # Reuse the qualified single-owner wrappers, rebinding only source discovery.
    (root/'tools').mkdir()
    compiler=(a.gm_tools/'Serial-QACompile102.ps1').read_text(encoding='utf-8-sig')
    compiler=compiler.replace('D:\\ArmaProjects\\Gepard-GM-Hotfix-1.0.2-20261021\\native-stage',str(root/'native-stage'))
    (root/'tools/Compile.ps1').write_text(compiler,encoding='utf-8')
    shutil.copy2(a.gm_tools/'Serial-GMScene102.ps1',root/'tools/Scene.ps1')
    job={
      'schema':1,'template':'startup','project':str(addon/'Gepard.gproj'),'world':'Worlds/GEP_GunRange.ent',
      'sourceRoots':[str(addon)],'addonRoots':[str(root/'native-stage'),'D:/SteamLibrary/steamapps/common/Arma Reforger/addons'],
      'requiredFiles':[{'path':str(p)} for p in sorted((root/'harness').rglob('*')) if p.is_file()]+[{'path':str(pin)}],
      'requiredClasses':['GEP_DamageScenario683','GEP_ManualSpawnLogic'],
      'capabilities':['isolated-reset','owned-lifecycle','native-load'],
      'adapter':{'executable':str(a.python),'script':str(root/'adapter.py'),'args':['{script}','{request}','{result}','{cancel}']},
      'cases':[], 'timeoutSeconds':180,'closeGraceSeconds':40,
      'notes':'Fresh 1.0.2 candidate. Healthy calibration only. Ordinary physical input/HUD/visual acceptance remains separate.'}
    assertions=['fixture.ready','world.loaded','input.positive-control','post-hit.traverse','post-hit.fire','crew.exit','crew.reentry','post-reentry.fire']
    for vehicle in ['gepard','gepard-charcoal']:
        job['cases'].append({'id':'healthy-'+vehicle,'parameters':{
          'runner':str(root/'harness/Damage.ps1'),'nativeCase':'baseline-egress','vehicle':vehicle,
          'powerShell':'C:/Users/marco/.cache/codex-runtimes/codex-primary-runtime/dependencies/native/powershell/pwsh.exe','healthyControl':True},'assertions':assertions})
    (root/'healthy-job.json').write_text(json.dumps(job,indent=2)+'\n')
    identity={'captureHeadCommit':subprocess.check_output(['git','-C',str(a.repo),'rev-parse','HEAD'],text=True).strip(),
      'workingTreeAtCapture':subprocess.check_output(['git','-C',str(a.repo),'status','--porcelain'],text=True),
      'baseline':'699f245afaf3e912b9ce9827b0f3f5f7dd32f3e7','harnessCommit':subprocess.check_output(['git','-C',str(a.harness),'rev-parse','HEAD'],text=True).strip(),
      'production':manifest(a.repo/'addon'),'nativeSourceManifestSha256':sha(pin),'nativeLaunched':False,
      'priorFixture':str(a.prior_qa/'source-fixture'),'priorFixtureHashes':manifest(a.prior_qa/'source-fixture')}
    (root/'checks/PREPARATION.json').write_text(json.dumps(identity,indent=2)+'\n')
    print(json.dumps({'prepared':str(root),'files':config['fileCount'],'nativeLaunched':False,'healthyJob':str(root/'healthy-job.json')}))

if __name__=='__main__':main()

"""Build immutable source-only follow-up stages and explicit gated native plans."""
from pathlib import Path
import argparse,hashlib,json,shutil,subprocess,sys

def write(path,text):path.parent.mkdir(parents=True,exist_ok=True);path.write_text(text,encoding='utf-8',newline='\n')
def digest(path):return hashlib.sha256(path.read_bytes()).hexdigest()
def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--repo',type=Path,required=True);ap.add_argument('--output',type=Path,required=True)
    ap.add_argument('--prior-qa',type=Path,required=True);ap.add_argument('--gm-tools',type=Path,required=True)
    ap.add_argument('--harness',type=Path,required=True);ap.add_argument('--python',type=Path,required=True)
    ap.add_argument('--overlay',type=Path,action='append',default=[])
    ap.add_argument('--plugin-launcher',type=Path,required=True)
    ap.add_argument('--world',default='Worlds/GEP015_Auto.ent')
    a=ap.parse_args();out=a.output.resolve();repo=a.repo.resolve()
    assert not out.exists(),'Fresh source stage required'
    subprocess.run([str(a.python),str(repo/'qa/evening-20261021/prepare_native.py'),'--repo',str(repo),'--output',str(out),'--prior-qa',str(a.prior_qa),'--gm-tools',str(a.gm_tools),'--harness',str(a.harness),'--python',str(a.python),'--include-smoke'],check=True)
    addon=out/'native-stage/gepard';overlay_pins=[]
    allowed={'.c','.meta','.et','.ent','.layer','.conf'}
    for overlay in a.overlay:
        assert overlay.is_dir(),str(overlay)
        for source in sorted(overlay.rglob('*')):
            if not source.is_file() or source.suffix.lower() not in allowed:continue
            relative=source.relative_to(overlay)
            assert relative.parts[0].lower() in {'scripts','prefabs','worlds','configs'},str(relative)
            dest=addon/relative;dest.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(source,dest)
            overlay_pins.append({'source':str(source),'relative':relative.as_posix(),'sha256':digest(source)})
    manifest={p.relative_to(addon).as_posix():digest(p) for p in sorted(addon.rglob('*')) if p.is_file()}
    pin=out/'checks/SOURCE_MANIFEST.json';write(pin,json.dumps(manifest,indent=2)+'\n')
    config_path=out/'harness/config683.json';config=json.loads(config_path.read_text(encoding='utf-8-sig'))
    config.update(fixtureReady=False,manifestSha256=digest(pin),fileCount=len(manifest));write(config_path,json.dumps(config,indent=2)+'\n')
    # Each copied native wrapper is deliberately dry-run by default. Dry-run exits
    # before profiles/directories, census, lease access or native executable calls.
    for wrapper in ['Compile.ps1','Scene.ps1']:
        path=out/'tools'/wrapper;text=path.read_text(encoding='utf-8-sig')
        if wrapper=='Compile.ps1':text=text.replace('[string]$RunRoot)','[string]$RunRoot,[switch]$Execute)',1)
        else:text=text.replace(' [switch]$Interactive,',' [switch]$Execute,\n [switch]$Interactive,',1)
        sentinel='if(Test-Path -LiteralPath $RunRoot)'
        assert sentinel in text
        plan="if(!$Execute){@{status='NOT_TESTED';dryRun=$true;nativeStarted=$false;leaseMutated=$false;project=$Project;runRoot=$RunRoot;requiredAdmission='Parent explicit native handback plus fresh atomic census/lease claim'}|ConvertTo-Json -Depth 6;exit 0}\n"
        write(path,text.replace(sentinel,plan+sentinel,1))
    resolver=a.plugin_launcher.read_text(encoding='utf-8-sig')
    resolver=resolver.replace('[ValidateSet("GEP_Update101CookOne","GEP_Update101CookAll")]', '[ValidateSet("GEP033_ResolveStockAudioPlugin")]')
    resolver=resolver.replace(' [switch]$Interactive,',' [switch]$Execute,\n [switch]$Interactive,',1)
    resolver=resolver.replace("if($Plugin -eq 'GEP_Update101CookOne'){$nativeArgs+='-wbSilent'}","$nativeArgs+=@('-wbSilent','-gep033Output',(Join-Path $RunRoot 'GEP033-RESOLVER.json'))")
    resolver=resolver.replace('Gepard serial texture cook coordinator','Gepard serial resource inspection coordinator')
    for old,new in [('starting_gepard_serial_cook','starting_gepard_resource_probe'),('running_gepard_serial_cook','running_gepard_resource_probe'),('released_gepard_serial_cook','released_gepard_resource_probe')]:resolver=resolver.replace(old,new)
    sentinel='if(Test-Path -LiteralPath $RunRoot)';assert sentinel in resolver
    plan="if(!$Execute){@{status='NOT_TESTED';dryRun=$true;nativeStarted=$false;leaseMutated=$false;project=$Project;plugin=$Plugin;manifest=$Manifest;runRoot=$RunRoot;durableOutput=(Join-Path $RunRoot 'GEP033-RESOLVER.json');requiredAdmission='Parent explicit native handback plus fresh atomic census/lease claim'}|ConvertTo-Json -Depth 6;exit 0}\n"
    write(out/'tools/ResolveAudio.ps1',resolver.replace(sentinel,plan+sentinel,1))
    job={'schema':1,'status':'SOURCE_PREPARED_NATIVE_NOT_TESTED','earliestManualCandidatePreserved':'cc5d0989376ed1d20ada109ce7c1ada5d96aac7e','nativeOwner':'Isfahan until explicit parent handback','project':str(addon/'Gepard.gproj'),'world':a.world,'manifest':str(pin),'manifestSHA256':digest(pin),'files':len(manifest),'overlays':overlay_pins,'nativeLaunched':False,'leaseTouched':False,'nextSteps':['Source parser checks/dry runs only while Isfahan owns slot','After explicit handback: fresh census then exact source compile via Compile.ps1 -Execute','Adopt exact successful compile/refresh generated-resource manifest before native scene','Run bounded stock-control/legacy/enabled/upward cases separately, inspect early telemetry and pixels','Run read-only audio resolver via ResolveAudio.ps1 -Execute; success is resource observation only','No audible implementation or gameplay acceptance from compilation/resolution alone'],'wrappersRequireExecute':True,'compileRunMustBeFresh':True}
    write(out/'SOURCE-PREPARED.json',json.dumps(job,indent=2)+'\n')
    print(json.dumps({'status':job['status'],'output':str(out),'files':len(manifest),'nativeLaunched':False,'leaseTouched':False}))

if __name__=='__main__':main()

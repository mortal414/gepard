"""Source-only checks for the uncompiled GEP033 resolver/authoring continuation."""
from pathlib import Path
import hashlib,json,math,re,runpy
QA=Path(__file__).resolve().parent
PROBE=QA/'signal-probe'
plugin=(PROBE/'scripts/WorkbenchGame/GEP033/GEP033_ResolveStockAudioPlugin.c').read_text()
lifecycle=(PROBE/'scripts/Game/GEP033/GEP_TrackLoopLifecycleProbe102.c').read_text()
signals=(PROBE/'scripts/Game/GEP033/GEP_TrackMovementSignals102.c').read_text()
template=(QA/'authoring/GEP_TrackMovement.acp.in').read_text()
for must in ('GEP033_ResolveStockAudioPlugin','-gep033Output','FileIO.FileExists(output)','m_Report.SaveToFile(output)','GEP033_PROBE_END observations=9 saved=1','override void RunCommandline()'):
    assert must in plugin,must
for denied in ('Workbench.Exit(', 'Workbench.OpenModule(', 'Workbench.OpenResource(', 'RunProcess(', 'RegisterResourceFile(', 'RebuildResourceFile(', 'SoundEvent(', '.Save()'):
    assert denied not in plugin,denied
assert len(re.findall(r'  Resolve\("',plugin))==9
assert 'm_bEnableCandidate 0' in (PROBE/'Prefabs/Vehicles/GEP033_LifecycleProbe102.et').read_text()
assert '[Attribute("0", UIWidgets.CheckBox' in lifecycle
assert lifecycle.count('sound.SoundEventOffset(')==1
assert 'if (m_StartAttempts > 0)' in lifecycle and 'm_StartAttempts = 1;' in lifecycle
assert 'm_Handle = AudioHandle.Invalid;' in lifecycle
assert 'AudioSystem.TerminateSoundFadeOut(owned, true, 0.22)' in lifecycle
assert 'StopOwnedLoop(false, "deleted")' in lifecycle
assert 'HasBoundTrackSignals()' in signals and 'GEPTrackVolumeDb' in signals and 'Math.Log10' in signals
def db(gain): return -60 if gain<=.001 else max(-60,min(0,20*math.log10(gain)))
assert db(0)==-60 and db(.001)==-60
assert abs(db(.45)-(-6.935749724493126))<1e-12
assert db(.001)<db(.01)<db(.1)<db(.45)
assert not list(PROBE.rglob('*.acp')) and not list(PROBE.rglob('*.sig'))
tokens=set(re.findall(r'@@([A-Z_]+)@@',template))
assert tokens=={'ORIGINAL_TRACK_SOUND_CONFIG_CANONICAL','NATIVE_VEH_TIRES_INPUT_PORT','STOCK_SLOW_CANONICAL_FROM_RESOLVER'}
for keyword in ('AudioClass {','SoundClass :','MixerClass {','ShaderClass {','BankLocalClass {','AudioBankSampleClass Sample','Volume -60','"Infinite loop" 1'):
    assert keyword in template,keyword
assert template.count('id 64')==2 and 'id 3100 port 65' in template and 'id 3200 port 65' in template
assert not (QA/'authoring/GEP_TrackMovement.acp.in.meta').exists()
parser=runpy.run_path(str(QA/'parse-resolver-log.py'))
parser_result=parser['selftest']()
paths=[p for p in PROBE.rglob('*') if p.is_file()]
guids=[]
for path in paths:
    if path.suffix=='.meta':
        guids.extend(re.findall(r'Name "\{([A-F0-9]{16})\}',path.read_text()))
assert len(guids)==len(set(guids))
result={'status':'PASS_SOURCE_AND_SYNTHETIC_ONLY','nativeCompile':'NOT_TESTED','nativeResolverRun':'NOT_TESTED','audibleImplementation':False,'graph':'NONLOADABLE_ACP_IN_TEMPLATE_ONLY','resolverObservationsPrepared':9,'playbackProbe':'DEFAULT_DISABLED_NOT_INSTANTIATED','parserSelftest':parser_result,'linearToDbExamples':{'idle':db(0),'gain001':db(.01),'gain01':db(.1),'gain045':db(.45)},'sourceHashes':{p.relative_to(QA).as_posix():hashlib.sha256(p.read_bytes()).hexdigest() for p in sorted(paths)}}
(QA/'AUTHORING-SOURCE-RESULT.json').write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8',newline='\n')
# Flat script-module pins can be used by the parent's admitted QA copy/launcher.
module=PROBE/'scripts/WorkbenchGame/GEP033'
pins={p.relative_to(PROBE).as_posix():hashlib.sha256(p.read_bytes()).hexdigest() for p in sorted(module.rglob('*')) if p.is_file()}
(QA/'RESOLVER-SOURCE-MANIFEST.json').write_text(json.dumps(pins,indent=2)+'\n',encoding='utf-8',newline='\n')
print(json.dumps({k:result[k] for k in ('status','nativeCompile','nativeResolverRun','graph','resolverObservationsPrepared','playbackProbe')},indent=2))

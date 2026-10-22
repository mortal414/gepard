from pathlib import Path
import json,hashlib,shutil,datetime
P=Path(__file__).resolve().parent;C=P.parent;T=Path(r'D:\ArmaProjects\Gepard-Demo10-TrackArt-20261022');U=T/'updates/demo10';R=Path(r'D:\ArmaProjects\Gepard-Consolidated-20261022')
def load(p):return json.loads(p.read_text(encoding='utf-8-sig'))
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def save(p,d):p.parent.mkdir(parents=True,exist_ok=True);p.write_text(json.dumps(d,indent=2)+'\n')
def copy(src,dst):dst.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(src,dst)
assert U.exists() and not (T/'DEMO10-MANIFEST.json').exists()
protected=load(P/'DEMO10-INITIAL-SOURCE.json')['demo09Protected'];assert all(sha(Path(k))==v for k,v in protected.items())
for row in load(P/'adopted/ACTUAL-COMPILE-ADOPTION.json')['closures']:
 root=Path(row['root']);assert {p.relative_to(root).as_posix():sha(p) for p in root.rglob('*') if p.is_file()}==load(Path(row['manifest']))
pack=T/'native-runs/private-pack-10b/RESULT.json';pk=load(pack);assert pk['operationalVerdict']=='PASS' and pk['nativeAbsent'] and pk['leaseReleased'] and all(s['exitCode']==0 for s in pk['steps'])
probe=T/'native-runs/private-demo-10b-sessions/probe-56a353ee-012f-4742-b203-7ab9b03c1294';pr=load(probe/'RESULT.json');assert all(pr[k] is True for k in ['functionalStartupObserved','ordinaryReady','nativeAbsent','leaseReleased','fourComparisonBays','actualLayoutHashes']) and pr['privateQA'] is False and pr['worldStartup']['pass'] and pr['exitCode']==0
drives={}
for skin in ['camo','charcoal']:
 f=P/f'drive-binding-v2/{skin.upper()}-REVIEW.json';d=load(f);required=['telemetry.join','clock.native','reverse.powered','reverse.neutral-handbrake','stop.engine-off-dwell','camera.native-view','crew.exit-reentry','sequence.terminal','tail.stationary'];assert all(d['assertions'][k]['status']=='PASS' for k in required)
 run=T/f'native-runs/demo10-drive-{skin}-02';e=load(run/'exit.json');assert e['exitCode']==0 and e['verifiedExited'] and e['leaseReleased']
 ledger=load(P/f'drive-binding-v2/{skin}/continuous/LEDGER.json');assert ledger['status']=='NATIVE_RETURNED_AUDIO_FINALIZED'
 drives[skin]={'review':str(f),'reviewSHA256':sha(f),'nativeRun':str(run),'exitSHA256':sha(run/'exit.json'),'metrics':d['metrics'],'strictNativeVerdict':d['status'],'rawErrorCount':len(d['nativeErrorRows']),'recording':ledger['wav']}
 copy(f,U/f'evidence/drive/{skin}-review.json');copy(run/'exit.json',U/f'evidence/drive/{skin}-exit.json');copy(P/f'drive-binding-v2/{skin}/continuous/forward_default.png',U/f'evidence/drive/{skin}-forward.png')
 copy(P/f'drive-binding-v2/{skin}/command.json',U/f'coordinator/drive-binding-v2/{skin}/command.json');copy(P/f'drive-binding-v2/{skin}/job.json',U/f'coordinator/drive-binding-v2/{skin}/job.json')
for name in ['RESULT.json','exit.json','STARTUP-CHECKPOINT.json']:copy(probe/name,U/'evidence/package-only'/name)
driveAudit=C/'GEP-Demo10-DriveContinuity-20261022/paired-native-audit-02'
for f in driveAudit.iterdir():
 if f.is_file() and f.suffix.lower() in {'.json','.md'}:copy(f,U/'evidence/drive-independent'/f.name)
copy(pack,U/'evidence/package-build/RESULT.json')
copy(C/'Gepard-Demo10-PackTools-v2-20261022/INSTALLED-PACKAGE-CLOSURE.json',U/'evidence/package-build/INSTALLED-PACKAGE-CLOSURE.json')
copy(P/'ART-NATIVE-REBUILD-RESULT.json',U/'evidence/ART-NATIVE-REBUILD-RESULT.json')
copy(P/'PACKAGE-PROBE-PREFLIGHT-FAILURE.json',U/'evidence/PACKAGE-PROBE-PREFLIGHT-FAILURE.json')
guide='''# Private Demo10 playtest

Double-click **Launch-Gepard-Demo10.cmd** in this folder. This opens one untimed local session; exit normally when finished. SpaceCore8.4.0 and WCS Armaments8.4.0 are loaded separately from the pinned local cache. Keep the staged folders in place. Nothing is published.

Use **keep / change / skipped**, with the vehicle label and a screenshot or continuous sound recording for issues.

1. **Comparison bay:** OLD keeps the original exterior/rolling sound with current shared scripts and optic. NEW has the approved cumulative rear-housing, gun-cover, lower-cheek, barrel and sprocket refinements, plus tracks and smoke. Compare both skins close up and farther away. Check full turret/gun arcs, seams, hatches, moving clearance and visible LOD changes; those full judgments remain open. V16/V19 and conditional roof proposals are excluded.
2. **Drive loop:** use the NEW route vehicle. Drive forward/reverse, vary speed, brake, then switch the engine off with R. Judge motion-linked clatter/rumble, stopping tails and engine/cannon balance. Exit/re-enter; after everyone exits, use the explicit station reset and listen for lingering sound. The5/30/100m marks support a second observer/driver; solo cabin/third-person checks do not prove100m attenuation. Stock cannon assets and accepted driving settings are unchanged.
3. **Optic/target lane:** judge the approved larger reticle, German panels and vehicle diagram at your usual zooms. Repeated firing/seat-transition behavior was checked automatically. Positive range accuracy and broader lighting/aspect judgment remain open. No new stabilization, ballistic correction or working radar is included.
4. **Smoke/station07:** from a NEW gunner/commander seat use WCS DeploySmoke (installed default Left Shift+G). Try the eight-round salvo, then empty. Judge useful screening: prior visual evidence showed two lateral groups with a central gap. Exit and use the ordinary bank rearm hold action beside a maintenance point. One restored tube costs one supply from the **shared eight-supply crate**. Re-fire a restored tube, check full/empty feedback and reset the service fixture only while clear. Full shared-world accounting, physical roles/cooldown and rotated-turret port fit remain unaccepted.

The35mm refill/refuel/repair stop and isolated damage area remain available. Setup is not proof of actual fuel/repair transfer; those service limitations remain open. Hostiles stay off. This is one test candidate, not blanket acceptance of GEP tasks.

Rollback: D:\\ArmaProjects\\Gepard-Consolidated-20261022\\Launch-Gepard-NewFeatures.cmd (Demo09). All failed tests and raw diagnostics are preserved. See DEMO10-MANIFEST.json and updates/demo10/evidence for observed checks and remaining gates.
'''
(T/'PLAYTEST-DEMO10.md').write_text(guide,newline='\n')
accepted={1,2,3,4,28,29,30,31,32};titles={5:'HUD',6:'HUD lifecycle',7:'stabilization',8:'range display regression',9:'ballistic elevation/impact correction',10:'familiar controls',11:'armor/crew protection',12:'independent turret damage',13:'surviving crew exit',14:'radar',15:'smoke',16:'interior',17:'exterior',18:'charcoal',19:'custom gun sounds',20:'seats/hatches/periscopes',21:'commander prompts',22:'commander position/view alignment',23:'refuel side/reach',24:'release debug text',25:'MP/JIP/dedicated',26:'console',27:'tracker/publication text',33:'speed-synced track audio'}
states=[]
for n in range(1,34):
 status='Deferred';scope='Out of this bounded successor; no global blocker or new implementation claim.';acceptance='not assessed'
 if n in accepted:status='Ready';scope='Prior accepted task preserved; not reimplemented.';acceptance='accepted prior per trusted handoff'
 elif n in [5,6,8,10,18,20,21,22,23,24]:status='Needs user test';scope='Current implementation preserved; native healthy HUD/crew/fire and drive regressions cover narrow paths only. Broader task acceptance not promoted.'
 elif n==15:status='Needs user test';scope='Included experimental full8 smoke; both final skins8to0/eightmovingpayloads/emptyrequest observed. Cloud/coverage/ports/physical roles/shared8 rearm remain pending.'
 elif n==17:status='Needs user test';scope='Approved cumulative frozen art included as reviewable candidate;8nativebuilds,14static images, native model/material/bone observations and bounded both-skin motion. Full clearance/collision/LOD/user appearance remain pending; art lane continues separately.'
 elif n==27:status='Needs user test';scope='Prior1.0.2 upload/server processing/package GM spawn evidence exists. Public-page visibility remains independently unverified; nothing published here.'
 elif n==33:status='Needs user test';scope='User accepted Candidate02 audible direction. Normal SpaceCore dependency now on NEW+route; reverse/stop/cabin/exterior/exit-reentry observations recorded. Balance/distance/delete-reset/human/MP gates stay open.';acceptance='audible direction accepted only'
 states.append({'id':f'GEP-{n:03d}','title':titles.get(n,'prior accepted item'),'status':status,'acceptance':acceptance,'scope':scope})
save(U/'GEP-STATUS.json',{'wholeTaskCheckboxesChanged':0,'globalBlockedItemsAdded':0,'items':states})
refs={'artImport':P/'ART-NATIVE-REBUILD-RESULT.json','artStatic':C/'GEP-Demo10-ApprovedArt-Static-IndependentReview-20261022/INDEPENDENT-REVIEW.json','corePair':C/'GEP-Demo10-CoreCompatibility-20261022/paired-native-audit-01/PAIRED-RESULT.json','smokePair':C/'GEP-Demo10-Smoke-Paired-Audit-20261022/PAIRED-REPORT.json','drivePair':driveAudit/'PAIRED-EVIDENCE.json','compile':P/'adopted/ACTUAL-COMPILE-ADOPTION.json','pack':pack,'packagedStartup':probe/'RESULT.json','packagedExit':probe/'exit.json'}
evidence={k:{'path':str(v),'sha256':sha(v)} for k,v in refs.items()}
save(U/'EVIDENCE-INDEX.json',{'observed':evidence,'drive':drives,'strictNativeResults':'Original strict FAIL/NOT_TESTED remain. Compile/pack known exact editor-source warnings plus25 EOF diagnostics classified separately; no clean-health claim. Game runs retain59E/F each.','knownRecoveries':['Art import01 compile failure -> fresh correctedcook02','Art static worldlog GUID regex mismatch -> separate exactsource/argv/worldmeta plusall14pixels reconciliation; originalNT preserved','Package10 TXO PAK assumption -> evidence-backedfresh10b; originalfailurepreserved','Launcher v2 path guard -> exact2substitutionlauncheronlyv3, no repack','Drive01 stalealias/map -> two-linewrapperonlyv2; nativecourseunchanged, originalpreexecutionfailureretained'],'rawEvidenceStaysInNativeRuns':True})
files={p.relative_to(U).as_posix():sha(p) for p in U.rglob('*') if p.is_file() and p.name!='FINAL-FILES.json'};save(U/'FINAL-FILES.json',{'files':files,'thirdPartyPayloadIncluded':False})
out={'status':'PRIVATE_DEMO10_PLAYABLE_TEST_CANDIDATE_READY_WITH_EXPLICIT_LIMITS','createdUtc':datetime.datetime.now(datetime.timezone.utc).isoformat(),'entrypoint':{'path':str(T/'Launch-Gepard-Demo10.cmd'),'sha256':sha(T/'Launch-Gepard-Demo10.cmd'),'mode':'Untimed manual; PlanOnly tested through exactCMD; samecopied3filelauncher/sourcehash alreadyactual75sprobe'},'guide':{'path':str(T/'PLAYTEST-DEMO10.md'),'sha256':sha(T/'PLAYTEST-DEMO10.md')},'branch':'sprint/demo10-track-art-20261022','baseCommit':'eacbab07aa7b6bf54f1873d4b78c006a43cd10d6','published':False,'releaseFilesChanged':False,'rollbackDemo09Protected':protected,'installedOwnFiles':12,'ownedAddonCount':4,'separateDependencyPackages':{'SpaceCore':'5E389BB9F58B79A6','WCS_Armaments':'629B2BA37EFFD577'},'dependencyVersionAssumption':'Installed8.4.0 exact pinnedtrios; not a claim about latestonlineversion','BradleyRequired':False,'dependenciesCopiedIntoOwnedPayload':False,'currentCore1152AndSmoke14Exact':True,'included':['Current approvedGermanHUD/functionalscripts','NormalSpaceCore track graph references onNEWbothskins+route','Frozen approvedcumulativeV12/V14/V15R1/V17/V18R1over safeV11; V16/V19excluded','Existing experimentalWCS8roundsmoke+shared8service','OrdinaryOLD/NEW comparison/driving/target/service/damage provingground'],'excluded':['FAILED01tire-sampleadapter','UnapprovedV16/V19/conditionalroof/bakeexperiments','Newstabilization/ballisticcorrection/radarsensing','Customcannonaudio','Publication'], 'observedNative':evidence,'finalBothSkinDrive':drives,'manualJudgments':['Appearance/fullarticulation/clearance/collision/LOD','Audio cabin/cannon/engine balance,nearfar,delete/reset tails','Smokeusefulscreen/portfit/physicalroles/cooldown/sharedrearm','Positive rangeaccuracy/broadlighting/aspects','Actualfuel/repairtransfer anddamage/crewprotection/MP/JIP/console'], 'newWholeGEPCheckboxes':0,'evidenceIndex':{'path':str(U/'EVIDENCE-INDEX.json'),'sha256':sha(U/'EVIDENCE-INDEX.json')},'sourceSnapshot':{'path':str(U/'FINAL-FILES.json'),'sha256':sha(U/'FINAL-FILES.json')},'noOutstandingPermissionDenial':True,'sourceAndPackingFailuresPreserved':True}
save(T/'DEMO10-MANIFEST.json',out)
save(P/'FINAL-OUTCOME.json',{'candidate':str(T/'DEMO10-MANIFEST.json'),'candidateSHA256':sha(T/'DEMO10-MANIFEST.json'),'entrypoint':out['entrypoint'],'readyForUser':True,'releaseReady':False,'published':False,'nativeBatchComplete':True,'finalGitCheckpoint':'PENDING_LOCAL_COMMIT'})
print(json.dumps({'status':out['status'],'entrypoint':out['entrypoint']['path'],'sourceFiles':len(files),'newGEPCheckboxes':0,'published':False}))

"""Independent closed-result/read-only source audit. Writes only this report folder."""
from pathlib import Path
from PIL import Image
import hashlib,json,re,datetime
A=Path(__file__).resolve().parent;R=A.parent
N=Path(r'D:\ArmaProjects\Gepard-Demo10-TrackArt-20261022/native-runs')
pins={}
def load(p):return json.loads(p.read_text(encoding='utf-8-sig'))
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def pin(p):pins[str(p)]=sha(p);return pins[str(p)]
def mapfiles(root):return {p.relative_to(root).as_posix():sha(p) for p in root.rglob('*') if p.is_file()}
ad=load(R/'adopted/COMPILE-ADOPTION.json');pin(R/'adopted/COMPILE-ADOPTION.json')
compile=Path(ad['compileRun']);receipt=load(compile/'RESULT.json')
assert pin(compile/'RESULT.json')==ad['compilerResultSHA256'] and pin(Path(receipt['console']))==ad['compilerConsoleSHA256']
assert receipt['status']=='SCRIPT_COMPILE_PASS' and receipt['exitCode']==0 and receipt['nativeAbsent'] is True and receipt['leaseReleased'] is True
assert load(compile/'owner-private.json')['project']==ad['project'];pin(compile/'owner-private.json')
closures=[]
for c in ad['closures']:
 root=Path(c['root']);mp=Path(c['manifest']);assert pin(mp)==c['sha256'] and mapfiles(root)==load(mp)
 closures.append({'root':str(root),'files':len(load(mp)),'exact':True})
assert [c['files'] for c in closures]==[1152,14,68,3,3,12]
before=load(R/'SEALED-SOURCE-MANIFEST.json');changes=[]
for rel,h in before['files'].items():
 f=R/rel
 if not f.exists() or sha(f)!=h:changes.append(rel)
assert set(changes)<={'native-stage/single/qaCore/resourceDatabase.rdb'},changes
jobpath=R/'adopted/core-native-job.json';job=load(jobpath);pin(jobpath)
for row in job['requiredFiles']:assert pin(Path(row['path']))==row['sha256'],row['path']
expected={'camo':'{2150FC6F1E081188}Prefabs/ArtApproved20261022/Camo/GEP_MobileCarrier.et','charcoal':'{8095474EC9806D0D}Prefabs/ArtApproved20261022/Charcoal/GEP_MobileCarrier.et'}
runrows={}
for skin in ['camo','charcoal']:
 batch=N/f'demo10-compat-{skin}-01';case=batch/f'v11-healthy-v11-{skin}';native=case/'native-run'
 summary=load(batch/'summary.json');normalized=load(case/'result.json');bridge=load(case/'adapter-result.json');rawpath=native/'profile/profile/DamageHarness-result683.json';raw=load(rawpath);m=raw['measurements']
 for p in [batch/'summary.json',case/'result.json',case/'adapter-result.json',rawpath]:pin(p)
 assert summary['jobSha256']==sha(jobpath) and summary['status']==normalized['status']=='FAIL'
 assert summary['automaticRetries']==0 and summary['stopReason'] is None and not summary['incomplete']
 assert normalized['runId']==bridge['runId']==raw['runId'] and normalized['caseId']==bridge['caseId']==f'v11-healthy-v11-{skin}'
 assert normalized['infrastructure']['status']==bridge['infrastructure']['status']=='PASS'
 assert normalized['lifecycle']['status']==bridge['lifecycle']['status']=='PASS'
 exit=load(native/'exit.json');owner=load(native/'owner.json');private=load(native/'owner-private.json');plan=load(native/'plan.json')
 for p in [native/'exit.json',native/'owner.json',native/'owner-private.json',native/'plan.json']:pin(p)
 assert exit['exitCode']==0 and exit['verifiedExited'] is True and exit['leaseReleased'] is True and exit['cancelRequested'] is False
 assert owner['args']==plan['args'] and owner['project']==private['project']==plan['project']==ad['project']
 assert Path(private['workRoot']).resolve()==native.resolve() and owner['leaseId']==private['leaseId']
 assert owner['profile']==str(native/'profile') and plan['manifestSHA256'].lower()==sha(Path(plan['manifest']))
 nativejob=load(case/'native-job.json');pin(case/'native-job.json');assert nativejob['nonce']==raw['nonce'] and nativejob['prefab']==expected[skin]
 assert raw['status']=='PASS' and raw['reason']=='native_yaw_elevation_both_feeds_exit_reentry_fire_completed_QA' and raw['caseId']=='v11-healthy' and raw['vehicleId']==f'v11-{skin}'
 assert raw['healthSettersUsed'] is False and m['subjectPrefab']==expected[skin] and m['stimulus']=='none' and m['issued']==m['damageCallbacks']==0
 assert m['operationStartedQA'] is True and m['crewConsciousAfter688'] is True and m['publicReentryRequestedQA'] is True and m['publicReentryCompletedQA'] is True
 assert m['rightTraverseDegreesQA']>=1 and m['leftTraverseDegreesQA']>=1 and m['upElevationDegreesQA']>=.5 and m['downElevationDegreesQA']<=-.5
 assert min(m[k] for k in ['leftRoundsUsedQA','rightRoundsUsedQA','reentryLeftRoundsUsedQA','reentryRightRoundsUsedQA'])>0
 console=native/'logs/console.log';pin(console);lines=console.read_text(encoding='utf-8-sig',errors='strict').splitlines()
 ready='GEP_DAMAGE683_READY|'+raw['runId']+'|'+raw['nonce'];done='GEP_DAMAGE683_DONE|'+raw['runId']+'|'+raw['nonce'];text='\n'.join(lines)
 assert text.count(ready)==text.count(done)==1 and text.index(ready)<text.index(done)
 subject=[{'line':i+1,'raw':l} for i,l in enumerate(lines) if 'GEP_DAMAGE683_SUBJECT|' in l];assert len(subject)==1 and expected[skin] in subject[0]['raw']
 assert any('QA_INPUT_READY controlled=1 camera=1 ads=1 turretContext=1' in l for l in lines)
 assert any('QA_REENTRY_INPUT ads=1 context=1 occupied=1' in l for l in lines)
 blocks=[i for i,l in enumerate(lines) if 'Loaded addons:' in l];assert len(blocks)==1
 loaded=[]
 for l in lines[blocks[0]+1:]:
  match=re.search(r"ENGINE\s*: gproj: '.*?' guid: '([A-Fa-f0-9]{16})'",l)
  if not match:break
  loaded.append(match[1].upper())
 assert len(loaded)==8 and len(set(loaded))==8 and set(loaded)=={'5614BBCCBB55ED1C','58D0FB3206B6F859','D561AD08932E7B40','629B2BA37EFFD577','7E0150C026101500','A9F84D1626102251','5E51C5D8F04FDB80','5E389BB9F58B79A6'}
 errors=[{'line':i+1,'raw':l} for i,l in enumerate(lines) if re.search(r'\b([A-Z][A-Z0-9_]*)(?:\s*\([^)]*\))*\s*\(([EF])\)',l)]
 errpath=case/'ALL-DOMAIN-NATIVE-ERRORS.json';er=load(errpath);pin(errpath);assert er['rows']==errors and er['omitted']==0 and er['waivers']==[] and len(errors)==59
 assert normalized['assertions']['native.errors']['status']=='FAIL' and normalized['assertions']['native.errors']['actual']==59
 required=['fixture.ready','world.loaded','native.case-result','scripts.compiled','input.positive-control','post-hit.traverse','post-hit.elevation','post-hit.fire','crew.exit','crew.reentry','post-reentry.fire','addons.exact-stack','smoke.not-triggered']
 assert all(normalized['assertions'][k]['status']=='PASS' for k in required)
 assert 'WCS_NATIVE_REARM phase=' not in text and 'WCS_MANUAL_PROGRAM' not in text
 captures=[]
 for p in sorted((native/'profile/profile').glob('*.bmp')):
  with Image.open(p) as im:assert im.size==(1280,720);im.load()
  captures.append({'path':str(p),'sha256':pin(p),'bytes':p.stat().st_size,'dimensions':[1280,720]})
 assert len(captures)==4
 observations=[{'line':i+1,'raw':l} for i,l in enumerate(lines) if any(x in l for x in ['QA_INPUT_READY','QA_POSTHIT_','GEP_CORE_ELEVATION','QA_REENTRY_','GEP_CORE_COMPAT_SIGHTS'])]
 placeholders={k:normalized['assertions'][k] for k in ['art.new-bound','audio.accepted','smoke.accepted','glyphs.reviewed','visual.approved']}
 runrows[skin]={'strictStatus':normalized['status'],'rawNativeBehaviorStatus':raw['status'],'subjectPrefabObserved':m['subjectPrefab'],'subjectConsoleEvidence':subject,'pid':owner['pid'],'leaseId':owner['leaseId'],'createdUtc':owner['creationUtc'],'exit':exit,'loadedAddons':loaded,'measurements':{k:m[k] for k in ['rightTraverseDegreesQA','leftTraverseDegreesQA','upElevationDegreesQA','downElevationDegreesQA','leftRoundsUsedQA','rightRoundsUsedQA','reentryLeftRoundsUsedQA','reentryRightRoundsUsedQA','publicReentryCompletedQA','elapsedGameSeconds']},'nativeErrors':errors,'captures':captures,'actualNativeObservations':observations,'inheritedPlaceholdersPreserved':placeholders}
c=runrows['camo'];h=runrows['charcoal'];assert c['pid']!=h['pid'] and c['leaseId']!=h['leaseId']
assert datetime.datetime.fromisoformat(c['exit']['utc'].replace('Z','+00:00'))<datetime.datetime.fromisoformat(h['createdUtc'].replace('Z','+00:00'))
out={'status':'PAIRED_NATIVE_FUNCTIONAL_PROOF_VERIFIED_ORIGINAL_STRICT_FAIL_PRESERVED','actualSourceClosures':closures,'sourceSealChanges':changes,'runs':runrows,'pins':pins,'annotation':'Inherited art.new-bound/audio.excluded/smoke.excluded text is generic out-of-scope startup wording, not an assessment of this observed subject. Actual exact Demo10 subject binding is separately proven by native job,raw measurements and unique subject marker. This does not promote art render/PBR/LOD, audio listening or smoke launch acceptance. Originals remain unchanged.'}
(A/'PAIRED-RESULT.json').write_text(json.dumps(out,indent=2)+'\n')
print('AUDIT_PASS_SCOPE_ONLY',[(k,r['measurements'],len(r['nativeErrors'])) for k,r in runrows.items()])

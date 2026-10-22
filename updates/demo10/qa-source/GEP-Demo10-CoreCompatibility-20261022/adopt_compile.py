"""Adopt actual fresh QA compile then emit the existing two-case coordinator job."""
from pathlib import Path
import argparse,copy,hashlib,json
R=Path(__file__).resolve().parent;Q=R/'native-stage/single/qaCore';PROJECT=Q/'GEP_SmokeCoreCompatQA.gproj'
def load(p):return json.loads(p.read_text(encoding='utf-8-sig'))
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def encoded(j):return (json.dumps(j,indent=2)+'\n').encode()
def filemap(root):return {p.relative_to(root).as_posix():sha(p) for p in root.rglob('*') if p.is_file()}
p=argparse.ArgumentParser();p.add_argument('--run',type=Path,required=True);p.add_argument('--execute',action='store_true');a=p.parse_args();run=a.run.resolve()
j=load(run/'RESULT.json');assert j['status']=='SCRIPT_COMPILE_PASS' and j['exitCode']==0 and j['nativeAbsent'] is True and j['leaseReleased'] is True
owner=load(run/'owner-private.json');assert Path(owner['project']).resolve()==PROJECT.resolve()
assert 'Script validation successful.' in Path(j['console']).read_text(encoding='utf-8-sig')
before=load(R/'SOURCE-CLOSURES.json');assert load(run/'SOURCE-CLOSURES.json')==before
out=R/'adopted';assert not out.exists(),'Fresh adoption only'
docs={};closures=[];mutations=[]
for c in before:
 root=Path(c['root']);mp=Path(c['manifest']);assert sha(mp)==c['sha256'] and sha(run/'precompile-source-pins'/mp.name)==c['sha256']
 old=load(mp);now=filemap(root);changed=[k for k in old if now.get(k)!=old[k]];added=list(now.keys()-old.keys());removed=list(old.keys()-now.keys());assert not removed
 if root==Q:assert set(changed+added)<= {'resourceDatabase.rdb'},'Only private QA native RDB output may change'
 else:assert not changed and not added,'Final root-owned Demo10 closure changed; preserve and reconcile'
 for rel in changed+added:
  assert 0<(root/rel).stat().st_size<=32*1024**2
  mutations.append({'root':str(root),'path':rel,'sha256':now[rel],'scope':'Only original privateQA native-generated resource database'})
 post=out/'postcompile'/mp.name;data=encoded(now);docs[post]=data
 closures.append({'root':str(root),'manifest':str(post),'sha256':hashlib.sha256(data).hexdigest(),'files':len(now)})
descriptor=out/'postcompile/SOURCE-CLOSURES.json';docs[descriptor]=encoded(closures)
proof={'status':'SCRIPT_COMPILE_PASS_ADOPTED','project':str(PROJECT),'compileRun':str(run),'compilerResultSHA256':sha(run/'RESULT.json'),'compilerConsoleSHA256':sha(Path(j['console'])),'closures':closures,'precompileClosures':before,'explicitDerivedIndexChanges':mutations,'nativeGameplay':'NOT_TESTED','parentFinalClosuresUnchanged':True}
docs[out/'COMPILE-ADOPTION.json']=encoded(proof)
job=load(R/'core-native-job.template.json');jobpath=out/'core-native-job.json'
for case in job['cases']:
 cp=case['parameters'];cp.update(closures=str(descriptor),closurePinSHA256=hashlib.sha256(docs[descriptor]).hexdigest(),completeManifestPins={c['manifest']:c['sha256'] for c in closures},qaManifest=str(out/'postcompile/qaCore.json'),boundJob=str(jobpath),compileAdoption=str(out/'COMPILE-ADOPTION.json'))
required=[]
for pattern in ['*.py','*.ts','package.json','cases/*.json','vehicles/*.json','tools/*.ps1','*.ps1','manifests/*.json','SOURCE-CLOSURES.json','PARENT-ADOPTION-PINS.json']:
 for f in R.glob(pattern):
  row={'path':str(f),'sha256':sha(f)}
  if row not in required:required.append(row)
required += [{'path':str(f),'sha256':hashlib.sha256(data).hexdigest()} for f,data in docs.items()]
job['requiredFiles']=required
job['notes']='Exact final Demo10 art/audio source closure, unchanged reused native healthy mechanism; fresh privateQA compile. Both skins retain all-domain E/F and separate human/manual/MP limits. No smoke dispatcher or production change.'
docs[jobpath]=encoded(job)
if a.execute:
 for f,data in docs.items():f.parent.mkdir(parents=True,exist_ok=True);f.write_bytes(data)
print(json.dumps({'status':'ACTUAL_QA_COMPILE_ADOPTED' if a.execute else 'ACTUAL_QA_COMPILE_ADOPTION_PLAN','job':str(jobpath),'jobSHA256':hashlib.sha256(docs[jobpath]).hexdigest(),'sourceFiles':sum(c['files'] for c in closures),'nativeStarted':False},indent=2))

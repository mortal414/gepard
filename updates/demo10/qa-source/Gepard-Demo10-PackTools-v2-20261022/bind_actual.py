"""C-only normalized view of root's actual immutable compile/source receipts."""
from pathlib import Path
import json,hashlib,re
r=Path(__file__).resolve().parent;t=r.parent;p=t/'Gepard-Demo10-Integration-20261022'
R=Path(r'D:\ArmaProjects\Gepard-Demo10-TrackArt-20261022')
def sha(q):return hashlib.sha256(Path(q).read_bytes()).hexdigest()
def read(q):return json.loads(Path(q).read_text(encoding='utf-8-sig'))
def write(n,v):
 q=r/n;assert not q.exists(),str(q);q.write_text(json.dumps(v,indent=2)+'\n',encoding='utf-8',newline='\n')
source=p/'adopted/ACTUAL-COMPILE-ADOPTION.json';a=read(source)
assert a['status']=='SOURCE_MATCHED_NATIVE_COMPILE_ADOPTED' and sha(a['receipt'])==a['receiptSHA256'] and sha(a['console'])==a['consoleSHA256']
actual=read(a['receipt']);assert actual['status']=='SCRIPT_COMPILE_PASS' and actual['exitCode']==0 and actual['nativeAbsent'] is True and actual['leaseReleased'] is True
assert 'Script validation successful.' in Path(a['console']).read_text(encoding='utf-8-sig')
assert sha(a['compilerIdentity'])==a['compilerIdentitySHA256'];identity=read(a['compilerIdentity'])
assert str(a['actualTarget']) in identity['CommandLine']
rows=a['closures'];assert len(rows)==6 and sorted(x['files'] for x in rows)==[3,3,11,14,68,1152]
for row in rows:
 assert sha(row['manifest'])==row['manifestSHA256']
 root=Path(row['root']);now={f.relative_to(root).as_posix():sha(f) for f in root.rglob('*') if f.is_file()};assert now==read(row['manifest'])
 if row['role'] not in ['integration','provingGround']:assert not row['changed'] and not row['added']
 else:assert set(row['changed']+row['added'])<={'resourceDatabase.rdb'}
by={x['role']:x for x in rows}
normalized={'status':'ROOT_DEMO10_COMPILE_ADMITTED_CURRENT_CLOSURES','project':a['actualTarget'],'resultPath':a['receipt'],'resultSHA256':a['receiptSHA256'],'consolePath':a['console'],'consoleSHA256':a['consoleSHA256'],'sourceClosures':[{'role':x['role'],'root':x['root'],'manifest':x['manifest'],'manifestSHA256':x['manifestSHA256'],'files':x['files']} for x in rows],'originalRootAdoption':str(source),'originalRootAdoptionSHA256':sha(source),'compilerIdentity':a['compilerIdentity'],'compilerIdentitySHA256':a['compilerIdentitySHA256'],'nativeGameplay':'NOT_TESTED','normalization':'Field mapping only; actual source/result/console/compiler receipts read and verified, no native result changed'}
write('ACTUAL-COMPILE-ADMISSION.json',normalized)
review={'status':'ROOT_PRIVATE_DEMO10_SOURCE_ADMITTED','explicitPack10Requested':True,'strictNativeFailuresPreserved':True,'dependenciesWCSAndSpaceCoreConfirmed':True,'noBradley':True,'noPublication':True,'authorizationScope':'Parent directed private Demo10 packaging preparation/contract/plan from actual admitted inputs; root exclusively owns all native execution and final acceptance.','approvedArtScope':['V15R1','V17','V18R1'],'excludedArt':['V16','unreviewed active proposals'],'nativeBuild':a['nativeBuild'],'nativeBuildSHA256':sha(a['nativeBuild']),'currentBothSkinGameplay':'PENDING_ROOT_NATIVE_QA','manualAudioArtAndMPAcceptance':'PENDING','newAcceptedCheckboxes':0,'rootTaskEvidence':'Final compile closed PASS actual0/nativeabsent/released. Root actual adoption P/adopted/ACTUAL-COMPILE-ADOPTION.json verified exact compiler project+argv; final manifests under P/adopted/SourceManifest-{role}.json. Core1152 and smoke14 byte-identical, deps3each exact; only integrationRDB regenerated and worldRDBadded. Integration68/world11. RootAdmission may state authorized privatecandidate, scriptcompile8modelnativebuild observed, gameplaypending and no publication.'}
write('ROOT-10-ADMISSION.json',review)
c=read(r/'ROOT-COMPILED-SOURCE-CONTRACT.template.json');c['status']='ROOT_FINAL_SOURCE_READY_FOR_PRIVATE_PACK'
c['actualCompileReceipt']=str(r/'ACTUAL-COMPILE-ADMISSION.json');c['actualCompileReceiptSHA256']=sha(r/'ACTUAL-COMPILE-ADMISSION.json');c['rootReview']=str(r/'ROOT-10-ADMISSION.json');c['rootReviewSHA256']=sha(r/'ROOT-10-ADMISSION.json')
for field,roles in [('supportingSourceClosures',['gepard','wcsSmoke']),('dependencyClosures',['SpaceCore_5E389BB9F58B79A6','WCS_Armaments_629B2BA37EFFD577'])]:c[field]=[{'role':n,'root':by[n]['root'],'manifest':by[n]['manifest'],'manifestSHA256':by[n]['manifestSHA256']} for n in roles]
integration=read(by['integration']['manifest']);runtime=[n for n in integration if n.lower().endswith(('.et','.xob','.txo','.emat','.edds','.c','.conf','.wav','.acp','.bank','.sdef'))]
assert sum(n.endswith('.et') for n in runtime)==17 and sum(n.endswith('.xob') for n in runtime)==sum(n.endswith('.txo') for n in runtime)==8
for project in c['projects']:
 row=by[project['role']];project['sourceManifest']=row['manifest'];project['sourceManifestSHA256']=row['manifestSHA256']
 if project['role']=='integration':project['requiredPackageResources']=sorted(runtime)
write('ROOT-COMPILED-SOURCE-CONTRACT.json',c)
w=read(r/'PACKAGE10-WORLD-CONTRACT.template.json');binding=read(p/'WORLD-BINDING.json')
assert binding['registry']['objects']==34 and binding['registry']['boards']==24
w.update({'status':'ACTUAL_ROOT_FINAL_WORLD_SOURCE_BOUND_PACKAGE_STARTUP_PENDING','registryHash':binding['registrySHA256'],'runtimeHash':binding['runtimeSHA256'],'sourceManifestSHA256':by['provingGround']['manifestSHA256'],'worldCandidate':str(p/'WORLD-BINDING.json'),'worldCandidateSHA256':sha(p/'WORLD-BINDING.json'),'newWrappers':{n:binding['registry']['bindings'][n] for n in ['NEW-CAMO','NEW-CHARCOAL']}})
write('PACKAGE10-WORLD-CONTRACT.json',w)
write('ACTUAL-ROOT-BINDING.json',{'status':'ACTUAL_CLOSED_COMPILE_AND_FINAL_SOURCE_BOUND_C_ONLY_NO_PACKAGE_EXECUTION','originalRootAdoption':str(source),'originalRootAdoptionSHA256':sha(source),'sourceFiles':sum(x['files'] for x in rows),'sourceCounts':{x['role']:x['files'] for x in rows},'requiredIntegrationResources':len(runtime),'FBXRequired':False,'coreAndSmokeRebuilt':False,'contractSHA256':sha(r/'ROOT-COMPILED-SOURCE-CONTRACT.json'),'worldContractSHA256':sha(r/'PACKAGE10-WORLD-CONTRACT.json'),'gameplayAndPackageStartup':'PENDING','DWritten':False,'nativeStarted':False,'packaged':False})
print(json.dumps(read(r/'ACTUAL-ROOT-BINDING.json'),indent=2))

function supporting(plan){
 const fixed='D:\\ArmaProjects\\Gepard-Demo10-TrackArt-20261022\\private-demo-source10';
 insist(plan.sourceAddonRoot===path.join(fixed,'single')&&JSON.stringify(plan.dependencyAddonRoots)===JSON.stringify([path.join(fixed,'dependencies')]),'Only exact Demo10 owned/supporting and dependency roots');
 insist(plan.supportingSourceClosures.length===2&&plan.dependencyClosures.length===2,'Exact core1152+smoke14 and two separate dependency3 closures required');
 const supportCounts={gepard:1152,wcsSmoke:14},depIds={SpaceCore_5E389BB9F58B79A6:'5E389BB9F58B79A6',WCS_Armaments_629B2BA37EFFD577:'629B2BA37EFFD577'};
 insist(plan.supportingSourceClosures.map(x=>x.role).sort().join(',')==='gepard,wcsSmoke','Exact supporting roles required');
 insist(plan.dependencyClosures.map(x=>x.role).sort().join(',')===Object.keys(depIds).sort().join(','),'Exact dependency roles; no Bradley/other addon');
 insist(fs.readdirSync(plan.sourceAddonRoot).sort().join(',')==='gepard,integration,provingGround,wcsSmoke','Exact four owned/supporting source directories');
 insist(fs.readdirSync(path.join(fixed,'dependencies')).sort().join(',')===Object.keys(depIds).sort().join(','),'Exact two dependency directories only');
 for(const row of [...plan.supportingSourceClosures,...plan.dependencyClosures]){
  insist(sha(row.manifest)===row.manifestSHA256,'Supporting/dependency manifest differs');const pins=read(row.manifest),actual=tree(row.root);
  const count=supportCounts[row.role]??3;
  insist(Object.keys(actual).length===count&&Object.keys(pins).length===count&&Object.keys(pins).every(n=>actual[n]===pins[n]),'Complete immutable supporting/dependency bytes differ');
  if(Object.hasOwn(supportCounts,row.role))insist(row.root===path.join(fixed,'single',row.role),'Exact supporting source role root');
  else insist(row.root===path.join(fixed,'dependencies',row.role)&&guidOf(fs.readFileSync(path.join(row.root,'addon.gproj'),'utf8'))===depIds[row.role],'Exact separate dependency GUID/root');
 }
 const allowed=new Set(['5614BBCCBB55ED1C','58D0FB3206B6F859','D561AD08932E7B40','7E0150C026101500','5E51C5D8F04FDB80','A76A420022000001',...Object.values(depIds)]);
 for(const folder of ['gepard','wcsSmoke','integration','provingGround']){
  const dir=path.join(plan.sourceAddonRoot,folder),gprojects=fs.readdirSync(dir).filter(x=>/\.gproj$/i.test(x));insist(gprojects.length===1,'Exactly one owned source project');
  const text=fs.readFileSync(path.join(dir,gprojects[0]),'utf8'),matches=[...text.matchAll(/\bDependencies\s*\{([^}]+)\}/g)];insist(matches.length===1,'Exactly one dependency graph declaration');
  const ids=[...matches[0][1].matchAll(/"([0-9A-Fa-f]{16})"/g)].map(x=>x[1].toUpperCase());insist(ids.length>0&&ids.every(x=>allowed.has(x)),'Unapproved/Bradley dependency edge');
 }
}
function admission(c){
 insist(sha(c.actualCompileReceipt)===c.actualCompileReceiptSHA256&&sha(c.rootReview)===c.rootReviewSHA256,'Actual compile/root review pins required');
 const review=read(c.rootReview),a=read(c.actualCompileReceipt);
 insist(review.status==='ROOT_PRIVATE_DEMO10_SOURCE_ADMITTED'&&review.explicitPack10Requested===true&&review.strictNativeFailuresPreserved===true&&review.dependenciesWCSAndSpaceCoreConfirmed===true&&review.noBradley===true&&review.noPublication===true,'Explicit root Demo10 review missing');
 insist(a.status==='ROOT_DEMO10_COMPILE_ADMITTED_CURRENT_CLOSURES'&&a.project===c.projects.find(x=>x.role==='provingGround').project,'Actual current world compile adoption required');
 insist(sha(a.resultPath)===a.resultSHA256&&sha(a.consolePath)===a.consoleSHA256,'Actual compiler result/console changed');
 const result=read(a.resultPath),owner=read(path.join(path.dirname(a.resultPath),'owner-private.json'));
 insist(result.status==='SCRIPT_COMPILE_PASS'&&result.exitCode===0&&result.nativeAbsent===true&&result.leaseReleased===true&&owner.project===a.project,'Actual compile not PASS/normal0/absent/released/currentproject');
 insist(result.console===a.consolePath&&fs.readFileSync(a.consolePath,'utf8').includes('Script validation successful.'),'Actual compile success missing');
 const rows=[...c.projects.map(x=>({role:x.role,root:path.dirname(x.project),manifest:x.sourceManifest,manifestSHA256:x.sourceManifestSHA256})),...c.supportingSourceClosures,...c.dependencyClosures];
 insist(a.sourceClosures.length===6&&new Set(a.sourceClosures.map(x=>x.role)).size===6,'Exact six actual compiled source closures required');
 for(const row of rows){const actual=a.sourceClosures.find(x=>x.role===row.role);insist(actual&&actual.root===row.root&&actual.manifest===row.manifest&&actual.manifestSHA256===row.manifestSHA256,'Compiled/current source closure identity differs');}
 insist(Object.keys(c.reusedTrioInputs).sort().join(',')==='gepard,wcsSmoke','Exact two reused own addons required');
 for(const role of ['gepard','wcsSmoke']){
  const input=c.reusedTrioInputs[role];insist(sha(input.path)===input.sha256,'Reuse descriptor changed');const p=read(input.path),proof=read(p.originalSameBuildProof.proofPath);
  insist(p.role===role&&p.status==='EXACT_INSTALLED_TRIO_MATCHES_ORIGINAL_SAME_BUILD_PROOF_NO_COPY'&&Object.keys(p.files).length===3,'Reuse descriptor role/count differs');
  insist(sha(p.originalSameBuildProof.proofPath)===p.originalSameBuildProof.proofSHA256&&proof.status==='SAME_BUILD_TRIO_AND_CONTENTS_VERIFIED'&&proof.role===role&&proof.privateQAExcludedFromProduction===true,'Original reuse same-build/QA exclusion proof differs');
  insist(sha(p.currentSourceManifest)===p.currentSourceManifestSHA256,'Original reuse source manifest changed');
  const support=c.supportingSourceClosures.find(x=>x.role===role);insist(JSON.stringify(Object.entries(read(support.manifest)).sort())===JSON.stringify(Object.entries(read(p.currentSourceManifest)).sort()),'Supporting source differs from reused addon build');
  for(const [n,h]of Object.entries(p.files)){
   insist(sha(path.join(p.root,n))===h,'Previously installed reuse bytes changed');
   const original=Object.values(proof.trio).find(f=>(/\.gproj$/i.test(f.path)?'addon.gproj':path.basename(f.path))===path.basename(n));insist(original&&sha(original.path)===h&&original.sha256===h,'Reuse trio not original same-build bytes');
  }
 }
}

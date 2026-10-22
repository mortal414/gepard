function dependency(){
 const file=path.join(HERE,'SEPARATE-DEPENDENCIES.json'),d=read(file);
 const expectedFolders=['SpaceCore_5E389BB9F58B79A6','WCS_Armaments_629B2BA37EFFD577'];
 insist(d.root===dependencyRoot&&Object.keys(d.files).length===6&&d.closures.length===2,'Exact two separate dependency trios required');
 insist(fs.readdirSync(d.root).sort().join(',')===expectedFolders.sort().join(','),'No Bradley/extra dependency folder');complete(d.root,d.files);
 const ids={SpaceCore_5E389BB9F58B79A6:'5E389BB9F58B79A6',WCS_Armaments_629B2BA37EFFD577:'629B2BA37EFFD577'};
 for(const [folder,id]of Object.entries(ids)){
  insist(guidOf(fs.readFileSync(path.join(d.root,folder,'addon.gproj'),'utf8'))===id,'Dependency GUID differs');
  const row=d.closures.find(x=>x.role===folder);insist(row&&row.root===path.join(d.root,folder)&&sha(row.manifest)===row.manifestSHA256,'Separate dependency source proof differs');
  const pins=read(row.manifest);insist(Object.keys(pins).length===3&&Object.keys(pins).every(n=>d.files[folder+'/'+n]===pins[n]),'Dependency per-trio pins differ');
 }
 return {file,d};
}
function reused(role){
 const name=role==='gepard'?'CORE08-TRIO-REUSE-INPUT.json':'SMOKE09-TRIO-REUSE-INPUT.json';
 const p=read(path.join(HERE,name)),row=p.originalSameBuildProof;
 insist(p.status==='EXACT_INSTALLED_TRIO_MATCHES_ORIGINAL_SAME_BUILD_PROOF_NO_COPY'&&p.role===role&&Object.keys(p.files).length===3,'Exact reuse input status/role differs');
 insist(sha(row.proofPath)===row.proofSHA256&&sha(p.currentSourceManifest)===p.currentSourceManifestSHA256,'Original same-build/source proof changed');
 const proof=read(row.proofPath);insist(proof.status==='SAME_BUILD_TRIO_AND_CONTENTS_VERIFIED'&&proof.role===role&&proof.guid===expected[role]&&proof.privateQAExcludedFromProduction===true,'Original same-build/QA exclusion proof differs');
 if(role==='wcsSmoke')insist(proof.automatedQAExcluded===true,'Original optional09 automated QA exclusion proof absent');
 const contract=read(read(planPath).contract),support=contract.supportingSourceClosures.find(x=>x.role===role);
 insist(support&&JSON.stringify(Object.entries(read(support.manifest)).sort())===JSON.stringify(Object.entries(read(p.currentSourceManifest)).sort()),'Demo10 supporting source is not the reused same-build source');
 const trio=[];for(const [n,h]of Object.entries(p.files)){
  insist(n.startsWith(expected[role]+'/')&&sha(path.join(p.root,n))===h,'Previously installed reuse bytes changed');
  const original=Object.values(proof.trio).find(f=>(/\.gproj$/i.test(f.path)?'addon.gproj':path.basename(f.path))===path.basename(n));
  insist(original&&original.sha256===h&&sha(original.path)===h,'Reuse bytes differ from original same-build trio');
  trio.push({path:path.join(p.root,n),bytes:fs.statSync(path.join(p.root,n)).size,sha256:h});
 }
 return {role,guid:expected[role],reused:true,proofPath:row.proofPath,proofSHA256:row.proofSHA256,trio};
}

from pathlib import Path
import json,hashlib,shutil,re
P=Path(__file__).resolve().parent;T=Path(r'D:\ArmaProjects\Gepard-Demo10-TrackArt-20261022');S=T/'private-demo-source10/single';A=Path(r'D:\ArmaProjects\Gepard-ArtApproved-20261022');D=S/'integration'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def inv(p):return {x.relative_to(p).as_posix():sha(x) for x in p.rglob('*') if x.is_file()}
def save(p,d):p.parent.mkdir(parents=True,exist_ok=True);p.write_text(json.dumps(d,indent=2)+'\n')
m=json.loads((A/'APPROVED_ART_HANDOFF.json').read_text());assert sha(A/'APPROVED_ART_HANDOFF.json')=='e846d6aed7dba2277d5bab9da5ed715815934f3ed98e592156a7bdcd855a3ae3'
assert m['visual_approval']==['V15R1','V17','V18R1']
before={r:inv(S/r) for r in ['gepard','wcsSmoke','integration']};new=[]
for rel,record in m['files'].items():
 src=A/'overlay'/rel;_,inside=rel.split('/',1);dst=D/inside
 assert sha(src)==record['sha256'],rel
 assert not dst.exists(),('Additive destination occupied',inside)
 dst.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(src,dst);new.append(inside)
parents={}
for skin,guid in [('Camo','A76A420022100002'),('Charcoal','A76A420022100004')]:
 p=D/f'Prefabs/ArtApproved20261022/{skin}/GEP_MobileCarrier.et';text=p.read_text();old='{7E0150C02610300'+('2' if skin=='Camo' else '4')+'}Prefabs/ArtV11Smoke/'+skin+'/GEP_MobileCarrierV11Smoke.et';newParent='{'+guid+'}Prefabs/Demo10/'+skin+'/GEP_MobileCarrierDemo10.et'
 assert text.count(old)==1;text=text.replace(old,newParent);p.write_text(text,newline='\n');parents[skin]={'before':old,'after':newParent,'sha256':sha(p)}
assert inv(S/'gepard')==before['gepard'];assert inv(S/'wcsSmoke')==before['wcsSmoke']
after=inv(D);assert all(after[k]==v for k,v in before['integration'].items())
assert len(after)==len(before['integration'])+49
models=[]
for rel in new:
 if not rel.endswith('.fbx'):continue
 stem=D/rel;target=stem.with_suffix('.xob');assert target.exists()
 models.append({'new_fbx':rel,'new_model':re.search(r'Name "([^"]+)"',Path(str(target)+'.meta').read_text())[1],'seed_xob_sha256':sha(target),'fbx_sha256':sha(stem)})
assert len(models)==8
save(P/'ART-INTEGRATION-RESULT.json',{'status':'SOURCE_INTEGRATED_NATIVE_REBUILD_PENDING','sourceIntegrated':True,'manifest':str(A/'APPROVED_ART_HANDOFF.json'),'manifestSHA256':sha(A/'APPROVED_ART_HANDOFF.json'),'newFiles':new,'parentRebases':parents,'models':models,'core1152AndSmoke14Unchanged':True,'baselineAudioWrapperUnchanged':True,'visualScope':m['visual_approval'],'inheritedBase':m['inherited_art_base'],'exclusions':m['exclusions'],'native':'NOT_TESTED','source':str(D)})
save(P/'ART-IMPORT-EXPECTATIONS.json',{'models':models})
print(json.dumps({'newFiles':len(new),'models':len(models),'currentCodeUnchanged':True,'native':'NOT_TESTED'}))

"""Prepare source helpers only; staged lock refresh waits for coordinator READY."""
from pathlib import Path
import hashlib,json
ROOT=Path(__file__).resolve().parent;OLD=ROOT.parent/'GEP-ProvingGround-QA-20261022'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
assert sha(OLD/'DELIVERABLE-MANIFEST.json')=='05217d8f6751c1184bab09573a27349e67bdb1dd3ef89e595526ed29e6916d90'
for r in json.loads((OLD/'DELIVERABLE-MANIFEST.json').read_text())['files']:assert sha(OLD/r['relative'])==r['sha256']
def write(name,text):
 with (ROOT/name).open('x',encoding='utf-8',newline='\n') as f:f.write(text)
for name in ['evidence.py','plan_job.py']:write(name,(OLD/name).read_text())
s=(OLD/'prepare.py').read_text()
s=s.replace('assert len(pins)==1112','assert len(pins)==1147')
anchor="# Exact original pins, independently checked against the integrated source trees."
s=s.replace(anchor,"""# The earlier sealed GPG bridge and its original1112-file runtime are immutable.
PREVIOUS=ROOT.parent/'GEP-ProvingGround-QA-20261022'
assert sha(PREVIOUS/'DELIVERABLE-MANIFEST.json')=='05217d8f6751c1184bab09573a27349e67bdb1dd3ef89e595526ed29e6916d90'
for prior in load(PREVIOUS/'DELIVERABLE-MANIFEST.json')['files']:assert sha(PREVIOUS/prior['relative'])==prior['sha256']
"""+anchor)
anchor="assert all(sha(GEP/p)==h for p,h in pins.items())"
s=s.replace(anchor,anchor+"""
core=load(PREVIOUS/'GEPARD_SOURCE_MANIFEST.json');assert len(core)==1112
assert all(pins.get(p)==h for p,h in core.items()),'Original core changed'
ART=Path(r'D:\\ArmaProjects\\Gepard-ArtFunctional-V11-20261022')
overlay=load(ART/'COMPARISON_OVERLAY_MANIFEST.json');added=set(pins)-set(core)
assert len(added)==35 and added==set(overlay['files']),'Unexpected additive file set'
assert sha(ART/'COMPARISON_OVERLAY_MANIFEST.json')=='79d98415d1963782bff0d20e67226679c1dce69725cf579da80129443521e5ce'
assert all(pins[p]==overlay['files'][p]['sha256'] for p in added),'Art overlay bytes differ'
assert len(overlay['models'])==5 and all(m['seed_is_baseline_not_v11'] and m['must_rebuild_before_binding_NEW'] for m in overlay['models'])
assert all(sha(GEP/m['new_model'].split('}',1)[1])==m['seed_xob_sha256'] for m in overlay['models']),'Seed xob unexpectedly rebuilt or changed'
assert all(sha(STAGE/'addon'/p)==h for p,h in pins.items()),'Root addon and staged source differ'
js(ROOT/'ART-V11-SOURCE-RECEIPT.json',{'status':'ADDITIVE_SOURCE_PRESENT_NEW_PADS_UNBOUND_NATIVE_NOT_TESTED','previousManifestSHA256':sha(PREVIOUS/'DELIVERABLE-MANIFEST.json'),'originalCoreUnchanged':1112,'addedOverlayFiles':35,'overlayManifestSHA256':sha(ART/'COMPARISON_OVERLAY_MANIFEST.json'),'functionalHandoffSHA256':sha(ART/'FUNCTIONAL_HANDOFF.json'),'seedModels':5,'seedState':'BASELINE_REGISTRATION_SEEDS_NOT_NATIVE_V11_GEOMETRY','newPads':'UNBOUND','nativeCompile':'NOT_TESTED','nativeImport':'NOT_TESTED','visualAcceptance':'NOT_TESTED','dWrites':False})
""")
anchor="required += [WORLD/'checks/variant-lock.json'"
s=s.replace(anchor,"required += [ROOT/'ART-V11-SOURCE-RECEIPT.json',ART/'COMPARISON_OVERLAY_MANIFEST.json',ART/'FUNCTIONAL_HANDOFF.json',PREVIOUS/'DELIVERABLE-MANIFEST.json',WORLD/'checks/variant-lock.json'")
# The existing indexed executable paths remain last2 in required[].
write('prepare.py',s)
s=(OLD/'selftest.py').read_text().replace('closure1122','closure1157').replace("==1122)","==1157)")
anchor="for f in ['adapter.py','evidence.py','prepare.py','selftest.py']:"
addition="""art=read_json(ROOT/'ART-V11-SOURCE-RECEIPT.json')
check('prior1112 core unchanged',lambda:require(art['originalCoreUnchanged']==1112))
check('exact35 additive overlay files',lambda:require(art['addedOverlayFiles']==35))
check('five baseline seeds not V11 native models',lambda:require(art['seedModels']==5 and art['seedState']=='BASELINE_REGISTRATION_SEEDS_NOT_NATIVE_V11_GEOMETRY'))
check('NEW pads and native import remain pending',lambda:require(art['newPads']=='UNBOUND' and art['nativeImport']=='NOT_TESTED' and contract['unboundArt']==['NEW-CAMO','NEW-CHARCOAL']))
"""
s=s.replace(anchor,addition+anchor);write('selftest.py',s)
s=(OLD/'seal.py').read_text().replace("files=['README.md'","files=['bootstrap.py','ART-V11-SOURCE-RECEIPT.json','README.md'").replace('==1122','==1157').replace("'closureFiles':1122","'closureFiles':1157").replace("['tests']==75","['tests']==79").replace("'offlineTests':75","'offlineTests':79")
write('seal.py',s)
s=(OLD/'README.md').read_text().replace('# GPG bounded startup/survey bridge — source only','# GPG + additive ArtV11 startup/survey bridge — source only').replace('**75 PASS**','**79 PASS**').replace('1,112 files','1,147 files').replace('all1,122','all1,157')
intro="""
This refresh adds exactly35 isolated ArtV11 source files while preserving every byte
of the previous1112-file shared core and the previous sealed QA packages. The five
added cooked xobs are baseline registration seeds, **not rebuilt native V11 geometry**.
NEW-CAMO/NEW-CHARCOAL remain unbound. Art native import, rig/material verification and
visual acceptance remain NOT_TESTED. ART-V11-SOURCE-RECEIPT.json records the source-only
adoption; no new startup objects or behavior are introduced by this refresh.

"""
s=s.replace('This fills one concrete planning/adapter gap',intro+'This fills one concrete planning/adapter gap')
write('README.md',s)
print(json.dumps({'status':'HELPER_SOURCE_PREPARED_WAITING_FOR_ROOT_READY','folder':str(ROOT),'nativeInvoked':False}))

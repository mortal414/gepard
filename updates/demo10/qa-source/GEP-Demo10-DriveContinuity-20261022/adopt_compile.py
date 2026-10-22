"""Bind only an actual successful closed compile; permit native RDB outputs only."""
from pathlib import Path
import argparse,hashlib,json
R=Path(__file__).resolve().parent
def load(p):return json.loads(p.read_text(encoding='utf-8-sig'))
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
ap=argparse.ArgumentParser();ap.add_argument('--receipt',type=Path,required=True);a=ap.parse_args()
receipt=a.receipt.resolve();j=load(receipt)
out=R/'binding/COMPILE-ADOPTION.json';assert not out.exists(),'No adoption overwrite'
assert j['status']=='SCRIPT_COMPILE_PASS' and j['exitCode']==0 and j['nativeAbsent'] is True and j['leaseReleased'] is True
identity=receipt.parent/'compiler-identity.json';native=load(identity)
expected='-gproj "'+str(R/'native-stage/audioQA/GEP_TrackRollingQA033.gproj')+'"'
assert expected in native['CommandLine'],'Actual compiler must target this successor project'
assert '-validate PC' in native['CommandLine'] and '-wbModule=ScriptEditor' in native['CommandLine']
before=load(R/'binding/SOURCE-BEFORE-COMPILE.json');closures=[]
for c in load(R/'binding/SOURCE-CLOSURES.json'):
 root=Path(c['root']);mp=Path(c['manifest']);old=before[str(root)]
 now={p.relative_to(root).as_posix():sha(p) for p in root.rglob('*') if p.is_file()}
 removed=old.keys()-now.keys();assert not removed,removed
 changed=[k for k in old if now[k]!=old[k]];added=list(now.keys()-old.keys())
 assert set(changed+added)<= {'resourceDatabase.rdb'},(root,changed,added)
 # External dependency packages must remain byte-for-byte unchanged.
 if not root.is_relative_to(R/'native-stage'):assert not changed and not added
 mp.write_text(json.dumps(now,indent=2)+'\n')
 closures.append(dict(c,manifestSha256=sha(mp),changed=changed,added=added,files=len(now)))
out.write_text(json.dumps({'status':'SOURCE_MATCHED_NATIVE_COMPILE_ADOPTED','receipt':str(receipt),'receiptSha256':sha(receipt),'compilerIdentity':str(identity),'compilerIdentitySha256':sha(identity),'closures':closures,'nativeAudio':'NOT_TESTED','sourceOnlyUntilRuntime':True},indent=2)+'\n')
print(out)

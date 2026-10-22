from pathlib import Path
import json,hashlib
P=Path(__file__).resolve().parent;T=Path(r'D:\ArmaProjects\Gepard-Demo10-TrackArt-20261022');R=T/'native-runs/compile-demo10-core-01';O=P/'adopted'
def load(p):return json.loads(p.read_text(encoding='utf-8-sig'))
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
assert not O.exists();e=load(R/'RESULT.json');native=load(R/'compiler-identity.json')
assert e['status']=='SCRIPT_COMPILE_PASS' and e['exitCode']==0 and e['nativeAbsent'] and e['leaseReleased']
assert '-gproj "'+str(T/'private-demo-source10/single/provingGround/GepardProvingGround.gproj')+'"' in native['CommandLine']
assert '-wbModule=ScriptEditor' in native['CommandLine'] and '-validate PC' in native['CommandLine']
assert not e['scriptErrors'];O.mkdir();closures=[]
for c in load(P/'compile/SOURCE-CLOSURES.json'):
 root=Path(c['root']);old=load(Path(c['manifest']));assert sha(Path(c['manifest']))==c['manifestSHA256']
 now={x.relative_to(root).as_posix():sha(x) for x in root.rglob('*') if x.is_file()}
 assert not (set(old)-set(now));changed=[k for k in old if old[k]!=now[k]];added=list(set(now)-set(old));assert set(changed+added)<= {'resourceDatabase.rdb'},(root,changed,added)
 if '/dependencies/' in root.as_posix():assert not changed and not added
 dest=O/f'SourceManifest-{root.name}.json';dest.write_text(json.dumps(now,indent=2)+'\n')
 closures.append({'role':root.name,'root':str(root),'manifest':str(dest),'manifestSHA256':sha(dest),'files':len(now),'changed':changed,'added':added,'beforeManifest':c['manifest'],'beforeManifestSHA256':c['manifestSHA256']})
out={'schema':1,'status':'SOURCE_MATCHED_NATIVE_COMPILE_ADOPTED','receipt':str(R/'RESULT.json'),'receiptSHA256':sha(R/'RESULT.json'),'console':e['console'],'consoleSHA256':sha(Path(e['console'])),'compilerIdentity':str(R/'compiler-identity.json'),'compilerIdentitySHA256':sha(R/'compiler-identity.json'),'actualTarget':str(T/'private-demo-source10/single/provingGround/GepardProvingGround.gproj'),'closures':closures,'nativeGameplay':'NOT_TESTED','nativeBuild':str(P/'ART-NATIVE-REBUILD-RESULT.json'),'published':False}
(O/'ACTUAL-COMPILE-ADOPTION.json').write_text(json.dumps(out,indent=2)+'\n');print(json.dumps({'adoption':str(O/'ACTUAL-COMPILE-ADOPTION.json'),'closures':[{'role':c['role'],'files':c['files'],'changed':c['changed'],'added':c['added']} for c in closures]}))

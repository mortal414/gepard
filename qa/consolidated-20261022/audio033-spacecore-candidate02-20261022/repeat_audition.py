"""Repeat only the frozen, bounded dependency-candidate drive/stop recording."""
from pathlib import Path
import json,hashlib,uuid,secrets,datetime,subprocess,sys,argparse
V=Path(__file__).resolve().parent;B=V/'binding';R=Path(r'D:\ArmaProjects\Gepard-Consolidated-20261022')
def load(p):return json.loads(p.read_text(encoding='utf-8-sig'))
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def save(p,d):p.write_text(json.dumps(d,indent=2)+'\n')
p=argparse.ArgumentParser();p.add_argument('--execute',action='store_true');a=p.parse_args()
cfg=load(B/'commands/SPACECORE_TRACK_DEPENDENCY.json');wrapper=Path(cfg['command'][3]);assert sha(wrapper)==cfg['wrapperSHA256']
adopt=load(B/'COMPILE-ADOPTION.json');assert adopt['status']=='SOURCE_MATCHED_NATIVE_COMPILE_ADOPTED' and sha(Path(adopt['receipt']))==adopt['receiptSha256']
for x in adopt['closures']:
 manifest=Path(x['manifest']);assert sha(manifest)==x['manifestSha256'];root=Path(x['root']);actual={f.relative_to(root).as_posix():sha(f) for f in root.rglob('*') if f.is_file()};assert actual==load(manifest)
stamp=datetime.datetime.now(datetime.timezone.utc).strftime('%Y%m%dT%H%M%SZ')+'-'+uuid.uuid4().hex[:8]
out=V/'repeat-runs'/stamp;run=R/'native-runs'/('audio-spacecore-repeat-'+stamp)
assert not out.exists() and not run.exists()
if not a.execute:print(json.dumps({'status':'READ_ONLY_PLAN','nativeStarted':False,'nativeRun':str(run),'recording':str(out/'recording'),'duration':'About55seconds including loading; automatic4secondforward,brake,rest,engineoff; closes normally','candidate':'SpaceCore dependency, human audibility not yet accepted'}));sys.exit(0)
out.mkdir(parents=True);job=load(B/'jobs/SPACECORE_TRACK_DEPENDENCY.json');job.update(runId=str(uuid.uuid4()),nonce=secrets.token_hex(32));jf=out/'job.json';save(jf,job)
cmd=list(cfg['command']);cmd[cmd.index('-RunRoot')+1]=str(run);cmd[cmd.index('-NativeJob')+1]=str(jf);cmd[cmd.index('-NativeJobSHA256')+1]=sha(jf)
command=out/'command.json';save(command,{'command':cmd,'nativeRun':str(run),'wrapperSHA256':sha(wrapper)})
print('This bounded audition drives automatically. Keep playback volume fixed; avoid adding driving inputs. It closes normally after the rest/engine-off observation.')
supervisor=V.parent.parent/'sprint-tools/record_owned_audio_pass.py'
code=subprocess.call([sys.executable,str(supervisor),'--command',str(command),'--output',str(out/'recording'),'--execute'])
print('Evidence:',out);sys.exit(code)

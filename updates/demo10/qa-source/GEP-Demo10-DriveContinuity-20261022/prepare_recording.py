"""At most one continuous bounded recording per final skin; no native launch."""
from pathlib import Path
import argparse,hashlib,json,uuid,secrets,sys
R=Path(__file__).resolve().parent
def load(p):return json.loads(p.read_text(encoding='utf-8-sig'))
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def save(p,j):p.write_text(json.dumps(j,indent=2)+'\n')
p=argparse.ArgumentParser();p.add_argument('--skin',choices=['camo','charcoal'],required=True);p.add_argument('--native-run',type=Path,required=True);p.add_argument('--apply',action='store_true');a=p.parse_args()
ad=load(R/'binding/COMPILE-ADOPTION.json');assert ad['status']=='SOURCE_MATCHED_NATIVE_COMPILE_ADOPTED' and sha(Path(ad['receipt']))==ad['receiptSha256']
for c in ad['closures']:
 mp=Path(c['manifest']);assert sha(mp)==c['manifestSha256'];root=Path(c['root']);assert {f.relative_to(root).as_posix():sha(f) for f in root.rglob('*') if f.is_file()}==load(mp)
out=R/'recordings'/a.skin;assert not out.exists() and not a.native_run.exists(),'Fresh one-per-skin outputs required'
if not a.apply:print(json.dumps({'status':'READ_ONLY_RECORDING_PLAN','skin':a.skin,'nativeRun':str(a.native_run),'nativeStarted':False,'view':'exterior','seconds':120}));raise SystemExit(0)
out.mkdir(parents=True);j=load(R/'cases'/f'{a.skin}.json');j.update(runId=str(uuid.uuid4()),nonce=secrets.token_hex(32));jf=out/'job.json';save(jf,j)
cmd=load(R/'COMMAND-TEMPLATE.json')['command']
for flag,value in [('-RunRoot',str(a.native_run)),('-NativeJob',str(jf)),('-NativeJobSHA256',sha(jf))]:cmd[cmd.index(flag)+1]=value
save(out/'command.json',{'command':cmd,'nativeRun':str(a.native_run),'wrapperSHA256':sha(Path(cmd[3]))})
print(json.dumps({'prepared':str(out/'command.json'),'rootOnlyNextCommand':[sys.executable,str(R.parent/'sprint-tools/record_owned_audio_pass.py'),'--command',str(out/'command.json'),'--output',str(out/'continuous'),'--execute'],'nativeStarted':False},indent=2))

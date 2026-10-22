"""Fresh wrapper-only alias repair after known pre-execution rejection; native source unchanged."""
from pathlib import Path
import json,hashlib,uuid,secrets
P=Path(__file__).resolve().parent;A=P.parent/'GEP-Demo10-DriveContinuity-20261022';D=P/'drive-binding-v2'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def save(p,d):p.parent.mkdir(parents=True,exist_ok=True);p.write_text(json.dumps(d,indent=2)+'\n')
assert not D.exists();D.mkdir()
old=A/'binding/tools/Scene.ps1';s=old.read_text();lines=s.splitlines(keepends=True)
gate=next(x for x in lines if "throw 'Native case/vehicle unbound'" in x);mapping=next(x for x in lines if x.startswith('$prefabsCore='))
aliases={'demo10-camo':'{2150FC6F1E081188}Prefabs/ArtApproved20261022/Camo/GEP_MobileCarrier.et','demo10-charcoal':'{8095474EC9806D0D}Prefabs/ArtApproved20261022/Charcoal/GEP_MobileCarrier.et'}
newgate="if($coreJob.caseId -ne 'v11-wheeled-course' -or $coreJob.vehicleId -notin @('demo10-camo','demo10-charcoal')){throw 'Native case/vehicle unbound'}\n"
newmap='$prefabsCore=@{'+ ';'.join("'"+k+"'='"+v+"'" for k,v in aliases.items())+'}\n'
t=s.replace(gate,newgate).replace(mapping,newmap);assert t.replace(newgate,gate).replace(newmap,mapping)==s
wrapper=D/'Scene.ps1';wrapper.write_text(t,newline='\n')
base=json.loads((A/'recordings/camo/command.json').read_text());oldjob=json.loads((A/'recordings/camo/job.json').read_text())
commands={}
for skin in ['camo','charcoal']:
 job=dict(oldjob,vehicleId='demo10-'+skin,prefab=aliases['demo10-'+skin],runId=str(uuid.uuid4()),nonce=secrets.token_hex(32));jp=D/f'{skin}/job.json';save(jp,job)
 cmd=list(base['command']);cmd[3]=str(wrapper)
 root=r'D:\ArmaProjects\Gepard-Demo10-TrackArt-20261022\native-runs\demo10-drive-'+skin+'-02'
 assert not Path(root).exists();cmd[cmd.index('-RunRoot')+1]=root;cmd[cmd.index('-NativeJob')+1]=str(jp);cmd[cmd.index('-NativeJobSHA256')+1]=sha(jp)
 c=dict(base,command=cmd,nativeRun=root,wrapperSHA256=sha(wrapper));cp=D/f'{skin}/command.json';save(cp,c);commands[skin]={'command':str(cp),'output':str(D/skin/'continuous'),'nativeRun':root}
save(D/'REPAIR-RECEIPT.json',{'status':'WRAPPER_ONLY_EXACT_TWO_ALIAS_REPAIR_NATIVE_NOT_STARTED','originalWrapper':str(old),'originalWrapperSHA256':sha(old),'newWrapper':str(wrapper),'newWrapperSHA256':sha(wrapper),'changedLines':2,'inverseExact':True,'allowedAliases':aliases,'oldAliasesRejected':True,'nativeSourcesAndCompileUnchanged':True,'priorFailure':str(A/'recordings/camo/continuous/native-wrapper.stderr.log'),'priorRunDirectoryAbsent':not Path(base['nativeRun']).exists(),'commands':commands})
print(json.dumps(commands))

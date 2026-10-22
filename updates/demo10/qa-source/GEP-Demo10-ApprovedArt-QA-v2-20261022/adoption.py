"""Pure verification/adoption helpers. No native, wrapper or lease control."""
from pathlib import Path
import re,json,hashlib,shlex
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def read(p):
 p=Path(p);assert p.stat().st_size<=4*1024**2,'JSON exceeds bound'
 def pairs(items):
  out={}
  for key,value in items:assert key not in out,'Duplicate JSON key';out[key]=value
  return out
 return json.loads(p.read_bytes().decode('utf-8-sig'),object_pairs_hook=pairs)
def validate_compile(run,project):
 run=Path(run).resolve();project=Path(project).resolve()
 result=read(run/'RESULT.json');owner=read(run/'owner-private.json');identity=read(run/'compiler-identity.json');exit=read(run/'launcher-exit.json')
 assert result['status']=='SCRIPT_COMPILE_PASS','Compiler did not pass'
 assert result['exitCode']==exit['exitCode']==0,'Compiler exit not zero'
 assert result['nativeAbsent'] is True and result['leaseReleased'] is True,'Compiler closure/release not verified'
 assert result['scriptErrors']==[],'Compiler script errors retained'
 assert Path(owner['project']).resolve()==project and Path(owner['workRoot']).resolve()==run,'Compiler owner project/workRoot differs'
 assert type(identity['ProcessId']) is int and identity['ProcessId']>0,'Missing compiler PID identity'
 expected=Path(r'D:\SteamLibrary\steamapps\common\Arma Reforger Tools\Workbench\ArmaReforgerWorkbenchSteamDiag.exe')
 assert Path(identity['ExecutablePath']).resolve()==expected.resolve(),'Unexpected compiler executable'
 args=[s[1:-1] if s.startswith('"') and s.endswith('"') else s for s in shlex.split(identity['CommandLine'],posix=False)]
 assert len(args)>1 and Path(args[0]).resolve()==expected.resolve(),'Compiler command executable differs'
 assert args.count('-gproj')==1 and Path(args[args.index('-gproj')+1]).resolve()==project,'Compiler -gproj differs'
 assert args.count('-profile')==1 and Path(args[args.index('-profile')+1]).resolve()==run/'profile','Compiler profile differs'
 assert args.count('-logsDir')==1 and Path(args[args.index('-logsDir')+1]).resolve()==run/'logs/validate','Compiler log root differs'
 assert '-validate' in args and '-wbSilent' in args and '-wbModule=ScriptEditor' in args,'Not the expected native validation command'
 console=Path(result['console']).resolve();assert console==run/'logs/validate/console.log','Compiler exact console differs'
 assert console.is_file() and console.stat().st_size<=64*1024**2,'Compiler console missing/oversize'
 text=console.read_bytes().decode('utf-8-sig');assert 'Script validation successful.' in text,'Compiler success marker absent'
 errors=[line for line in text.splitlines() if re.search(r'SCRIPT.*\(E\)|Cannot compile|error:',line)]
 assert not errors,'Compiler console contains script errors'
 return {'schema':1,'status':'SCRIPT_COMPILE_PASS_ADOPTED','project':str(project),'compileRun':str(run),'compilerResultSHA256':sha(run/'RESULT.json'),'compilerConsoleSHA256':sha(console),'compilerIdentitySHA256':sha(run/'compiler-identity.json'),'launcherExitSHA256':sha(run/'launcher-exit.json'),'compilerOwnerProjectVerified':True,'nativeRuntime':'NOT_TESTED','nativeGameplay':'NOT_TESTED'}

def validate_source_and_generated(stage_gep,source_gep,stage_gpg,source_gpg,prior_core,overlay):
 stage_gep,source_gep,stage_gpg,source_gpg=map(Path,[stage_gep,source_gep,stage_gpg,source_gpg])
 def files(root):return {p.relative_to(root).as_posix():sha(p) for p in root.rglob('*') if p.is_file()}
 sourcepins=files(source_gep);nativepins=files(stage_gep);worldsource=files(source_gpg);worldnative=files(stage_gpg)
 assert len(sourcepins)==1147 and set(sourcepins)==set(nativepins),'Exact1147 Gepard source/native file sets required'
 assert len(prior_core)==1112 and all(sourcepins.get(p)==h for p,h in prior_core.items()),'Original core source changed'
 assert len(overlay)==35 and set(sourcepins)-set(prior_core)==set(overlay),'Exact additive35 overlay required'
 assert all(sourcepins.get(p)==r['sha256'] for p,r in overlay.items()),'Overlay source bytes changed'
 differences=[p for p in sourcepins if sourcepins[p]!=nativepins[p]]
 assert set(differences)<= {'resourceDatabase.rdb'},'Unexpected source/native Gepard change'
 assert len(worldsource)==10 and len(worldnative)==11 and set(worldnative)==set(worldsource)|{'resourceDatabase.rdb'},'World native closure must add only generated RDB'
 assert all(worldnative[p]==h for p,h in worldsource.items()),'Staged world source differs'
 assert (stage_gpg/'resourceDatabase.rdb').stat().st_size>0,'Empty generated GPG RDB'
 assert (stage_gep/'resourceDatabase.rdb').stat().st_size>0,'Empty generated Gepard RDB'
 return {'sourceGepard':sourcepins,'nativeGepard':nativepins,'sourceWorld':worldsource,'nativeWorld':worldnative,'generatedChanges':[{'root':str(stage_gep),'relative':p,'sourceSHA256':sourcepins[p],'nativeSHA256':nativepins[p]} for p in differences]+[{'root':str(stage_gpg),'relative':'resourceDatabase.rdb','sourceSHA256':None,'nativeSHA256':worldnative['resourceDatabase.rdb']}],'originalSourceCoreUnchanged':1112,'overlaySourceUnchanged':35}

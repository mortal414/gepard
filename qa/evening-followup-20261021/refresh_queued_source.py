from pathlib import Path
import hashlib,json,shutil
r=Path(r'D:\ArmaProjects\Gepard-EveningSprint-20261021');stage=r/'followup-deferred-launch-01';repo=r/'diagnosis'
relative='scripts/Game/GEP015Stock/GEP_StockSmoke015.c';source=repo/'qa/evening-smoke-deferred-20261021/source-overrides'/relative;dest=stage/'native-stage/gepard'/relative
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def write(p,o):p.write_bytes((json.dumps(o,indent=2)+'\n').encode())
old=sha(dest);shutil.copy2(source,dest);addon=stage/'native-stage/gepard'
pins={p.relative_to(addon).as_posix():sha(p) for p in sorted(addon.rglob('*')) if p.is_file()};pin=stage/'checks/SOURCE_MANIFEST.json';oldManifest=sha(pin);write(pin,pins)
p=stage/'harness/config683.json';config=json.loads(p.read_bytes());config.update(fixtureReady=False,manifestSha256=sha(pin),fileCount=len(pins));write(p,config)
record={'status':'SOURCE_UPDATED_NATIVE_NOT_TESTED','reason':'Early owner-null/deleted guard and repeatable15-contract source checks integrated before any native compile/run of this stage','sourceCommit':'69f93e4','relative':relative,'oldSHA256':old,'newSHA256':sha(dest),'priorManifestSHA256':oldManifest,'currentManifestSHA256':sha(pin),'nativeCompile':'NOT_TESTED','nativeLaunched':False,'leaseTouched':False}
write(stage/'SOURCE-UPDATE.json',record);write(repo/'qa/evening-followup-20261021/DEFERRED-STAGE-UPDATE.json',record);print(json.dumps(record))

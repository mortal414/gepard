"""Regression cases for refusing stale frozen source; never invokes native tools."""
from pathlib import Path
import importlib.util,json,shutil,tempfile
spec=importlib.util.spec_from_file_location('gpg_generate',Path(__file__).with_name('generate.py'))
module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module)
source=module.ROOT
cases=[]
def save(path,value):path.write_text(json.dumps(value))
for case in ('unchanged','registry-edit','world-edit','world-extra-file','manifest-digest-edit','runtime-digest-edit'):
 with tempfile.TemporaryDirectory(prefix='gpg-freeze-source-') as temp:
  root=Path(temp)/'fixture';shutil.copytree(source,root,ignore=shutil.ignore_patterns('__pycache__'))
  module.ROOT=root
  if case=='registry-edit':
   path=root/'registry/variants.json';data=json.loads(path.read_text());data['routeSubjectSlot']='NEW-CAMO';save(path,data)
  if case=='world-edit':
   path=root/'addon/Worlds/GPG_Arland.ent';path.write_text(path.read_text()+'\n// changed after freeze\n')
  if case=='world-extra-file':(root/'addon/UNPINNED.txt').write_text('unreviewed')
  if case=='manifest-digest-edit':
   path=root/'checks/world-source-manifest.json';data=json.loads(path.read_text());data['sha256']='0'*64;save(path,data)
  if case=='runtime-digest-edit':
   path=root/'checks/variant-lock.json';data=json.loads(path.read_text());data['sharedRuntime']['sha256']='0'*64;save(path,data)
  error=None
  try:module.verify_frozen()
  except ValueError as exc:error=str(exc)
  assert (error is None)==(case=='unchanged'),(case,error)
  cases.append({'case':case,'status':'PASS','expected':'accept' if case=='unchanged' else 'reject','observed':error or 'accepted unchanged source'})
module.ROOT=source
print(json.dumps({'status':'PASS_OFFLINE_ONLY','checks':len(cases),'cases':cases,'nativeStarted':False},indent=2))

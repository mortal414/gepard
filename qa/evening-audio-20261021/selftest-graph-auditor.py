"""Synthetic tests of the source auditor; no audio resource is written or played."""
from pathlib import Path
import json,re,runpy
qa=Path(__file__).resolve().parent
audit=runpy.run_path(str(qa/'audit-authored-graph.py'))['audit']
template=(qa/'authoring/GEP_TrackMovement.acp.in').read_text()
template=re.sub(r'//[^\n]*','',template)
stock='{0123456789ABCDEF}Sounds/Vehicles/_SharedData/Tracks/Rotate/Vehicles_Tracks_Rotate_Slow.wav'
source=template.replace('@@STOCK_SLOW_CANONICAL_FROM_RESOLVER@@',stock).replace('@@NATIVE_VEH_TIRES_INPUT_PORT@@','80001').replace('@@ORIGINAL_TRACK_SOUND_CONFIG_CANONICAL@@','{FEDCBA9876543210}Sounds/Gepard/Tracks/SyntheticTestOnly.conf')
report={'status':'NATIVE_RESOLVER_OBSERVATIONS_ONLY','resources':[{'label':'stock-slow','canonical':stock,'hasGuid':True}]}
good=audit(source,report)
assert good['status']=='PASS_SOURCE_GRAPH_SHAPE_ONLY'
assert good['playbackAdmission'].startswith('BLOCKED_') and not good['gep033Complete']
rejections=0
for bad in [source+'\n@@UNRESOLVED@@',source.replace(stock,stock.replace('Slow.wav','Fast.wav')),source.replace('id 3100 port 65','id 9999 port 65'),source.replace('"Loop count" 255','"Loop count" 8'),source.replace('"GEP_TRACK_MOVEMENT_LP"','"SOUND_ENGINE_LP"')]:
    try: audit(bad,report)
    except ValueError: rejections+=1
    else: raise AssertionError('Invalid source was accepted')
assert rejections==5
result={'status':'PASS_SYNTHETIC_SOURCE_AUDITOR_ONLY','acceptedShapeStillBlocksPlayback':True,'rejectionCases':rejections,'nativeEvidence':False,'nativeAppsLaunched':False,'graphWritten':False}
(qa/'SOURCE-AUDITOR-SELFTEST.json').write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8',newline='\n')
print(json.dumps(result,indent=2))

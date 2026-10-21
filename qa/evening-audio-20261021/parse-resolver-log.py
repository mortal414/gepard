"""Normalize one real native GEP033 resolver log; no native process control."""
from pathlib import Path
import argparse, hashlib, json, re

LABELS=('stock-very-slow','stock-slow','stock-medium','stock-fast','stock-wheel-left','stock-wheel-right','stock-engine','candidate-graph','missing-control')
def parse(text):
    if text.count('GEP033_PROBE_BEGIN')!=1 or text.count('GEP033_PROBE_END')!=1:
        raise ValueError('Exactly one complete probe invocation is required; preserve other runs separately.')
    if 'GEP033_PROBE_END observations=9 saved=1' not in text:
        raise ValueError('Probe did not confirm durable report save.')
    resources={}; graphs={}
    for marker,dest in [('GEP033_RESOURCE_JSON',resources),('GEP033_GRAPH_JSON',graphs)]:
        for hit in re.finditer(marker+r'\s+',text):
            row,_=json.JSONDecoder().raw_decode(text[hit.end():])
            label=row.get('label')
            if label in dest: raise ValueError('Duplicate observation '+str(label))
            dest[label]=row
    if set(resources)!=set(LABELS): raise ValueError('Missing or extra resource observations: '+str(set(resources)^set(LABELS)))
    for row in resources.values():
        for field in ('hasGuid','pathLoadValid','canonicalLoadValid'):
            if type(row.get(field)) is not bool: raise ValueError('Boolean field missing or invalid: '+field)
        for field in ('input','canonical','canonicalPath','pathLoadedIdentity','canonicalLoadedIdentity'):
            if not isinstance(row.get(field),str): raise ValueError('String field missing or invalid: '+field)
    identity={}
    for label in LABELS[:7]:
        row=resources[label]
        guid=re.match(r'^\{[0-9A-Fa-f]{16}\}',row['canonical']) is not None
        identity[label]=row['hasGuid'] and guid and row['canonicalPath'].lower()==row['input'].lower()
    missing=resources['missing-control']
    negative_control=not missing['hasGuid'] and not missing['pathLoadValid'] and not missing['canonicalLoadValid']
    slow=resources['stock-slow']
    if slow['pathLoadValid'] and slow['canonicalLoadValid']:
        path_status='PASS' if slow['pathLoadedIdentity']==slow['canonicalLoadedIdentity'] and slow['pathLoadedIdentity'] else 'FAIL_IDENTITY_DIFFERS'
    elif not slow['pathLoadValid'] and not slow['canonicalLoadValid']:
        path_status='NOT_TESTED_GENERIC_RESOURCE_LOADER_DID_NOT_ACCEPT_WAV'
    else:
        path_status='FAIL_PATH_AND_CANONICAL_LOAD_DIFFER'
    candidate=resources['candidate-graph']; candidate_graph=graphs.get('candidate-graph',{})
    candidate_status='ABSENT_EXPECTED_BEFORE_AUTHORING'
    if candidate['hasGuid']:
        if candidate['canonicalLoadValid'] and candidate_graph.get('containerAvailable'):
            candidate_status='METADATA_MATCHES_ONLY' if candidate_graph.get('events')==['GEP_TRACK_MOVEMENT_LP'] and candidate_graph.get('bankCount')==1 and 'VEH_Tires' in candidate_graph.get('mixerRoutes',[]) else 'NEEDS_GRAPH_METADATA_REVIEW'
        else: candidate_status='PRESENT_BUT_NOT_GENERICALLY_INSPECTABLE'
    return {'status':'NATIVE_RESOLVER_OBSERVATIONS_ONLY','identityGate':'PASS' if all(identity.values()) and negative_control else 'FAIL','stockIdentityCases':identity,'negativeControl':negative_control,'pathOnlyWaveLoad':path_status,'candidateGraph':candidate_status,'resources':resources,'graphs':graphs,'graphCompile':'NOT_ESTABLISHED_BY_RESOURCE_LOAD','eventPlayback':'NOT_TESTED','loopLifecycle':'NOT_TESTED','audition':'NOT_TESTED','gep033Complete':False}

def selftest():
    rows=[]
    for label in LABELS:
        exists=label not in ('candidate-graph','missing-control')
        path='synthetic/'+label
        canonical='{0123456789ABCDEF}'+path if exists else path
        rows.append({'label':label,'input':path,'canonical':canonical,'canonicalPath':path,'hasGuid':exists,'pathLoadValid':exists,'canonicalLoadValid':exists,'pathLoadedIdentity':canonical if exists else '', 'canonicalLoadedIdentity':canonical if exists else ''})
    text='GEP033_PROBE_BEGIN\n'+'\n'.join('GEP033_RESOURCE_JSON '+json.dumps(r) for r in rows)+'\nGEP033_PROBE_END observations=9 saved=1'
    good=parse(text); assert good['identityGate']=='PASS' and good['pathOnlyWaveLoad']=='PASS' and not good['gep033Complete']
    for broken in [text.replace('GEP033_PROBE_END',''),text+'\nGEP033_RESOURCE_JSON '+json.dumps(rows[0]),text.replace('"hasGuid": true','"hasGuid": "true"',1)]:
        try: parse(broken)
        except ValueError: pass
        else: raise AssertionError('Incomplete/duplicate/invalid observation accepted')
    rows[1]['pathLoadValid']=False
    unequal='GEP033_PROBE_BEGIN\n'+'\n'.join('GEP033_RESOURCE_JSON '+json.dumps(r) for r in rows)+'\nGEP033_PROBE_END observations=9 saved=1'
    assert parse(unequal)['pathOnlyWaveLoad']=='FAIL_PATH_AND_CANONICAL_LOAD_DIFFER'
    return {'status':'PASS_SYNTHETIC_PARSER_SELFTEST_ONLY','cases':5,'nativeEvidence':False}

if __name__=='__main__':
    p=argparse.ArgumentParser()
    p.add_argument('--selftest',action='store_true')
    p.add_argument('--log',type=Path)
    p.add_argument('--output',type=Path)
    args=p.parse_args()
    if args.selftest:
        print(json.dumps(selftest(),indent=2))
    else:
        if not args.log or not args.output: p.error('--log and --output are required')
        raw=args.log.read_bytes()
        result=parse(raw.decode('utf-8-sig',errors='replace'))
        result['nativeLogSha256']=hashlib.sha256(raw).hexdigest()
        # Exclusive creation retains earlier attempts and refuses accidental evidence replacement.
        with args.output.open('x',encoding='utf-8',newline='\n') as target:
            target.write(json.dumps(result,indent=2)+'\n')
        print(json.dumps({k:result[k] for k in ('status','identityGate','pathOnlyWaveLoad','candidateGraph','gep033Complete')},indent=2))

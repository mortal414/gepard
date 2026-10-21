"""Read-only source-shape audit of a NEW ORIGINAL authored graph; never approves playback."""
from pathlib import Path
import argparse,hashlib,json,re

def blocks(source,classname):
    result=[]
    for match in re.finditer(r'(?<![A-Za-z0-9_])'+re.escape(classname)+r'\b',source):
        # Resource GUID braces may occur inside the quoted inheritance header.
        quoted=False; escape=False; start=None
        for pos in range(match.end(),len(source)):
            ch=source[pos]
            if escape: escape=False; continue
            if quoted and ch=='\\': escape=True; continue
            if ch=='"': quoted=not quoted; continue
            if ch=='{' and not quoted:
                start=pos+1; break
        if start is None: raise ValueError('Missing body for '+classname)
        depth=1; quoted=False; escape=False
        for pos in range(start,len(source)):
            ch=source[pos]
            if escape: escape=False; continue
            if quoted and ch=='\\': escape=True; continue
            if ch=='"': quoted=not quoted; continue
            if quoted: continue
            if ch=='{': depth+=1
            if ch=='}': depth-=1
            if not depth:
                result.append(source[start:pos]); break
        else: raise ValueError('Unclosed '+classname)
    return result

def scalar(source,key):
    found=re.search(r'^\s*(?:'+re.escape(key)+r'|"'+re.escape(key)+r'")\s+("(?:\\.|[^"\\])*"|[^\s{}]+)',source,re.M)
    if not found: return None
    value=found.group(1)
    return value[1:-1] if value.startswith('"') else value

def audit(source,report):
    if report.get('status')!='NATIVE_RESOLVER_OBSERVATIONS_ONLY': raise ValueError('Native resolver report status is missing or differs.')
    if '@@' in source: raise ValueError('Unresolved authoring tokens: this is not a loadable graph.')
    source=re.sub(r'/\*.*?\*/','',source,flags=re.S)
    source=re.sub(r'//[^\n]*','',source)
    rows=report.get('resources',[])
    if isinstance(rows,dict): rows=list(rows.values())
    slow=[row for row in rows if row.get('label')=='stock-slow']
    if len(slow)!=1: raise ValueError('Exactly one native stock-slow resource observation is required.')
    canonical=slow[0].get('canonical','')
    expected='Sounds/Vehicles/_SharedData/Tracks/Rotate/Vehicles_Tracks_Rotate_Slow.wav'
    if slow[0].get('hasGuid') is not True or not re.fullmatch(r'\{[A-Fa-f0-9]{16}\}'+re.escape(expected),canonical):
        raise ValueError('Native report does not establish the exact stock Slow canonical identity.')
    classes={name:blocks(source,name) for name in ('SoundClass','BankLocalClass','ShaderClass','MixerClass','AudioBankSampleClass')}
    if any(len(items)!=1 for items in classes.values()): raise ValueError('Bounded candidate needs exactly one Sound, Bank, Shader, Mixer and stock Sample.')
    event=scalar(classes['SoundClass'][0],'name')
    if event!='GEP_TRACK_MOVEMENT_LP': raise ValueError('Unexpected event name.')
    sample=scalar(classes['AudioBankSampleClass'][0],'Filename')
    if sample!=canonical: raise ValueError('Sample identity differs from the exact native-resolved Slow WAV.')
    bank=classes['BankLocalClass'][0]
    if scalar(bank,'Loop count')!='255' or scalar(bank,'Infinite loop')!='1': raise ValueError('Native loop properties differ from the bounded design.')
    if any(word.lower() in source.lower() for word in ('SpaceCore','WCS_','Vehicles_LAV25_Engine','Weapons_HeavyWeapons_M242')):
        raise ValueError('Graph contains an unrelated engine/gun/third-party reference.')
    mixer=classes['MixerClass'][0]
    if '"VEH_Tires"' not in mixer: raise ValueError('Expected named native movement mix is missing.')
    names={name:scalar(body[0],'id') for name,body in classes.items() if name!='AudioBankSampleClass'}
    if None in names.values() or len(set(names.values()))!=4: raise ValueError('Native node IDs are missing or collide.')
    sound=classes['SoundClass'][0]
    out_port=scalar(sound,'outStatePort')
    if not out_port or not out_port.isdigit() or scalar(sound,'outState')!=names['MixerClass']:
        raise ValueError('Sound output state/port is not native-authored.')
    # Checks only the documented minimal audio edge shape; native routing remains a separate observation.
    for consumer,producer in [('ShaderClass','BankLocalClass'),('SoundClass','ShaderClass'),('MixerClass','SoundClass')]:
        if not re.search(r'\bid\s+'+re.escape(names[producer])+r'\s+port\s+65\b',classes[consumer][0]):
            raise ValueError('Missing '+producer+' -> '+consumer+' native edge.')
    return {'status':'PASS_SOURCE_GRAPH_SHAPE_ONLY','event':event,'sampleCanonical':canonical,'nodeIds':names,'serializedBankVolume':scalar(bank,'Volume'),'serializedBankPitch':scalar(bank,'Pitch'),'nativeSoundOutputPort':out_port,'playbackAdmission':'BLOCKED_PENDING_NATIVE_PORT_UNITS_MUTE_LIVE_UPDATE_EVENT_LIFECYCLE','portUnits':'NOT_ESTABLISHED_BY_SOURCE_SHAPE','exactMute':'NOT_TESTED','dynamicUpdate':'NOT_TESTED','eventLifecycle':'NOT_TESTED','audition':'NOT_TESTED','gep033Complete':False}

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--graph',type=Path,required=True)
    p.add_argument('--resolver-report',type=Path,required=True)
    p.add_argument('--output',type=Path,required=True)
    args=p.parse_args()
    raw=args.graph.read_bytes(); native=args.resolver_report.read_bytes()
    result=audit(raw.decode('utf-8-sig'),json.loads(native.decode('utf-8-sig')))
    result['graphSha256']=hashlib.sha256(raw).hexdigest()
    result['nativeResolverReportSha256']=hashlib.sha256(native).hexdigest()
    with args.output.open('x',encoding='utf-8',newline='\n') as target:
        target.write(json.dumps(result,indent=2)+'\n')
    print(json.dumps({k:result[k] for k in ('status','playbackAdmission','gep033Complete')},indent=2))

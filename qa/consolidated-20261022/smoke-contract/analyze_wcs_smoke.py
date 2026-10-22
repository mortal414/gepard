"""Offline analysis of original GEP215 WCS probe output; never launches or edits native state."""
from __future__ import annotations
import argparse, hashlib, json, math, re, struct
from pathlib import Path

EXPECTED_FRAME_SECONDS = {1:.25,2:1,3:3,4:5,5:12,6:20,7:30,8:40}
SCOPE = 'WCS_DEPENDENCY_ENGINEERING_DIAGNOSTIC'

def digest(p): return hashlib.sha256(p.read_bytes()).hexdigest()
def finite(x): return isinstance(x,(int,float)) and not isinstance(x,bool) and math.isfinite(x)
def vector(value):
    if isinstance(value,str): value=re.findall(r'[-+]?(?:\d*\.\d+|\d+\.?\d*)(?:[eE][-+]?\d+)?',value)
    if not isinstance(value,list) or len(value)!=3: raise ValueError('Vector must be a three-element array or native vector string')
    result=[float(x) for x in value]
    if not all(math.isfinite(x) for x in result): raise ValueError('Nonfinite vector')
    return result
def distance(a,b): return math.sqrt(sum((x-y)**2 for x,y in zip(a,b)))
def truth(value): return str(value).lower() in ('true','1')
def status(checks):
    values=[v['status'] for v in checks.values()]
    return 'FAIL' if 'FAIL' in values else ('NOT_TESTED' if 'NOT_TESTED' in values else 'PASS')

def image_header(p):
    data=p.read_bytes();out={'path':str(p),'bytes':len(data),'sha256':hashlib.sha256(data).hexdigest(),'validation':'HEADER_ONLY_NOT_PIXEL_REVIEW'}
    if len(data)>=54 and data[:2]==b'BM':
        declared,offset=struct.unpack_from('<I',data,2)[0],struct.unpack_from('<I',data,10)[0]
        width,height=struct.unpack_from('<ii',data,18);bits=struct.unpack_from('<H',data,28)[0];compression=struct.unpack_from('<I',data,30)[0]
        valid=width>0 and height!=0 and bits in (24,32) and declared<=len(data) and offset>=54
        if valid and compression==0: valid=len(data)>=offset+((width*bits+31)//32*4)*abs(height)
        out.update(format='BMP',width=width,height=abs(height),validHeader=valid)
    elif len(data)>=33 and data[:8]==b'\x89PNG\r\n\x1a\n' and data[12:16]==b'IHDR':
        width,height=struct.unpack_from('>II',data,16);out.update(format='PNG',width=width,height=height,validHeader=width>0 and height>0)
    else: out.update(format='UNSUPPORTED_OR_INVALID',validHeader=False)
    return out

def analyze(run_root:Path,mode:str,console:Path|None=None,profile:Path|None=None,origin=(600.,0.,2600.),max_distance=200.,min_horizontal=1.,min_travel=1.):
    if mode not in ('single','full') or any(not finite(x) or x<=0 for x in (max_distance,min_horizontal,min_travel)): raise ValueError('Mode and strictly positive finite motion bounds required')
    expected=1 if mode=='single' else 8;after=8-expected
    profile=profile or run_root/'profile';errors=[];inputs=[];checks={}
    def check(name,condition,available=True,detail=None):
        checks[name]={'status':('PASS' if condition else 'FAIL') if available else 'NOT_TESTED'}
        if detail is not None: checks[name]['detail']=detail
    def read_json(p):
        if not p.is_file(): return None
        inputs.append({'path':str(p),'sha256':digest(p)})
        try: return json.loads(p.read_text(encoding='utf-8-sig'))
        except (ValueError,OSError) as e: errors.append({'path':str(p),'error':str(e)});return None
    if console is None:
        candidates=[p for p in (run_root/'logs/console.log',run_root/'console.log') if p.is_file()]
        if not candidates and (run_root/'logs').exists(): candidates=list((run_root/'logs').rglob('console.log'))
        if len(candidates)==1: console=candidates[0]
        elif len(candidates)>1: errors.append({'error':'Ambiguous console files: supply --console','paths':[str(p) for p in candidates]})
    text=''
    if console and console.is_file():
        inputs.append({'path':str(console),'sha256':digest(console)});text=console.read_text(encoding='utf-8-sig',errors='replace')
    lines=text.splitlines();have_console=bool(text)
    programs=[(n,l) for n,l in enumerate(lines) if 'GEP215_WCS_PROGRAM ' in l]
    preflights=[l for l in lines if 'GEP215_WCS_PREFLIGHT ' in l]
    empties=[(n,l) for n,l in enumerate(lines) if 'GEP215_WCS_EMPTY ' in l]
    terminals=[l for l in lines if 'GEP215_WCS_TERMINAL ' in l]
    discoveries=[];discovery_errors=[]
    for n,line in enumerate(lines):
        if 'GEP215_WCS_ENTITY ' not in line: continue
        m=re.search(r'\bid=(-?\d+).*?actualOrigin=(.*?)\s+launched=(\w+)\s+launchAge=([-+\d.eE]+)',line)
        if not m: discovery_errors.append(line);continue
        try: discoveries.append({'id':int(m[1]),'origin':vector(m[2]),'launched':truth(m[3]),'launchAge':float(m[4]),'line':n+1})
        except (ValueError,TypeError) as e: discovery_errors.append(str(e))
    terminal=read_json(profile/'GEP215-wcs-result.json')
    exit_receipt=read_json(run_root/'exit.json')
    rows=[]
    for p in sorted(profile.glob('GEP215-wcs-life-*.json')) if profile.exists() else []:
        row=read_json(p)
        if not isinstance(row,dict):
            if row is not None: errors.append({'path':str(p),'error':'Lifecycle record is not an object'})
            continue
        try:
            if row.get('scope')!=SCOPE: raise ValueError('Unexpected lifecycle scope')
            if not all(finite(row.get(k)) for k in ('sequence','elapsedSinceProgram','ammo','knownPayloadEntities')): raise ValueError('Missing/nonfinite lifecycle fields')
            arrays=[row.get(k) for k in ('entityIds','actualOrigins','travelObserved','launchAges')]
            if any(not isinstance(a,list) for a in arrays) or len({len(a) for a in arrays})!=1: raise ValueError('Mismatched native array lengths')
            if any(not isinstance(i,int) or isinstance(i,bool) for i in arrays[0]): raise ValueError('Invalid entity IDs')
            if len(set(arrays[0]))!=len(arrays[0]): raise ValueError('Duplicate entity ID within one readback')
            row['actualOrigins']=[vector(v) for v in arrays[1]]
            if any(not finite(x) for a in arrays[2:] for x in a): raise ValueError('Invalid travel/age readback')
            rows.append(row)
        except (ValueError,TypeError) as e: errors.append({'path':str(p),'error':str(e)})
    rows.sort(key=lambda r:r['sequence'])
    have_rows=bool(rows);have_terminal=isinstance(terminal,dict)
    if terminal is not None and not have_terminal: errors.append({'error':'Terminal record is not an object'})
    terminal=terminal if have_terminal else {}
    check('inputSchemaValid',not errors and not discovery_errors,have_console or have_rows or have_terminal,{'errors':errors,'discoveryParseErrors':discovery_errors})
    check('oneAuthorizedProgramRequest',len(programs)==1 and f'shotsExpected={expected}' in programs[0][1] and 'callback=RunProgramFromController_S' in programs[0][1],have_console)
    check('settledCrewInitialEight',len(preflights)==1 and 'crew=settled' in preflights[0] and re.search(r'\bammo=8\b',preflights[0]) is not None and re.search(r'\bcapacity=8\b',preflights[0]) is not None,have_console)
    check('terminalProgramRecorded',terminal.get('programRequested') is True,have_terminal)
    check('terminalScopeMatches',terminal.get('scope')==SCOPE,have_terminal)
    check('oneSuccessfulDiagnosticTerminal',len(terminals)==1 and terminal.get('reason')=='diagnostic_complete_acceptance_pending',have_console and have_terminal)
    check('terminalAmmoMatches',terminal.get('ammo')==after,have_terminal,{'expected':after,'observed':terminal.get('ammo') if have_terminal else None})
    check('terminalSettledCrew',terminal.get('settledCrew') is True,have_terminal)
    sequences=[r['sequence'] for r in rows];times=[r['elapsedSinceProgram'] for r in rows]
    check('uniqueOrderedSamples',len(sequences)==len(set(sequences)) and all(b>a for a,b in zip(times,times[1:])),len(rows)>=2)
    late=[r for r in rows if r['elapsedSinceProgram']>=3]
    check('ammoReachesAndRemainsExpected',all(r['ammo']==after for r in late),bool(late))
    check('ammoNeverIncreases',all(0<=r['ammo']<=8 for r in rows) and all(b['ammo']<=a['ammo'] for a,b in zip(rows,rows[1:])),len(rows)>=2)
    ids=[r['id'] for r in discoveries];unique=set(ids)
    check('exactUniqueNativeLaunches',len(ids)==expected and len(unique)==expected and all(d['launched'] for d in discoveries),have_console,{'expected':expected,'loggedIds':ids})
    check('allKnownCountsBounded',all(0<=r['knownPayloadEntities']<=expected for r in rows) and terminal.get('knownPayloadEntities')==expected,have_rows and have_terminal)
    check('launchOriginsNearFixture',all(distance(d['origin'],origin)<=max_distance for d in discoveries),bool(discoveries),{'fixtureOrigin':list(origin),'maximumDistance':max_distance})
    tracks={};invalid_positions=[]
    for row in rows:
        for entity,pos,source_travel,age in zip(row['entityIds'],row['actualOrigins'],row['travelObserved'],row['launchAges']):
            if distance(pos,origin)>max_distance: invalid_positions.append({'sequence':row['sequence'],'id':entity,'position':pos});continue
            tracks.setdefault(entity,[]).append({'seconds':row['elapsedSinceProgram'],'position':pos,'sourceTravel':source_travel,'launchAge':age})
    check('allObservedPositionsNearFixture',not invalid_positions,have_rows,invalid_positions)
    check('readbackIdentityMatchesNativeLaunches',set(tracks)==unique and len(tracks)==expected,bool(tracks) and bool(discoveries))
    movement=[]
    for entity in sorted(unique|set(tracks)):
        samples=tracks.get(entity,[]);positions=[s['position'] for s in samples]
        computed=sum(distance(a,b) for a,b in zip(positions,positions[1:]))
        horizontal=max((math.hypot(p[0]-positions[0][0],p[2]-positions[0][2]) for p in positions),default=0.)
        moved=len(samples)>=2 and computed>=min_travel and horizontal>=min_horizontal
        movement.append({'id':entity,'validSamples':len(samples),'computedTravel':computed,'maxHorizontalDistanceFromFirstReadback':horizontal,'flightMotionObserved':moved,'first':positions[0] if positions else None,'last':positions[-1] if positions else None})
    check('actualMotionForEveryPayload',len(movement)==expected and all(m['flightMotionObserved'] for m in movement),bool(movement) and all(m['validSamples']>=2 for m in movement),{'minComputedTravel':min_travel,'minHorizontalDistance':min_horizontal,'tracks':movement})
    if mode=='full':
        check('oneEmptyProgramAfterInitial',len(empties)==1 and len(programs)==1 and empties[0][0]>programs[0][0],have_console)
        check('emptyProgramRecorded',terminal.get('emptyProgramRequested') is True,have_terminal)
        after_empty=[r for r in rows if r['elapsedSinceProgram']>=6.5]
        check('continuedEmptyObservation',any(r['elapsedSinceProgram']>=12 for r in after_empty),have_rows)
        check('emptyAddsNoObservedAmmoOrPayload',all(r['ammo']==0 and r['knownPayloadEntities']==8 for r in after_empty) and len(unique)==8,bool(after_empty) and bool(discoveries))
    else:
        check('singleHasNoEmptyOrSecondProgram',not empties and terminal.get('emptyProgramRequested') is False,have_console and have_terminal)
    native_errors=[l for l in lines if re.search(r'SCRIPT\s*\(E\)|Cannot compile|Can.t compile|Unhandled exception|Fatal error',l,re.I)]
    check('noNativeScriptOrFatalErrors',not native_errors,have_console,native_errors)
    frames=[];requests=[]
    for line in lines:
        m=re.search(r'GEP215_WCS_FRAME index=(\d+) elapsed=([-+\d.eE]+) requested=(\w+)',line)
        if m: requests.append({'index':int(m[1]),'elapsed':float(m[2]),'requested':truth(m[3])})
    for index,seconds in EXPECTED_FRAME_SECONDS.items():
        candidates=[p for p in profile.glob(f'GEP215-wcs-frame-{index}.*') if p.is_file()] if profile.exists() else []
        item={'index':index,'expectedSeconds':seconds,'status':'NOT_TESTED','files':[]}
        if len(candidates)==1:
            header=image_header(candidates[0]);item['files']=[header];item['status']='PASS' if header['validHeader'] else 'FAIL'
        elif candidates: item.update(status='FAIL',reason='Ambiguous capture file names')
        frames.append(item)
    frame_status='FAIL' if any(f['status']=='FAIL' for f in frames) else ('PASS' if all(f['status']=='PASS' for f in frames) else 'NOT_TESTED')
    lifecycle={'status':'NOT_TESTED','scope':'COORDINATOR_RECEIPT_ONLY'}
    if isinstance(exit_receipt,dict): lifecycle['status']='PASS' if exit_receipt.get('verifiedExited') is True and exit_receipt.get('leaseReleased') is True and exit_receipt.get('exitCode')==0 else 'FAIL';lifecycle['receipt']=exit_receipt
    return {'schema':1,'mode':mode,'scope':'OFFLINE_NATIVE_TELEMETRY_AND_CAPTURE_AVAILABILITY_ONLY','runRoot':str(run_root),'profile':str(profile),'console':str(console) if console else None,
            'deterministicTelemetry':{'status':status(checks),'checks':checks},'captureAvailability':{'status':frame_status,'frames':frames,'requestTelemetry':requests,'pixelContentReviewed':False},
            'lifecycle':lifecycle,'inputs':inputs,'impact':'NEEDS_NATIVE_OBSERVATION_OR_PIXEL_REVIEW','visibleCloud':'NEEDS_PIXEL_REVIEW','coverageDurationPlacement':'NEEDS_USER_TEST','rearmRolesCooldownMP':'NOT_TESTED','overallFeatureReadiness':'NEEDS_USER_TEST',
            'limits':['HasLaunched plus changing valid native positions supports observed flight motion; it does not establish safe muzzle clearance or impact.','A capture file/header proves availability only, never visible cloud or useful coverage.','A motion plateau or fuse/TTL value is not automatically an impact/cloud pass.','No compilation/source identity qualification is inferred; pair this output with the exact staged manifest and fresh native compile receipt.','If evidence is missing or malformed the corresponding checks remain NOT_TESTED or FAIL; no inferred successful delivery.']}

def main():
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--run-root',type=Path,required=True);ap.add_argument('--mode',choices=('single','full'),required=True);ap.add_argument('--profile',type=Path);ap.add_argument('--console',type=Path);ap.add_argument('--output',type=Path);ap.add_argument('--origin',default='600 0 2600');ap.add_argument('--max-distance',type=float,default=200.);ap.add_argument('--min-horizontal',type=float,default=1.);ap.add_argument('--min-travel',type=float,default=1.)
    a=ap.parse_args();result=analyze(a.run_root,a.mode,a.console,a.profile,vector(a.origin),a.max_distance,a.min_horizontal,a.min_travel)
    text=json.dumps(result,indent=2)+'\n'
    if a.output: a.output.write_text(text,encoding='utf-8')
    print(json.dumps({'telemetry':result['deterministicTelemetry']['status'],'captures':result['captureAvailability']['status'],'lifecycle':result['lifecycle']['status'],'visibleCloud':result['visibleCloud'],'overall':result['overallFeatureReadiness'],'output':str(a.output) if a.output else None}) if a.output else text,end='\n' if a.output else '')
    return 2 if result['deterministicTelemetry']['status']=='FAIL' else (0 if result['deterministicTelemetry']['status']=='PASS' else 3)
if __name__=='__main__': raise SystemExit(main())

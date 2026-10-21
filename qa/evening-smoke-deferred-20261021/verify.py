"""Offline identity and one-shot deferred launch protocol checks; never launches native software."""
from pathlib import Path
import argparse,hashlib,json,re,sys
qa=Path(__file__).resolve().parent
ap=argparse.ArgumentParser();ap.add_argument('--output',type=Path);args=ap.parse_args()
checks={}
manifest=json.loads((qa/'SOURCE_MANIFEST.json').read_text(encoding='utf-8'))
for name,sha in manifest.items():
    p=qa/name;checks['sha256:'+name]=p.is_file() and hashlib.sha256(p.read_bytes()).hexdigest()==sha
s=(qa/'source-overrides/scripts/Game/GEP015Stock/GEP_StockSmoke015.c').read_text(encoding='utf-8')
checks['oneLaunch']=s.count('move.Launch(')==1
checks['oneSetLive']=s.count('fuse.SetLive();')==1
checks['noNativeCallInsideDispatch']='move.Launch(' not in s[s.index(' int Dispatch('):s.index(' protected void AbortDeferred015')]
checks['laterWorldTimestamp']='if(now<=m_fDeferredQueuedMs015)return;' in s
checks['latchAndClearBeforeNative']=s.index('m_iDeferredStage015=2;ClearEventMask')<s.index('move.Launch(')
checks['issuedLatchBeforeVoidCall']=s.index('m_bDeferredNativeIssued015=true;')<s.index('move.Launch(')
checks['nullWeaponAndCapturedRoot']='m_DeferredRoot015,null,null);' in s
checks['noOtherNativeLifecycleMutation']=not re.search(r'\.EnableSimulation\(|ChangeSimulationState\(|SetActive\(|SetOrigin\(|SetTransform\(|SetTimer\(|SpawnParticleEffect\(',s)
checks['positionGuardNoRepair']='if(positionError>2)' in s
checks['noMaskRefund']='m_iLoadedMask=m_iLoadedMask |' not in s[s.index(' int Dispatch('):s.index(' protected void InspectRound')]
checks['salvoRejected']='if (!singleRound)' in s
checks['ownerBeforeClock']=s.index('owner_missing_before_clock')<s.index('float now=owner.GetWorld()')
checks['muzzlePitchUnchanged']='Vector(sign * (10 + 10 * ordinal), -30, 0)' in s
stripped=re.sub(r'//[^\n]*|"[^"\n]*"','',s);checks['bracesBalanced']=stripped.count('{')==stripped.count('}')
result={'status':'PASS' if all(checks.values()) else 'FAIL','scope':'OFFLINE_IDENTITY_AND_SOURCE_CONTRACT_ONLY','nativeCompile':'NOT_TESTED','gameplay':'NOT_TESTED','checks':checks}
if args.output:args.output.write_bytes((json.dumps(result,indent=2)+'\n').encode('utf-8'))
print(json.dumps({'status':result['status'],'checks':len(checks),'failed':[k for k,v in checks.items() if not v],'native':'NOT_TESTED'},indent=2));sys.exit(0 if result['status']=='PASS' else 1)

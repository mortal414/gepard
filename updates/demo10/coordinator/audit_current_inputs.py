from pathlib import Path
import hashlib,json
C=Path(__file__).resolve().parent;R=Path(r'D:\ArmaProjects\Gepard-Consolidated-20261022');T=Path(r'D:\ArmaProjects\Gepard-Demo10-TrackArt-20261022')
def load(p):return json.loads(p.read_text(encoding='utf-8-sig'))
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
pins=load(R/'qa/consolidated-20261022/private-final-source-v6/CurrentSourceManifest-gepard.json')
actual={p.relative_to(T/'addon').as_posix():sha(p) for p in (T/'addon').rglob('*') if p.is_file()}
missing=sorted(set(pins)-set(actual));extra=sorted(set(actual)-set(pins));changed=[p for p,h in pins.items() if p in actual and actual[p]!=h]
original=R/'private-smoke-demo-source-v2/single/gepard'
originalDiff=[p for p,h in pins.items() if sha(original/p)!=h]
record={'qualifiedCoreFiles':len(pins),'worktreeCoreFiles':len(actual),'worktreeMissing':missing,'worktreeExtra':extra,'worktreeChanged':changed,'qualifiedOriginalDiff':originalDiff,'status':'EXACT_CORE_CHECKOUT' if not (missing or extra or changed) else 'CHECKOUT_BYTE_DIFFERENCES_REQUIRE_SOURCE_BINDING_REVIEW'}
(C/'CURRENT-INPUT-AUDIT.json').write_text(json.dumps(record,indent=2)+'\n')
print(json.dumps({**record,'worktreeChanged':changed[:12]}))

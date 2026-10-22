from pathlib import Path
import json,hashlib
OUT=Path(__file__).resolve().parent
c=json.loads((OUT/'COMPARISON.json').read_text())
lines=['# Legacy WORLD diagnostics comparison — source-only', '',
'**Proposal: classify the seven startup-core-02 WORLD(E) diagnostics as exact legacy baseline matches. No new WORLD(E) signature was found.** The recorded strict startup result remains **FAIL**. This report changes no collector, whitelist, result, source or runtime state.', '',
f"Audit UTC: `{c['auditUtc']}`. All new files are under `{OUT}` on C:. The full machine-readable report is `COMPARISON.json`; raw logs are retained in their original D: locations, identified below by SHA256 and exact line numbers.", '',
'## Rank 1 — exact retained native error and resource-chain evidence', '',
'The startup errors occur while native logs load stock M151A2, BRDM2 and BTR70 prefabs, before proving-ground scene construction. Matching below requires the error kind, keyword, decimal offset and enclosing prefab GUID/name; timestamps and indentation are not treated as semantic differences. Every startup signature matches all six compared historical consoles, including both 1.0.2 source runs, packaged test and the exact uploaded trio.', '',
'| Keyword and decimal offset | Enclosing stock prefab | Startup line | Source-02 line | Packaged line | Exact uploaded line |',
'| --- | --- | --- | --- | --- | --- |']
for e in c['startup']['errors']:
    short=e['enclosingPrefab'].split('}')[1]
    lines.append('| '+f"`{e['keyword']}` {e['offsetDecimal']} | `{short}` | {e['line']} | "+', '.join(map(str,e['matchingBaselines']['gm-source-02']))+' | '+', '.join(map(str,e['matchingBaselines']['gm-package-01']))+' | '+', '.join(map(str,e['matchingBaselines']['gm-uploaded-01']))+' |')
lines += ['', 'Exact stock GUIDs:', '',
'- M151A2: `{F649585ABB3706C4}Prefabs/Vehicles/Wheeled/M151A2/M151A2.et`.',
'- BRDM2: `{532B5F3CF8496E59}Prefabs/Vehicles/Wheeled/BRDM2/BRDM2_base.et`.',
'- BTR70: `{1C5FE7B7FF49BB8D}Prefabs/Vehicles/Wheeled/BTR70/BTR70_Base.et`.', '',
'| Console | WORLD(E) total | SHA256 |', '| --- | --- | --- |']
for label,run in [('startup-core-02',c['startup']),*c['baselines'].items()]:
    lines.append(f"| {label} | {run['worldErrorCount']} | `{run['console']['sha256']}` |")
lines += ['', 'Console paths:', '']
for label,run in [('startup-core-02',c['startup']),*c['baselines'].items()]:lines += [f"- {label}: `{run['console']['path']}`."]
lines += ['',
'Historical consoles also contain other WORLD(E) diagnostics: `m_bShowDebugShape` (offset 2307) in all six historical logs, plus the private catalogless fixture’s `SCR_PlaceableEntitiesRegistryFromCatalog` unknown class (offset 7583). These are retained separately, absent from the seven startup diagnostics, and confer no blanket exemption. Other error channels are outside this narrow classification.', '',
'## Rank 2 — exact released Gepard fields and source identity', '',
'All **66 original released Gepard ET files** match the released manifest and the actual source bytes, startup-core-02’s frozen Gepard source manifest, the current native stage and current production source. No standalone `Parent` field and no `SlidingTrackMaterial` field occurs in any of those 66 ETs. This proves the current seven errors do not originate from newly added occurrences of those fields in original Gepard ET files. It does not decode the stock PAK payloads.', '',
'Critical original resource hashes, identical across those four comparisons:', '',
'| Resource | SHA256 |', '| --- | --- |']
critical=['Prefabs/Vehicles/GEP_MobileCarrier.et','Prefabs/Vehicles/GEP_WheeledMobility101.et','Prefabs/Vehicles/GEP_TrackedMobility.et','Prefabs/Weapons/GEP_VehicleTurret.et','Prefabs/Weapons/GEP_StationaryTurret.et']
for name in critical:
    r=next(x for x in c['sourceComparison']['originalETRows'] if x['relativePath']==name)
    lines.append(f"| `{name}` | `{r['releaseSha256']}` |")
lines += ['',
'Do not describe the entire core as byte-identical to release: of 1112 common files, 1104 match startup pins and eight differ—seven intentional control/HUD scripts plus generated `resourceDatabase.rdb`. No original ET is among those differences. The full per-file ET hashes and exact eight-file list are in `COMPARISON.json`.', '',
'## Rank 3 — source recipe and native environment corroboration', '',
'The startup plan resolves private provingGround GUID `A76A420022000001`, base Reforger `58D0FB3206B6F859`, core `5614BBCCBB55ED1C`, and one source Gepard `D561AD08932E7B40` at `native-stage/gepard`. The source closure names the frozen 1147-file manifest; compile-adoption receipt is source identity evidence only. The historical exact-uploaded test resolves its packed Gepard and QA102 probe against the installed base game. Their log engine version is the same `192142`, and stock package counts are the same 14 data + 2 core.', '',
f"Startup Gepard manifest: `{c['sourceRecipe']['startupGepardManifest']['path']}`, SHA256 `{c['sourceRecipe']['startupGepardManifest']['sha256']}`.",
f"Released manifest: `{c['sourceRecipe']['releaseSourceManifest']['path']}`, SHA256 `{c['sourceRecipe']['releaseSourceManifest']['sha256']}`.", '',
'Historical full stock PAK/executable byte identity is **not established** by these launch plans. Same engine version/GUID/path/package counts are corroboration, not a retrospective full-binary hash. Actual stock ET field payload bytes were not decoded; resource attribution is bounded to the directly logged prefab-load chain and identical error offsets. If a literal stock-field payload proof is required, that remains missing evidence rather than an invented hash.', '',
'## Classification boundary and next decision', '',
'- `SlidingTrackMaterial` at 19441 under the exact M151A2 GUID/name: **LEGACY_EXACT_BASELINE_MATCH**.',
'- `Parent` at 35955/36915 under the exact BRDM2 GUID/name: **LEGACY_EXACT_BASELINE_MATCH**.',
'- `Parent` at 42646/43645/44617/45590 under the exact BTR70 GUID/name: **LEGACY_EXACT_BASELINE_MATCH**.',
'- Different keyword/offset/resource or any additional WORLD(E): **NEW_OR_UNEXPLAINED**, outside this proposal.', '',
'Baseline matching does not prove harmlessness, driving/service/damage correctness, playable-build acceptance or permission to soften a gate. Raw native diagnostics and the strict startup FAIL remain intact. The integrator can review this bounded classification alongside other evidence; no production fix is proposed without an attributable new source defect.', '',
'Reproduce: run `compare_legacy.py`, then `render_report.py`, using the installed Python 3.11 interpreter. Both scripts read D: evidence/source and write only to this C: report folder. No native launch or collector mutation is performed.', '']
(OUT/'REPORT.md').write_text('\n'.join(lines),encoding='utf-8')
# Verify every retained log and source-recipe input still matches its recorded snapshot pin.
pins=json.loads((OUT/'INPUT-PINS.json').read_text())
mismatch=[]
for item in pins.values():
    path=Path(item['path'])
    if not path.exists() or hashlib.sha256(path.read_bytes()).hexdigest()!=item['sha256']:mismatch.append(str(path))
verification={'status':'PASS' if not mismatch else 'INPUT_CHANGED','checkedPins':len(pins),'changedInputs':mismatch,'productionWrites':False,'nativeActions':False,'collectorChanges':False,'strictStartupStatusRetained':'FAIL'}
(OUT/'VERIFICATION.json').write_text(json.dumps(verification,indent=2)+'\n',encoding='utf-8')
print(json.dumps({'report':str(OUT/'REPORT.md'),**verification},indent=2))

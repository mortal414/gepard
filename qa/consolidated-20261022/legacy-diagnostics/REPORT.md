# Legacy WORLD diagnostics comparison — source-only

**Proposal: classify the seven startup-core-02 WORLD(E) diagnostics as exact legacy baseline matches. No new WORLD(E) signature was found.** The recorded strict startup result remains **FAIL**. This report changes no collector, whitelist, result, source or runtime state.

Audit UTC: `2026-10-22T02:44:01.376522+00:00`. All new files are under `C:\Users\marco\Documents\Codex\2026-10-21\task\legacy-diagnostics-20261022` on C:. The full machine-readable report is `COMPARISON.json`; raw logs are retained in their original D: locations, identified below by SHA256 and exact line numbers.

## Rank 1 — exact retained native error and resource-chain evidence

The startup errors occur while native logs load stock M151A2, BRDM2 and BTR70 prefabs, before proving-ground scene construction. Matching below requires the error kind, keyword, decimal offset and enclosing prefab GUID/name; timestamps and indentation are not treated as semantic differences. Every startup signature matches all six compared historical consoles, including both 1.0.2 source runs, packaged test and the exact uploaded trio.

| Keyword and decimal offset | Enclosing stock prefab | Startup line | Source-02 line | Packaged line | Exact uploaded line |
| --- | --- | --- | --- | --- | --- |
| `SlidingTrackMaterial` 19441 | `Prefabs/Vehicles/Wheeled/M151A2/M151A2.et` | 164 | 162 | 162 | 165 |
| `Parent` 35955 | `Prefabs/Vehicles/Wheeled/BRDM2/BRDM2_base.et` | 171 | 167 | 167 | 170 |
| `Parent` 36915 | `Prefabs/Vehicles/Wheeled/BRDM2/BRDM2_base.et` | 172 | 168 | 168 | 171 |
| `Parent` 42646 | `Prefabs/Vehicles/Wheeled/BTR70/BTR70_Base.et` | 179 | 173 | 173 | 176 |
| `Parent` 43645 | `Prefabs/Vehicles/Wheeled/BTR70/BTR70_Base.et` | 180 | 174 | 174 | 177 |
| `Parent` 44617 | `Prefabs/Vehicles/Wheeled/BTR70/BTR70_Base.et` | 181 | 175 | 175 | 178 |
| `Parent` 45590 | `Prefabs/Vehicles/Wheeled/BTR70/BTR70_Base.et` | 182 | 176 | 176 | 179 |

Exact stock GUIDs:

- M151A2: `{F649585ABB3706C4}Prefabs/Vehicles/Wheeled/M151A2/M151A2.et`.
- BRDM2: `{532B5F3CF8496E59}Prefabs/Vehicles/Wheeled/BRDM2/BRDM2_base.et`.
- BTR70: `{1C5FE7B7FF49BB8D}Prefabs/Vehicles/Wheeled/BTR70/BTR70_Base.et`.

| Console | WORLD(E) total | SHA256 |
| --- | --- | --- |
| startup-core-02 | 7 | `0878b21f1d9b0450732d20abbc765b7168039aaf9a4466643e71ae6b0b39dc45` |
| gm-baseline-01 | 8 | `cc480056a3113b58f4a27e69aa3239f777557f196d9f3fc4090573c361261561` |
| gm-source-01 | 8 | `9e52ec221ffe82be915211f8bcd90a0cc51c0bef778d480da3623b654545094f` |
| gm-source-02 | 8 | `3cafebf54278108b8fb774e5323098d82d3d5c3d8c84c948ffddad80630d0038` |
| gm-package-01 | 8 | `bf6f7c7b5b3375d128996c0e27bf6c887377ed153f89dbed611651a3fb1b1558` |
| gm-uploaded-01 | 8 | `aae7d33e8124ac3173e6cd64f9a52252fa11d4b61d7fed3610c2dcb2bc01ad3c` |
| gm-catalogless-01 | 9 | `27f98b01a1e3dee35d77a41be3a6864d0cdf48792127558c420541487dcabe18` |

Console paths:

- startup-core-02: `D:\ArmaProjects\Gepard-Consolidated-20261022\native-runs\startup-core-02\gpg-startup-survey\native-run\logs\console.log`.
- gm-baseline-01: `D:\ArmaProjects\Gepard-GM-Hotfix-1.0.2-20261021\native-runs\gm-baseline-01\logs\console.log`.
- gm-source-01: `D:\ArmaProjects\Gepard-GM-Hotfix-1.0.2-20261021\native-runs\gm-source-01\logs\console.log`.
- gm-source-02: `D:\ArmaProjects\Gepard-GM-Hotfix-1.0.2-20261021\native-runs\gm-source-02\logs\console.log`.
- gm-package-01: `D:\ArmaProjects\Gepard-GM-Hotfix-1.0.2-20261021\native-runs\gm-package-01\logs\console.log`.
- gm-uploaded-01: `D:\ArmaProjects\Gepard-GM-Hotfix-1.0.2-20261021\native-runs\gm-uploaded-01\logs\console.log`.
- gm-catalogless-01: `D:\ArmaProjects\Gepard-GM-Hotfix-1.0.2-20261021\native-runs\gm-catalogless-01\logs\console.log`.

Historical consoles also contain other WORLD(E) diagnostics: `m_bShowDebugShape` (offset 2307) in all six historical logs, plus the private catalogless fixture’s `SCR_PlaceableEntitiesRegistryFromCatalog` unknown class (offset 7583). These are retained separately, absent from the seven startup diagnostics, and confer no blanket exemption. Other error channels are outside this narrow classification.

## Rank 2 — exact released Gepard fields and source identity

All **66 original released Gepard ET files** match the released manifest and the actual source bytes, startup-core-02’s frozen Gepard source manifest, the current native stage and current production source. No standalone `Parent` field and no `SlidingTrackMaterial` field occurs in any of those 66 ETs. This proves the current seven errors do not originate from newly added occurrences of those fields in original Gepard ET files. It does not decode the stock PAK payloads.

Critical original resource hashes, identical across those four comparisons:

| Resource | SHA256 |
| --- | --- |
| `Prefabs/Vehicles/GEP_MobileCarrier.et` | `d8513ab4636c1940fb65ef3128d60e3ffc607622ed301deba21c3860affd7543` |
| `Prefabs/Vehicles/GEP_WheeledMobility101.et` | `e2f0458ad83877a79f988627908a2d325b348547207f84c64a21f5f93e7ad912` |
| `Prefabs/Vehicles/GEP_TrackedMobility.et` | `6b539cb6e8377d7214113233e9f41fd49ed9a642477c8f66a11c2c92412e6521` |
| `Prefabs/Weapons/GEP_VehicleTurret.et` | `3d4ff9a29a4689777172baf576fe69f8b17f8332a42c98faa5949516750d14ed` |
| `Prefabs/Weapons/GEP_StationaryTurret.et` | `501444c9b1a851a00a86ecc02b8a2a61e2fafe74067c555715bd9675c31caa55` |

Do not describe the entire core as byte-identical to release: of 1112 common files, 1104 match startup pins and eight differ—seven intentional control/HUD scripts plus generated `resourceDatabase.rdb`. No original ET is among those differences. The full per-file ET hashes and exact eight-file list are in `COMPARISON.json`.

## Rank 3 — source recipe and native environment corroboration

The startup plan resolves private provingGround GUID `A76A420022000001`, base Reforger `58D0FB3206B6F859`, core `5614BBCCBB55ED1C`, and one source Gepard `D561AD08932E7B40` at `native-stage/gepard`. The source closure names the frozen 1147-file manifest; compile-adoption receipt is source identity evidence only. The historical exact-uploaded test resolves its packed Gepard and QA102 probe against the installed base game. Their log engine version is the same `192142`, and stock package counts are the same 14 data + 2 core.

Startup Gepard manifest: `C:\Users\marco\Documents\Codex\2026-10-21\task\GEP-ProvingGround-Startup-Retry-QA-20261022\GEPARD_SOURCE_MANIFEST.json`, SHA256 `2af0052f5f726504f438aeca8cdc726ba809391145c3b2ffbd90a5ec11818c7e`.
Released manifest: `D:\ArmaProjects\Gepard-GM-Hotfix-1.0.2-20261021\public-source\SOURCE_MANIFEST.json`, SHA256 `c5bbc4d425a57592a133ce20be3b326d59a66ea53fab2de3c45b7ad3b7f2436a`.

Historical full stock PAK/executable byte identity is **not established** by these launch plans. Same engine version/GUID/path/package counts are corroboration, not a retrospective full-binary hash. Actual stock ET field payload bytes were not decoded; resource attribution is bounded to the directly logged prefab-load chain and identical error offsets. If a literal stock-field payload proof is required, that remains missing evidence rather than an invented hash.

## Classification boundary and next decision

- `SlidingTrackMaterial` at 19441 under the exact M151A2 GUID/name: **LEGACY_EXACT_BASELINE_MATCH**.
- `Parent` at 35955/36915 under the exact BRDM2 GUID/name: **LEGACY_EXACT_BASELINE_MATCH**.
- `Parent` at 42646/43645/44617/45590 under the exact BTR70 GUID/name: **LEGACY_EXACT_BASELINE_MATCH**.
- Different keyword/offset/resource or any additional WORLD(E): **NEW_OR_UNEXPLAINED**, outside this proposal.

Baseline matching does not prove harmlessness, driving/service/damage correctness, playable-build acceptance or permission to soften a gate. Raw native diagnostics and the strict startup FAIL remain intact. The integrator can review this bounded classification alongside other evidence; no production fix is proposed without an attributable new source defect.

Reproduce: run `compare_legacy.py`, then `render_report.py`, using the installed Python 3.11 interpreter. Both scripts read D: evidence/source and write only to this C: report folder. No native launch or collector mutation is performed.

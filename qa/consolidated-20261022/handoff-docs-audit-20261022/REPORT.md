# Current handoff documentation audit

**Ready: two-file documentation proposal and exact manifest recommendations.** No D files, Git state, native sessions, packages, launchers or acceptance results were changed.

The audited candidate is ordinary-v6 / compile06 / pack08 / installed08. Parent independently confirmed commit `bfdb732` clean. All 3,734 explicit file/hash checks passed (2,426 distinct files); all 42 local Markdown links resolve. Exact file-set checks passed for the canonical 1,152-file addon and 10-file world, ordinary-v6's 1,152 + 11 files including its generated world RDB, and the six installed package files. These are source/pin checks, not new gameplay tests.

| Handoff inconsistency | Bounded correction |
|---|---|
| Q README opens with left-only refill and cloud/full-empty pending, while its later paragraphs correctly describe both refills, right fuel, Hull repair and isolated smoke06. | Update those opening summaries; retain whole-service/manual gates. Proposed exact bytes are in `proposal/qa/consolidated-20261022/README.md`. |
| Root EVIDENCE-INDEX contains a corrupted ammo arrow, says cloud remains untested, and the Visual04 paragraph still calls full-eight/empty a next gate despite the appended full06 observation. | Label angle03 historical, use ASCII `8-to-7`, and point to later Visual04/full06 observations while keeping rearm, current V11 integration and useful-screening judgment pending. Exact proposal is `proposal/EVIDENCE-INDEX.md`. |
| Both current candidate manifests retain `evidence.ordinary-gepard` and `evidence.ordinary-provingGround` aliases pointing to source-v5. Authoritative `ordinarySource`, compile and installed08 fields correctly use v6. | Redirect the two aliases to the existing current v6 manifests. Core manifest bytes are equal; world manifest bytes differ (`295483...` historical v5 versus `593b20...` v6). This is an ambiguous metadata alias, not evidence that installed08 is the old world. |
| Both manifests copy Visual04's historical `remaining[0]` (“Full eight ... and empty negative”) alongside the later isolated full06 observation. | Explicitly label that item as the Visual04-era snapshot subsequently observed in isolated full06. Preserve the raw Visual04 report and current-integration gate. |

`RECOMMENDATIONS.json` contains exact preimages, proposed hashes and JSON pointers. `DOCS-ONLY.diff` changes only two Markdown files. Root should guard preimages and preserve backups before adoption. If manifest recommendations are adopted, finalize the root manifest first, then update Q's `fullCurrentManifest.sha256` to those final bytes; do not leave a stale full-manifest pointer. No automatic D apply helper is provided.

The main README is current, including its explicit service-supplement override. The frozen TEST-GUIDE has the expected sealed hash and valid UTF-8; its older service sentence is intentionally superseded by `Q/repair-observation-v4/MANUAL-SUPPLEMENT.md`. Leave that guide and the launcher unchanged. The whole-service A03 outcome remains incomplete despite observed subcriteria; do not turn it into acceptance. A02's Package07 world citation is labeled historical and can optionally gain the Package08 link; it does not invalidate the measured geometry or current package proof. STATUS-PER-GEP already describes the current narrow service/smoke observations and remaining gates.

## Verified Package08 pins

| Artifact | Actual SHA-256 |
|---|---|
| Root current candidate manifest | `df6988b3ce160863b2d1193939d9dbd322d0e785aaf6957203d83c2247659bbb` |
| Ordinary-v6 Gepard source manifest | `27034b480c00f7640607d35cd00d43a226febb0148bf92df6d37aa3bd5db1b09` |
| Ordinary-v6 world source manifest | `593b20c11c708d04ba7468b5c36c49390b2813d9ad443de67e9b89e24fc3b2de` |
| Compile06 adoption receipt | `2162584d833731de11681868d42dd6241313981ea30e61cf65a12aff50d38841` |
| Installed package closure | `8a8728e1177495308aef4d482fb0105db1e0b410fde947f1e1eef23fb80e560d` |
| Gepard data.pak (build and installed) | `411aed2752653471015e385412d7abf4e2cd037b313401c1b5dd09ee42533519` |
| World data.pak (build and installed) | `a63681487f900dd5b47747573d1accb25a977d28e7a494a85154ba86932fcd3d` |
| Installed guarded PS1 | `91f0eb0ffe086f67dec485a570024400b3e8e548246852b9d60e249f544556d6` |
| Installed CMD shim | `d3940d6d3d741970b266a1f03c222cae7071181fb81e3adbc48e19d58d7e05df` |
| Frozen installed guide | `e5d6dc8e6615d71169bc30106e140b7a8054a9a0d6843a95f0905177daa7c209` |

The pre-existing `PACKAGE08-LAUNCH-PLAN-RECHECK-20261022.json` points to manual plan `4a45e727-f498-4505-81c5-56b1a1af07aa`, SHA `bc8786a8b769e2e5ac7aed275af3626ded19759950be44d1233709cc3c961a88`. Its plan, installed closure and four native-tool pins match actual files. It selects only installed08 plus stock addons, the two expected addon GUIDs and `Worlds/GPG_Arland.ent`, without force-focus. `keepOpen=true` and the manual close policy are untimed; the wrapper rejects a manual probe timeout. The receipt's native-pending status is a plan-only scope limit, not a replacement for the already recorded ordinary08 probe.

`AUDIT.json` contains the individual checks and snapshots. `PROPOSAL-TESTS.json` confirms exact two-file scope, valid UTF-8, resolving proposed links, preserved strict wording, and unchanged D handoff preimages. Reproduce in a new C output folder by copying the two Python scripts there and running `audit.py`, then `prepare_proposal.py`; retain this sealed report unchanged.

Strict native outcomes remain unchanged: ordinary08 FAIL59, package validate/pack FAIL100, relevant service FAIL57 and isolated smoke06 FAIL58. No new human judgment, service-wide acceptance, current V11 smoke integration, audio implementation, tracker checkbox or publication is claimed.

The child's read-only `git status`/`rev-parse` against `D:\ArmaProjects\Gepard-Consolidated-20261022` was denied by access controls. It was not retried or bypassed. File audit completed; Git cleanliness is parent-provided evidence. D adoption of these documentation recommendations remains pending with root.

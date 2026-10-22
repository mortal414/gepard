# Package08 diagnostic severity audit

No demonstrated demo-blocking regression was found in the retained diagnostics. Package08 remains unchanged. This is a source/evidence classification, not a clean-native pass or user acceptance.

| Actual errors | Attribution and observed consequence | Decision |
|---|---|---|
| Startup GUI: 1 | Official IntroSplashScreen.layout references SCR_WidgetExportRuleRoot before Game scripts compile. Installed source describes layout-code export parameters. The later world/player render succeeds. Early class availability is an inference from ordering. | No Gepard HUD edit justified. |
| Startup WORLD: 7 | Official faction-catalog preloads: M151A2 SlidingTrackMaterial (1), BRDM2 wheel Parent fields (2), BTR70 wheel Parent fields (4). Exact installed resource GUIDs, source hashes and lines were checked; neither candidate addon overrides these resources. These are not Gepard world-field serialization errors. | Preserve stock resources/catalogs. Other affected stock vehicle behavior is not qualified by this audit. |
| Shutdown SCRIPT: 2 | Both catalog-manager messages occur 42/39 ms before Game destroyed, none during the ready interval; package07 has the same two. Independent source audit verifies an inherited catalog manager already exists. BaseItem support code can revalidate during deletion; exact caller/owner remains unlogged. Actual V9 left refill worked. | Teardown-order inference, no demonstrated live catalog omission. Do not add a duplicate manager. Right refill/fuel/repair/repeat gates remain pending. |
| After Game destroyed: 49 | Header plus 48 stock UI font/texture/atlas resource entries. The client exits normally with the lease released and an actual rendered player world. | Retain cleanup diagnostics. No long-session memory/performance claim. |
| Four packing steps: 100 | 25 errors per step, all after two Game destroyed markers, each exact ordered signature list from the tested 1.0.2 validate/pack baseline. All four operations completed and exited 0 normally. | Operational build success with strict raw FAIL100 preserved. |

The ordinary probe's strict FAIL59 is preserved. No suppression, stock-data edit, catalog deletion, unqualified pass or new GEP acceptance was introduced. No successor code fix is warranted on this evidence.

`RESULT.json` contains every startup error with raw line/time/context, four packing timing records, baseline paths/hashes, and source-attribution receipts. `STOCK-RESOURCE-PROVENANCE.json` maps the four official stock assets to installed PAK/index/resource GUID evidence. The separate catalog audit is referenced with its sealed report hash. The actual package08 probe remains the source of rendered startup evidence; this audit launches nothing.

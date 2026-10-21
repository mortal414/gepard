# Evening playtest

Recommended candidate: local branch `sprint/evening-20261021`, based on verified release 1.0.2. Default and charcoal retain the accepted driving/R/brakes and shipped sight. No new art, smoke, track loop or custom gun sounds are enabled in the production addon.

Choose **Quick (5 minutes)**: E01–E03 once; **Standard (10–12 minutes)**: all cases, three seat cycles; **Extended**: Standard plus the separately prepared damage regression and bright/dark scenes. Use your current bindings and the binding displayed by the game; do not reset your controls.

| Case / GEP | Steps | Expected result / useful reply |
|---|---|---|
| E01 / 010,020,021 | Driver: enter, use hatch/periscopes, press F to leave the periscope, exit/re-enter. Repeat three times; stop/restart with R and briefly drive/reverse/brake. | No lockup or lost controls. Leave-periscope text stays hidden by design; F still exits. Reply **works / stuck in view / F fails / controls missing**. Accepted driving is a regression check, not a new tuning choice. |
| E02 / 005,006,008,010,024 | Gunner: sight/cabin, range ground with C, range a different distance, then clear sky; fire each feed/group. Exit/re-enter and switch seats; repeat. Try both appearances. | Fresh correct range or explicit no-return; no stale range after sky/scope loss. HUD and both guns return. No unexpected diagnostic text. Reply **works / stale range / missing HUD / duplicate HUD / lost gun control**. Range display does not imply ballistic correction. |
| E03 / 020,021,022 | Commander: enter/switch, inspect hold-F actions, hatch and each periscope; leave each view and return to gunner/driver. Check seat, head and view relative to hatch/roof. | Actions remain reachable and controls restore. Internal switch says **Move to commander**. Reply alignment **good / too high / too low / too far forward / side offset**, plus which view. Hidden Leave-periscope text alone is expected. |
| E04 / 023 | On foot with ordinary fuel equipment, approach both authored fuel contexts from ground level. Identify the sensible refuel side and perform a real transfer. | Reach and occlusion work without climbing through the hull; actual fuel changes. Reply **left / right / both sensible / unreachable**, with location. No side was guessed or moved in this sprint. |
| E05 / 024 | Inspect normal driver/commander/gunner views, transitions, ADS off/on, and exit/re-entry. Include a screenshot of any unwanted text. | Release-facing UI has no unintended debug/status overlay. Bearing/elevation/ammo/range instruments are intentional sight content; Marco decides visual polish. Reply **clean / unwanted text in [seat/view]**. |
| E06 / 006,013,020 | Optional: use the fresh bounded fragment regression, then ordinary controls on the surviving crew. Repeat exit/re-entry, traverse and both feeds. | Surviving crew leave and re-enter valid seats; functioning equipment resumes. One automated 2 m fragment case passed. This does not establish all hull/turret destruction states. |

Do not spend the playtest on smoke or track audio yet. Smoke diagnostic failed flight/fuse/cloud qualification; track audio has no qualified audible graph. Their experimental source and exact next gates are retained separately. Stabilization and ballistic correction remain disabled and unfinished.

Current manual scene is `Worlds/GEP_Update101.ent` in the separately frozen `evening-final/native-stage/gepard` fixture. The legacy **101** identifier names the recovered scene, not the addon version; labels now say **1.0.2 evening**. It starts on foot beside current camouflage and charcoal, with ordinary player camera and no automatic boarding/driving. The owner must admit and launch it when Marco is ready. No current live game is promised by this document.

Leave checklist boxes unchecked until the requested behavior and Marco's judgment are satisfied. Record case, appearance, seat, expected/actual result and a screenshot for a failure.

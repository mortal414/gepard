# Gepard evening functional candidate

Current entrypoint: `qa/evening-20261021/`. Read this file and `MANUAL-TEST.md`; older lane READMEs and source/static receipts are historical checkpoints.

The recommended candidate changes four crew scripts only: conscious/exact-occupant control guards and **Move to commander** wording. Driver input/physics, R/brakes, HUD/range logic, hatch/exit actions, default/charcoal artwork and stock audio remain exact released bytes. The intentional hidden Leave-periscope presentation override is preserved. No publication, push or tracker mutation occurred.

Verified baseline: local and remote Gepard main/release/1.0.2 `699f245afaf3e912b9ce9827b0f3f5f7dd32f3e7`. Hotfix HANDOFF/FINAL_RESULT confirm the uploaded trio and actual packaged GM tests. Installed harness and remote main both `1ec5bb72fb2a6e78da865312190477c14e8e421f`, initially clean. The live tracker was read; its GEP-027 publication notes are stale. No attempt to republish or infer public-page visibility.

Four parallel owners prepared crew, stock-only smoke, track-audio and passive range lanes; the integrator reviewed and selected changes. All work is isolated under `D:/ArmaProjects/Gepard-EveningSprint-20261021`. Released worktrees and accepted settings were preserved. Local experiment branches and failed native stages remain available.

## Measured evidence

- `qa-compile-01`: integrated crew/recovered-smoke scripts plus original private fixtures compiled successfully; normal exit/lease release.
- `evidence/healthy-batch-01`: default and charcoal independently PASS actual native right/left traverse, both feeds, public exit/reentry and post-reentry fire. Fresh scenes, native nonce/result records, complete source hashes and normal exits. Inspected after captures show the rendered sight, two ammunition readouts and no loading screen; complete readability/physical input acceptance remains manual.
- `evidence/fragment-batch-01`: one native 35 mm fragment stimulus at 2 m produced five real damage callbacks; surviving crew traversed, fired both feeds, exited/re-entered and fired again. Healthy same-build input control was pinned first. No armor realism or all damaged states claim.
- `diagnostic-compile-01` FAIL retained: new QA observer used an engine type name as a variable and wrong override parameter name. Both were corrected; `diagnostic-compile-02` PASS also compiles the passive range helper.
- `smoke-diagnostic-01` CRASH retained: exact crash stack points at recovered `GEP_StockSmoke015.Dispatch` native Launch line 107. A turret entity had been supplied as the weapon argument. A separate repair uses the null weapon argument seen in the qualified projectile fixture; `smoke-repair-compile-01` PASS.
- `smoke-diagnostic-02` is a failed functional diagnostic despite normal exit: action eligibility and one callback occurred, ammo changed 8 to 7, but observed grenade origin stayed zero, fuse stalled near 0.967 s and never triggered. Inspected 0.25/5/12 s captures showed no cloud. The apparent 2668 m displacement is invalid origin data, not flight. Observer terminal rows repeat; they are not a pass or proof of completion. Ordinary thrown-stock positive control, clearance, rearm, concealment and MP/JIP remain unqualified.

Smoke is therefore moved outside `addon/` into `qa/evening-smoke-20261021/source-candidate`. `experiment/evening-smoke-native-20261021` preserves the tested repair checkpoint. Do not use older smoke staging commands that assume those six files are in production. `prepare_native.py --include-smoke` creates an explicitly experimental diagnostic stage when further engineering is scheduled. No WCS source/assets/dependency were introduced.

Track audio remains QA-only: stock loop recordings exist, but there is no qualified resource GUID/original native audio graph or audible implementation. The optional adapter does not make sound. Stock engine, wheel and gun graphs remain intact. No renewed custom cannon-audio effort.

Range helper compiled in diagnostic fixtures; it is passive and unbound. GEP-008 range/sky/lifecycle tests are prepared. GEP-007 and GEP-009 remain disabled in released source; table lookups do not establish barrel actuation or projectile impacts.

The final reduced stage is `evening-final/`. See `NATIVE-EVIDENCE.json`, `SOURCE-REGRESSION.json`, `STATUS.json` and final native run receipts for exact current results and hashes. Historical native stages include experimental smoke; only `evening-final` is the recommended manual candidate.

## Repeatable automation

Use the installed Python `C:/Users/marco/AppData/Local/Programs/Python/Python311/python.exe` and PowerShell 7 `C:/Users/marco/.cache/codex-runtimes/codex-primary-runtime/dependencies/native/powershell/pwsh.exe`.

1. `verify_candidate.py --repo <integration> --baseline <hotfix/public-source> --output <new-check.json>`: hashes all 1112 released addon files, requires exactly the four reviewed runtime edits and no new production resources, and verifies hidden periscope UX plus driving/art/audio/project preservation.
2. `prepare_native.py --repo <integration> --output <fresh-root> --prior-qa <DriveHud/updates/release-candidate-101-r2-20261021/qa> --gm-tools <hotfix/tools> --harness <installed-harness> --python <python.exe>`: creates a fresh full-source private fixture and job; no native launch. Adapter starts disabled.
3. Run `<fresh-root>/tools/Compile.ps1 -Project <fresh-root>/native-stage/gepard/Gepard.gproj -RunRoot <fresh-compile-run>`. This reuses the single-owner atomic legacy lease guard and hidden native validator. Read its result before continuing.
4. Repeat preparation arguments with `--refresh-manifest --adopt-compile <fresh-compile-run>` to pin generated resources and adopt that exact successful compilation. This does not grant gameplay acceptance.
5. Installed `tools/workflow.py plan --job <fresh-root>/healthy-job.json`, then `run --job ... --output <fresh-evidence> --execute`. Both appearances reset independently. Do not run against an occupied/unknown native session.
6. `collect_evidence.py --sprint-root <sprint-root> --output <new-summary.json>` rechecks complete native fixture closure and summarizes original receipts; no launches.

For manual testing, the qualified scene wrapper is `<evening-final>/tools/Scene.ps1` with explicit project, `-World Worlds/GEP_Update101.ent`, exact manifest and a fresh RunRoot. The default wrapper is bounded; a persistent user handoff needs the established persistent-controller lifetime/cross-turn check before promising readiness. Do not use a tool-lifetime-bound process as a new untimed user session. Normal close only; preserve unexpected user sessions.

## Remaining acceptance and coordination

The parent handed the native slot over after Isfahan release at 16:28:24 UTC. Fresh census at 16:36:13 UTC was empty/released. All launches used qualified serial wrappers with atomic admission, exact process identity and independent profiles. Final census/release is retained in `checks/FINAL_RUNTIME_STATUS.json` at sprint root. No further native launches are planned after the final checkpoint.

No operation was denied by automatic approval review. A local attempt to send a checkpoint directly to parent thread `01a15259-930d-7752-b93e-02d417fdf0fc` returned **thread not found**; no message was sent by that action. Parent receives the assistant updates/final through the delegated thread. This is a failed coordination route, not a gameplay block.

All whole-task acceptance remains as listed in `STATUS.json`; no live checkbox was changed. GEP-005/018 remain unfinished despite accepted shipped increments. GEP-017 belongs to the separate art owner. GEP-027 upload/server processing/packaged spawn are verified; public webpage visibility is independently unverified.

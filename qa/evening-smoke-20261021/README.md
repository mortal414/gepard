# GEP-015 recovered stock-only candidate

Status: **Needs user test**, with native compilation of this 1.0.2 composition **pending**. This is an optional test candidate; GEP-015 remains unchecked. No new native process, lease action, publishing or session control occurred.

The six optional addon resources are byte-for-byte from the corrected previously compiled candidate. Historical stock-smoke compile-02 and manual-scene compile-02 report SCRIPT_COMPILE_PASS with exit 0 and zero script errors. RECOVERY.json pins their exact source and evidence hashes. The stale initial handoff and first failed compile are not treated as the final result. Compilation establishes no flight, smoke, rearm, human-input or replication acceptance.

The optional carrier inherits the released GEP_MobileCarrier and changes only its turret slot and test display name. Its turret inherits the current released GEP_VehicleTurret and adds smoke plus actions. No accepted R/driving/braking values, normal default/charcoal prefab, gun sound, released source, dependency or geometry is modified. Stock AN-M8 is referenced directly; no external implementation or asset is included. It is a gameplay surrogate, not a historically accurate 76 mm launcher. Visible launcher geometry is absent.

## Repeatable offline preparation

Run `python qa/evening-smoke-20261021/static_check.py --output qa/evening-smoke-20261021/STATIC_RESULT.json` from this repository. It checks exact recovery, every pre-existing addon file against 699f245, inheritance, dependency and fixture boundaries. It cannot compile Enforce or execute smoke behavior. An integrator can explicitly allow reviewed crew-file differences with repeated `--allow-existing addon/path` arguments; never allow drivetrain, gun audio or geometry changes to hide regression.

Run `python qa/evening-smoke-20261021/stage.py` once to create an exclusive private copy of the candidate addon and the previously compiled manual-scene overlay. It refuses overwriting an existing stage. Default project: `qa/evening-smoke-20261021/native-stage/gepard/Gepard.gproj`. World: `{B7E96C96A1D02024}Worlds/GEP_Demo.ent`. STAGE.json freezes all native-stage files. Re-stage under a fresh `--output` after any integrator change; old stages remain immutable evidence.

The overlay is kept outside the production addon. It spawns a smoke variant, three unarmed stock AN-M8 grenades, and an ordinary on-foot player/camera in single-player only. It does not board, fire, reload, write driver/turret inputs or time out the human session. The user may throw a stock control grenade, then use ordinary contextual gunner/commander controls. Historical scene compilation does not verify the terrain point, physics settling, resource loading or current stage.

## Native queue

Only the coordinator may schedule the existing qualified serial native owner after Isfahan handoff. The dry-run Workbench command below prints a plan and omits `--execute`; it does not acquire ownership. Do not append `--execute` without the qualified coordinator's admission/owned-close wrapper. Select only the staged project and stock addon directory so duplicate Gepard GUIDs are not loaded.

`npm run workbench -- validate --project "D:/ArmaProjects/Gepard-EveningSprint-20261021/smoke/qa/evening-smoke-20261021/native-stage/gepard/Gepard.gproj" --configuration PC --game-addons "D:/SteamLibrary/steamapps/common/Arma Reforger/addons" --logs-dir "D:/ArmaProjects/Gepard-EveningSprint-20261021/smoke/qa/evening-smoke-20261021/native-stage/compile-logs"`

From the maintained harness, the corresponding editor dry-run is `npm run workbench:open -- --editor world --project "<the exact staged Gepard.gproj>" --world "Worlds/GEP_Demo.ent"`. Retain native log, visible evidence and normal owned closure. Scenarios.json defines the acceptance sequence and stop gates.

## Evening manual checklist

1. **Control and one round (first gate, about 2 minutes):** pick up/throw one stock AN-M8 normally and see its normal white cloud. Board gunner and select “Launch one smoke round”. Require 8/8 to 7/8, physical departure without self-hit, fuse activation and visible native white smoke. Observe at a second turret heading if the first succeeds. Stop immediately if control or candidate fails; do not try a salvo.
2. **Salvo and cooldown (optional after first gate):** immediately retry during 2.5 seconds; require no extra dispatch/count change. After cooldown, salvo spends the seven remaining tubes once, then rejects empty fire. Confirm useful coverage and clear crew visibility; visual judgement remains Marco's.
3. **Reload and roles (optional, about 3 minutes):** leave the vehicle, carry a stowed AN-M8, approach the left loading point while stopped, hold the 3-second action. Require exactly one carried item consumed and one loaded tube credited only after inventory completion. Try cancellation, no-item/full/moving rejection, driver/on-foot firing rejection, and gunner/commander firing. Both seat access and service reach are untested.
4. **Regression:** normal stock gun audio, engine, reverse, braking and HUD remain usable; smoke must not disable exit/reentry. Check destruction blocks new launch/reload. MP/JIP/dedicated remain separate later tests and are not covered by this single-player fixture.

DISPATCH logs mean only that void native Launch/SetLive were issued; OBSERVE timer/motion and AMMO convergence are diagnostic. Neither proves a cloud or concealment. Inventory OnComplete credits once; unknown native completion remains pending rather than guessed refund/retry. Keep native smoke activation, per-round clearance, reach, rearm, visible coverage, duration and human acceptance NOT_TESTED until measured.

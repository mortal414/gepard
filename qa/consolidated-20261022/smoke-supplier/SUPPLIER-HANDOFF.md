Source patch ready; vehicle smoke remains NOT_TESTED in native runtime.

Current entrypoint: this file. Isolated repository is
`C:/Users/marco/Documents/Codex/2026-10-21/task-4/smoke-supplier`, branch
`supplier/v11-measured-smoke-20261022`, based on dependency review `b2360a0`.
The original D: candidate, all releases, art overlay, coordinator source/native-stage,
native applications and shared lease were not modified. No outreach or publication.

Delivered changes:

- Original bank roots and all eight PointInfo offsets/directions now derive from
  the art owner's actual retained GEP_TurretYaw port measurements. The previous
  minimum 0.958132431 m mismatch is preserved as history. Config source positions
  reconstruct the supplied 10mm outside points to <1e-7 m. This is a source-math
  result, not native alignment or swept-volume clearance acceptance.
- Separate original V11 camo/charcoal smoke wrappers inherit their respective
  complete art carrier/turret. They preserve the original skin/material/weapon
  parents and add the same smoke manager, seat names and banks. Existing OLD/NEW
  comparison resources are not replaced or reparented. Select these wrappers only
  for separate smoke pads, through the coordinator registry; do not replace OLD.
- Existing original WCS QA now samples at 25ms during the first second, 100ms
  through 6 seconds, then 500ms. Actual frame times govern sampling. Logs include
  actual payload forward/velocity and independent expected world-port transforms
  computed through PointInfo.Set from the measured bases. These transforms only
  observe expectations; they do not alter WCS muzzle/projectile state.
- Eight paired capture times (.25/1/3/5/12/20/30/40s) request near and far views
  after >=100ms camera dwell. The far camera is 60m forward of the hull, looking
  through the smoke toward the vehicle; a matching prefire far view is requested.
  Actual camera pose/time and screenshot request results are logged. This is
  capture instrumentation; no native images/video exist from this supplier run.
- A manual fixture spawns the genuine dependency-overridden maintenance station
  and a native finite supply crate. Its ledger records ammo, finite station
  aggregate, explicit crate values, resource source/parent IDs, global/station
  supplies state and consumer presence. It issues no fire/rearm/ammo/supply setter
  and leaves ordinary character camera/input intact. Do not add aggregate and
  child values together; inspect the recorded resource identities.

Production optional addon remains `7E0150C026101500`, private QA `7E0150C026102500`.
WCS Armaments `629B2BA37EFFD577` remains a separately installed 8.4.0 dependency.
`DEPENDENCY-MANIFEST.json` pins the complete PAK/RDB/gproj identity. The unchanged
mechanism review and notice remain applicable: no WCS implementation, model,
particle, audio, texture or binary is copied or transformed by this patch.
Existing WCS launch, native shell impact effects, ammo and support-station
mechanisms are retained. No custom fuse/flight/effect implementation was added.

New smoke vehicles (in either immutable mode stage):

- `{7E0150C026103002}Prefabs/ArtV11Smoke/Camo/GEP_MobileCarrierV11Smoke.et`
- `{7E0150C026103004}Prefabs/ArtV11Smoke/Charcoal/GEP_MobileCarrierV11Smoke.et`

Offline evidence: `checks/SUPPLIER-SOURCE-CHECKS.json` reports 162 source/identity
checks PASS, including all 1,112 retained released-base pins. `git diff --check`
passed. `checks/ALIGNMENT-REVIEW.json` retains every measured-to-authored mapping.
`checks/NO-NATIVE-EVIDENCE.json` confirms the existing analyzer returns
NOT_TESTED for missing telemetry/captures/lifecycle and NEEDS_USER_TEST overall.
These checks cannot establish native compilation, serialized-resource load,
flight/fuse/cloud, ammo behavior, rearm or regression acceptance.

Reproduce source checks from the isolated repository:

```powershell
& 'C:\Users\marco\AppData\Local\Programs\Python\Python311\python.exe' tools/align_v11.py
& 'C:\Users\marco\AppData\Local\Programs\Python\Python311\python.exe' tools/prepare_delivery.py
& 'C:\Users\marco\AppData\Local\Programs\Python\Python311\python.exe' tools/verify_supplier.py
```

Native request to coordinator `01a15f4d-277b-76d9-b72f-4dde42bec0f4`:

**Pending, not denied or attempted:** admit one fresh compile plus a 75-second
single-round `MechanismControl`, using the exact `single` source stage and WCS
8.4.0 pins. `NATIVE-REQUEST.json` provides the literal compile argv and game argv,
exact executable, project, world, roots, fresh profile/log/download directories,
expected count and duration. The existing serial owner must bind those arrays,
complete source closures, recorder, cancellation, normal close and lease release.
There is no qualified smoke adapter or local binding, so do not execute a generic
client command and infer ownership or acceptance. The source argv record itself
does not launch anything. Existing generic harness bindings are unrelated.

Example read-only extraction of the first requested scenario:

```powershell
$request = Get-Content -LiteralPath NATIVE-REQUEST.json -Raw | ConvertFrom-Json
$request.requests | Where-Object { $_.mode -eq 'single' -and $_.case -eq 'MechanismControl' } | ConvertTo-Json -Depth 12
```

The mechanism control uses the retained original model. It discriminates the
WCS integration path without waiting for art cooking; it cannot accept V11 visual
alignment. All V11 cases additionally require the art owner's additive overlay
in the selected Gepard source and native rebuilding of all five seeded xobs,
followed by native rig/material checks. The retained reference base currently has
no V11 overlay. Coordinator must install it in a separate owned stage, change the
dependency root in its command plan to that exact stage, and regenerate closure
pins. Do not cook/merge into the retained reference base or frozen releases.

After a single round has observed correct muzzle departure, actual flight and
native impact/deployment/cloud, qualify full mode in a separate immutable stage.
Never expose `single` and `full` stages together: their addon GUIDs intentionally
match. Single requires 8->7/exactly one payload. Full requires 8->0/eight payloads,
then the one explicit empty request after 6s must produce no additional payload
or ammo. Missing queried rounds are inconclusive until native query delivery and
pixels are diagnosed. No retry or substitute cloud is automatic.

Normal and edge qualification sequence:

1. Startup template: fresh native compile, exact loaded stack/resource identities,
   valid serialized world, actual settled original crew and capacity eight.
2. Visual template: inspect each real port versus native expected-port and actual
   launch-vector records; verify upward/outward travel, clearance and real native
   impact/deployment. Inspect near/far original images and video, retaining the
   baseline and actual camera/weather/time/exposure. Report cloud onset, last
   useful coverage and gaps, and actual observer/vehicle/cloud positions. Let the
   user judge useful screening; TTL or particle existence does not pass coverage.
3. V11Camo/V11Charcoal repeat the above on cooked V11. Yaw90/Yaw180/YawMinus90
   exercise whole-hull coordinate rotation. They do not prove animated turret yaw:
   the manual case must traverse the turret through front/side/rear and exercise
   gun elevation -5/0/85, hatch/crew clearance, stationary and moving launches.
4. Manual rearms: use the actual on-foot eligible native action, one completed
   iteration at a time. Pair ammo +1 with finite station aggregate -1 and native
   source receipts; repeat to eight. Full bank, genuinely depleted source,
   outside 9m service range, outside bank action reach and invalid actor states
   must reject without charging supplies or adding ammo. The crate's authored
   40 is not assumed to be the station's total. Keep unrelated consumers inactive.
   Test the 2500ms per-tube cooldown separately if an ordinary rearm can complete
   quickly enough; otherwise retain that branch NOT_TESTED.
5. Crew-input/mobility templates: gunner and commander ordinary configured smoke
   input (installed default LeftShift+G; verify the active binding); driver,
   onfoot, wrong vehicle, dead/unconscious and transition denials; hold/repeated
   press, simultaneous requests and empty repeats. Restore ordinary driving,
   reverse/braking/steering, seat/hatch transitions, exit/reentry, ADS/HUD/optics,
   both cannon groups/firing/recoil and radar. WCS global scripts load on this
   dependency stack, so inherited unchanged Gepard files are insufficient evidence.
6. Retain actual native error scan, continuous video, normal exit and released
   lease receipt per case. Multiplayer/dedicated/JIP authority, cloud visibility
   and rearm accounting remain separate required acceptance if claimed.

Analyze a completed automatic run, without interpreting telemetry PASS as cloud
acceptance:

```powershell
& 'C:\Users\marco\AppData\Local\Programs\Python\Python311\python.exe' tools/analyze_wcs_smoke.py --run-root '<actual-completed-run>' --mode single --output '<new-analysis.json>'
```

Use `--profile` if the engine puts `$profile:` files in a nested profile folder;
retain actual path identity. The reused original analyzer checks flight/counts and
capture availability and leaves impact/cloud/coverage/user acceptance pending.
Paired near/far files, actual launch vectors and manual supply ledgers additionally
require direct review; they have no automatic acceptance shortcut.

Research: harness AGENTS.md, feature-recipes.md, workflows/templates/bindings,
harness handoff and operations playbook; the existing candidate/review/stock
failure and successful character-control records; actual V11 art handoff and
measured-port receipt; installed WCS dispenser/manager/projectile/smoke/slot,
support station and sample prefabs; installed native PointInfo/Math3D/vector API
documentation and the retained rearm/supply contract. API examples establish
spherical angle math; PointInfo serialization signs remain a native gate.
The current official startup wiki fetch returned 403; the actual maintained
harness client source confirms comma-separated addon roots. No undocumented
resource GUID or new WCS method body was invented.

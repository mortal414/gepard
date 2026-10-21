# Evening sprint follow-up

The earliest manually testable candidate remains frozen at `cc5d0989376ed1d20ada109ce7c1ada5d96aac7e` in `integration` on `sprint/evening-20261021`. No publication. Release main1.0.2 was verified at `699f245afaf3e912b9ce9827b0f3f5f7dd32f3e7`; its uploaded payload hashes still match.

`diagnosis` is the source successor. Its only production changes versus release are the conscious/exact-occupant guards in CrewActions579, DriverStation and RadarSystem. All other1,109 of1,112 production files match release bytes. Internal commander wording was previously changed to “Move to commander” to distinguish a seat switch; the audit found no functional need for different text, so the successor restores the user's requested “Get in commander” both internally and externally. The frozen earliest candidate is unchanged. Hidden Leave-periscope UI is intentional and preserved; actual F exit still needs Marco's test.

## Actual lane owners

- `/root/crew`: crew/HUD/control/refuel/debug source audit and human checklist. No art/vehicle tuning edits.
- `/root/smoke`: recovered stock-only candidate, lifecycle fixtures, stock character throw control, cloud camera and deferred-launch source experiment. No third-party/WCS dependency.
- `/root/audio`: signal/graph resource and unit/mute investigation. Signal-only9a93a646 remains insufficient; no audible graph integrated.
- `/root/range_fixture`: passive stabilization/range/ballistic observation fixture; no actual correction enabled.
- `/root`: sole integrator and native coordinator; source isolated from Blender/Isfahan.

## Evidence and acceptance

The frozen candidate's earlier evidence remains under `qa/evening-20261021`: exact compiled final source, healthy default+charcoal native traverse/both-feed firing/exit/re-entry/refire, and one same-build native fragment-survivor case. Those are bounded automated scenarios, not proof of ordinary input or whole-task acceptance.

The source successor compiled with the restored commander wording in `followup-base-compile-02`. `COMPILE-RESULTS.json` retains eight follow-up compile records, including the initial recoverable unsupported diagnostic getter failure. Compilation does not prove gameplay.

`NATIVE-SMOKE-FOLLOWUP.json` retains hashes and actual native telemetry for six isolated cases:

| Case | Actual outcome |
| --- | --- |
| Stock-helper drop, no Launch | Correct initial pose; first tick entity/body pose resets to0; local fuse later triggers. No qualified cloud at requested location. |
| Original launcher/null weapon repair | One public callback,8→7; first-tick pose reset; no valid travel, fuse stalls. Earlier native crash from wrong weapon argument is retained separately. |
| EnableSimulation before Launch | Compiles; same failure. |
| Enabled + upward pitch | Compiles; velocity worldY changes from−10.0188 to+10.0188 as intended; same travel/fuse failure. Sign alone does not solve it. |
| Explicit rigid-body SIMULATION drop | State0→2 succeeds; entity/body pose still resets on first tick. No launcher fix proven. |
| Character inventory/equip/weapon-manager Throw | Exact new ANM8/current grenade/current slot/CanThrow gates pass; one Throw;41.19m observed travel and fuse trigger. Native projectile movement works while rigid-body telemetry says inactive/NONE. Camera followed thrower, so cloud coverage is not yet qualified. |

Successful character flight also has null parent-projectile pointers; null pointers alone are not a failure condition. The numeric EntityFlags dump does not identify2048 among the initially captured names; no flag was cleared or guessed. No particles, local SetTimer, velocity or pose setter was used as a smoke fix. All smoke remains outside production.

## Native blocker: not released

At17:43:45 UTC read-only census, only owned Workbench PID56448 remained (`audio-resource-resolver-02`, born17:36:32.6341700Z). Its `Script Authorization Required` dialog appeared before any resolver observation. `System.GetCLIParam` authorization is pending, neither granted nor denied. Normal project-window WM_CLOSE and dialog WM_CLOSE did not dismiss it. No forced termination, security approval, unexpected user session closure or lease takeover occurred. `NATIVE-BLOCKER.json` and `NATIVE-CENSUS-BLOCKED.json` identify the target exactly. Do not launch any queued native job until manual dismissal/normal close and verified lease recovery.

The computer-use skill's required guidance says: “Do not act on security or privacy permission requests.” Human dismissal is therefore required. This is a native application authorization blocker, not automatic approval review rejection. The first audio run's separate CLI configuration failure closed normally after hidden-window recovery; its recovery evidence remains in its run directory.

## Queued bounded native batch

`QUEUED-NATIVE-TESTS.json` contains admission and expected outcomes. Source-only stages are not native passes.

1. `SMK_CHARACTER_CLOUD`: `followup-character-cloud-01`, world `Worlds/GEP015_CharacterThrow.ent`,50s. Fresh compilation/adoption first. Same one Throw; camera follows actual round, extra20/30s frames and32s terminal. Require motion/fuse and inspected visible native cloud.
2. `SMK_DEFERRED_LAUNCH`: separately authored next-frame launch experiment, based on actual stock Airstrike ordering. Require exact reservation/dispatch identity and once-only callback; fail closed on invalid state/origin. Do not accept from ammo count.
3. `AUD_PROFILE_RESOURCE_METADATA`: `audio-resolver-profile-01`, fixed `$profile:GEP033-RESOLVER.json`, no System.GetCLIParam or GetCmdLine, no playback. Fresh unique profile; locate and record the actual unique report path after exit. Require9 observations, stock identities and missing-resource negative control.
4. Audible graph work only after resource/port gates: original scratch graph, verified dB/linear units, zero active event at idle, stable start/stop cleanup, actual dynamic updates and listening. No linear0..0.45 into dB volume;−60dB is not exact silence. Stock cannon sound remains untouched.

## Marco's manual checklist

Use the frozen candidate for the existing bounded regression; use a separately qualified successor to assess restored internal commander wording. A short pass can do1–3; the broader damage/refuel/UI pass adds4–6. These are tests, not checked acceptance boxes.

1. Every driver periscope: return prompt hidden; one F exits. Repeat after exit/re-entry and check other needed hold-F actions.
2. “Get in commander” outside/inside on the successor; cycle driver→gunner→commander→driver twice; test each hatch/prompt and inspect commander eye/roof/free-look alignment.
3. Gunner ADS, C range on near/far ground then sky, left/right/both guns; menus, seat changes and exit/re-entry twice; repeat default/charcoal. Check fresh HUD/range/ammo and restored normal UI/controls.
4. Conscious wounded survivor: controls, hatch/exit/re-entry/fire; driver brake lamps. Separately test incapacitation/recovery and restored settled controls/view. The one automated fragment case does not cover every destruction state.
5. Partial fuel and stock support from each authored rear side: actual fuel transfer, cancellation, near/far reach and duplicate/through-hull prompts.
6. Every seat/view/menu/transition/appearance: record exact unexpected debug text or stale UI.

Current exact ledger: `STATUS-CURRENT.json` contains all33 GEP IDs. Ready001–004/028–032 are previously accepted behavior preserved, not reimplemented. Needs user test005/006/008/010/013/018/020–024. Blocked007/009/011/012/014–017/019/025–027/033. GEP017 art is owned elsewhere; GEP027's upload/server/package tests passed but public webpage visibility remains independently unverified. No tracker checkboxes changed.

# Range / fire-control lane, 21 October 2026

Source-only checkpoint against released Gepard `699f245afaf3e912b9ce9827b0f3f5f7dd32f3e7`.
No production file was edited. No native process, shared lease, input, profile flag,
resource reload, Workbench or game launch was attempted. Isfahan retains the native slot.

## Findings that determine scope

- `addon/scripts/Game/GEP/GEP_FireControl.c`, `OnPostInit`: `m_Enabled=false` and
  `m_DirectExperiment=false` unconditionally. Released C can measure range; it does
  not qualify stabilization or physically apply the experimental correction.
- `GEP_Range676.c`: actual rendered player-camera ray, 5000 m bound, exact ADS scope,
  self/crew exclusions, sample world point/slant metres/simulation age. A miss removes
  the prior sample; scope loss releases its listener. The callback requests correction,
  but the released controller's hard disable prevents actuation.
- `GEP_GunStatus.c`: current sight shows `ENTFERNUNG`, rounded metre value, sample age;
  miss is `KEIN MESSWERT`, unavailable is `MESSUNG NICHT VERFUEGBAR`. Pixel/readability
  verification and physical C/rebound input remain native/human checks.
- Experimental direct correction uses the next loaded projectile's native direct-fire
  table, selected feed/group, actual muzzle transform and measured barrel pose. It
  rejects >2 degree LOS slope, >1 degree mount tilt, >3 degree correction and >2 second
  servo duration. These bounds are code facts, not qualified ballistic accuracy.
- A native table value is neither measured barrel motion nor a projectile impact.
  Stabilization needs actual hull motion and stable world LOS. Pose setters and
  generated aim arrows cannot establish either behavior.

## Recovered source and provenance

The original passive query is at
`D:/ArmaProjects/Gepard-Range-676/native-stage/gepard/scripts/Game/GEP/GEP_Range676.c`,
`BallisticDiagnostic676` (lines 165-193). Its original handoff is
`D:/ArmaProjects/Gepard-ReleasePolish-678/RANGE676_HANDOFF.md`.
The older `GEP_TurretBallistic416.c` is a damage/repair probe and explicitly calls a
pose setter during preparation; it is unsuitable evidence for fire-control actuation.
The old range world's stationary turret and many development classes are also stale;
do not copy its entire source tree over 1.0.2 or treat old qualification as current.

`fixture/GEP_EveningRangeProbe.c` adapts that original passive query into an unbound
private helper. It has no automatic execution, input listeners, aim/pose/physics
setters, firing, networking or profile flags. It logs current presentation/operational
scope, range validity/state/point/age, player camera and physical elevation bone.
The optional table query defaults off and always labels `aimSolution=0`.
**This new helper has not been compiled.** It belongs only in an isolated QA fixture,
outside production packaging. Copy it into that fixture's Game script root, compile,
then call `GEP_EveningRangeProbe.Sample("RNG-300-before", turret, false)` from the
fixture owner after an observed action. Identify the exact current mobile turret
component; do not use the hull or old stationary prefab by assumption. A second call
with `true` samples the selected loaded table only, if actual presentation/operation
and range are valid. No table helper is needed for GEP-008 acceptance.

The sampler's current-camera line occurs after the measured sample. For geometric
ray assessment hold the camera/vehicle still between physical ranging and sampling,
or capture the camera in a private callback at actual ranging time. A later camera
pose after movement must never be compared as though it was the original ray.

## Queued batch

`scenarios.json` is the acceptance contract. Begin with release behavior and GEP-008,
using both camouflage variants. Keep stabilization/direct correction disabled for
this release-control batch. Development actuation cases require a **separate**, fresh
fixture and explicit code delta; no hidden profile flag enables the released source.
Use an ordinary local gunner/player camera and actual input. Logs must be paired with
native captures/video. Use native damage cases only after a healthy input control.

`analyze_observations.py` assesses independently normalized records and hashes each
referenced raw evidence file. Missing records/evidence/measurements remain NOT_TESTED.
It checks geometric range consistency and explicit pixel observations separately;
it cannot prove that a human pressed C or that supplied annotations are true.
The 0.05 m / 0.02 degree consistency limits are diagnostic capture tolerances, not a
claim of rangefinder accuracy or GEP acceptance. Impact target dimensions, native LOS
stabilization limit and human acceptance must be declared before an actuation run.

Example offline assessment after collecting real evidence:

```powershell
& 'C:/Users/marco/AppData/Local/Programs/Python/Python311/python.exe' ./analyze_observations.py --observations ./observations.json --evidence-root ./native-evidence --output ./analysis.json
```

The included empty `observations-pending.json` yields NOT_TESTED by design. No
synthetic sample is shipped as gameplay evidence. Do not run the old range launcher;
admission and native fixture wiring remain the integration owner's responsibility.

## Exact status

| GEP | Current lane status | Remaining acceptance |
| --- | --- | --- |
| 007 | Blocked: released stabilization disabled | Separate actuator fixture, native compile/ownership/control calibration, actual-motion LOS, human test, MP |
| 008 | Needs user test: regression scenarios and private sampler prepared | New helper compile; physical input, range pixels, miss/lifecycle and both appearances; MP remains separate |
| 009 | Blocked: released direct correction disabled | Separate physical actuator and actual projectile impact tests at multiple ranges/feeds; no table-only completion |

No checklist item was checked off. No native compilation or gameplay pass is claimed.

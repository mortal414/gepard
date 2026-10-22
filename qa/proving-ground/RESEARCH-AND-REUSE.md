# Research and reuse — 22 October 2026

The harness AGENTS.md, workflows.md, feature-recipes.md, harness-handoff.md,
operations-playbook.md, driving-review.md, knowledge-sources.md and selective-pak-source.md
were read before implementation. `workflow.py templates` and `bindings` were run with
the existing Python311 interpreter (the WindowsApps `python` alias is unavailable).
Existing bindings: gepard-healthy, gepard-damage and gepard-driving. They remain unchanged.

The actual source checkout points at Gepard-PublicSource-694; its shared evening
integration is D:/ArmaProjects/Gepard-EveningSprint-20261021/integration. No AGENTS.md
or .agents/skills were found in either relevant integration/approved-optic checkout.
The older authoritative Gepard-20261012/AGENTS.md was read. Do not import the stale
range676 driving/UI restrictions as new user authorization: current trusted delegation
and the explicit 00:47 runtime hold govern this work.

Reuse boundaries:

- GEP_StationaryRange.c (Range676 native-stage) supplies the established Arland
  parent, world managers, ordinary actor/camera startup, stock Ural ammunition truck,
  M923A1 repair and tanker resources, and one-time finite supplies initialization.
  Its many profile-selected automatic test probes are deliberately not included.
- GEP_ServiceProbe.c establishes the genuine native timed support action and cost
  measurement recipe. Do not transplant its initial health setters or scripted
  service actions into this manual map. Native actions on the actual vehicle own
  refueling/repair/rearm eligibility, tool guards, duration and costs.
- Reforger-QA/gepard/jobs/gepard-driving.json, the addon-owned Damage683 adapter and
  harness driving_review.py already implement repeatable measured driving. They
  are integration leads, not qualification of this new geometry. Keep signed net
  progress, action/power/contact overlap, speed, grade and grounded stop dwell.
- GVR_ReviewDirector.c demonstrates the supported profile command mechanism and
  actual ordinary-camera setup. The proving ground uses no forced camera after
  startup, vehicle pose pinning, artificial vehicle invulnerability or capture loop.
- Native SCR_FiringRangeTarget.SetState/SetAutoResetTarget supply pop-up targets
  and explicit resets. Vehicle silhouettes avoid explosive crews/vehicles in the
  main shooting lane. Stock M16/M249/M2 tripod/M72 and their real magazines provide
  damage stimuli; no fabricated projectile, damage or ammo setter is used.
- Stock SCR_AIGroup.GetAgents/AddWaypoint and stock sentry/defend prefabs are used
  for the optional encounter. The existing ConvoyFollower native group helper was
  inspected for signatures only. None of its unrelated convoy logic is copied.

Exact installed native resource content hashes, package index identity and read date
are in checks/native-resource-provenance.json. Stock content is referenced in place;
no native source or asset payload is redistributed. Original private Gepard assets
remain owned by Marco; whole-mod license remains undecided. No WCS/SpaceCore or other
third-party source/art is copied, installed or added as a dependency. The proving-ground
code is minimal original glue, required because no existing fixture combines local
station resets, identity labels and finite support sources into this requested map.

The official [World Editor documentation](https://community.bistudio.com/wiki/Arma_Reforger:World_Editor)
was located; direct fetch returned 403. Installed world/API examples are the fallback.
The official [Bohemia sample repository](https://github.com/BohemiaInteractive/Arma-Reforger-Samples)
and local sample LICENSE were consulted. Samples use APL; no sample asset was copied.
The accessible [Test Shooting Range author page](https://reforger.armaplatform.com/workshop/60CEF689B0796250)
lists lovelyToph, APL, version1.0.0/game1.0.0.98, last changed7 March2024, and no useful
implementation documentation. That old example is not a supported maintained dependency
for current1.8.0.13. No mod was downloaded or adapted. A resource name/Workshop description
is not behavior proof.

Remaining evidence boundaries: Arland's existing apron is proven in earlier fixtures,
but this extended course/target fan requires terrain collision, slope, wheel material
and sightline survey. Native range logs compute actual terrain Y, slant distance and
terrain trace fraction. Registry distance labels are nominal horizontal metres, not a
claim of clear fire lanes. The optional hostile enclosure and ray-block checks cannot
prove projectile containment; the encounter is locked until a dedicated native review.
Night toggles are omitted because they add little before a reliable manual session is
qualified. MP/JIP is unsupported by this first private single-player fixture.

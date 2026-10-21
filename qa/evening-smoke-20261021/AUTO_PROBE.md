# One-round diagnostic overlay

New QA-only code, NOT native compiled/tested yet. Apply auto-overlay in addition to native-overlay on a fresh private staged addon; do not add these files to production addon. Explicit world: `{41B99401C815A015}Worlds/GEP015_Auto.ent`.

Uses qualified DamageScenario683 APIs for real native gunner boarding and verifies settled occupancy, actual living/conscious crew, stopped hull, smoke authority/operational state and8 rounds. Retrieves the configured single-round user action from native actions list, checks its eligibility, invokes OnActionStart and PerformAction exactly once. No retry/salvo or driver/turret pose input. It is programmatic public-action callback evidence, never physical UI input acceptance.

The only modded smoke method is a read-only InspectRound observation hook calling the original method first. Original dispatch/Launch/SetLive/tube/cooldown/rearm methods are unchanged. Existing callback gives first grenade entity at about5s; private probe then samples its actual position, GrenadeMove velocity, native fuse timer/WasTriggered and replication proxy state every0.5s, with actual world-time elapsed. Distance from resolved original muzzle is recorded, not inferred from a dispatch count. It cannot observe earlier flight velocity; retain original DISPATCH log and native frames.

Exterior screenshot requests at0.25/1/3/5/8/12 seconds after callback cover flight and native cloud; pixel inspection is mandatory. Camera is an observer only; it does not move the hull, turret or grenade. GEP015-auto-result.json and GEP015_AUTO_* logs state diagnostic scope and missing ordinary positive control/cloud/concealment/rearm/MP acceptance. Normal process closure remains exclusively with coordinator's owned wrapper; the fixture never calls RequestClose.

This diagnostic does not automate an ordinary thrown AN-M8 positive control and does not satisfy full S015-01/02 by itself. Stop after one callback regardless of outcome. Scripted instant boarding and direct action callback are not familiar-controls or reach acceptance. Check stage manifest/compile/resource resolution before native interpretation.

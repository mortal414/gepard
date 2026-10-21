# GEP-015 lifecycle continuation

Source-only continuation branch: research/smoke-lifecycle-20261021. No native process, lease, session, release, publish or integrator-owned source was modified. Integrator's earliest recommended manual candidate cc5d098 remains preserved outside this lane. Every new file is under this separate QA directory.

## What failed, and what is known

Native smoke-diagnostic-01 crashed in Dispatch at Launch with the turret supplied as the last weapon argument. The integrator's null-weapon repair compiled and diagnostic02 issued an eligible public single-round action8->7 without crash. Its first actual entity observation at5seconds showed origin0, unchanging projectile velocity, local timer0.966946 and WasTriggered=false; retained frames showed no cloud. The original DISPATCH row printed the requested muzzle, not the spawned entity's actual origin. Therefore original logs cannot establish that the entity was ever created at the requested muzzle. No new source interpretation turns this failure into smoke acceptance.

Installed generated ProjectileMoveComponent exposes EnableSimulation(IEntity owner) separately from Launch. Installed stock SCR_ScenarioFrameworkActionSetGrenadeLive schedules trigger.SetLive1000ms later, with a preparation comment, and optionally calls GrenadeMoveComponent.EnableSimulation(entity). These are concrete source facts. They do not prove the correct Launch order or that simulation enable is the only missing launcher step. GetTimer is documented local and unreplicated. Math3D MatrixCopy(source,out destination) matches existing argument order; no spawn/transform rewrite is included.

The previous probe also repeated terminal rows/samples beyond13seconds despite a bool guard. Its cause is not proven. The replacement uses a distinct integer terminal latch, clears its FRAME event mask, detaches the static observer and removes its own pending diagnostic callbacks. It records the scene identity to distinguish multiple instances. No claim about the engine's field/lifetime semantics is made before new native evidence.

## Four bounded variants

VARIANTS.json is the exact schema1 stage/case contract. The self-contained control-source-candidate has the integrator's null-weapon launch repair, an optional simulation flag default0, and empty observation hooks at each immediate phase. The diagnostic overlay implements those hooks with read-only logs/JSON, including actual creation position, entity/component ownership, parent projectile, physics state and native timer. No timer, velocity, grenade pose, particle/effect or physical-input setters are added.

- launcher-control: instrumented null-weapon control, no EnableSimulation, immediate SetLive unchanged.
- launcher-enabled: identical code/observations, with exactly one serialized prefab flag enabling EnableSimulation(round) immediately before Launch. Immediate SetLive stays unchanged to isolate simulation.
- launcher-enabled-upward: optional separate fourth experiment, enabled simulation plus pitch+30 in place of-30 only. Original measured worldY velocity-10.0188 proves the old discharge was downward; the opposite nominal unit direction is a static prediction until native readback.
- stock-helper-drop: independent stock grenade at2m with no Launch, using the supported stock-helper EnableSimulation plus delayed SetLive1000 semantics. This is an engineering positive control, not an ordinary throw.

Apply overlays only to a fresh private source stage selected by the integrator. Source/class GUID paths remain the existing optional smoke paths. Do not put all variant source folders into one addon or edit production addon. The observation overlay includes the recovered manual scene/respawn source so it is self-contained. The native coordinator owns compilation, admission, launch and normal closure; this directory has no launcher and starts nothing.

Read each per-phase GEP015-life-<sequence>.json and GEP015_LIFE_* logs. First actual observation occurs immediately after SpawnEntityPrefab, before component validation or native launch. Queued0/33/100/250ms and later readbacks use actual world timestamps. Store screenshots at0.25/1/3/5/8/12s for review. Requested screenshots alone are not evidence of visible smoke. Origin0 is retained as invalid-near-muzzle diagnostic data rather than counted as2668m of flight; no motion PASS is calculated.

Run unchanged control and enabled variant as independently reset cases when native admission returns. Compare actual data, not requested inputs. If enabled flight/fuse succeeds, inspect pixels and then schedule ordinary stock throw/manual controls before broader launcher acceptance. If both launcher variants fail but helper-drop succeeds, a stock preparation delay is a separate next experiment; do not silently add it to the simulation test. If helper-drop also fails, retain that precise lifecycle/control failure and investigate before inventing timers or effects. All compilation/gameplay for these new files remains NOT_TESTED.

DIRECTION_CHECK.json retains the measured old velocity, calculated old velocity pitch and nominal +/-30 vertical unit signs. It labels actual parent/muzzle orientation limits. First control/enabled A/B keeps the old angle unchanged. Physics/entity/event flags are captured; they do not establish a contact. No contact-history getter was qualified and this observer does not change native collision subscriptions.

Repeat offline identity/contract checks with `python qa/evening-smoke-lifecycle-20261021/verify.py --output qa/evening-smoke-lifecycle-20261021/STATIC_RESULT.json`. This starts no native process.

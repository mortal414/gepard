The remaining execution gate is a recorded automatic-review rejection of the
isolated angle replay. It is separate from audio authoring and is not a pending
Workbench smoke permission dialog. No separate new smoke human-approval request
was found in the inspected records. Existing trusted history already authorizes
using WCS as a declared dependency; no new blanket dependency-permission gap was
identified by this audit.

Current observations, checked read-only:

| Item | Persisted outcome |
|---|---|
| Camera patch | Adopted from fbddc18, compiled and replayed in smoke-cameraoff-02. CaptureEnabled=false, no camera VM exception, no screenshots, normal exit0 and released lease. The enabled custom-camera branch remains untested. |
| Last program/ammo | One authorized program, one full-identity launched payload, eight -> seven rounds. Program/payload assertion PASS. |
| Last flight | Only one live-payload position among147 samples; computed travel0m. Launch forward `<0.685525,-0.294085,0.66601>` remains downward. Flight is NOT_TESTED; the1m travel/horizontal gates are unsatisfied. |
| Cloud/fuse/impact | NOT_TESTED. No new visual evidence. Old requested exterior captures showed the cabin and cannot establish cloud coverage. |
| Strict diagnostics | Last camera-off run remains overall FAIL, with57 retained native errors. Normal closure does not erase them. |
| Angle patch | Eight pairs in two original bank configs applied to separate native-smoke-angle-stage; native compile05 PASS and compiler adoption sealed. All1,200 current closure pins match. Gameplay replay has no receipt; requested output directory remains absent. |

Last actual smoke runtime evidence:

- `D:/ArmaProjects/Gepard-Consolidated-20261022/native-runs/smoke-cameraoff-02/smoke-mechanism-control/native-run/logs/console.log`: capture-off marker line280, preflight283, program292, launch vector293, full-ID payload294, ammo7 at314, terminal744.
- Same run's `profile/profile/GEP215-wcs-result.json`: one payload, ammo7,
  capture disabled and zero requests. `exit.json`: verified normal exit0,
  leaseReleased=true, no cancellation.
- `.../smoke-mechanism-control/native-review.json`: original assertions and57
  strict errors retained, including dependency localization, inherited WORLD
  serialization errors and shutdown resource diagnostics.
- `D:/ArmaProjects/Gepard-Consolidated-20261022/native-runs/compile-smoke-angle-05/RESULT.json`:
  SCRIPT_COMPILE_PASS, exit0, nativeAbsent=true, leaseReleased=true.
- `C:/Users/marco/Documents/Codex/2026-10-21/task/GEP-Smoke-Angle-QA-20261022/COMPILE-ADOPTION.json`
  and `PLAN-RECEIPT.json`: exact angle project/compiler/source closure adoption.

The exact action recorded as denied before execution is:

```text
workflow.py run --job C:\Users\marco\Documents\Codex\2026-10-21\task\GEP-Smoke-Angle-QA-20261022\smoke-cameraoff-job.json --output D:\ArmaProjects\Gepard-Consolidated-20261022\native-runs\smoke-angle-03 --execute
```

Its target is
`D:/ArmaProjects/Gepard-Consolidated-20261022/native-smoke-angle-stage/single/qaSmoke/GEP_WCSSmoke215_QA.gproj`,
world `Worlds/GEP215_WCSAuto.ent`, the existing camera-off single-round75s case.

The coordinator's recorded reason, copied unchanged from its checkpoint, is:

> Native run uses WCS smoke project/dependency; review cited earlier stock-only smoke restriction to avoid unapproved WCS dependency. Angle-only change did not resolve the stated authorization conflict.

Source of this record:
`C:/Users/marco/Documents/Codex/2026-10-21/task/RECOVERED-CHECKPOINT-20261022-0724.json`,
`smokeBlocker`. Coordinator thread01a15f4d-277b-76d9-b72f-4dde42bec0f4 also explicitly
reported the rejection before launch, including that the output directory was
not created. The requested directory is still absent. The original automatic
reviewer's tool response/verbatim rejection text was not present in the inspected
files or returned thread items. The quoted reason is therefore an attributed
coordinator record, not a newly verified verbatim reviewer response.

The earlier stock-only constraint was subsequently lifted in the trusted parent
history: the user explicitly encouraged a maintained WCS dependency, and the
parent instructed removal of the self-imposed stock-only restriction. Dependency
reviewb2360a0 separately established the original-config/API/inheritance boundary,
without copied protected implementation/assets. That evidence should be presented
when reconciling the specific rejected action. This audit does not establish a new
requirement for WCS-author contact or renewed general user permission.

The smallest next step is to reconcile that recorded automatic-review conflict
with the existing later authorization before any native retry. The source does
not need another redesign: the exact eight-angle counterfactual is already
compiled, sealed and prepared. Once the execution gate is legitimately resolved,
the coordinator can replay that same owned job with capture still off, unchanged
1m flight/horizontal thresholds, strict errors and normal owner/lease closure.
No alternative executable, renamed job, removed dependency, relaxed collector or
launch route should be used to bypass the rejected operation.

If the corrected-angle replay still gives one transient position and no flight,
the next technical diagnosis is the actual initial velocity/direction and native
payload lifetime/contact on that corrected candidate. Self-impact and fuse failure
are hypotheses until native observations establish them. Near/far cloud coverage,
full/empty firing, finite-supply rearm and crew/HUD/driving regression remain later
acceptance gates. A compile, source hash or program/count pass does not qualify them.

No runtime implementation patch was made in this audit: a technical successor is
already prepared, and making another path while its native replay is denied would
not resolve the discriminating test. This supplier launched nothing, retried no
denied action, altered no coordinator file, touched no native owner/lease and did
not interact with any approval dialog. Only this isolated audit report and its
hashed JSON evidence index were added. Audio's distinct FileIO/authoring approval
was not retried or extended to smoke.

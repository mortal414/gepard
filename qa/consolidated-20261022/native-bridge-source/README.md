# Consolidated Gepard QA: first authoritative startup bridge

**SOURCE ONLY. User runtime hold remains active until explicit user resume.**
No game/Workbench, GUI/focus/close, recorder, adapter main, wrapper or native action
was invoked during this preparation. No D: source/world or live lease was edited.
This fills the missing adapter/job/planning connection; it does not claim tomorrow's
whole proving-ground session is implemented or any native acceptance has passed.

## Reuse boundary and current entrypoint

Read harness AGENTS, workflows, workflow-adapters, recovery, handoff, recipes and
relevant operations playbook before implementation. The addon worktree has no
AGENTS.md. `workflow.py templates` selected **startup**; `bindings` found only older
healthy/damage/driving fixtures, none for this HUD. The existing addon-owned
`qa/optic-approved-20261022/tools/Scene.ps1` already provides complete source guards,
exclusive claim/attach/release, exact PID/executable/birth/parsed argv, hidden bounded
scene launch and normal owned close. Its small cancellation/hold patch is proposed
in C only. No renderer manager, parallel admission or replacement harness was added.

- Job: `hud-startup-job.json`, one `hud-render-startup` case.
- Project: `D:/ArmaProjects/Gepard-OpticApproved-20261022/native-stage/hudQA/GEP_ApprovedHUD705_QA.gproj`.
- World: `Worlds/GEP_ApprovedRender705.ent`, existing scripted healthy-seat HUD render.
- Source roots: exact hudQA + gepard trees; complete closures total1121 files.
- Addon roots: only that native-stage parent and installed game/addons.
- Planner resolves QA7E0050C026102500 -> one GepardD561AD08932E7B40 + base/core.
  Gepard itself has only the base dependency. No optional dependency is enabled.
- Adapter: `adapter.py`, request/result/cancel positional contract, exact PowerShell
  argument array with `-Execute` only inside the future harness-executed adapter.
- `bindings.proposed.json` is a separate explicit registry; live registry unchanged.
- `PLAN.json` and `PLAN-RECEIPT.json` are actual authoritative **static** plan results.
  Old plan revisions are retained under plan-history; only current pins are selected.

Safe read-only planning command (no execute flag):

```powershell
& 'C:/Users/marco/AppData/Local/Programs/Python/Python311/python.exe' 'C:/Users/marco/Desktop/reforger-agent-harness/tools/workflow.py' plan --registry './bindings.proposed.json' --binding gepard-consolidated-hud-startup
```

## Hold, cancellation and lifecycle

The adapter rejects shared status `user_runtime_hold...` or `userRuntimeHold.active`
before invoking the wrapper. Malformed nested hold state fails closed. The patched
wrapper independently checks hold before admission, again inside the existing atomic
claim, in both owned loops and
**immediately before its sole existing CloseMainWindow call**. If hold becomes active
with an owned game running, it throws/preserves process+lease and reports NOT_TESTED.
The adapter records attached PID/birth from the owner receipt and current lease
status, without claiming a fresh process census. It requests no close under hold,
publishes censored infrastructure/lifecycle evidence and starts no following case.
It never treats a later cleared hold as automatic authorization to resume.
The atomic claim preserves an existing inactive shared hold field. Release also
rechecks hold under the same file lock and refuses to clear it if a hold arrived
while the native process was exiting; root must reconcile that retained lease.

While hold is inactive, `REFORGER_QA_CANCEL_PATH` is checked before launch and in both
owned loops. Cancellation uses the **same original exact-owned normal-close path**,
not a second close mechanism. Close grace expiry or identity uncertainty preserves
ownership; no force kill, PID takeover, lease repair or force release exists here.
Cancellation/deadline/log overflow can never provide a behavior PASS. A late result
cannot overwrite the censored result; the authoritative coordinator also censors
deadline/cancel outcomes independently.

Adapter child stdout/stderr use bounded4MiB retained prefixes per stream, continuing
to drain overflow while requesting cooperative cancellation. The native console is
bounded64MiB for review. After25s unresolved cancellation, publish uncertainty and
keep observing the owned wrapper without killing it; the coordinator has35s grace
and stops the batch if closure is unresolved. No new case or automatic retry follows.

Lifecycle PASS requires the original wrapper's verified exit/release plus matching
lease ID, owner capability hash (never exposed), work root, project, argv and released
slot with no active PID/runtime. Native/wrapper exit must be zero for an uncensored
successful capture. Native engine errors are retained before/after capture; no
unqualified baseline whitelist hides them.

The adapter cannot pause a native probe already loaded in a preserved process.
Its autonomous bounded script may finish its current native phase; a hold prevents
new adapter/GUI commands and following cases. Any such held run is censored and
requires explicit root/user reconciliation, never automatic resume or acceptance.

## Fresh evidence and acceptance limits

Require one ordered sequence from the exact fresh console: loaded addon identities,
world entities load, GAME, scene staged, healthy local player, native boarding,
settled-seat ADS/visible HUD, actual ammo/range/zoom data and matching widget text,
one successful capture, then CAPTURED terminal. Duplicates, wrong scope, invalid
states, wrong German count/range/zoom bindings and stale/ambiguous files fail closed.

Require one exact `approved705-render-case-0.bmp` in one of the two known fresh profile
locations, a valid complete24/32bit BMP payload, expected dimensions, fresh timestamp
and SHA256. This proves a fresh native capture **file**, not clean pixels, correct
glyphs, concept fidelity or physical controls. Those three extra result assertions
always remain NOT_TESTED and are deliberately separate from automated startup gates.
An eventual startup/capture PASS would not accept GEP005/006/008 or the whole HUD.

The existing probe emits fixed case0 rather than a native run nonce. Binding here
uses a fresh attempt/native directory, exact owned process/argv/profile receipts,
complete source/compile pins and fresh console/image. That association itself needs
native qualification; no literal native nonce claim is made. The probe source/world
was not edited or silently expanded by this lane.

## Concrete pending execution gates

1. Explicit user resume; root rechecks ownership/admission. Do not clear the hold
   because time elapsed, a day changed, or this source package exists.
2. Root reviews/copies this minimal wrapper patch and adapter/job into its chosen
   private QA location; regenerate paths/pins with `prepare_job.py` if relocated.
3. Root compiles the exact QA project using the existing compile wrapper, verifies
   compiler normal exit/release, refreshes generated-resource closure manifests if
   needed, and records actual compile adoption. `COMPILE-ADOPTION-PENDING.json`
   intentionally remains NOT_TESTED and prevents native invocation right now.
4. Rebuild the job pins and plan again. The adapter never fabricates compile proof.
5. After admission, qualify cancellation, hold-arriving-during-run and normal closure
   on the single existing HUD case before adding cases or using the bridge for a
   user session. Keep failed/censored attempts intact.

Expected future compile-adoption record fields (not a success template to copy):
`schema:1`, `status:SCRIPT_COMPILE_PASS_ADOPTED`, exact `project`, real `compileRun`,
`compilerResultSHA256`, `compilerConsoleSHA256`, and `closures` matching the adapter's
full `{root,manifest,sha256,files}` list. The adapter verifies existing RESULT.json,
owner-private project identity, compiler success console and both actual closures.
Updating adoption requires regenerated job pins and a fresh authoritative plan.

Only after those conditions may root invoke the authoritative `workflow.py run`
with its ordinary explicit execute opt-in. No execution command was run here.

## Source/offline verification

`selftest.py`:51 synthetic parser/source/receipt tests PASS, including invalid/duplicate
scope, fresh BMP payload, complete-closure rejection, compile-pending rejection,
cancel censoring, exact released lease receipts and hold-before-close source guards.
Tests import pure functions only; they never call adapter.main, Popen or Scene.ps1.
`plan_job.py` calls authoritative plan() only and verifies all1121 source file hashes.
This is code/planning evidence, not native compilation, cancellation or gameplay.

Future proving-ground world, manual consolidated session, art comparison, smoke,
audio, stabilization/ballistics, refuel/damage and MP modules remain **NOT_TESTED and
not bound** in this job. The proving-ground worker retains its own world ownership;
no edits were made to `D:/ArmaProjects/Gepard-ProvingGround-20261022`.

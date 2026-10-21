# GEP-033 evening checkpoint

**Status: Blocked.** This is a rebase of the existing signal probe onto verified release 1.0.2 (`699f245`), not an audible implementation. No production addon file changes. Do not mark GEP-033 complete or make it a release dependency.

The optional `signal-probe/GEP033SignalProbe.gproj` depends only on stock Reforger and the chosen Gepard addon. Its new child uses the released `GEP_MobileCarrier` rather than the older experimental wheel-E prefab. It adds the original body-motion signal component, renamed to avoid the prior prototype class, with three read-only getters for a future native fixture. It has no ACP and calls no playback, driving input or physics setters. It must resolve exactly one chosen Gepard source; do not load duplicate addon GUIDs.

## Exact remaining action

Pending, not denied or uncertain: resolve `Sounds/Vehicles/_SharedData/Tracks/Rotate/Vehicles_Tracks_Rotate_Slow.wav` through supported native resource APIs; author/save/register/compile an original `GEP_TrackMovement.acp` and its signal binding in Workbench; then qualify the resulting event lifecycle and listen. Isfahan owns the qualified native slot, so this lane launched no Workbench/game and changed no process or lease. There is no qualified headless graph-authoring route. Stock package index reinspection found the four track WAV paths and no stock track ACP/SIG; index metadata contains no WAV GUID. A filename is not qualified graph identity, and the inert contract is not ACP source.

`AUDIO-GRAPH-CONTRACT.json` records the bounded design; never rename it `.acp`. The existing LAV25 engine, APC wheel graphs and stock gun graphs remain exact. Before authoring, inspect which inherited wheel branch is rolling audio: appending a track loop over it may duplicate movement sound. Keep separable impact/skid/suspension behavior.

## Repeatable evidence

Run `python qa/evening-audio-20261021/check.py` from this worktree (Python 3.11+). It verifies every production file against the released source manifest, probe inheritance/dependencies, no playback/physics writes or redistributed audio/graphs, and signal-model invariants. It writes `STATIC-RESULT.json`. This is neither Enforce compilation nor gameplay/audition evidence. The full-production preservation check is intentionally scoped to this audio branch at the released baseline; unrelated integrated crew/smoke deltas require the integrator's separate allowlist and must not be silently accepted here. `inspect-stock.py --harness <maintained-harness> --node <node.exe> --stock-root <stock-addons-data>` repeats the read-only index query and writes `STOCK-INDEX-RESULT.json`; it reads no audio payload and launches no native app. `PROVENANCE.json` records original adapter and official schema reference identities; no official graph or third-party asset is redistributed.

The no-slip estimate follows actual forward body velocity plus yaw at +/-1.46 m. It does not model wheelspin, contact/airborne state, lateral sliding, native wheel rotation, or wreck rolling. These limits remain even after a successful signal test.

## Native and evening checklist

The five named cases and expected outcomes are in `NATIVE-SCENARIOS.json`. A033-00 is an authoring prerequisite, not a drive/listen test ready to run. Signal-only compilation/readback can be queued independently when the integration owner obtains the slot. Native tests must use fresh source pins, one qualified adapter, actual motion telemetry, ordinary input and retained loopback plus visible evidence. Record failures without lowering thresholds. Never infer listening quality from signals.

After A033-00 passes, Marco can choose a short two-minute cabin/exterior comparison (A033-01 through A033-03) or the full distance/lifecycle pass including A033-04. Ask for judgment on loudness, pitch, repetition/clicks, engine masking and cabin/distance balance. All checkboxes remain unchecked: native compile/load [ ], audible stock loop [ ], ordinary controls [ ], signal behavior [ ], listening acceptance [ ], cleanup [ ], MP/JIP [ ].

## Integration choice

Safe default for tonight: retain released audio and keep this QA pack as a follow-on. Optional signal-probe inclusion adds no audible benefit. Do not hold crew/smoke fixes on GEP-033 and do not reopen custom gun audio.


## Source continuation

The source-only continuation adds a read-only Workbench resolver with explicit fresh evidence output, a native-log parser, a NONLOADABLE original ACP authoring template, an explicit dB diagnostic signal, and a default-disabled one-attempt lifecycle probe. None is natively compiled or instantiated. See [the exact next native batch](authoring/NEXT-NATIVE-BATCH.md) before copying or running anything. The resolver-only copy root is separate from the Game/playback sources. Run check-authoring.py for source/synthetic checks; these do not provide native evidence. The current evening manual candidate remains untouched.

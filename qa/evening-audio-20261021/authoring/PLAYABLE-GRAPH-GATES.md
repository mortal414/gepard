# Exact safe path from the resolver to a playable graph

The resolver stage `audio-resolver-01` contains the committed Workbench module: staged script SHA256 `c6e3da2275b44419a2fe20e2f507815db6b867470e0e484416d638c7c6f55a4e` matches the source pin. The admitted root owns compilation, execution and normal close. This lane inspected source only and changed no stage/native/production file.

The staged wrapper passes the expected plugin class and fresh evidence target. Its exit/lease report is a lifecycle receipt, not resolver acceptance: after normal close, require terminal `GEP033_PROBE_END observations=9 saved=1`, durable JSON status/9 unique rows, matching raw console, and parser output. Keep script/resource errors separate from deliberate missing-control behavior. An absent candidate graph is expected in the first batch.

## Stages and stop conditions

1. **Resource identity.** Preserve the actual four stock WAV identities and current wheel/engine metadata. The Slow row must name exactly the known stock path with a native-resolved GUID. A raw-path Resource.Load that matches a canonical load proves only that loader's resolution; two invalid WAV loads leave sample-path support unresolved. Do not manufacture a canonical name from the package filename.
2. **Native source authoring.** In a fresh isolated successor, create one original Slow bank, shader, Sound `GEP_TRACK_MOVEMENT_LP` and native `VEH_Tires` route. Use the native editor to create/save the original sound configuration and mixer input mapping. It must replace the intended rolling branch after inspection, rather than silently duplicate inherited tire rolling. Keep all engine/gun graphs exact. The `.acp.in` remains a nonloadable topology worksheet; it has unresolved tokens and no registered resource.
3. **Quiet static preview.** Keep the loop at a documented low/quiet level while establishing node/port semantics in Audio Editor. A bank's serialized `Volume` attribute is dB; this does not establish every connected port's unit or whether the port is live. Do not connect linear `GEPTrackLevel` (0..0.45) directly to a dB input. Require the observation matrix below before admitting movement playback.
4. **Signal coupling and live updates.** Bind the custom signals by name using original native signal source. Verify which ports consume linear amplitude, dB, semitones or pitch ratio. Preserve exact-zero linear mute intent. `GEPTrackVolumeDb` is a convenience conversion (0 -> -60dB, .1 -> -20dB, .45 -> approximately -6.936dB); a -60dB floor is attenuation, not mathematical silence. Use a natively verified mute mechanism when gain is zero. Enable and prove dynamic parameter updates on the running loop.
5. **Single-event fixture.** Only after graph/event validation, instantiate a separate QA successor with the default-disabled lifecycle probe deliberately enabled. Verify one native handle/event, hull-following position and native signal context. Stop/delete once, with no per-frame restarts. The existing conservative lifecycle may become terminal on distance-culling/IsSoundPlayed semantics; distance-away/return is a required rejection test, not already qualified behavior.
6. **Listening and candidate admission.** Record actual body motion, signals, native handle counts, visible behavior and synchronized loopback. Marco's unchecked listening choices remain idle/blip, slow, fast, matched-speed reverse, turns, coast/engine-off coast, full stop, cabin/exterior, then distance/lifecycle. No audio-quality or gameplay claim follows from compilation or metadata.

## Native observation matrix

| Gate | Native stimulus | Required observation | Reject/hold condition |
| --- | --- | --- | --- |
| Port units | Hold the same isolated loop/sample; compare explicit native volume values and the corresponding input controls | Record the editor's parameter unit/port label and effective level behavior; identify the actual amplitude multiplier or dB conversion | Unit inferred only from a variable name or serialized static `Volume` field |
| Idle mute | Drive gain from .1 to exact 0 while the event remains running | Verified native mute/gain state suppresses this event, with an isolated output meter/loopback corroboration after the fade | Linear zero is treated as 0dB, or only a finite attenuation floor is presented as exact mute |
| Live volume | Start once at quiet level; vary gain through 0, .01, .1 and .45 | The same handle's level follows each change, with correct documented mapping | Only startup value is sampled or each change starts a new event |
| Live pitch | Start once; vary proposed semitone control -3, 0 and +5 | Correct native pitch units and continuously updated bounded pitch | Semitone values are fed into a ratio input, or connected port never updates |
| Mixer route | Save native named VEH_Tires connection and inspect effective output | Native-generated route/config agrees with the named movement bus | A port integer is guessed from the engine sample or route label alone |
| One event | Spawn one QA vehicle, drive, stop, exit/reenter | Exactly one intended loop instance per vehicle; no inherited rolling duplicate | Two rolling layers, repeated START_ISSUED, detached/world-fixed source |
| Culling return | Move the listener away and back to the same still-present rolling subject | Expected native culling and audible return without leaked or duplicate events | Terminal mute after return, repeated restarts, stale handle assumption |
| Cleanup | Destroy/delete then respawn one fresh subject | Original event ends once; fresh subject owns only its new event | Orphan loop, duplicate termination/restart, callback touching deleted source |

The resolver does not measure any of these port/mute/lifecycle gates. Do not use a `METADATA_MATCHES_ONLY` parser outcome as their acceptance. Dynamic-input/loop behavior needs current native evidence even though official historical notes describe per-port Update flags.

## Read-only audit tooling

After the native editor saves a new **original** source graph, `audit-authored-graph.py --graph <new-original.acp> --resolver-report <native-GEP033-RESOLVER.json> --output <fresh-audit.json>` checks the bounded topology, exact stock Slow canonical sample, one bank/event/shader/mixer, no unresolved tokens or unrelated engine/gun/third-party references, and loop properties. It refuses overwrite and pins both input hashes. It deliberately outputs `BLOCKED_PENDING_NATIVE_PORT_UNITS_MUTE_LIVE_UPDATE_EVENT_LIFECYCLE` even when source shape passes. It must not be run against a protected third-party graph. A source audit is neither compilation nor playback admission.

No playable ACP has been emitted in this lane because native identity/port/mute evidence has not yet arrived. This preserves an honest, bounded path to one safe graph without treating the prior incomplete qualification as a reason to stop useful source work.

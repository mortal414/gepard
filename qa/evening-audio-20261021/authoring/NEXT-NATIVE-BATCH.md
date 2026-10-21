# GEP033 source continuation: exact native batch

Status: source/tooling Ready; GEP-033 audible movement remains Blocked. Do not modify or instantiate the current evening manual candidate. No native app/process/lease/session operation was performed by this continuation.

## Copy and compile boundary

Workbench-only copy root: `signal-probe/scripts/WorkbenchGame/GEP033` into the selected private QA addon's `scripts/WorkbenchGame/GEP033`. This is enough for the resolver plugin. It uses base-game Workbench APIs only, has no game-world dependency, and does not require the signal/gameplay scripts. WorkbenchGame is the supported plugin module directory ([official plugin tutorial](https://community.bistudio.com/wiki/Arma_Reforger%3AWorkbench_Plugin_Tutorial)). Alternatively use the optional project `signal-probe/GEP033SignalProbe.gproj`, resolving exactly one chosen Gepard source. Never load duplicate Gepard GUIDs.

First compile that frozen private QA successor using the maintained admitted native wrapper. Read actual compile output and retain a clean original console. All new Enforce source remains UNCOMPILED until then. The resolver is a WorkbenchPlugin, not a Game probe. Graph/lifecycle Game scripts need not be copied or instantiated for this first batch.

## Batch 1: resolver only, no playback or Audio Editor opening

Use the parent's qualified serial Workbench plugin wrapper, with its atomic lease/identity/normal-close gates and dry-run default. Plugin class: `GEP033_ResolveStockAudioPlugin`. Native arguments: `-wbModule=ResourceManager -plugin=GEP033_ResolveStockAudioPlugin -gep033Output <fresh-run-root>/GEP033-RESOLVER.json`. The target directory must already exist and the report must not. The plugin rejects a missing argument or existing report. It never calls Workbench.Exit, opens a module, plays audio, changes source resources, saves a graph, or controls sessions; the parent closes its admitted owned Workbench normally.

Expected durable report `status=NATIVE_RESOLVER_OBSERVATIONS_ONLY`, `audibleImplementation=false`, 9 resource rows, and up to 4 graph summaries. Success terminal prefix: `GEP033_PROBE_END observations=9 saved=1`. `saved=0` is a failed evidence save, not success. CONFIG_ERROR produces no observation batch. Preserve the original console even when parsing fails.

The 9 rows cover four stock track WAV paths, current left/right APC wheel ACPs, current LAV25 engine ACP, the later original candidate graph, and a known-missing path control. Expected stock identities are GUID-qualified canonical names with exact matching paths. The missing control should have no GUID/load. The candidate should initially be absent. Both raw path and canonical Resource.Load calls are measured; equal valid loaded identities support path-only resolution in that loader. Two invalid WAV loads establish no path-versus-GUID conclusion because the generic loader may not accept that resource type. They do not disprove Audio Editor sample references. No fallback GUID is fabricated.

Run `python parse-resolver-log.py --log <original-console> --output <fresh-result.json>`; it refuses overwrite/incomplete/duplicate/untyped observations and pins the original log SHA. It does not treat resource loading or available graph metadata as compilation, playback, event lifetime or listening acceptance. `--selftest` exercises only synthetic parser cases and writes no native result. A native graph returning no BaseContainer is an explicit inspection limit, not an empty graph.

Limited graph summaries report root type, bank count, event names and named mixer inputs/resources. They do not export graph internals, algorithms or audio payloads. Native results should establish the current APC wheel event names and whether VEH_Tires is actually in its output path; inherited FileNames and wheel slot resource names alone do not prove the runtime mix.

## Batch 2: original source graph authoring after stock identity is known

`GEP_TrackMovement.acp.in` is NONLOADABLE authoring material with original node IDs and names. Native node classes and the Bank(out65) -> Shader(in64/out65) -> Sound(in64/out65) -> Mixer topology follow the official car source schema already inspected. It has no .meta and no runtime reference. Tokens deliberately remain for the canonical stock WAV, a newly authored track sound configuration, and the native-generated VEH_Tires input port. This is not a runnable ACP or a transformed JSON file. The Mixer input table/port and Sound config must be rebuilt by the native editor; do not guess a numeric port from the sample engine route. The template is held at -60dB with no gain/pitch signal connections, so it is not a movement implementation even after token substitution.

In the admitted isolated Audio Editor, author a new original graph, one stock Slow bank, one shader, one `GEP_TRACK_MOVEMENT_LP` Sound, and native VEH_Tires mixer routing. Resolve the stock path via the resolver; test a raw sample path only in this new scratch graph, then inspect the native-saved Filename/canonical identity. The official car source uses GUID-plus-path samples; it does not prove path-only sample references work. Save/register/build only this successor graph and its original config/signals. Keep all inherited engine, gun and vehicle drive files untouched. Inspect which existing APC wheel rolling branch it would replace; no second rolling layer is accepted silently.

Two new authoring gates matter: Bank Volume is documented in dB, while `GEPTrackLevel` is linear gain. Never feed idle linear zero into a dB input as silence. The probe now also offers `GEPTrackVolumeDb=20log10(gain)`, floored at -60dB; this does not itself supply exact mute. Verify the native input units and an exact-zero mute gate. Ensure volume/pitch input ports have Update enabled and change on an already-running loop; connected static ports can preserve the startup value. Source: [official node reference](https://community.bistudio.com/wiki/Arma_Reforger%3AAudio_Editor%3A_Nodes), [official port-update change notes](https://reforger.armaplatform.com/news/experimental-october-12-2023). Confirm those facts against the installed current editor, rather than treating historical notes as current qualification.

Start with Audio Editor simulation/live signal controls: idle gain0, creep0.1m/s, slow1m/s, fast14m/s and reverse4m/s. Verify exact resource identity, no missing nodes/attributes, correct pitch units and live gain/pitch updates. Native compile plus editor preview still does not prove vehicle playback. Do not activate a template by renaming its extension.

## Batch 3: one-event lifecycle only after graph/event gates

`signal-probe/Prefabs/Vehicles/GEP033_LifecycleProbe102.et` is a default-disabled optional source child. Do not instantiate it until the graph/event owner is ready for a deliberate lifecycle test. In a fresh QA successor, attach only the registered original ACP to its inherited SoundComponent and explicitly enable `m_bEnableCandidate`. Do not add the custom event to production or infer arbitrary new events auto-start.

The original lifecycle component waits for the four diagnostic signals to bind, makes at most one SoundComponent.SoundEventOffset call, owns its returned handle, never restarts per-frame/throttle/culling, and terminates it on destroyed/deleted cleanup. It gives up after 5 seconds without ready signals and never retries uncertain starts. START_ISSUED/STOP_ISSUED are issued-call diagnostics, not successful playback evidence. Qualified native measurement must show one playing instance, no duplicate inherited rolling, motion-following source, bounded signals, real stop/delete cleanup and normal native closure. AudioSystem.IsSoundPlayed semantics under culling/dynamic simulation are unqualified; the conservative probe may become terminal when a handle is not observable. Verify distance-return behavior before adopting this as production lifecycle. Keep it excluded if it falls silent permanently, leaks, restarts, duplicates loops or touches engine/gun events.

## Marco's short listening pass after all earlier gates

Keep every acceptance box unchecked until observed. Choose the 2-minute cabin/exterior pass, or add the full distance/lifecycle option.

- [ ] 10s parked idle, then stationary throttle blip: no added track rumble.
- [ ] 10s slow forward, then 10s safe faster forward: natural speed-linked loudness/pitch, no repeating restart/shot rhythm, no clicks.
- [ ] 5s reverse at a matched actual speed: comparable texture/loudness to forward.
- [ ] Gentle rolling turn each way, then throttle-release coast and safe engine-off coast: audio follows motion without engine masking or abrupt gates.
- [ ] Brake to a full stop and wait 5s: smooth fade, no idle creep noise or stuck loop.
- [ ] Repeat cabin/exterior: convincing balance without excessive pitch/volume. Stock guns sound unchanged.
- [ ] Full option: listen at 5/30/100m, move away/back, exit/reenter, destroy/delete/respawn: no doubled layer, orphan loop, lost distance-return playback or cleanup error.

Retain synchronized loopback, actual body motion/signal/handle telemetry and visible evidence, with a same-condition stock baseline comparison. Marco owns sound-quality acceptance. MP/JIP/dedicated remain separately untested. Current safest evening option remains released stock audio.


## Resolver v2 configuration repair and bounded port metadata

Root reported v1 script compilation PASS but native invocation collected no observations because module.GetCmdLine did not find the custom argument. V2 reads System.GetCLIParam with the undashed key, the form explicitly documented by installed System API for '-world something'. Existing two-argument launcher syntax remains appropriate; preserve failed v1 evidence. V2 also records each Sound's canonical parent config, output state/port and volume variable, optional native enum names/IDs, and mixer active input IDs. An empty enum list or unavailable container is a limit, not a permission to guess a VEH_Tires port. Public BaseContainerTools.LoadContainer is a bounded fallback attempt when Resource.Load exposes no BaseContainer. These metadata do not establish dynamic volume units, mute, or actual playback. Current v2 remains source-only until root compiles/runs it.

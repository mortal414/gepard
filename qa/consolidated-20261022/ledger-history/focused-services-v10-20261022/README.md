# Consolidated private Gepard playtest

**Double-click [Launch-Gepard-ProvingGround.cmd](Launch-Gepard-ProvingGround.cmd)** for the single handoff path. It calls the unchanged [guarded PowerShell launcher](Launch-Gepard-Private-Demo.ps1). Both are already installed from verified source. The default is an untimed human session with a fresh owned profile and run each time. The shim changes no execution policy or security settings.

The current candidate is **ordinary source v6, compile06, private pack08, installed08**. The package-only probe loaded the actual player and world, four OLD/NEW bays, two targets and matching layout hashes, then closed normally. Both package trios are verified. Strict native diagnostics remain **FAIL**: 59 errors in the ordinary08 probe, 100 across validation/packaging, and 57 in the separate rearm fixture are retained. This supports a bounded manual session; it adds no gameplay acceptance or publication.

Use the [current candidate manifest](CURRENT-CANDIDATE-MANIFEST.json), [evidence index](EVIDENCE-INDEX.md) and [GEP status](qa/consolidated-20261022/STATUS-PER-GEP.json). The old root SOURCE_MANIFEST.json is historical release 1.0.2 provenance.

The canonical source contains 1,152 Gepard files (1,112 core + 35 overlay + 5 generated textures) and 10 world files. Of the original release files, 1,105 remain exact and seven have deliberate approved functional/HUD script changes. The art overlay itself preserved the then-current core. Both NEW V11 skins are bound. **OLD means old exterior with the same current scripts and optic**, not an independent release-code runtime.

Five actual rebuilt XOB hashes changed from the registration seeds; 15 resource outputs were adopted with GUID metadata preserved. Static registration is separate from dynamic acceptance: the final art case captured one of six camo poses. Remaining poses, charcoal extremes, clearance, hatches, LOD and performance still need review.

HUD V3 passed eight narrow data assertions at both 1280x720 and 1920x1080, with 15 phase snapshots and captures at each resolution. Five sampled 720p views and three sampled 1080p views were visually reviewed, all at 16:9. These are daytime 16:9 observations; night/low-light cycle judgment is still pending. Positive target ranging, accuracy, other aspects and physical/design acceptance remain pending. Seat exit hid the optic and return restored one optic; broader repeated or damaged-seat lifecycle acceptance is unfinished.

The proving ground contains 31 owned roots, 23 boards, eight route gates and 100m/300m targets. Measured geometry, pristine reset and occupied/near-player refusal observations are indexed. Board V3 also observed actual registered read/reset/debounce behavior; its original adapter remains NOT_TESTED because emitted inline ID fields disagree. Physical F/menu use, rendered read hints, route handling and target impacts remain for manual review. Hostiles remain OFF and locked.

One source-bound camo **left-feed refill** was observed: 315 to 320 rounds, with the selected finite US-provider container falling from 1,000 to 997 supplies. The two other observed containers stayed unchanged; total observed supplies fell from 3,000 to 2,997. Pack08 includes the stock US M923 service-ammo provider; DamageAmmo remains the stock Ural. The original private case remains FAIL because of instrumentation mismatches, with 57 native errors retained. This is not a whole-service pass: right-feed, repeated/full/empty-negative checks, repair, refuel and human/MP acceptance remain pending. The guide offers one optional left-feed check.

Smoke and new track audio remain excluded under separate gates. The audited smoke angle replay now shows one payload and measurable flight; visible cloud, alignment and full/empty/rearm qualification remain pending. Bounded audio authoring approval is still pending. Stabilization and ballistic correction remain disabled/deferred. Stock cannon sounds are retained.

No new GEP completion checkbox is checked. Preserved accepted driving, R and braking behavior is compatibility evidence, not new user homework. The manual session focuses on the new exterior, optic and proving-ground convenience. Historical failed results are preserved, and no upload or publication is included.

[Manual test guide](TEST-GUIDE.md) gives the station sequence and open judgments. Root installs its exact sealed bytes separately.


Current source/evidence audit: [59 startup and100 packing diagnostics classified](qa/consolidated-20261022/diagnostic-severity-08/REPORT.md). No demonstrated demo-blocking regression or justified stock-resource edit; strict raw failures remain. [Isolated smoke flight03](qa/consolidated-20261022/smoke-flight-03/RESULT.json) is a narrow observation, not smoke inclusion or GEP acceptance.

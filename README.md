# Gepard 35 mm SPAA - 1.0.5

Playable WIP Gepard for Arma Reforger with twin35mm guns, camouflage and charcoal variants. Current published Workshop item: **D561AD08932E7B40**.

## Current status and visual plans (2026-10-28)

We hope to revisit the exterior and interior models over the next few months, depending on the success of our current vehicle project and the blend-review workflow. Gepard is more complex, but we think this approach could help us improve it. There is no firm timeline or promised release for these visual updates.

## Changes in1.0.5

- Optical stabilization with entity/position tracking and a confirmed entity-lock reticle cue.
- Independent C rangefinding and approximate ballistic elevation adjustment for left/right/both current HEI-T feeds, with stabilization OFF or ON.
- Accepted reference-led exterior detail batch across the side storage/fascia, front guards, rear lamp shell/service cover, tracking-head casing and roof equipment.
- Cleaned runtime resource closure; useful development tests and model exports remain separate.

## Controls

RMB enters the optic. Shift+V cycles Both/Left/Right. C ranges the aimed point and adjusts elevation without changing the selected stabilization mode. Hold Space toggles stabilization; tap Space while enabled to designate/clear a target. Entity-lock endcaps require a confirmed usable entity lock; terrain/position tracking does not show them. Re-range after changing the gun group/ammunition or leaving the optic.

## Dependencies

Requires the stock game, **WCS_SpaceCore (5E389BB9F58B79A6)** and **WCS_Armaments (629B2BA37EFFD577)**. Their code/assets remain in the original Workshop packages. Bradley is not required.

## Known limitations

Ballistic correction is approximate and game-balanced, not validated real-world fire control. Both guns share an elevation axis; incompatible mixed ballistic feeds are not supported by the common correction. Crew protection against fragments/nearby explosions remains unfinished. Independent destruction of the mounted default turret hit zone remains disabled while the hull is viable; hull, crew and component damage remain active. Radar deployment/sweep are cosmetic; radar sensing is unavailable and separate from optical tracking. MP/JIP/dedicated/console behavior was not newly qualified. No realistic armor/protection claim is made.

Existing smoke deployment/rearming, optic HUD, track audio and component-damage behavior are retained from earlier releases.

## Source, tests and evidence

`addon/` is the exact clean runtime source closure. [Release record](releases/1.0.5.md), `manifests/`, and `qa/release105/` separate source/content evidence, development-only worlds and manual acceptance from strict native diagnostics. Changed native model FBXs are under `source/models/1.0.5/`; unchanged historical authoring inputs remain preserved in the1.0.4 branch/tag history. Earlier QA/proving-ground sources remain available and are not part of the public runtime package.

Workshop1.0.5 and its changelog are verified published. The intended Workshop description refresh remains pending because the native CLI reused prior metadata; this repository does not claim that text was updated.

Feedback: https://github.com/mortal414/gepard/issues/new?template=bug_report.yml

Native content remains under Bohemia Interactive's terms. The limited Kiichi tracked-guide MIT notice remains in the source/package. Generic tooling: https://github.com/mortal414/reforger-agent-harness

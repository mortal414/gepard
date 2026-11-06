# Gepard 35 mm SPAA - 1.0.6

Playable WIP Gepard for Arma Reforger with twin 35 mm guns, camouflage and charcoal variants. [Workshop release](https://reforger.armaplatform.com/workshop/D561AD08932E7B40): **D561AD08932E7B40**.

This repository now matches the published **1.0.6 Build07** source. It does not include the later Small size candidate or subsequent overnight corrections. See the [release record](releases/1.0.6.md) and [exact source manifest](manifests/addon-1.0.6-build07-sha256.json).

The release updates the model and gunner HUD, preserves both finishes and grey metal mechanisms, and replaces Workshop screenshots. Reported model flaws remain for a follow-up. The separate [Blender showcase](https://github.com/mortal414/gepard-blender-showcase) contains authoring work and rendered images; those are not gameplay evidence.

## Controls

RMB enters the optic. Shift+V cycles Both/Left/Right. C ranges the aimed point and adjusts elevation without changing the selected stabilization mode. Hold Space toggles stabilization; tap Space while enabled to designate/clear a target. Entity-lock endcaps require a confirmed usable entity lock; terrain/position tracking does not show them. Re-range after changing the gun group/ammunition or leaving the optic.

## Dependencies

Requires the stock game, **WCS_SpaceCore (5E389BB9F58B79A6)** and **WCS_Armaments (629B2BA37EFFD577)**. Their code/assets remain in the original Workshop packages. Bradley is not required.

## Known limitations

Ballistic correction is approximate and game-balanced, not validated real-world fire control. Both guns share an elevation axis; incompatible mixed ballistic feeds are not supported by the common correction. Crew protection against fragments/nearby explosions remains unfinished. Independent destruction of the mounted default turret hit zone remains disabled while the hull is viable; hull, crew and component damage remain active. Radar deployment/sweep are cosmetic; radar sensing is unavailable and separate from optical tracking. MP/JIP/dedicated/console behavior was not newly qualified. No realistic armor/protection claim is made.

Existing smoke deployment/rearming, optic HUD, track audio and component-damage behavior are retained from earlier releases.

## Source and provenance

`addon/` contains the complete immutable editable addon source frozen for the published Build07, including import metadata and authoring FBX/TIF/TXO files. Use Git LFS when cloning (`git lfs pull`). Workbench regenerates `resourceDatabase.rdb`. The exact published PAK hash and source hashes are recorded in the release manifest. Dependencies remain in their own Workshop packages. Private native test fixtures and machine-specific logs are excluded from this sync; historical QA/source remains in Git history.

Feedback: https://github.com/mortal414/gepard/issues/new?template=bug_report.yml

Native content remains under Bohemia Interactive's terms. The limited Kiichi tracked-guide MIT notice remains in the source/package. Generic tooling: https://github.com/mortal414/reforger-agent-harness

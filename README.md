# Gepard 35 mm SPAA - 1.0.7

Playable WIP Gepard for Arma Reforger with twin 35 mm guns, camouflage and charcoal variants. [Workshop release](https://reforger.armaplatform.com/workshop/D561AD08932E7B40): **D561AD08932E7B40**.

The repository matches published **1.0.7**. Its actual runtime download is **191.30 MB**, about 60% smaller than 1.0.6. It uses the selected Small texture tier and includes the accepted interior/clearance, driver hatch/entry and gunner HUD updates, plus restored commander smoke permission. See the [release record](releases/1.0.7.md) and [exact source/package manifest](manifests/addon-1.0.7-sha256.json).

The prepared 151.98 MB container was rebuilt by the native publisher; it is not the live download size. All 545 published runtime resources match the tested candidate. The release record explains the size discrepancy and test limits.

## Controls

RMB enters the optic. Shift+V cycles Both/Left/Right. C ranges the aimed point and adjusts elevation without changing the selected stabilization mode. Hold Space toggles stabilization; tap Space while enabled to designate/clear a target. Entity-lock endcaps require a confirmed usable entity lock; terrain/position tracking does not show them. Re-range after changing the gun group/ammunition or leaving the optic.

## Dependencies

Requires the stock game, **WCS_SpaceCore (5E389BB9F58B79A6)** and **WCS_Armaments (629B2BA37EFFD577)**. Their code/assets remain in the original Workshop packages. Bradley is not required.

## Known limitations

Ballistic correction is approximate and game-balanced, not validated real-world fire control. Both guns share an elevation axis; incompatible mixed ballistic feeds are not supported by the common correction. Crew protection against fragments/nearby explosions remains unfinished. Independent destruction of the mounted default turret hit zone remains disabled while the hull is viable; hull, crew and component damage remain active. Radar deployment/sweep are cosmetic; radar sensing is unavailable and separate from optical tracking. MP/JIP/dedicated/console behavior was not newly qualified. No realistic armor/protection claim is made.

Existing smoke deployment/rearming, optic HUD, track audio and component-damage behavior are retained from earlier releases.

Commander smoke permission is restored through the existing WCS system. Its original Left Shift+G binding and radio-conflict behavior still require a real-key check; action-driven deployment/rearm tests do not establish keyboard acceptance.

## Source and provenance

`addon/` contains the exact immutable editable source for the published release, including model/texture import and authoring inputs. Use Git LFS when cloning (`git lfs pull`). Workbench regenerates `resourceDatabase.rdb`; the published trio hashes and per-file source hashes are recorded in the manifest. Dependencies remain in their own Workshop packages. Private QA, profiles and machine logs are excluded. Prior 1.0.6 remains at `v1.0.6`.

Feedback: https://github.com/mortal414/gepard/issues/new?template=bug_report.yml

Native content remains under Bohemia Interactive's terms. The limited Kiichi tracked-guide MIT notice remains in the source/package. Generic tooling: https://github.com/mortal414/reforger-agent-harness

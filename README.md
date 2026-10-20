# Gepard 35 mm SPAA — early WIP

Release source and cooked assets for an experimental Gepard in Arma Reforger. This is an early WIP: several functions are unfinished and improvements are planned.

## Known limitations

- Driving and handling need more work.
- Crew can still be injured or killed by fragments and nearby outside explosions. Crew protection is unfinished.
- Independent destruction of the mounted default turret hit zone is temporarily disabled while the hull remains alive. Hull, crew and other component damage remain active; this is not validated realistic armor.
- Radar deployment and sweep are cosmetic. Radar sensing, search and radar tracking are unavailable.
- Ballistics, aiming correction and stabilization remain unfinished.
- Multiplayer, join-in-progress, dedicated-server and console behavior are not fully qualified.
- Sounds use stock game assets.

## Basic controls

Open the appropriate hatch and use Get in. Driver: hold R to start, W/A/S/D to drive/brake/steer, Space for the handbrake. Gunner: right-click for sights, mouse to aim and left-click to fire. Use the crew controls to change seats; X exits. Crew and optics bindings can be changed in the controls menu. Rearm each feed through its side action near a supply source.

## Contents and local use

Open `addon/Gepard.gproj` in Arma Reforger Workbench. It depends only on native Arma Reforger game data (`58D0FB3206B6F859`). Mount one copy of the project GUID `D561AD08932E7B40` at a time.

This repository contains the reviewed release prefab, configuration and script sources plus cooked model, material, texture and resource-database files. Raw authoring models/textures are not included. `SOURCE_MANIFEST.json` pins the exact release payload. The Workshop preview, private QA scenes, profiles, logs, reference downloads, captures, experimental audio and the private development history are excluded. Large cooked files use Git LFS; use `git lfs pull` after cloning.

The temporary turret gate is `m_bIndependentTurretDamageEnabled692 0` in the mounted turret prefab. Setting it to `1` restores native independent default-zone damage. Existing destroyed saved turrets are not automatically healed; use a fresh vehicle when checking the change.

## Feedback

Report gameplay bugs through the [bug report form](https://github.com/MarcoBetti1/gepard/issues/new?template=bug_report.yml) and visual/model feedback through the [visual feedback form](https://github.com/MarcoBetti1/gepard/issues/new?template=visual_feedback.yml). Include game/mod versions, reproduction steps, other enabled mods and relevant screenshots or logs. Avoid posting private information or credentials.

The separate [Reforger Agent Harness](https://github.com/MarcoBetti1/reforger-agent-harness) contains the generic development tools. [Tooling feedback belongs there](https://github.com/MarcoBetti1/reforger-agent-harness/issues).

## Notices and license status

The limited Kiichi tracked-guide MIT notice is retained under `addon/Docs/Licenses/` and embedded in the included running-gear source. It applies only to that contribution. Native dependency content remains under its original terms. A whole-mod reuse/Workshop license has not yet been selected; the harness's Apache-2.0 license does not license these vehicle assets.

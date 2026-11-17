# Gepard 35 mm SPAA

Playable WIP Gepard for Arma Reforger, with twin 35 mm guns and camouflage and charcoal variants.

**[Workshop · 1.0.7 · 191.30 MB](https://reforger.armaplatform.com/workshop/D561AD08932E7B40)**

## Blender development gallery

[See the updated model](https://github.com/mortal414/gepard-blender-showcase). We're working to bring this updated look into Arma Reforger. The renders show work in progress in Blender, not the current in-game release.

## Controls

- **RMB:** enter the optic.
- **Shift+V:** cycle Both / Left / Right guns.
- **C:** range the aimed point and adjust elevation.
- **Hold Space:** toggle stabilization.
- **Tap Space while stabilized:** designate or clear a target.

Re-range after changing guns/ammunition or leaving the optic. Lock endcaps appear only for an entity lock, not terrain tracking.

## Requirements

- [Arma Reforger](https://reforger.armaplatform.com/)
- WCS_SpaceCore (`5E389BB9F58B79A6`)
- WCS_Armaments (`629B2BA37EFFD577`)

Bradley is not required.

## Known limitations

- Radar movement is cosmetic; no radar sensing.
- Ballistic correction is approximate. Both guns share elevation, so incompatible mixed ammunition is unsupported.
- Crew protection is unfinished. The default turret cannot be destroyed independently while the hull remains viable; other damage remains active.
- Commander smoke works in action-driven tests; **Left Shift+G and its radio conflict still need a real-key check**.
- Multiplayer, join-in-progress, dedicated servers and console behavior were not newly tested for 1.0.7.

[Release details and testing](releases/1.0.7.md) · [Report a bug](https://github.com/mortal414/gepard/issues/new?template=bug_report.yml)

## Source

[`addon/`](addon/) contains the published 1.0.7 source. Clone with Git LFS and run `git lfs pull`. Dependencies stay in their Workshop packages.

[File manifest](manifests/addon-1.0.7-sha256.json) · [Model provenance](manifests/model-provenance-1.0.7.json) · [Tooling](https://github.com/mortal414/reforger-agent-harness)

Native content remains under Bohemia Interactive's terms. The [Kiichi tracked-guide MIT notice](addon/Docs/Licenses/Kiichi-TrackedGuide-MIT.txt) applies to that contribution.

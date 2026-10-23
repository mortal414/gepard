# Gepard 35 mm SPAA - 1.0.4

Early WIP Gepard for Arma Reforger. Release source is in `addon/`; the existing Workshop item is **D561AD08932E7B40**.

## Changes

- Enabled smoke deployment and rearming.
- Improved component damage while preserving living-crew access.

## Dependencies

The project declares the stock game, **SpaceCore (5E389BB9F58B79A6)** and **WCS Armaments (629B2BA37EFFD577)**. The tested external packages were version8.4.0. Workshop installs them separately; their code/audio/assets are not redistributed in this repository or Gepard's package. Bradley is not required.

## Known limitations

Damage values may need further tuning. Component failures and crew access are game-behavior improvements, not validated real-world protection. Crew protection against fragments/nearby explosions remains unfinished. Independent destruction of the mounted default turret hit zone remains disabled while the hull is viable; other component damage remains active.

Smoke deployment and finite rearm were accepted in the private test; smoke spread/screening can improve. Stabilization, ballistic correction, radar sensing, interior details and wider multiplayer/JIP/dedicated/console qualification remain unfinished. Radar deployment and sweep are cosmetic. Broader fuel/service behavior is not newly blanket-qualified.

## Reusable proving ground

The private comparison map remains available for later versions. [Concise GEP test scope](PLAYTEST-1.0.4.md) is separate from operational instructions. Private QA and proving-ground worlds are excluded from the public vehicle package.

## Evidence

See [release record](releases/1.0.4.md) and `qa/release104/`. Earlier frozen release records and rollback snapshots remain preserved. Native compile, packaging, gameplay observations and strict engine diagnostics are reported separately.

Feedback: https://github.com/mortal414/gepard/issues/new?template=bug_report.yml

Native content remains under Bohemia Interactive's terms. The existing limited Kiichi tracked-guide MIT notice remains in the source and package. Generic development tooling: https://github.com/mortal414/reforger-agent-harness

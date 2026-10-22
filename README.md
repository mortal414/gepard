# Gepard 35 mm SPAA — 1.0.3

Early WIP Gepard for Arma Reforger. Release source is in `addon/`; the existing Workshop item is **D561AD08932E7B40**.

## Changes

- Updated the gunner optic HUD.
- Added tracked-vehicle movement audio.

## Dependencies

The project declares the stock game, **SpaceCore (5E389BB9F58B79A6)** and **WCS Armaments (629B2BA37EFFD577)**. The tested external dependency packages were version 8.4.0. They are installed separately through Workshop; their code/audio/assets are not redistributed in this repository or Gepard's package. Bradley is not required. Cannon and engine audio assets and accepted driving/reverse/braking/R controls are unchanged.

## Known limitations

Crew protection, independent turret damage, stabilization, ballistic correction, radar sensing, interior details and wider multiplayer/JIP/dedicated/console qualification remain unfinished. Radar deployment and sweep are cosmetic. Experimental smoke remains included but unresolved: Marco could not deploy it in the final demo, so it is not accepted or advertised as a working feature. Fuel/repair transfer and the wider smoke service behavior remain unqualified.

## Reusable proving ground

The private OLD/NEW proving ground is retained for future version comparisons. [GEP test points](PLAYTEST-DEMO10.md) are separate from [operating instructions](PLAYTEST-OPERATIONS-DEMO10.md). Demo10 and Demo09 remain frozen rollback payloads; the proving-ground QA is excluded from the public vehicle package.

## Evidence

See [release record](releases/1.0.3.md), `qa/release103/` and the preserved Demo10 evidence under `updates/demo10/`. Compilation/package creation does not establish gameplay acceptance. Raw diagnostic failures and manual-test limits are preserved separately from observed behavior.

Feedback: https://github.com/mortal414/gepard/issues/new?template=bug_report.yml

Native content remains under Bohemia Interactive's terms. The existing limited Kiichi tracked-guide MIT notice remains in the source and package. Generic development tooling: https://github.com/mortal414/reforger-agent-harness

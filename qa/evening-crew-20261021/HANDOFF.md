# Crew/control lane for evening candidate

Baseline: verified released Gepard 1.0.2 `699f245afaf3e912b9ce9827b0f3f5f7dd32f3e7`. Local branch `sprint/evening-crew-20261021`. No native process, lease/session action, push or publication.

Driver, crew and station-light eligibility now reject unconscious users; driver/crew additionally require the user to be the seat's actual occupant. These checks do not add a hull-damage veto or alter native exit/hatch actions. The internal commander switch says `Move to commander`; outside boarding retains `Get in commander`.

Four scripts changed: `GEP_DriverStation.c`, `GEP_RadarSystem.c`, `GEP_CrewActions579.c`, `GEP_SwitchSeatAction.c`. No prefab, geometry, driving/R/brake, charcoal, stock gun sound, HUD rendering or range/ballistic implementation changed. Refuel is already authored on both sides in baseline; no guessed repositioning was performed. Commander seat/view offsets require native inspection before adjustment.

The released `SCR_ActionMenuInteractionDisplay.SetDisplayData` override remains byte-for-byte equivalent after text normalization: it hides only the selected return-periscope prompt for this actual driver in nonzero periscope mode. This is intentional per the user's later instruction: hide Leave periscope while F retains the same functionality. An initial source-only prompt-restoration attempt was reverted before commit when this evidence arrived. No F listener, native action dispatch or display override changed in the final slice. Other hold-F/commander prompts remain native/manual checks.

Run `python qa/evening-crew-20261021/validate_static.py --output qa/evening-crew-20261021/static-result.json`. This checks the four-script runtime diff allowlist, baseline preservation of hidden periscope prompt, HUD/lifecycle/range/hatch code and no new dispatch calls. It does not compile Enforce, exercise a user action or prove gameplay. Retain native compile/load as a separate pending gate.

`scenarios.json` defines CREW-01 through CREW-05 with steps and expected outcomes. Batch these with the integrator's qualified fresh 1.0.2 fixture after the parent admits the native slot. Use both appearances; preserve healthy control and damage cases separately. GEP-006/010/020/021/022/023/024 all remain **Needs user test**, unchecked. Source/static candidate is ready; native compile/load and actual input/gameplay remain **NOT_TESTED**, pending integrator execution. Initial native wait was pending slot handoff, never denied or uncertain; the integrator now owns admission.

No new acceptance is claimed for already accepted GEP-001–004/028–032, unfinished whole GEP-005/018, damage exit GEP-013, MP/JIP GEP-025 or console GEP-026.

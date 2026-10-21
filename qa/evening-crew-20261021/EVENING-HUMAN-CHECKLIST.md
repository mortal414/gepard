# Evening human playtest

Test the exact candidate admitted by the integrator. Record its commit/build identifier, default or charcoal appearance, seat, step and observed failure. The wording-restored production source is `a12a193c006ccb8eb1336a24dec925632a982acc` (`production3guards`). The earlier integrated manual candidate `cc5d0989376ed1d20ada109ce7c1ada5d96aac7e` remains preserved unchanged; evidence from it must not be relabeled as a new test of this successor. A loaded scene or successful compile does not pass these checks.

Allow roughly 12–18 minutes for the healthy-vehicle steps. Run damage/incapacitation only in the separately prepared fresh fixture. Use current bindings if rebound; F below means the native interaction binding. Do not alter accepted driving/R/braking settings.

1. **Hidden periscope / one-press return — GEP-010/020/021.** Enter driver normally. Use forward, left and right periscopes in turn. The Leave periscope prompt should stay hidden; one F press should return to seated free look without a duplicate transition. Repeat each, then exit/re-enter. Hold F once to check other applicable station actions remain available. Record a hidden unrelated action or stuck camera as a failure; physical F behavior remains untested until this is performed.

2. **Commander, seats, view and hatches — GEP-010/020/021/022.** From outside, verify Get in commander. Cycle driver → gunner → commander → driver twice, allowing entry to finish before switching. Commander switching should also read Get in commander. At each seat open/close its own hatch, inspect applicable prompts and lamp/radar controls, then exit/re-enter. In commander inspect eye position, body/roof clipping and free look with the hatch open and closed. Expected: correct station controls, completed transitions and a comfortable view. Marco judges alignment and reach.

3. **HUD, range and firing lifecycle; charcoal — GEP-005/006/008/010/018/024.** In gunner ADS measure a visible target and fire left/right/both with the existing controls; check actual ammo counts and range feedback. Open/close the menu, leave ADS, switch seat and exit/re-enter twice, then range/fire again. Repeat the lifecycle on charcoal. Expected: sight only for the actual ADS gunner, fresh readings, normal UI after leaving, no stale overlay or lost controls. This checks displayed range regression, not ballistic elevation or impact correction. HUD style and charcoal acceptance do not finish their whole tasks.

4. **Conscious wounded crew, brake lamps, recovery — GEP-006/010/013/020.** In the prepared nonfatal damage fixture keep a conscious survivor and repeat station controls, own hatch, exit/re-entry and gunner use. For a conscious wounded driver press/release the brake and inspect the actual lamps. Separately incapacitate/recover the actor using the prepared fixture: custom controls should stop while unconscious and return for the settled recovered occupant; driver recovery should start in seated view. Expected: no new wound-based lockout or trapped survivor. Log any destroyed-system limitation separately. This is bounded survivor regression, not full GEP-013 acceptance.

5. **Refuel side and reach — GEP-023.** Start with partial fuel and a stock support source. Approach the authored left rear point and transfer actual fuel; reset partial fuel and repeat right side. Test near/far reach and cancel once. Expected: normal stock supply/fuel transaction, no duplicate or implausible through-hull/far-side action. Marco chooses whether intended side/reach feels right. Both points already exist in release; this candidate did not move them.

6. **Release debug sweep — GEP-024.** On driver, gunner and commander inspect seated view, periscopes/ADS where applicable, menu return, seat switch and exit/re-entry, on both appearances. Expected: no developer/probe/debug overlay, no stale sight and no hidden necessary prompt. Distinguish legitimate operational fault/range-unavailable messages from developer text. Record the exact text and seat if uncertain.

Stop a failed sequence, note the exact step and use a fresh fixture for the next independent case; do not make a later successful retry erase the failure. A short capture of hidden-prompt F return, commander view and any failure is useful. Human comfort, prompt usefulness, appearance and reach remain Marco's judgments.

## Current status

| Scope | Exact status | Remaining acceptance |
| --- | --- | --- |
| GEP-006 HUD lifecycle | Needs user test | Repeated transitions, range/fire and damage lifecycle with actual input |
| GEP-010 familiar controls | Needs user test | Current bindings, prompts, physical F and actual station use |
| GEP-020 seats/hatches/periscopes | Needs user test | Repeated healthy/damaged transitions and recovery without lockups |
| GEP-021 interaction prompts | Needs user test | Hidden periscope return must still work; other needed prompts remain usable |
| GEP-022 commander view alignment | Needs user test | Body/eye/roof alignment and comfortable free look |
| GEP-023 refuel side/reach | Needs user test | Actual transfer, usable intended side and sensible distance |
| GEP-024 release debug text | Needs user test | Both appearances and every relevant seat/lifecycle state |
| Related GEP-005 HUD | Needs user test | Whole task remains unfinished; this is regression coverage only |
| Related GEP-008 range regression | Needs user test | Current displayed measurements after lifecycle changes |
| Related GEP-013 survivor exit | Needs user test | Bounded current survivor check; broader damage acceptance still outstanding |
| Related GEP-018 charcoal | Needs user test | Shipped increment preserved; whole appearance task remains unfinished |

**Ready:** checklist, source audit and static three-script preservation evidence. **Blocked:** further native automation/session admission while Isfahan owns native; pending handoff, not denied or uncertain. This document authorizes no launch. No GEP completion checkbox or tracker value was changed.

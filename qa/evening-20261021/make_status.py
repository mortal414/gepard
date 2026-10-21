"""Record per-ID acceptance boundaries for this bounded sprint, without editing the tracker."""
from pathlib import Path
import argparse,json

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--output',type=Path,required=True);a=ap.parse_args()
    descriptions={
      5:('Needs user test','Shipped sight increment accepted; whole HUD unfinished. Bright/dark readability and wider lifecycle still open.'),
      6:('Needs user test','Both appearances passed native traverse/twin fire/exit/reentry/refire; repeated seat/view/damage lifecycle and physical input pending.'),
      7:('Blocked','Released stabilization hard-disabled; no qualified actual-motion LOS/actuator calibration.'),
      8:('Needs user test','Passive helper compiled; ground/different range/sky clear/reentry display matrix prepared, not executed with ordinary C input.'),
      9:('Blocked','Released ballistic correction hard-disabled; no physical elevation/impact qualification at multiple ranges/feeds.'),
      10:('Needs user test','Conscious/exact-occupant guards and clearer internal commander switch compiled; normal bindings/prompts need actual play.'),
      11:('Blocked','Armor/crew-protection tuning outside this sprint; one fragment case cannot qualify realism.'),
      12:('Blocked','Independent turret damage remains stubbed while hull alive; no speculative damage rewrite.'),
      13:('Needs user test','One exact same-build native2m fragment-survivor case passed damage callbacks, traverse/fire, exit/reentry/refire; broader hull/turret destruction states unqualified.'),
      14:('Blocked','Real radar sensing/tracking not implemented; existing deployment remains cosmetic.'),
      15:('Blocked','Recovered stock candidate compiles; first Launch crashed, null-weapon repair avoids crash but flight/fuse/cloud gate fails. Experimental source isolated under qa; absent from production candidate.'),
      16:('Blocked','Interior/cabin lighting redesign outside sprint.'),
      17:('Blocked','Exterior actively owned by separate Blender/art thread; no conditional geometry imported.'),
      18:('Needs user test','Quiet charcoal release increment accepted; whole skin/PBR/seam/LOD refinement remains unfinished. Preserved exactly.'),
      19:('Blocked','User-deferred custom cannon audio; released stock sounds retained, no renewed effort.'),
      20:('Needs user test','Native healthy exit/reentry and one fragment-survivor pass; repeated all-seat/hatch/periscope physical cycles still pending.'),
      21:('Needs user test','Intentional hidden Leave-periscope prompt preserved with F behavior; hold-F/commander actions need current-build physical regression.'),
      22:('Needs user test','Commander position/view offsets preserved pending Marco alignment judgment.'),
      23:('Needs user test','Released fuel contexts already authored both sides; actual side/reach/transfer not newly verified.'),
      24:('Needs user test','Inspected default/charcoal gunner post-reentry and on-foot readiness captures; complete all-seat/lifecycle debug-text inspection pending.'),
      25:('Blocked','Dedicated/MP/JIP qualification outside this single-player bounded batch.'),
      26:('Blocked','Console device/control/performance qualification unavailable in this PC batch; compile configuration is not console gameplay.'),
      27:('Blocked','1.0.2 upload, server processing and exact packaged GM spawn tests verified in hotfix evidence. Public webpage visibility remains independently unverified; tracker text stale, no republish.'),
      33:('Blocked','Signal-only QA adapter has no qualified audible stock graph/WAV GUID/event lifecycle/listening. Released audio preserved.')}
    accepted={1,2,3,4,28,29,30,31,32}
    rows=[]
    for n in range(1,34):
        status,reason=('Ready','Previously accepted baseline behavior/source preserved; not reimplemented or newly marked complete.') if n in accepted else descriptions[n]
        rows.append({'id':f'GEP-{n:03}','status':status,'reason':reason,'doneAcceptedBeforeSprint':n in accepted,'newlyCompleted':False})
    result={'scope':'Ready means preserved accepted baseline or prepared bounded slice, not new whole-task acceptance. All pending items remain unchecked.','trackerReadOnly':True,'newCheckboxesChecked':0,'rows':rows}
    a.output.write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8',newline='\n')

if __name__=='__main__':main()

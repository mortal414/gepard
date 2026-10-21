class GEP_SeatReadyState
{
 BaseCompartmentSlot Seat;
 IEntity Occupant;
 float Stable;
}
class GEP_SeatReadinessClass : ScriptComponentClass {}
class GEP_SeatReadiness : ScriptComponent
{
 ref array<ref GEP_SeatReadyState> States={};
 override void OnPostInit(IEntity owner){SetEventMask(owner,EntityEvent.FRAME);}
 override void EOnFrame(IEntity owner,float timeSlice)
 {
  BaseCompartmentManagerComponent manager=BaseCompartmentManagerComponent.Cast(owner.FindComponent(BaseCompartmentManagerComponent));
  if(!manager)return;
  array<BaseCompartmentSlot> seats={};manager.GetCompartments(seats);
  foreach(BaseCompartmentSlot seat:seats)
  {
   GEP_SeatReadyState state=null;
   foreach(GEP_SeatReadyState existing:States) {if(existing.Seat==seat){state=existing;break;}}
   if(!state){state=new GEP_SeatReadyState();state.Seat=seat;States.Insert(state);}
   IEntity occupant=seat.GetOccupant();
   if(occupant!=state.Occupant){state.Occupant=occupant;state.Stable=0;}
   if(!occupant){state.Stable=0;continue;}
   CompartmentAccessComponent access=CompartmentAccessComponent.Cast(occupant.FindComponent(CompartmentAccessComponent));
   if(!access || access.IsGettingIn() || access.IsGettingOut() || access.IsSwitchingSeatsAnim()){state.Stable=0;continue;}
   state.Stable+=timeSlice;
  }
 }
 bool Ready(IEntity actor)
 {
  foreach(GEP_SeatReadyState state:States) {if(state.Occupant==actor && state.Stable>=1) return true;}
  return false;
 }
}
// The native slot/command can appear before immediate re-entry finishes.
// Require one second of stable occupancy, then call the unchanged native action.
class GEP_SwitchSeatAction : SCR_SwitchSeatAction
{
 override bool GetActionNameScript(out string outName)
 {
  if(CargoCompartmentSlot.Cast(GetCompartmentSlot())){outName="Move to commander";return true;}
  return super.GetActionNameScript(outName);
 }

 override bool CanBePerformedScript(IEntity user)
 {
  ChimeraCharacter character=ChimeraCharacter.Cast(user);
  if(!character) return false;
  CompartmentAccessComponent access=character.GetCompartmentAccessComponent();
  if(!access || access.IsGettingIn() || access.IsGettingOut() || access.IsSwitchingSeatsAnim()) return false;
  CharacterCommandHandlerComponent handler=character.GetAnimationComponent().GetCommandHandler();
  IEntity root=GetOwner().GetRootParent();
  GEP_SeatReadiness readiness=GEP_SeatReadiness.Cast(root.FindComponent(GEP_SeatReadiness));
  if(!handler || !handler.GetCommandVehicle() || (readiness && !readiness.Ready(user)))
  {
   SetCannotPerformReason("Finishing seat entry");return false;
  }
  return super.CanBePerformedScript(user);
 }
}

// Passive diagnostics only, isolated candidate674. Never writes native seat,
// queue, character, door, input values, camera, physics or inventory state.
// Explicit startup flag belongs only to a future fresh candidate profile.

// The wrapper calls the installed public action exactly once. A returned void
// action is deliberately not logged as an accepted native exit request.

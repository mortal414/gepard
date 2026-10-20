// Ordinary contextual vehicle actions, sharing the keybind authority path.
class GEP_RadarAction : ScriptedUserAction
{
 [Attribute("0",UIWidgets.EditBox,"0 search power, 1 range, 2/3 selection, 4 tracking power, 5 selected acquire, 6 sighted acquire, 7 untrack")]
 int Command;
 [Attribute("0",UIWidgets.EditBox,"0 driver, 1 gunner, 2 commander")]
 int Station;
 bool SeatMatches(GEP_RadarSystem radar,IEntity user)
 {
  if(!radar || !radar.Crew(user)) return false;
  if(Station==1) return radar.Gunner(user);
  if(Station==2) return !radar.Gunner(user) && !PilotCompartmentSlot.Cast(CompartmentAccessComponent.Cast(user.FindComponent(CompartmentAccessComponent)).GetCompartment());
  CompartmentAccessComponent access=CompartmentAccessComponent.Cast(user.FindComponent(CompartmentAccessComponent));
  return access && PilotCompartmentSlot.Cast(access.GetCompartment())!=null;
 }

 override bool HasLocalEffectOnlyScript() {return false;
  }
 override bool CheckOnServerFirstScript() {return true;
  }
 override bool CanBeShownScript(IEntity user)
 {
  GEP_RadarSystem radar=GEP_RadarSystem.Find(GetOwner());
  return Command==0 && SeatMatches(radar,user);

 }
 override bool CanBePerformedScript(IEntity user)
 {
  GEP_RadarSystem radar=GEP_RadarSystem.Find(GetOwner());
  if(Command!=0 || !SeatMatches(radar,user) || !radar.Alive(radar.GetOwner())) return false;

  if(Command>=2 && !radar.Gunner(user)) {SetCannotPerformReason("Use the gunner station");
  return false;
  }
  if(Command==6 && (!radar.Gun || !radar.Gun.IsWeaponADS())) {SetCannotPerformReason("Enter gun sights and aim at an airborne helicopter");
  return false;
  }
  if(Command==7 && radar.Model.ActiveTarget<0) {SetCannotPerformReason("No active track");
  return false;
  }
  if(Command==5 && !radar.Selected()) {SetCannotPerformReason("Select a search return first");
  return false;
  }
  return true;

 }
 override void PerformAction(IEntity pOwnerEntity,IEntity pUserEntity)
 {
  GEP_RadarSystem radar=GEP_RadarSystem.Find(pOwnerEntity);
  if(radar && CanBePerformedScript(pUserEntity)) radar.Command(pUserEntity,Command);

 }
 override bool GetActionNameScript(out string outName)
 {
  string name;

  GEP_RadarSystem radar=GEP_RadarSystem.Find(GetOwner());

  if(Command==0) {name="Deploy radar";
  if(radar && radar.Deployed) name="Stow radar";
  }
  if(Command==1) name="Change search range";

  if(Command==2) name="Select next air contact";

  if(Command==3) name="Select previous air contact";

  if(Command==4) {name="Switch tracking radar on";
  if(radar && radar.Model.TrackingPowered) name="Switch tracking radar off";
  }
  if(Command==5) name="Track selected air contact";

  if(Command==6) name="Track sighted aircraft";

  if(Command==7) name="Untrack aircraft";

  outName=name;
  return true;

 }
}

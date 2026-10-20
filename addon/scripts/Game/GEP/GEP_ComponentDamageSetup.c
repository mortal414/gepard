// Original adapter for the installed tracked base's duplicate, unbound legacy zones.
// The collider-bound native Engine_01/Gearbox_01 remain the simulation authorities.
class GEP_ComponentDamageSetupClass : ScriptComponentClass {}
class GEP_ComponentDamageSetup : ScriptComponent
{
 bool Ready;
 float Delay;
 override void OnPostInit(IEntity owner){SetEventMask(owner,EntityEvent.FRAME);}
 override void EOnFrame(IEntity owner,float timeSlice)
 {
  if(Ready || !GetGame().InPlayMode()){return;}
  Delay+=timeSlice;if(Delay<1){return;}
  SCR_VehicleDamageManagerComponent damage=SCR_VehicleDamageManagerComponent.Cast(owner.FindComponent(SCR_VehicleDamageManagerComponent));
  if(!damage){return;}
  array<HitZone> zones={};damage.GetAllHitZonesInHierarchy(zones);int removed;
  foreach(HitZone zone:zones)
  {
   if(zone.GetName()!="Engine" && zone.GetName()!="Gearbox"){continue;}
   array<string> names={};zone.GetAllColliderNames(names);
   if(!names.IsEmpty()){continue;}
   damage.UnregisterVehicleHitZone(zone);removed++;
  }
  damage.UpdateVehicleState();Ready=true;
  PrintFormat("GEP_COMPONENT_SIM_BINDING unboundLegacyRegistrationsRemoved=%1 engineFunctional=%2 gearboxFunctional=%3",removed,damage.GetEngineFunctional(),damage.GetGearboxFunctional());
 }
}

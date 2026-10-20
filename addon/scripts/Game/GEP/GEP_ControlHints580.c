// Correct the native hint only while this player actually controls the Gepard gunner.
// Native widget resolves the action binding, including user rebindings.
[BaseContainerProps(), SCR_AvailableActionContextTitle()]
modded class SCR_AvailableActionContext
{
 bool GEP_IsGroupHint580()
 {
  if(m_sTag!="TURRET_FIRE_MODE_MANAGER_WEAPON_GROUP" && m_sTag!="TURRET_WEAPON_NEXT")return false;
  PlayerController player=GetGame().GetPlayerController();if(!player)return false;
  IEntity user=player.GetControlledEntity();if(!user)return false;
  CompartmentAccessComponent access=CompartmentAccessComponent.Cast(user.FindComponent(CompartmentAccessComponent));if(!access)return false;
  BaseCompartmentSlot seat=access.GetCompartment();if(!seat || !seat.GetOwner())return false;
  GEP_GunInputContext579 input=GEP_GunInputContext579.Cast(seat.GetOwner().FindComponent(GEP_GunInputContext579));
  return input && input.Eligible();
 }
 override string GetActionName()
 {
  if(GEP_IsGroupHint580())return "GEPNextGunGroup579";
  return super.GetActionName();
 }
 override string GetUIName()
 {
  if(GEP_IsGroupHint580())return "Cannon group";
  return super.GetUIName();
 }
}

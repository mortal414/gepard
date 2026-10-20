// Original Gepard turret-drive gate. Uses native input processing and native damage state.
class GEP_TurretController410Class : SCR_TurretControllerComponentClass {}
class GEP_TurretController410 : SCR_TurretControllerComponent
{
 HitZone DriveZone;
 bool IsDriveFailed()
 {
  if(!DriveZone)
  {
   SCR_DamageManagerComponent damage=SCR_DamageManagerComponent.Cast(GetOwner().FindComponent(SCR_DamageManagerComponent));
   if(damage)
   {
    array<HitZone> zones={};damage.GetAllHitZonesInHierarchy(zones);
    foreach(HitZone zone:zones)
    {
     if(zone.GetName()=="TurretDrive")DriveZone=zone;
    }
   }
  }
  return DriveZone&&DriveZone.GetDamageState()==EDamageState.DESTROYED;
 }
 override void OnPrepareControls(IEntity owner,ActionManager am,float dt,bool player)
 {
  super.OnPrepareControls(owner,am,dt,player);
  if(!am)return;
  if(!IsDriveFailed())
  {
   GEP_FireControl fireControl=GEP_FireControl.Cast(owner.FindComponent(GEP_FireControl));
   if(fireControl && fireControl.PrepareControls(am,dt,player))return;
   GEP_Optical444 optic=GEP_Optical444.Cast(owner.FindComponent(GEP_Optical444));
   if(optic)optic.PrepareControls(am,dt,player);
   return;
  }
  am.SetActionValue("TurretRotateUp",0);am.SetActionValue("TurretRotateDown",0);
  am.SetActionValue("TurretRotateLeft",0);am.SetActionValue("TurretRotateRight",0);
  am.SetActionValue("TurretAimUp",0);am.SetActionValue("TurretAimDown",0);
  am.SetActionValue("TurretAimLeft",0);am.SetActionValue("TurretAimRight",0);
  am.SetActionValue("TurretAimGamepadUp",0);am.SetActionValue("TurretAimGamepadDown",0);
  am.SetActionValue("TurretAimGamepadLeft",0);am.SetActionValue("TurretAimGamepadRight",0);
 }
}

// Inherits native support-station eligibility, tool animation, timing and resource charges.
class GEP_RepairTurretDriveAction410 : SCR_RepairAtSupportStationAction
{
 void Diagnostic411(IEntity user)
 {
  PrintFormat("GEP_TURRET411_SERVICE zones=%1 healable=%2 intensity=%3 managerState=%4 defaultZone=%5 showDestroyed=%6 gadget=%7",m_aHitZonesToHeal.Count(),m_DamageManagerComponent.CanBeHealed(false),m_DamageIntensityHolder.GetValidIntensityType(m_aHitZonesToHeal),m_DamageManagerComponent.GetState(),m_DamageManagerComponent.GetDefaultHitZone().GetName(),CanShowDestroyed(),SCR_SupportStationGadgetComponent.GetHeldSupportStationGadget(GetSupportStationType(),user));
  foreach(HitZone zone:m_aHitZonesToHeal)PrintFormat("GEP_TURRET411_ZONE name=%1 health=%2 state=%3",zone.GetName(),zone.GetHealthScaled(),zone.GetDamageState());
  IEntity root=GetOwner().GetRootParent();SCR_DamageManagerComponent hull=SCR_DamageManagerComponent.Cast(root.FindComponent(SCR_DamageManagerComponent));
  PrintFormat("GEP_TURRET413_ROOT owner=%1 root=%2 defaultHealth=%3 hullHealth=%4",GetOwner(),root,m_DamageManagerComponent.GetDefaultHitZone().GetHealthScaled(),hull.GetDefaultHitZone().GetHealthScaled());
 }
 override protected void SetHitZonesToHeal()
 {
  m_aHitZonesToHeal.Clear();
  if(!m_DamageManagerComponent)return;
  array<HitZone> zones={};m_DamageManagerComponent.GetAllHitZonesInHierarchy(zones);
  foreach(HitZone zone:zones)
  {
   if(zone.GetName()=="TurretDrive")m_aHitZonesToHeal.Insert(zone);
  }
 }
}

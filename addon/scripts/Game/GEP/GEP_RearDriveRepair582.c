// Original hull discovery adapter. Installed native support-station code still
// owns eligibility, held tool, timing, authority, repair and supply charges.
class GEP_RearDriveRepair582 : GEP_RepairTurretDriveAction410
{
 protected int m_Attempts582;
 protected override void DelayedInit(IEntity owner)
 {
  if(!owner)return;
  super.DelayedInit(owner);
  // Never leave the provisional root manager's zone list available while the
  // registered turret is still spawning. The action must repair that turret.
  m_aHitZonesToHeal.Clear();
  m_DamageManagerComponent=null;
  GEP_RadarSystem mount=GEP_RadarSystem.Cast(owner.FindComponent(GEP_RadarSystem));
  if(!mount || !mount.Rig)
  {
   m_Attempts582++;
   if(m_Attempts582<20)GetGame().GetCallqueue().CallLater(DelayedInit,250,false,owner);
   return;
  }
  m_DamageManagerComponent=SCR_DamageManagerComponent.Cast(mount.Rig.FindComponent(SCR_DamageManagerComponent));
  if(m_DamageManagerComponent)SetHitZonesToHeal();
  // super retains the native whole-vehicle fire check. The inherited scoped
  // SetHitZonesToHeal selects only the actual child TurretDrive hit zone.
 }
 override bool CanBePerformedScript(IEntity user)
 {
  if(!m_DamageManagerComponent || m_aHitZonesToHeal.IsEmpty())return false;
  return super.CanBePerformedScript(user);
 }
}

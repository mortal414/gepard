// Original hull action adapter; native support-station logic owns supply cost,
// timing, authority and rearm. Only locate the registered child weapon feed.
class GEP_RearBelt580 : SCR_ResupplyTurretBeltSupportStationAction
{
 protected int m_Attempts580;
 protected override void DelayedInit(IEntity owner)
 {
  super.DelayedInit(owner);
  GEP_RadarSystem radar=GEP_RadarSystem.Cast(owner.FindComponent(GEP_RadarSystem));
  if(!radar || !radar.Rig)
  {
   m_Attempts580++;
   if(m_Attempts580<20)GetGame().GetCallqueue().CallLater(DelayedInit,250,false,owner);
   return;
  }
  array<Managed> slots={};radar.Rig.FindComponents(WeaponSlotComponent,slots);
  foreach(Managed item:slots)
  {
   WeaponSlotComponent slot=WeaponSlotComponent.Cast(item);
   if(slot.GetWeaponSlotIndex()!=m_iWeaponSlotID)continue;
   array<BaseMuzzleComponent> muzzles={};slot.GetMuzzlesList(muzzles);
   if(muzzles.IsEmpty())return;
   m_GunMuzzleComp=muzzles[0];m_Magazine=m_GunMuzzleComp.GetMagazine();return;
  }
 }
}

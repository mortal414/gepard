// Original non-destructive lamp zones. The stock SCR_LightHitZone output pattern
// uses SetLightFunctional, but its destructible ancestry is unsuitable on a hull.
// This direct SCR_HitZone subclass has no destruction handler, occupant or AI effects.
class GEP_LampHitZone584 : SCR_HitZone
{
 [Attribute("0",UIWidgets.EditBox,"0 head left, 1 head right, 2 rear red left, 3 rear red right, 4 reverse left, 5 reverse right")]
 protected int m_iLampGroup584;
 override EHitZoneGroup GetHitZoneGroup(){return EVehicleHitZoneGroup.LIGHT;}
 bool Matches584(BaseLightSlot slot)
 {
  if(!slot || m_iLampGroup584<0 || m_iLampGroup584>5)return false;
  int side=0;if(m_iLampGroup584==1 || m_iLampGroup584==3 || m_iLampGroup584==5)side=1;
  if(slot.GetLightSide()!=side)return false;
  ELightType type=slot.GetLightType();
  if(m_iLampGroup584<2)return type==ELightType.Head;
  if(m_iLampGroup584<4)return type==ELightType.Rear || type==ELightType.Brake;
  return type==ELightType.Reverse;
 }
 void RefreshOutputs584()
 {
  IEntity owner=GetOwner();if(!owner || owner.IsDeleted())return;
  RplComponent replication=SCR_EntityHelper.GetEntityRplComponent(owner);
  if(replication && replication.IsProxy())return;
  BaseLightManagerComponent manager=BaseLightManagerComponent.Cast(owner.FindComponent(BaseLightManagerComponent));if(!manager)return;
  array<BaseLightSlot> lights={};manager.GetLights(lights);
  bool functional=GetDamageState()!=EDamageState.DESTROYED;
  foreach(BaseLightSlot light:lights)
  {
   if(Matches584(light))
   {
    light.SetLightFunctional(functional);
    // Preserve root hit-zone authority and the requested native switch state.
    // Native surface control also gates the linked reflector/bulb visual child.
    int surfaceID592=light.GetSurfaceID();
    if(surfaceID592>=0)manager.TrySetSurfaceFunctional(surfaceID592,functional);
   }
  }
  // Do not change the requested switch state. Native slots keep off lamps off;
  // repair restores function and lets the native manager evaluate that state.
 }
 override protected void OnDamageStateChanged(EDamageState newState,EDamageState previousDamageState,bool isJIP)
 {
  super.OnDamageStateChanged(newState,previousDamageState,isJIP);
  RefreshOutputs584();
 }
}

class GEP_LampStartup584Class : ScriptComponentClass {}
class GEP_LampStartup584 : ScriptComponent
{
 protected bool m_Ready584;
 protected float m_Timer584;
 protected int m_Attempts584;
 override void OnPostInit(IEntity owner){SetEventMask(owner,EntityEvent.FRAME);}
 override void EOnFrame(IEntity owner,float timeSlice)
 {
  float dt=timeSlice;
  if(m_Ready584 || !GetGame().InPlayMode())return;
  RplComponent replication=SCR_EntityHelper.GetEntityRplComponent(owner);
  if(replication && replication.IsProxy()){m_Ready584=true;return;}
  m_Timer584+=dt;if(m_Timer584<0.25)return;m_Timer584=0;m_Attempts584++;
  BaseLightManagerComponent manager=BaseLightManagerComponent.Cast(owner.FindComponent(BaseLightManagerComponent));
  if(!manager){if(m_Attempts584>=20)m_Ready584=true;return;}
  array<BaseLightSlot> lights={};manager.GetLights(lights);
  if(lights.Count()<8){if(m_Attempts584>=20)m_Ready584=true;return;}
  SCR_DamageManagerComponent damage=SCR_DamageManagerComponent.Cast(owner.FindComponent(SCR_DamageManagerComponent));if(!damage){m_Ready584=true;return;}
  array<HitZone> zones={};damage.GetAllHitZonesInHierarchy(zones);
  foreach(HitZone zone:zones)
  {
   GEP_LampHitZone584 lamp=GEP_LampHitZone584.Cast(zone);
   if(lamp && lamp.GetOwner()==owner)lamp.RefreshOutputs584();
  }
  m_Ready584=true;
 }
}

// Native repair eligibility, tool, movement/fire checks, timing and charging remain
// in SCR_RepairAtSupportStationAction. Each point selects only its named lamp zone.
class GEP_RepairLampAction584 : SCR_RepairAtSupportStationAction
{
 [Attribute("",UIWidgets.EditBox,"Exact authored GEP lamp hit-zone name")]
 protected string m_sLampZone584;
 override protected void SetHitZonesToHeal()
 {
  m_aHitZonesToHeal.Clear();if(!m_DamageManagerComponent || m_sLampZone584.IsEmpty())return;
  array<HitZone> zones={};m_DamageManagerComponent.GetAllHitZonesInHierarchy(zones);
  foreach(HitZone zone:zones)
  {
   GEP_LampHitZone584 lamp=GEP_LampHitZone584.Cast(zone);
   if(lamp && lamp.GetOwner()==GetOwner() && lamp.GetName()==m_sLampZone584)m_aHitZonesToHeal.Insert(lamp);
  }
 }
 override bool CanBePerformedScript(IEntity user)
 {
  if(!m_DamageManagerComponent || m_aHitZonesToHeal.IsEmpty())return false;
  return super.CanBePerformedScript(user);
 }
}

// ORIGINAL non-local physical/operator gate. No local player/camera or aim writer.
class GEP_StabOperatorContext708
{
 static bool CompatibleRange726(IEntity owner)
 {
  WCS_Armament_FireControlComponent fc726=WCS_Armament_FireControlComponent.Cast(owner.FindComponent(WCS_Armament_FireControlComponent));
  return !fc726 || GEP_NextFireControlConsumer726.Cast(fc726)!=null;
 }
 static bool Functional(IEntity owner)
 {
  if(!owner || owner.IsDeleted())return false;
  GEP_TurretController410 control=GEP_TurretController410.Cast(owner.FindComponent(TurretControllerComponent));
  if(!control || control.GetOwner()!=owner || control.IsDriveFailed())return false;
  GEP_Optical444 optical=GEP_Optical444.Cast(owner.FindComponent(GEP_Optical444));
  if(!optical || optical.State!=0 || !CompatibleRange726(owner))return false;
  SCR_DamageManagerComponent damage=SCR_DamageManagerComponent.Cast(owner.FindComponent(SCR_DamageManagerComponent));
  if(damage && damage.GetState()==EDamageState.DESTROYED)return false;
  IEntity hull=owner.GetRootParent();
  if(!hull || hull.IsDeleted())return false;
  damage=SCR_DamageManagerComponent.Cast(hull.FindComponent(SCR_DamageManagerComponent));
  return !damage || damage.GetState()!=EDamageState.DESTROYED;
 }
 static bool Read(IEntity owner,IEntity user,out BaseCompartmentSlot seat)
 {
  seat=null;
  if(!Functional(owner) || !user || user.IsDeleted())return false;
  CharacterControllerComponent life=CharacterControllerComponent.Cast(user.FindComponent(CharacterControllerComponent));
  if(!life || life.IsUnconscious() || life.GetLifeState()!=ECharacterLifeState.ALIVE)return false;
  CompartmentAccessComponent access=CompartmentAccessComponent.Cast(user.FindComponent(CompartmentAccessComponent));
  if(!access || access.IsGettingIn() || access.IsGettingOut() || access.IsSwitchingSeatsAnim())return false;
  seat=access.GetCompartment();
  if(!seat || !TurretCompartmentSlot.Cast(seat) || seat.GetOccupant()!=user || seat.GetOwner()!=owner)return false;
  TurretControllerComponent control=TurretControllerComponent.Cast(seat.GetController());
  if(!control)control=seat.GetAttachedTurret();
  return control && control.GetOwner()==owner;
 }
}

// Original PRIVATE consumer integration. Calls exposed dependency contracts only.
// FC-ABSENT variant: GEP676 and the original SCR roof sight remain unchanged.
// WCS owns all assistance actuation; the disabled GEP prototype stays disabled.
class GEP_NextAimingBinding625
{
 IEntity User625;
 BaseCompartmentSlot Seat625;
 BaseSightsComponent Sight625;
 BaseWeaponComponent Weapon625;
 BaseMuzzleComponent Muzzle625;
 BaseMagazineComponent Magazine625;
 WeaponSlotComponent Slot625;
 RplComponent Rpl625;
 IEntity Hull625;

 static bool Read625(IEntity owner, out IEntity user, out BaseCompartmentSlot seat, out BaseSightsComponent sight)
 {
  user=null;
  seat=null;
  sight=null;
  string reason625;
  if(!GEP_GunnerSnapshot583.Evaluate679(owner,reason625))
  {
   return false;
  }
  GEP_TurretController410 control625=GEP_TurretController410.Cast(owner.FindComponent(TurretControllerComponent));
  if(!control625 || control625.GetOwner()!=owner || control625.IsDriveFailed())
  {
   return false;
  }
  PlayerController player625=GetGame().GetPlayerController();
  user=player625.GetControlledEntity();
  CompartmentAccessComponent access625=CompartmentAccessComponent.Cast(user.FindComponent(CompartmentAccessComponent));
  if(!access625)
  {
   return false;
  }
  seat=access625.GetCompartment();
  sight=control625.GetCurrentSights();
  SCR_2DSightsComponent optic625=SCR_2DSightsComponent.Cast(sight);
  if(!seat || seat.GetOccupant()!=user || seat.GetOwner()!=owner || !sight || sight.GetOwner()!=owner || !optic625 || !control625.GetCurrentSightsADS() || !optic625.IsSightADSActive())
  {
   return false;
  }
  CameraManager cameras625=GetGame().GetCameraManager();
  if(!cameras625 || !player625.GetPlayerCamera() || cameras625.CurrentCamera()!=player625.GetPlayerCamera())
  {
   return false;
  }
  GEP_Optical444 optical625=GEP_Optical444.Cast(owner.FindComponent(GEP_Optical444));
  RplComponent rpl625=RplComponent.Cast(owner.FindComponent(RplComponent));
  if(!optical625 || optical625.State!=0 || !rpl625 || !rpl625.IsOwner())
  {
   return false;
  }
  return true;
 }
 bool Capture625(IEntity owner)
 {
  if(!Read625(owner,User625,Seat625,Sight625))
  {
   return false;
  }
  // Stabilization follows operator/sight/root, independent of gun group/ammo.
  Rpl625=RplComponent.Cast(owner.FindComponent(RplComponent));
  Hull625=owner.GetRootParent();
  return Rpl625 && Hull625 && Hull625!=owner;
 }
 bool Matches625(IEntity owner)
 {
  IEntity user625;
  BaseCompartmentSlot seat625;
  BaseSightsComponent sight625;
  if(!Read625(owner,user625,seat625,sight625) || user625!=User625 || seat625!=Seat625 || sight625!=Sight625 || owner.GetRootParent()!=Hull625 || owner.FindComponent(RplComponent)!=Rpl625)
  {
   return false;
  }
  return true;
 }
}

class GEP_NextStabilizationConsumer625Class : WCS_Armament_TurretStabilizationComponentClass {}
class GEP_NextStabilizationConsumer625 : WCS_Armament_TurretStabilizationComponent
{
 protected ref GEP_NextAimingBinding625 m_Binding625;
 protected bool m_Revoked625=true;
 protected bool m_Cleanup625;
 protected int m_Generation625,m_Delivered625,m_Forwarded625,m_Blocked625;
 // Separate authority operator identity; never a local input/camera generation.
 protected IEntity m_AuthorityUser625,m_AuthorityHull625;
 protected BaseCompartmentSlot m_AuthoritySeat625;
 protected RplComponent m_AuthorityRpl625;


 bool Live625()
 {
  return !m_Revoked625 && m_Binding625 && m_Binding625.Matches625(GetOwner()) && GEP_StabOperatorContext708.CompatibleRange726(GetOwner()) && super.CanStabilize();
 }
 bool ControlEligible625()
 {
  IEntity user625;
  BaseCompartmentSlot seat625;
  BaseSightsComponent sight625;
  ref GEP_NextAimingBinding625 candidate625=new GEP_NextAimingBinding625();
  return GEP_StabOperatorContext708.CompatibleRange726(GetOwner()) && candidate625.Capture625(GetOwner()) && super.CanStabilize();
 }
 bool HasLocalOwner625()
 {
  RplComponent rpl625=RplComponent.Cast(GetOwner().FindComponent(RplComponent));
  return rpl625 && rpl625.IsOwner();
 }
 protected void ClearAuthority625()
 {
  m_AuthorityUser625=null;m_AuthorityHull625=null;m_AuthoritySeat625=null;m_AuthorityRpl625=null;
 }
 protected bool CaptureAuthority625(IEntity user625)
 {
  BaseCompartmentSlot seat625;
  if(!IsAuthority() || !GEP_StabOperatorContext708.Read(GetOwner(),user625,seat625))return false;
  m_AuthorityUser625=user625;m_AuthoritySeat625=seat625;
  m_AuthorityHull625=GetOwner().GetRootParent();
  m_AuthorityRpl625=RplComponent.Cast(GetOwner().FindComponent(RplComponent));
  return m_AuthorityRpl625!=null;
 }
 protected bool AuthorityLive625()
 {
  BaseCompartmentSlot seat625;
  return IsAuthority() && m_AuthorityUser625 && GEP_StabOperatorContext708.Read(GetOwner(),m_AuthorityUser625,seat625)
   && seat625==m_AuthoritySeat625 && GetOwner().GetRootParent()==m_AuthorityHull625
   && GetOwner().FindComponent(RplComponent)==m_AuthorityRpl625;
 }
 // Shared read-only scope for tracking observation, NOT an actuation permission.
 bool GuardScopeLive625()
 {
  if(IsAuthority())return AuthorityLive625();
  return Live625();
 }
 // Shared cancellation seam: observers never mutate another owner's state.
 void CancelAssistance625(string reason625)
 {
  bool authority625=IsAuthority();bool localOwner625=HasLocalOwner625();
  if(!authority625 && !localOwner625)return;
  if(localOwner625){m_Revoked625=true;m_Cleanup625=true;}
  if(authority625)ClearAuthority625();
  vector demand625;bool requested625=TryGetRequestedAim(demand625);
  // Uses WCS's authority execution or owning-client RPC/prediction route.
  // No direct native aim, replicated-field, target or health writes.
  if(GetMode()!=WCS_EStabilizationMode.OFF || GetStabilizationEnabled() || IsTracking() || requested625)
   SetStabilizationEnabled(false);
  PrintFormat("GEP_STAB708_CANCEL reason=%1 authority=%2 localOwner=%3 mode=%4 enabled=%5",reason625,authority625,localOwner625,GetMode(),GetStabilizationEnabled());
 }
 protected void GuardAuthority625()
 {
  if(!IsAuthority())return;
  if(GetMode()==WCS_EStabilizationMode.OFF){ClearAuthority625();return;}
  if(!AuthorityLive625())CancelAssistance625("authority_operator_or_function_lost");
 }
 void Guard625()
 {
  GuardAuthority625();
  // An observing proxy has no local gunner by design; it must not clear state.
  if(!HasLocalOwner625())return;
  if(Live625() && GetMode()!=WCS_EStabilizationMode.OFF)return;
  bool active625=GetMode()!=WCS_EStabilizationMode.OFF || GetStabilizationEnabled();
  if(!m_Cleanup625 && (m_Binding625 || active625))CancelAssistance625("local_scope_or_generation_lost");
 }
 override protected bool CanStabilize()
 {
  // Server command validation cannot depend on this peer's local input lease.
  return GEP_StabOperatorContext708.Functional(GetOwner()) && super.CanStabilize();
 }
 override protected bool CanDriveAim()
 {
  // WCS retains the native owner-only writer gate. Server/observers do not drive
  // a remote gunner's turret without a local exact binding.
  return Live625() && super.CanDriveAim();
 }
 override protected bool CanControlledEntityOperate(IEntity controlledEntity)
 {
  BaseCompartmentSlot seat625;
  return GEP_StabOperatorContext708.Read(GetOwner(),controlledEntity,seat625) && super.CanControlledEntityOperate(controlledEntity);
 }
 override void ExecuteCommandFromController_S(IEntity controlledEntity,int command,int mode,RplId targetId,vector value)
 {
  if(!IsAuthority() || !CanControlledEntityOperate(controlledEntity))return;
  GuardAuthority625();
  bool enable625=command==WCS_EArmamentTurretCommand.SET_MODE && mode==WCS_EStabilizationMode.STABILIZED;
  bool disable625=command==WCS_EArmamentTurretCommand.SET_MODE && mode==WCS_EStabilizationMode.OFF;
  // Native command/capability/target bounds validation still runs in super.
  if(enable625)
  {
   if(!CaptureAuthority625(controlledEntity))return;
  }
  else if(!disable625 && (!AuthorityLive625() || controlledEntity!=m_AuthorityUser625))return;
  super.ExecuteCommandFromController_S(controlledEntity,command,mode,targetId,value);
  if(GetMode()==WCS_EStabilizationMode.OFF)ClearAuthority625();
 }
 // Original bridge: retain WCS's hold/tap suppression and designation path.
 protected void ForwardDependencyTap625()
 {
  super.OnStabilizationTapAction();
 }
 override protected void OnStabilizationTapAction()
 {
  // Tracking/designation are outside this private STABILIZED-only consumer.
  m_Blocked625++;
 }
 override protected void OnStabilizationHoldAction()
 {
  m_Delivered625++;
  Guard625();
  if(!Live625())
  {
   vector oldDemand625;
   if(!ControlEligible625() || GetMode()!=WCS_EStabilizationMode.OFF || GetStabilizationEnabled() || IsTracking() || TryGetRequestedAim(oldDemand625))
   {
    m_Blocked625++;
    return;
   }
   ref GEP_NextAimingBinding625 fresh625=new GEP_NextAimingBinding625();
   if(!fresh625.Capture625(GetOwner()))
   {
    m_Blocked625++;
    return;
   }
   m_Binding625=fresh625;
   m_Revoked625=false;
   m_Cleanup625=false;
   m_Generation625++;
  }
  // Host/listen authority takes its normal direct WCS path, so establish the
  // same separate operator identity before forwarding the deliberate Hold.
  if(IsAuthority() && GetMode()==WCS_EStabilizationMode.OFF && !CaptureAuthority625(m_Binding625.User625))
  {
   CancelAssistance625("authority_capture_failed");m_Blocked625++;return;
  }
  m_Forwarded625++;
  super.OnStabilizationHoldAction();
  if(GetMode()==WCS_EStabilizationMode.OFF)CancelAssistance625("ordinary_hold_off");
  PrintFormat("GEP_CONSUMER_HOLD delivered=%1 forwarded=%2 generation=%3 mode=%4 enabled=%5",m_Delivered625,m_Forwarded625,m_Generation625,GetMode(),GetStabilizationEnabled());
 }
 override void EOnFrame(IEntity owner,float timeSlice)
 {
  Guard625();
  if(!Live625())
  {
   return;
  }
  super.EOnFrame(owner,timeSlice);
 }
 override void EOnPostFrame(IEntity owner,float timeSlice)
 {
  Guard625();
  if(!Live625())
  {
   return;
  }
  super.EOnPostFrame(owner,timeSlice);
 }
 override void EOnFixedFrame(IEntity owner,float timeSlice)
 {
  Guard625();
  if(!Live625())
  {
   return;
  }
  super.EOnFixedFrame(owner,timeSlice);
 }
 int Generation625(){return m_Generation625;}
 int Delivered625(){return m_Delivered625;}
 int Forwarded625(){return m_Forwarded625;}
 int Blocked625(){return m_Blocked625;}
}

// Independent scope guard remains alive when dependency event masks change.
class GEP_OvernightStabilizerGuard625Class : ScriptComponentClass {}
class GEP_OvernightStabilizerGuard625 : ScriptComponent
{
 override void OnPostInit(IEntity owner)
 {
  SetEventMask(owner,EntityEvent.FRAME);
 }
 override void EOnFrame(IEntity owner,float timeSlice)
 {
  GEP_NextStabilizationConsumer625 stab625=GEP_NextStabilizationConsumer625.Cast(owner.FindComponent(GEP_NextStabilizationConsumer625));
  if(stab625)
  {
   stab625.Guard625();
  }
 }
}

// Original local measured-range provider. No aim, camera, weapon, radar or RPC writes.
// Depends on our existing GEP_Optical444 and GEP_GunnerSnapshot583 public checks.
enum EGEP_Range676State
{
 NONE,
 VALID,
 NO_RETURN,
 UNAVAILABLE
}

class GEP_Range676Class : ScriptComponentClass {}
class GEP_Range676 : ScriptComponent
{
 const string ACTION676="GEPMeasureRange676";
 const string CONTEXT676="GEPRangeContext676";
 const float MAX_RANGE676=5000;
 const float REQUEST_INTERVAL676=0.25;

 protected EGEP_Range676State m_State676;
 protected float m_Clock676;
 protected float m_LastRequest676=-1;
 protected float m_SampledAt676=-1;
 protected float m_Distance676=-1;
 protected vector m_Point676;
 protected IEntity m_User676;
 protected BaseCompartmentSlot m_Seat676;
 protected BaseSightsComponent m_Sight676;
 protected bool m_Bound676;
 protected ref array<IEntity> m_Exclusions676={};

 // Reuse our original actual-seat/life/menu checks; never authorize an observer
 // merely because its cargo seat has an attached gun turret.
 protected bool CurrentScope676(out IEntity user,out BaseCompartmentSlot seat,out BaseSightsComponent sight)
 {
  user=null;seat=null;sight=null;
  if(!GetGame() || !GetGame().InPlayMode() || System.IsConsoleApp())return false;
  IEntity owner=GetOwner();
  if(!owner || owner.IsDeleted() || !GEP_GunnerSnapshot583.IsLocalGunner(owner))return false;
  GEP_Optical444 optical=GEP_Optical444.Cast(owner.FindComponent(GEP_Optical444));
  if(!optical || !optical.Active())return false;
  user=optical.User();
  if(!user || !optical.Crew(user))return false;
  CharacterControllerComponent life=CharacterControllerComponent.Cast(user.FindComponent(CharacterControllerComponent));
  if(!life || life.IsUnconscious() || life.GetLifeState()!=ECharacterLifeState.ALIVE)return false;
  CompartmentAccessComponent access=CompartmentAccessComponent.Cast(user.FindComponent(CompartmentAccessComponent));
  TurretControllerComponent gun=TurretControllerComponent.Cast(owner.FindComponent(TurretControllerComponent));
  if(!access || !gun)return false;
  seat=access.GetCompartment();sight=gun.GetCurrentSights();
  return seat && sight;
 }

 protected bool ScopeMatches676()
 {
  IEntity user;BaseCompartmentSlot seat;BaseSightsComponent sight;
  return CurrentScope676(user,seat,sight) && user==m_User676 && seat==m_Seat676 && sight==m_Sight676;
 }

 protected void ClearSample676(EGEP_Range676State state)
 {
  m_State676=state;m_Distance676=-1;m_Point676=vector.Zero;m_SampledAt676=-1;
 }

 protected void Bind676(bool wanted)
 {
  if(m_Bound676==wanted || !GetGame())return;
  InputManager input=GetGame().GetInputManager();if(!input)return;
  if(wanted)input.AddActionListener(ACTION676,EActionTrigger.DOWN,OnMeasure676);
  else input.RemoveActionListener(ACTION676,EActionTrigger.DOWN,OnMeasure676);
  m_Bound676=wanted;
 }

 protected void Release676()
 {
  Bind676(false);
  m_User676=null;m_Seat676=null;m_Sight676=null;m_LastRequest676=-1;
  ClearSample676(EGEP_Range676State.NONE);m_Exclusions676.Clear();
 }

 protected void ExcludeTree676(IEntity entity)
 {
  if(!entity || m_Exclusions676.Find(entity)>=0)return;
  m_Exclusions676.Insert(entity);
  for(IEntity child=entity.GetChildren();child;child=child.GetSibling())ExcludeTree676(child);
 }

 protected void ExcludeCrew676(IEntity owner)
 {
  if(!owner)return;
  BaseCompartmentManagerComponent manager=BaseCompartmentManagerComponent.Cast(owner.FindComponent(BaseCompartmentManagerComponent));
  if(!manager)return;
  array<BaseCompartmentSlot> seats={};manager.GetCompartments(seats);
  foreach(BaseCompartmentSlot slot:seats)
  {
   if(slot)ExcludeTree676(slot.GetOccupant());
  }
 }

 protected void PrepareExclusions676()
 {
  m_Exclusions676.Clear();
  IEntity owner=GetOwner();IEntity root=owner.GetRootParent();
  ExcludeTree676(root);ExcludeTree676(owner);ExcludeTree676(m_User676);
  // Seated characters need not be entity children. The root manager registers
  // the mobile turret seats; also cover this standalone turret's own manager.
  ExcludeCrew676(root);
  if(root!=owner)ExcludeCrew676(owner);
 }

 protected bool CameraRay676(out vector start,out vector direction)
 {
  start=vector.Zero;direction=vector.Zero;
  CameraManager cameras=GetGame().GetCameraManager();if(!cameras)return false;
  CameraBase camera=cameras.CurrentCamera();if(!camera)return false;
  PlayerController player=GetGame().GetPlayerController();
  if(!player || camera!=player.GetPlayerCamera())return false;
  vector pose[4];camera.GetWorldCameraTransform(pose);
  float length=pose[2].Length();float originLength=pose[3].Length();
  // Negated bounded comparisons also reject NaN; no fallback to an entity centre
  // or a stale native sight pose if the rendered ADS camera is unavailable.
  if(!(length>0.9 && length<1.1) || !(originLength>=0 && originLength<10000000))return false;
  start=pose[3];direction=pose[2]/length;return true;
 }

 protected void OnMeasure676(float value,EActionTrigger reason)
 {
  // Recheck in the real action callback: frame ordering must not leave a former
  // gunner authorized between a seat/menu/ADS transition and the next FRAME.
  if(!ScopeMatches676())
  {
   ClearSample676(EGEP_Range676State.NONE);return;
  }
  if(m_LastRequest676>=0 && m_Clock676-m_LastRequest676<REQUEST_INTERVAL676)return;
  m_LastRequest676=m_Clock676;
  vector start,direction;
  ClearSample676(EGEP_Range676State.UNAVAILABLE);
  if(!CameraRay676(start,direction))
  {
   GEP_FireControl unavailableControl=GEP_FireControl.Cast(GetOwner().FindComponent(GEP_FireControl));
   if(unavailableControl)unavailableControl.CancelCorrection("ZERO CAMERA UNAVAILABLE");
   return;
  }
  PrepareExclusions676();
  TraceParam trace=new TraceParam();
  trace.Start=start;trace.End=start+direction*MAX_RANGE676;
  trace.Flags=TraceFlags.WORLD|TraceFlags.ENTS;trace.ExcludeArray=m_Exclusions676;
  float fraction=GetGame().GetWorld().TraceMove(trace,null);
  // A terrain/world hit need not have TraceEnt. Conversely fraction1 is no
  // bounded return, not a measured hit at the configured maximum.
  if(!(fraction>0 && fraction<1))
  {
   ClearSample676(EGEP_Range676State.NO_RETURN);
   GEP_FireControl noReturnControl=GEP_FireControl.Cast(GetOwner().FindComponent(GEP_FireControl));
   if(noReturnControl)noReturnControl.CancelCorrection("ZERO NO RETURN");
   return;
  }
  m_Distance676=MAX_RANGE676*fraction;
  m_Point676=start+direction*m_Distance676;
  m_SampledAt676=m_Clock676;m_State676=EGEP_Range676State.VALID;
  GEP_FireControl fireControl=GEP_FireControl.Cast(GetOwner().FindComponent(GEP_FireControl));
  if(fireControl)fireControl.RequestCorrection(m_Point676);
 }

 // Private, read-only native-table probe. It does not publish an aim solution,
 // store an elevation, change zeroing, move a gun, or authorize fire.

 EGEP_Range676State GetState676()
 {
  if(!ScopeMatches676())return EGEP_Range676State.UNAVAILABLE;
  return m_State676;
 }

 // HUD consumers receive the original sampled WORLD point and simulation age.
 // A new miss removes the previous result; no current target position is inferred.
 bool Read676(out vector point,out float distance,out float ageSeconds)
 {
  point=vector.Zero;distance=-1;ageSeconds=-1;
  if(GetState676()!=EGEP_Range676State.VALID || m_SampledAt676<0)return false;
  point=m_Point676;distance=m_Distance676;ageSeconds=Math.Max(0,m_Clock676-m_SampledAt676);return true;
 }

 override void OnPostInit(IEntity owner)
 {
  if(System.IsConsoleApp())return;
  SetEventMask(owner,EntityEvent.FRAME);
 }

 override void EOnFrame(IEntity owner,float timeSlice)
 {
  m_Clock676+=Math.Max(0,timeSlice);
  IEntity user;BaseCompartmentSlot seat;BaseSightsComponent sight;
  if(!CurrentScope676(user,seat,sight)){Release676();return;}
  if(user!=m_User676 || seat!=m_Seat676 || sight!=m_Sight676)
  {
   ClearSample676(EGEP_Range676State.NONE);m_LastRequest676=-1;
   m_User676=user;m_Seat676=seat;m_Sight676=sight;
  }
  Bind676(true);
  InputManager input=GetGame().GetInputManager();if(input)input.ActivateContext(CONTEXT676);
 }

 override void OnDelete(IEntity owner){Release676();}
}

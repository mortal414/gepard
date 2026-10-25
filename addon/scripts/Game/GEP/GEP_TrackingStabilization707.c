// Original guarded extension. WCS supplies target selection, RPC, servo and tracking.
// No copied dependency implementation, custom ballistic solver or target/physics setter.
class GEP_TrackingStabilization707Class : GEP_NextStabilizationConsumer625Class {}
class GEP_TrackingStabilization707 : GEP_NextStabilizationConsumer625
{
 // Presentation-only confirmation. Remote prediction alone is not a lock cue.
 protected IEntity m_LockCueTarget715;
 protected int m_LockCueRevision715=-1;
 protected int m_LockCueAwaitRevision715=-1;
 protected void ForgetLockCue715()
 {
  m_LockCueTarget715=null;m_LockCueRevision715=-1;
 }
 override protected void OnStateReplicated()
 {
  super.OnStateReplicated();
  ForgetLockCue715();
  if(m_iStateRevision==m_LockCueAwaitRevision715)return;
  if(GetMode()==WCS_EStabilizationMode.TRACK_ENTITY)
  {
   m_LockCueTarget715=GetStabilizationTarget();
   m_LockCueRevision715=m_iStateRevision;
  }
 }
 bool HasConfirmedEntityLockCue715()
 {
  if(GetMode()!=WCS_EStabilizationMode.TRACK_ENTITY || !GetStabilizationEnabled() || !Live625())return false;
  IEntity target715=GetStabilizationTarget();
  if(!TargetUsable707(target715))return false;
  // Solo/listen authority has already validated the WCS command. A client waits
  // for WCS's replicated-state callback, which its prediction does not invoke.
  return IsAuthority() || (m_LockCueTarget715==target715 && m_LockCueRevision715==m_iStateRevision);
 }
 protected bool m_HadEntity707;
 protected IEntity m_Entity707;
 protected bool TargetUsable707(IEntity target707)
 {
  if(!target707 || target707.IsDeleted())return false;
  SCR_DamageManagerComponent own707=SCR_DamageManagerComponent.Cast(target707.FindComponent(SCR_DamageManagerComponent));
  if(own707 && own707.GetState()==EDamageState.DESTROYED)return false;
  IEntity root707=target707.GetRootParent();
  if(!root707 || root707.IsDeleted())return false;
  SCR_DamageManagerComponent rootDamage707=SCR_DamageManagerComponent.Cast(root707.FindComponent(SCR_DamageManagerComponent));
  return !rootDamage707 || rootDamage707.GetState()!=EDamageState.DESTROYED;
 }
 protected void ForgetEntity707()
 {
  ForgetLockCue715();
  m_HadEntity707=false;
  m_Entity707=null;
 }
 protected void CancelLostTarget707()
 {
  // Shared owner/authority seam uses the existing WCS OFF command route.
  // No local-generation/server-operator conflation or observer state clear.
  CancelAssistance625("tracking_target_lost");
  ForgetEntity707();
  Print("GEP_TRACKING707_TARGET_LOST requiresFreshHold=1");
 }
 override void Guard625()
 {
  super.Guard625();
  if(!GuardScopeLive625() || GetMode()==WCS_EStabilizationMode.OFF)
  {
   ForgetEntity707();
   return;
  }
  if(m_HadEntity707 && !TargetUsable707(m_Entity707))
  {
   CancelLostTarget707();
   return;
  }
  if(GetMode()==WCS_EStabilizationMode.TRACK_ENTITY)
  {
   IEntity target707=GetStabilizationTarget();
   if(!TargetUsable707(target707))
   {
    CancelLostTarget707();
    return;
   }
   m_Entity707=target707;
   m_HadEntity707=true;
  }
  else
  {
   // TRACK_POSITION intentionally has no entity and must not be treated as a loss.
   ForgetEntity707();
  }
 }
 override protected void OnStabilizationTapAction()
 {
  // Clear the presentation confirmation on both designate and clear attempts.
  ForgetLockCue715();
  Guard625();
  if(!Live625() || !GetStabilizationEnabled())
  {
   m_Blocked625++;
   return;
  }
  m_LockCueAwaitRevision715=m_iStateRevision;
  ForwardDependencyTap625();
  // A deliberate clear to STABILIZED must discard our old entity observation now.
  if(GetMode()!=WCS_EStabilizationMode.TRACK_ENTITY)ForgetEntity707();
  Guard625();
 }
}

class GEP_TrackingHints707Class : GEP_NextAimingHints625Class {}
class GEP_TrackingHints707 : GEP_NextAimingHints625
{
 override void EOnFrame(IEntity owner,float timeSlice)
 {
  super.EOnFrame(owner,timeSlice);
  if(!Visible625())return;
  GEP_NextStabilizationConsumer625 stab707=GEP_NextStabilizationConsumer625.Cast(owner.FindComponent(GEP_NextStabilizationConsumer625));
  if(!stab707 || !stab707.GetStabilizationEnabled())return;
  if(stab707.IsTracking())m_Mode625.SetLabel("AUS halten | Ziel loesen tippen");
  else m_Mode625.SetLabel("AUS halten | Ziel erfassen tippen");
 }
}

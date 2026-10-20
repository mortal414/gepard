// Original private development prototype. Not attached to frozen678.
// Uses native OnPrepareControls, never cinematic/direct aim setters or auto-fire.
// Default disabled. Direct correction is a separate, level-only native experiment.
class GEP_FireControlClass : ScriptComponentClass {}
class GEP_FireControl : ScriptComponent
{
 const string ACTION="GEPStabilization";
 const string CONTEXT="GEPFireControlContext";
 protected ref GEP_FireControlPress m_Press=new GEP_FireControlPress();
 protected bool m_Enabled;
 protected bool m_DirectExperiment;
 protected bool m_Bound;
 protected bool m_Stabilized;
 protected bool m_Reanchor;
 protected bool m_Zeroing;
 protected bool m_ObserveCorrection;
 protected float m_Clock;
 protected float m_LastCorrection=-1;
 protected float m_ActualZero;
 protected float m_PreviousBonePitch;
 protected float m_TargetBonePitch;
 protected float m_ZeroStarted;
 protected BaseWeaponComponent m_ZeroWeapon;
 protected ResourceName m_ZeroAmmo;
 protected int m_ZeroGroup=-1;
 protected bool m_ZeroStale;
 protected string m_ZeroFeedback="";
 protected float m_ZeroFeedbackUntil;
 protected bool m_SpaceArmed;
 protected int m_ContextFrames;
 protected int m_LastFrameTick;
 protected vector m_WorldLOS;
 protected IEntity m_User;
 protected BaseCompartmentSlot m_Seat;
 protected BaseSightsComponent m_Sight;
 protected string m_Status="";
 protected int m_LastProbeTick;
 protected int m_PrepareCalls;
 protected int m_PlayerPrepareCalls;
 protected float m_CommandRight;
 protected float m_CommandUp;

 // Passive once-per-second trace in explicitly enabled development profiles.
 // Observes public control callbacks and real runtime ownership; never sets it.
 protected void Probe(int tick)
 {
  if(!m_Enabled || tick-m_LastProbeTick<1000)return;
  m_LastProbeTick=tick;IEntity owner=GetOwner();if(!owner)return;
  RplComponent rpl=RplComponent.Cast(owner.FindComponent(RplComponent));
  GEP_Optical444 optical=GEP_Optical444.Cast(owner.FindComponent(GEP_Optical444));
  bool localGunner=GEP_GunnerSnapshot583.IsLocalGunner(owner);
  bool opticActive=optical && optical.Active();
  PrintFormat("GEP_FC_SCOPE enabled=%1 scope=%2 localGunner=%3 optic=%4 rpl=%5 prepareCalls=%6 playerCalls=%7 status=%8",m_Enabled,Scope(),localGunner,opticActive,rpl,m_PrepareCalls,m_PlayerPrepareCalls,m_Status);
  if(rpl)PrintFormat("GEP_FC_OWNER id=%1 node=%2 role=%3 isOwner=%4 isProxy=%5",rpl.Id(),rpl.GetNode(),rpl.Role(),rpl.IsOwner(),rpl.IsProxy());
  vector localPose[4],worldPose[4];float pitch;
  if(RawPose(localPose,worldPose,pitch))PrintFormat("GEP_FC_POSE bonePitch=%1 actualZero=%2 targetPitch=%3 commandRight=%4 commandUp=%5 stabilized=%6 zeroing=%7",pitch,m_ActualZero,m_TargetBonePitch,m_CommandRight,m_CommandUp,m_Stabilized,m_Zeroing);
  m_PrepareCalls=0;m_PlayerPrepareCalls=0;m_CommandRight=0;m_CommandUp=0;
 }

 protected bool Scope()
 {
  if(!m_Enabled || !GetGame() || !GetGame().InPlayMode() || System.IsConsoleApp())return false;
  IEntity owner=GetOwner();
  if(!owner || owner.IsDeleted() || !GEP_GunnerSnapshot583.IsLocalGunner(owner))return false;
  GEP_Optical444 optical=GEP_Optical444.Cast(owner.FindComponent(GEP_Optical444));
  if(!optical || !optical.Active())return false;
  IEntity user=optical.User();
  if(!user)return false;
  CharacterControllerComponent life=CharacterControllerComponent.Cast(user.FindComponent(CharacterControllerComponent));
  if(!life || life.IsUnconscious() || life.GetLifeState()!=ECharacterLifeState.ALIVE)return false;
  GEP_TurretController410 control=GEP_TurretController410.Cast(owner.FindComponent(TurretControllerComponent));
  if(!control || control.IsDriveFailed())return false;
  // A client proxy can be owner. IsMaster would incorrectly exclude that case.
  // Effective child/root hierarchy ownership must still be qualified natively.
  RplComponent rpl=RplComponent.Cast(owner.FindComponent(RplComponent));
  // Native Turret_Base supplies this runtime node. Never silently substitute
  // the hull node or change ownership if it is missing/unexpected.
  if(!rpl || !rpl.IsOwner())return false;
  CompartmentAccessComponent access=CompartmentAccessComponent.Cast(user.FindComponent(CompartmentAccessComponent));
  if(!access || access.IsGettingIn() || access.IsGettingOut() || access.IsSwitchingSeatsAnim())return false;
  return access.GetCompartment() && control.GetCurrentSights();
 }
 protected bool ScopeMatches()
 {
  if(!Scope())return false;
  GEP_Optical444 optical=GEP_Optical444.Cast(GetOwner().FindComponent(GEP_Optical444));
  IEntity user=optical.User();
  CompartmentAccessComponent access=CompartmentAccessComponent.Cast(user.FindComponent(CompartmentAccessComponent));
  TurretControllerComponent control=TurretControllerComponent.Cast(GetOwner().FindComponent(TurretControllerComponent));
  return user==m_User && access.GetCompartment()==m_Seat && control.GetCurrentSights()==m_Sight;
 }
 protected bool RenderedLOS(out vector direction)
 {
  direction=vector.Zero;
  CameraManager cameras=GetGame().GetCameraManager();PlayerController player=GetGame().GetPlayerController();
  if(!cameras || !player || cameras.CurrentCamera()!=player.GetPlayerCamera())return false;
  CameraBase camera=cameras.CurrentCamera();if(!camera)return false;
  vector pose[4];camera.GetWorldCameraTransform(pose);
  if(!GEP_FireControlGeometry.Direction(pose[2]))return false;
  direction=pose[2].Normalized();return true;
 }
 protected bool RawPose(out vector localPose[4],out vector worldPose[4],out float bonePitch)
 {
  bonePitch=0;IEntity owner=GetOwner();Animation animation=owner.GetAnimation();
  if(!animation)return false;
  TNodeId bone=animation.GetBoneIndex("GEP_GunElevation");
  if(!animation.GetBoneMatrix(bone,localPose))return false;
  if(!GEP_FireControlGeometry.Direction(localPose[2]))return false;
  vector ownerPose[4];owner.GetWorldTransform(ownerPose);
  Math3D.MatrixMultiply4(ownerPose,localPose,worldPose);
  bonePitch=GEP_FireControlGeometry.PitchDegrees(localPose[2]);return true;
 }
 protected void Observe(float bonePitch)
 {
  if(m_ObserveCorrection)m_ActualZero+=GEP_FireControlGeometry.ShortestDegrees(bonePitch-m_PreviousBonePitch);
  m_PreviousBonePitch=bonePitch;
 }
 // Called immediately before the existing roof-sight PointInfo is submitted.
 // Observes actual bone motion again to avoid assuming the last command moved it.
 void CorrectSight(vector localPose[4])
 {
  if(!ScopeMatches())return;
  Observe(GEP_FireControlGeometry.PitchDegrees(localPose[2]));
  GEP_FireControlGeometry.CounterPitch(localPose,m_ActualZero);
 }
 protected void Reset()
 {
  m_Press.Cancel();m_Stabilized=false;m_Reanchor=false;m_Zeroing=false;m_ObserveCorrection=false;
  m_ActualZero=0;m_WorldLOS=vector.Zero;m_User=null;m_Seat=null;m_Sight=null;m_Status="";
  m_ZeroWeapon=null;m_ZeroAmmo="";m_ZeroGroup=-1;m_ZeroStale=false;
  m_ZeroFeedback="";m_ZeroFeedbackUntil=0;
  m_SpaceArmed=false;m_ContextFrames=0;
  // Do not move a departing/new occupant's gun to unwind a former sight offset.
 }
 protected void Bind(bool wanted)
 {
  if(m_Bound==wanted || !GetGame())return;
  InputManager input=GetGame().GetInputManager();if(!input)return;
  if(wanted){input.AddActionListener(ACTION,EActionTrigger.DOWN,OnDown);input.AddActionListener(ACTION,EActionTrigger.UP,OnUp);}
  else{input.RemoveActionListener(ACTION,EActionTrigger.DOWN,OnDown);input.RemoveActionListener(ACTION,EActionTrigger.UP,OnUp);m_Press.Cancel();}
  m_Bound=wanted;
 }
 protected void OnDown(float value,EActionTrigger reason){if(m_SpaceArmed && ScopeMatches())m_Press.Down();}
 protected void OnUp(float value,EActionTrigger reason)
 {
  if(!ScopeMatches()){m_Press.Cancel();return;}
  int gap=System.GetTickCount()-m_LastFrameTick;
  if(gap<0 || gap>250){m_Press.Cancel();m_SpaceArmed=false;m_ContextFrames=0;return;}
  m_SpaceArmed=true;
  if(!m_Press.Up() || !m_Stabilized)return;
  GEP_Optical444 optical=GEP_Optical444.Cast(GetOwner().FindComponent(GEP_Optical444));
  if(optical)optical.ToggleTrack(m_User);
  m_Reanchor=true;
 }
 protected void Toggle()
 {
  if(!ScopeMatches())return;
  if(m_Stabilized)
  {
   m_Stabilized=false;CancelCorrection("STAB OFF");
   GEP_Optical444 stoppedOptic=GEP_Optical444.Cast(GetOwner().FindComponent(GEP_Optical444));
   if(stoppedOptic)stoppedOptic.Clear("MANUAL - STABILIZATION OFF");
   return;
  }
  vector direction;if(!RenderedLOS(direction)){m_Status="STAB UNAVAILABLE";return;}
  GEP_Optical444 optical=GEP_Optical444.Cast(GetOwner().FindComponent(GEP_Optical444));
  if(optical)optical.Clear();
  m_WorldLOS=direction;m_Stabilized=true;m_Reanchor=false;m_Status="STABILIZING";
 }
 protected bool Manual(ActionManager am)
 {
  return Math.AbsFloat(am.GetActionValue("TurretAimLeft"))+Math.AbsFloat(am.GetActionValue("TurretAimRight"))+
   Math.AbsFloat(am.GetActionValue("TurretAimUp"))+Math.AbsFloat(am.GetActionValue("TurretAimDown"))+
   Math.AbsFloat(am.GetActionValue("TurretAimGamepadLeft"))+Math.AbsFloat(am.GetActionValue("TurretAimGamepadRight"))+
   Math.AbsFloat(am.GetActionValue("TurretAimGamepadUp"))+Math.AbsFloat(am.GetActionValue("TurretAimGamepadDown"))+
   Math.AbsFloat(am.GetActionValue("GyroAimYaw"))+Math.AbsFloat(am.GetActionValue("GyroAimPitch"))+
   Math.AbsFloat(am.GetActionValue("TurretRotateLeft"))+Math.AbsFloat(am.GetActionValue("TurretRotateRight"))+
   Math.AbsFloat(am.GetActionValue("TurretRotateUp"))+Math.AbsFloat(am.GetActionValue("TurretRotateDown"))>0.001;
 }
 // Native input values have no documented degrees/second conversion. This is
 // an explicitly uncalibrated bounded servo, adapted from our original helper.
 protected void Command(ActionManager am,float rightDegrees,float upDegrees,float dt)
 {
  m_CommandRight=rightDegrees;m_CommandUp=upDegrees;
  SCR_2DOpticsComponent optics=SCR_2DOpticsComponent.Cast(m_Sight);
  float zoom=1;if(optics)zoom=Math.Max(1,optics.GetMagnification()/2);
  float gain=Math.Clamp(dt,0,0.05)*60*zoom;
  float x=Math.Clamp(rightDegrees*gain,-2*zoom,2*zoom);
  float y=Math.Clamp(upDegrees*gain,-2*zoom,2*zoom);
  if(x>0)am.SetActionValue("TurretAimRight",x);else if(x<0)am.SetActionValue("TurretAimLeft",-x);
  if(y>0)am.SetActionValue("TurretAimUp",y);else if(y<0)am.SetActionValue("TurretAimDown",-y);
 }
 protected int CurrentGroup()
 {
  SCR_FireModeManagerComponent groups=SCR_FireModeManagerComponent.Cast(GetOwner().FindComponent(SCR_FireModeManagerComponent));
  if(!groups)return -1;
  string name;return groups.GetWeaponGroupID(name);
 }
 protected bool BothFeedsCompatible(BaseWeaponManagerComponent manager,ResourceName ammo)
 {
  array<WeaponSlotComponent> slots={};manager.GetWeaponsSlots(slots);
  int matching;
  foreach(WeaponSlotComponent slot:slots)
  {
   if(!slot || slot.GetWeaponSlotIndex()<0 || slot.GetWeaponSlotIndex()>1)continue;
   BaseMuzzleComponent muzzle=slot.GetCurrentMuzzle();BaseMagazineComponent magazine;
   if(muzzle)magazine=muzzle.GetMagazine();
   if(muzzle && magazine && muzzle.GetAmmoCount()>0 && magazine.GetAmmoType()==ammo)matching++;
  }
  return matching==2;
 }
 protected void CheckZeroIdentity()
 {
  if(!m_ZeroWeapon)return;
  TurretControllerComponent controller=TurretControllerComponent.Cast(GetOwner().FindComponent(TurretControllerComponent));
  BaseWeaponManagerComponent manager=controller.GetWeaponManager();
  BaseWeaponComponent weapon;if(manager)weapon=manager.GetCurrentWeapon();
  BaseMagazineComponent magazine;if(weapon)magazine=weapon.GetCurrentMagazine();
  BaseMuzzleComponent muzzle;if(weapon)muzzle=weapon.GetCurrentMuzzle();
  bool compatible=weapon==m_ZeroWeapon && muzzle && muzzle.GetAmmoCount()>0 && magazine && magazine.GetAmmoType()==m_ZeroAmmo && CurrentGroup()==m_ZeroGroup;
  if(compatible && m_ZeroGroup==0)compatible=BothFeedsCompatible(manager,m_ZeroAmmo);
  if(compatible)return;
  m_Zeroing=false;m_ObserveCorrection=false;m_ZeroStale=true;
  // Retain only the physical camera offset; never pretend it is a new valid
  // solution or issue an automatic unwind on a group/ammunition change.
 }
 void CancelCorrection(string status)
 {
  vector localPose[4],worldPose[4];float pitch;
  if(RawPose(localPose,worldPose,pitch))Observe(pitch);
  m_Zeroing=false;m_ObserveCorrection=false;m_Status=status;
  m_ZeroFeedback=status;m_ZeroFeedbackUntil=m_Clock+3;
 }
 // Exactly one controller decides assistance. False permits existing optional
 // optical follow; true means this controller consumed assistance this frame.
 bool PrepareControls(ActionManager am,float dt,bool player)
 {
  m_PrepareCalls++;if(player)m_PlayerPrepareCalls++;
  if(!player || !am || !ScopeMatches())return false;
  vector localPose[4],raw[4];float pitch;if(!RawPose(localPose,raw,pitch))return false;
  Observe(pitch);m_ObserveCorrection=false;
  CheckZeroIdentity();
  if(Math.AbsFloat(m_ActualZero)>3){CancelCorrection("ZERO OFFSET LIMIT");m_Stabilized=false;return true;}
  if(Manual(am))
  {
   GEP_Optical444 manualOptic=GEP_Optical444.Cast(GetOwner().FindComponent(GEP_Optical444));
   if(manualOptic && manualOptic.State!=0)manualOptic.Clear("MANUAL - AIM OVERRIDE");
   m_Zeroing=false;m_Reanchor=true;m_Status="MANUAL AIM";return true;
  }
  GEP_Optical444 optical=GEP_Optical444.Cast(GetOwner().FindComponent(GEP_Optical444));
  if(optical && optical.State!=0){m_Zeroing=false;m_Reanchor=true;m_Status=optical.Feedback;return false;}
  if(m_Zeroing)
  {
   if(m_Clock-m_ZeroStarted>2){CancelCorrection("ZERO SERVO TIMEOUT");return true;}
   float error=m_TargetBonePitch-pitch;
   if(Math.AbsFloat(error)<0.02){m_Zeroing=false;m_Status="LEVEL ZERO APPLIED";return true;}
   Command(am,0,error,dt);m_PreviousBonePitch=pitch;m_ObserveCorrection=true;
   m_Status="LEVEL ZEROING";return true;
  }
  if(!m_Stabilized)return false;
  if(m_Reanchor)
  {
   if(!RenderedLOS(m_WorldLOS)){m_Stabilized=false;m_Status="STAB LOST CAMERA";return true;}
   m_Reanchor=false;
  }
  TurretControllerComponent controller=TurretControllerComponent.Cast(GetOwner().FindComponent(TurretControllerComponent));
  vector pose[4];float fov;if(!controller.GetCurrentSightsCameraTransform(pose,fov))return true;
  // Roll-aware actuator solution from the actual unrotated owner/mount.
  vector mount[4];GetOwner().GetWorldTransform(mount);
  vector required;float right,up;
  if(!GEP_FireControlGeometry.MountErrors(m_WorldLOS,mount,localPose,m_ActualZero,required,right,up))
  {m_Status="STAB LOST GEOMETRY";return true;}
  vector excess=controller.GetTurretComponent().GetAimingAngleExcess(pose[3]+required*1000);
  if(excess.Length()>0.1){m_Status="STAB LIMIT";return true;}
  if(Math.AbsFloat(right)>0.05 || Math.AbsFloat(up)>0.05){Command(am,right,up,dt);m_Status="STABILIZING";}
  else m_Status="STABILIZED";
  return true;
 }
 // Not the complete requested C function. This optional development experiment
 // follows native DIRECT-fire compensation, rejects slopes above2 degrees,
 // never calls the expensive indirect-only altitude helper and has no fallback
 // to guessed velocity/gravity. Its exact35mm table/impacts remain unqualified.
 protected void CalculateCorrection(vector sampledWorldPoint)
 {
  if(!ScopeMatches())return;
  if(!m_DirectExperiment){m_Status="RANGE ONLY / ZERO UNQUALIFIED";return;}
  if(m_LastCorrection>=0 && m_Clock-m_LastCorrection<0.25)return;
  m_LastCorrection=m_Clock;
  CancelCorrection("ZERO MEASURING");
  TurretControllerComponent controller=TurretControllerComponent.Cast(GetOwner().FindComponent(TurretControllerComponent));
  BaseWeaponManagerComponent manager=controller.GetWeaponManager();
  BaseWeaponComponent weapon;if(manager)weapon=manager.GetCurrentWeapon();
  BaseMuzzleComponent muzzle;if(weapon)muzzle=weapon.GetCurrentMuzzle();
  BaseMagazineComponent magazine;if(muzzle)magazine=muzzle.GetMagazine();
  if(!muzzle || !magazine || muzzle.GetAmmoCount()<=0){m_Status="ZERO NO LOADED PROJECTILE";return;}
  int group=CurrentGroup();WeaponSlotComponent selected=manager.GetCurrentSlot();
  if(!selected || group<0 || group>2){m_Status="ZERO GROUP UNAVAILABLE";return;}
  int selectedIndex=selected.GetWeaponSlotIndex();
  if((group==1 && selectedIndex!=0) || (group==2 && selectedIndex!=1) || selectedIndex<0 || selectedIndex>1)
  {m_Status="ZERO MUZZLE GROUP MISMATCH";return;}
  if(group==0 && !BothFeedsCompatible(manager,magazine.GetAmmoType())){m_Status="ZERO BOTH FEEDS UNQUALIFIED";return;}
  vector mount[4];GetOwner().GetWorldTransform(mount);
  if(vector.Dot(mount[1],vector.Up)<Math.Cos(Math.DEG2RAD))
  {m_Status="ZERO UNQUALIFIED MOUNT TILT";return;}
  vector muzzlePose[4];if(!manager.GetCurrentMuzzleTransform(muzzlePose)){m_Status="ZERO NO MUZZLE";return;}
  vector delta=sampledWorldPoint-muzzlePose[3];float range=delta.Length();
  float horizontal=Math.Sqrt(delta[0]*delta[0]+delta[2]*delta[2]);
  if(!(horizontal>1 && range<5000)){m_Status="ZERO RANGE REJECTED";return;}
  float slope=Math.Atan2(delta[1],horizontal)*Math.RAD2DEG;
  if(Math.AbsFloat(slope)>2){m_Status="ZERO UNQUALIFIED SLOPE";return;}
  float time;float height=BallisticTable.GetAimHeightOfNextProjectile(range,time,muzzle,true);
  if(!(time>0 && time<7 && height>=0 && height<1000000)){m_Status="ZERO TABLE REJECTED";return;}
  vector aimPoint;if(!GEP_FireControlGeometry.DirectAimPoint(muzzlePose[3],sampledWorldPoint,height,aimPoint,slope))return;
  vector excess=controller.GetTurretComponent().GetAimingAngleExcess(aimPoint);
  if(Math.AbsFloat(excess[1])>0.1){m_Status="ZERO ELEVATION LIMIT";return;}
  vector localPose[4],raw[4];float pitch;if(!RawPose(localPose,raw,pitch))return;
  Observe(pitch);
  vector launch=(aimPoint-muzzlePose[3]).Normalized();
  vector localLaunch=Vector(vector.Dot(launch,mount[0]),vector.Dot(launch,mount[1]),vector.Dot(launch,mount[2]));
  float up=GEP_FireControlGeometry.PitchDegrees(localLaunch)-pitch;
  vector original=(sampledWorldPoint-muzzlePose[3]).Normalized();
  vector localOriginal=Vector(vector.Dot(original,mount[0]),vector.Dot(original,mount[1]),vector.Dot(original,mount[2]));
  float requiredYaw=Math.Atan2(localLaunch[0],localLaunch[2])*Math.RAD2DEG;
  float baselineYaw=Math.Atan2(localOriginal[0],localOriginal[2])*Math.RAD2DEG;
  if(Math.AbsFloat(GEP_FireControlGeometry.ShortestDegrees(requiredYaw-baselineYaw))>0.01)
  {m_Status="ZERO YAW COMPENSATION UNQUALIFIED";return;}
  if(Math.AbsFloat(up)>3){m_Status="ZERO CORRECTION LIMIT";return;}
  // Replacement absolute target from the CURRENT physical barrel. Repeated C
  // on the same corrected LOS converges; it never adds the old request again.
  m_TargetBonePitch=pitch+up;m_PreviousBonePitch=pitch;m_ObserveCorrection=false;
  m_ZeroWeapon=weapon;m_ZeroAmmo=magazine.GetAmmoType();m_ZeroGroup=group;m_ZeroStale=false;
  m_Zeroing=true;m_ZeroStarted=m_Clock;m_Status="LEVEL ZERO REQUESTED";
 }
 void RequestCorrection(vector sampledWorldPoint)
 {
  CalculateCorrection(sampledWorldPoint);
  m_ZeroFeedback=m_Status;m_ZeroFeedbackUntil=m_Clock+3;
 }
 string GetStatus()
 {
  if(!ScopeMatches())return "";
  string status=m_Status;
  if(m_Clock<m_ZeroFeedbackUntil && m_ZeroFeedback!="" && m_ZeroFeedback!=status)status+=" / "+m_ZeroFeedback;
  if(m_ZeroStale)status+=" / ZERO STALE";
  return status;
 }
 override void OnPostInit(IEntity owner)
 {
  m_Enabled=FileIO.FileExists("$profile:GEP-FireControl-Development.flag");
  m_DirectExperiment=FileIO.FileExists("$profile:GEP-FireControl-DirectProbe.flag");
  if(!System.IsConsoleApp())SetEventMask(owner,EntityEvent.FRAME);
 }
 override void EOnFrame(IEntity owner,float timeSlice)
 {
  int tick=System.GetTickCount();int gap=tick-m_LastFrameTick;
  m_LastFrameTick=tick;
  Probe(tick);
  // A pause/long stall must not preserve a pending hold and fire it on resume.
  if(gap<0 || gap>250){m_Press.Cancel();m_SpaceArmed=false;m_ContextFrames=0;}
  m_Clock+=Math.Max(0,timeSlice);
  if(!Scope()){Bind(false);Reset();return;}
  GEP_Optical444 optical=GEP_Optical444.Cast(owner.FindComponent(GEP_Optical444));
  IEntity user=optical.User();CompartmentAccessComponent access=CompartmentAccessComponent.Cast(user.FindComponent(CompartmentAccessComponent));
  TurretControllerComponent controller=TurretControllerComponent.Cast(owner.FindComponent(TurretControllerComponent));
  if(user!=m_User || access.GetCompartment()!=m_Seat || controller.GetCurrentSights()!=m_Sight)
  {
   Reset();m_User=user;m_Seat=access.GetCompartment();m_Sight=controller.GetCurrentSights();
  }
  Bind(true);InputManager input=GetGame().GetInputManager();
  if(input)
  {
   input.ActivateContext(CONTEXT);m_ContextFrames++;
   if(m_ContextFrames>=2 && input.GetActionValue(ACTION)<0.001)m_SpaceArmed=true;
  }
  if(m_Press.Advance(timeSlice))Toggle();
 }
 override void OnDelete(IEntity owner){Bind(false);Reset();}
}

// Original fixed driver optics. View selection is private to the current local
// living pilot; driving remains on the native tracked controller. This is a
// simplified optical camera transfer, not a simulated internal prism renderer.
class GEP_DriverStationClass : ScriptComponentClass {}
class GEP_DriverStation : ScriptComponent
{
 protected IEntity m_ViewUser;
 protected int m_ViewMode;
 protected bool m_HatchReady;
 protected IEntity m_HatchVisual;
 protected vector m_HatchRest[4];
 protected float m_HatchProgress;
 override void OnPostInit(IEntity owner) { SetEventMask(owner,EntityEvent.FRAME|EntityEvent.POSTFRAME); }
 IEntity GetHatchVisual() {return m_HatchVisual;}
 float GetHatchProgress() {return m_HatchProgress;}
 float HatchEase(float value) {value=Math.Max(0,Math.Min(1,value));return value*value*(3-2*value);}
 bool IsDriver(IEntity user)
 {
  if(!user) return false;
  CharacterControllerComponent life=CharacterControllerComponent.Cast(user.FindComponent(CharacterControllerComponent));
  CompartmentAccessComponent access=CompartmentAccessComponent.Cast(user.FindComponent(CompartmentAccessComponent));
  if(!life || life.GetLifeState()!=ECharacterLifeState.ALIVE || !access || access.IsGettingIn() || access.IsGettingOut() || access.IsSwitchingSeatsAnim()) return false;
  BaseCompartmentSlot seat=access.GetCompartment();
  return seat && PilotCompartmentSlot.Cast(seat) && seat.GetVehicle()==GetOwner() && seat.IsPiloting();
 }
 void SelectView(IEntity user,int mode)
 {
  PlayerController player=GetGame().GetPlayerController();
  if(!player || player.GetControlledEntity()!=user || !IsDriver(user) || mode<0 || mode>3) return;
  m_ViewUser=user;m_ViewMode=mode;
 }
 int GetViewMode(IEntity user)
 {
  if(user!=m_ViewUser || !IsDriver(user)) return 0;
  return m_ViewMode;
 }
 bool GetOpticalTransform(IEntity user,out vector worldPose[4])
 {
  int mode=GetViewMode(user);if(mode==0) return false;
  float yaw;vector point="0.5 1.592 2.42";
  if(mode==2){yaw=-28;point[0]=0.32;}
  if(mode==3){yaw=28;point[0]=0.68;}
  vector localPose[4];Math3D.AnglesToMatrix(Vector(yaw,0,0),localPose);localPose[3]=point;
  vector vehiclePose[4];GetOwner().GetTransform(vehiclePose);Math3D.MatrixMultiply4(vehiclePose,localPose,worldPose);
  return true;
 }
 override void EOnFrame(IEntity owner,float timeSlice)
 {
  if(m_ViewUser && !IsDriver(m_ViewUser)){m_ViewMode=0;m_ViewUser=null;}
  // Entry starts in the native seated/free-look camera. Periscopes are deliberate.
  if(!m_ViewUser)
  {
   PlayerController player=GetGame().GetPlayerController();
   if(player && IsDriver(player.GetControlledEntity()))SelectView(player.GetControlledEntity(),0);
  }
 }
 override void EOnPostFrame(IEntity owner,float timeSlice)
 {
  if(!GetGame().InPlayMode()) return;
  if(!m_HatchVisual)
  {
   for(IEntity child=owner.GetChildren();child;child=child.GetSibling())
   {
    if(child.GetPrefabData() && child.GetPrefabData().GetPrefabName().Contains("GepardDriverHatch.et")){m_HatchVisual=child;break;}
   }
  }
  if(!m_HatchVisual) return;
  if(!m_HatchReady){m_HatchVisual.GetLocalTransform(m_HatchRest);m_HatchReady=true;}

  BaseCompartmentManagerComponent manager=BaseCompartmentManagerComponent.Cast(owner.FindComponent(BaseCompartmentManagerComponent));
  if(!manager) return;
  // Original game approximation: lift the lid clear of the coaming, translate it
  // vehicle-left, then settle it. Photos establish the endpoints,
  // not this linkage. Native door state remains the sole animation input.
  float target;if(manager.IsDoorOpen(0)) target=1;
  m_HatchProgress=Math.Lerp(m_HatchProgress,target,Math.Min(timeSlice*4,1));
  float lift=HatchEase(m_HatchProgress/0.2)-0.75*HatchEase((m_HatchProgress-0.8)/0.2);
  float travel=HatchEase((m_HatchProgress-0.2)/0.6);
  vector turn[4],pose[4];Math3D.AnglesToMatrix(Vector(90*travel,0,0),turn);
  Math3D.MatrixMultiply3(turn,m_HatchRest,pose);pose[3]=m_HatchRest[3]+Vector(-0.76*travel,0.03*lift,0);
  m_HatchVisual.SetLocalTransform(pose);m_HatchVisual.Update();
 }
}

class GEP_DriverViewAction : ScriptedUserAction
{
 [Attribute("0",UIWidgets.EditBox,"0 cabin; 1 forward; 2 left; 3 right")]
 int ViewMode;
 GEP_DriverStation Station() { return GEP_DriverStation.Cast(GetOwner().FindComponent(GEP_DriverStation)); }
 override bool HasLocalEffectOnlyScript() { return true; }
 override bool CanBeShownScript(IEntity user)
 {
  GEP_DriverStation station=Station();
  if(!station || !station.IsDriver(user))return false;
  int current=station.GetViewMode(user);
  // Entry lives at each physical optic; a visible return action owns the fixed view.
  if(ViewMode==0)return current!=0;
  return current==0;
 }
 override bool CanBePerformedScript(IEntity user) { return CanBeShownScript(user); }
 override void PerformAction(IEntity pOwnerEntity,IEntity pUserEntity)
 {
  GEP_DriverStation station=Station();if(station && CanBePerformedScript(pUserEntity)) station.SelectView(pUserEntity,ViewMode);
 }
}

// Installed pilot camera selection prioritizes the driving controller, so an
// attached turret periscope is not selected for pilots. Keep the native camera
// and input lifecycle, modifying only the optical transform for our component.
modded class CharacterCamera1stPersonVehicle
{
 override void OnAfterCameraUpdate(float pDt,bool pIsKeyframe,inout vector transformMS[4])
 {
  super.OnAfterCameraUpdate(pDt,pIsKeyframe,transformMS);
  if(!m_OwnerVehicle || !m_OwnerCharacter) return;
  GEP_DriverStation station=GEP_DriverStation.Cast(m_OwnerVehicle.FindComponent(GEP_DriverStation));
  if(!station || !station.IsDriver(m_OwnerCharacter) || station.GetViewMode(m_OwnerCharacter)!=0) return;
  // Reclining the native seat tilts its yaw axis. Keep the actual animated
  // eye translation, but let the pilot look around relative to the vehicle.
  vector look=m_Input.GetLookAtAngles();vector view[4],vehiclePose[4],worldPose[4],actorPose[4],localPose[4];
  Math3D.AnglesToMatrix(Vector(look[0]*Math.RAD2DEG,look[1]*Math.RAD2DEG-20,0),view);
  m_OwnerVehicle.GetTransform(vehiclePose);m_OwnerCharacter.GetTransform(actorPose);
  Math3D.MatrixMultiply3(vehiclePose,view,worldPose);Math3D.MatrixInvMultiply4(actorPose,worldPose,localPose);
  for(int axis=0;axis<3;axis++) transformMS[axis]=localPose[axis];
 }
 override void OnUpdate(float pDt,out ScriptedCameraItemResult pOutResult)
 {
  super.OnUpdate(pDt,pOutResult);
  if(!m_OwnerVehicle || !m_OwnerCharacter) return;
  GEP_DriverStation station=GEP_DriverStation.Cast(m_OwnerVehicle.FindComponent(GEP_DriverStation));
  vector worldPose[4];if(!station || !station.GetOpticalTransform(m_OwnerCharacter,worldPose)) return;
  vector actorPose[4];m_OwnerCharacter.GetTransform(actorPose);
  Math3D.MatrixInvMultiply4(actorPose,worldPose,pOutResult.m_CameraTM);
  pOutResult.m_pWSAttachmentReference=null;pOutResult.m_pOwner=m_OwnerCharacter;
  pOutResult.m_iDirectBone=-1;pOutResult.m_iDirectBoneMode=EDirectBoneMode.None;
  pOutResult.m_fDistance=0;pOutResult.m_fUseHeading=0;pOutResult.m_fNearPlane=0.025;
  pOutResult.m_fFOV=55;pOutResult.m_bAllowInterpolation=true;
 }
}

// Presentation only. The native interaction handler still owns and dispatches
// F to the existing return action. Do not hide/disable the action itself or
// register another F listener, which could double-dispatch another context.
modded class SCR_ActionMenuInteractionDisplay
{
 override void SetDisplayData(ActionDisplayData data)
 {
  if(data && data.pSelectedAction)
  {
   GEP_DriverViewAction view=GEP_DriverViewAction.Cast(data.pSelectedAction);
   if(view && view.ViewMode==0)
   {
    GEP_DriverStation station=view.Station();
    if(station && station.IsDriver(data.pUser) && station.GetViewMode(data.pUser)!=0)
    {
     super.SetDisplayData(null);
     Show(false,UIConstants.FADE_RATE_INSTANT);
     return;
    }
   }
  }
  super.SetDisplayData(data);
 }
}

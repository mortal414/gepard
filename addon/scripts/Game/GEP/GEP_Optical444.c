 // Original optional game-assisted optical follow. No radar queries, prediction,
 // range readout, ballistic lead, automatic trigger or cinematic aim setters.
 class GEP_Optical444Class : ScriptComponentClass {
 }
 class GEP_Optical444 : ScriptComponent
 {
  int State;
  // 0 manual, 1 acquiring, 2 tracking
  string Feedback="MANUAL";
  bool Night;
  bool Bound;
  int NightCamera=-1;
  float Clock;
  float AcquireStart;
  float LastValidated;
  float MessageUntil;
  IEntity Target;
  vector TargetLocal;
  vector Point;
  int GuidanceFrames;
  ref array<IEntity> Exclusions={
  }
  ;
  const float MAX_RANGE=3000;
  const int NIGHT_PRIORITY=17;
  IEntity User()
  {
   PlayerController player=GetGame().GetPlayerController();
   if(!player)return null;
   return player.GetControlledEntity();
  }
  bool Alive(IEntity entity)
  {
   if(!entity||entity.IsDeleted())return false;
   CharacterControllerComponent life=CharacterControllerComponent.Cast(entity.FindComponent(CharacterControllerComponent));
   if(life&&life.GetLifeState()!=ECharacterLifeState.ALIVE)return false;
   SCR_DamageManagerComponent damage=SCR_DamageManagerComponent.Cast(entity.FindComponent(SCR_DamageManagerComponent));
   return !damage||damage.GetState()!=EDamageState.DESTROYED;
  }
  bool Crew(IEntity user)
  {
   string reason679;return Crew679(user,reason679);
  }
  bool Crew679(IEntity user,out string reason679)
  {
   reason679="opticalUserMissing";if(!user)return false;
   reason679="opticalUserMismatch";if(user!=User())return false;
   reason679="opticalUserNotAlive";if(!Alive(user))return false;
   reason679="opticalOwnerNotAlive";if(!Alive(GetOwner()))return false;
   reason679="opticalRootNotAlive";if(!Alive(GetOwner().GetRootParent()))return false;
   CompartmentAccessComponent access=CompartmentAccessComponent.Cast(user.FindComponent(CompartmentAccessComponent));
   reason679="opticalAccessMissing";if(!access)return false;
   reason679="opticalGettingIn";if(access.IsGettingIn())return false;
   reason679="opticalGettingOut";if(access.IsGettingOut())return false;
   reason679="opticalSwitchingSeat";if(access.IsSwitchingSeatsAnim())return false;
   BaseCompartmentSlot seat=access.GetCompartment();
   reason679="opticalSeatMissing";if(!seat)return false;
   TurretControllerComponent control=TurretControllerComponent.Cast(seat.GetController());
   if(!control)control=seat.GetAttachedTurret();
   reason679="opticalControllerMissing";if(!control)return false;
   reason679="opticalControllerOwnerMismatch";if(control.GetOwner()!=GetOwner())return false;
   reason679="ready";return true;
  }
  bool Active()
  {
   if(!Crew(User())||GetGame().GetMenuManager().IsAnyMenuOpen())return false;
   TurretControllerComponent gun=TurretControllerComponent.Cast(GetOwner().FindComponent(TurretControllerComponent));
   BaseSightsComponent sight;
   if(gun)sight=gun.GetCurrentSights();
   return gun&&gun.GetCurrentSightsADS()&&sight&&sight.IsSightADSActive();
  }
  bool View(out vector pose[4],out float fov)
  {
   TurretControllerComponent gun=TurretControllerComponent.Cast(GetOwner().FindComponent(TurretControllerComponent));
   return gun&&gun.GetCurrentSightsCameraTransform(pose,fov);
  }
  void ExcludeTree(IEntity entity)
  {
   if(!entity)return;
   Exclusions.Insert(entity);
   for(IEntity child=entity.GetChildren();child;child=child.GetSibling())ExcludeTree(child);
  }
  TraceParam Ray(vector start,vector end)
  {
   TraceParam trace=new TraceParam();
   trace.Start=start;
   trace.End=end;
   trace.Flags=TraceFlags.WORLD|TraceFlags.ENTS;
   Exclusions.Clear();
   ExcludeTree(GetOwner().GetRootParent());
   if(User())Exclusions.Insert(User());
   trace.ExcludeArray=Exclusions;
   GetGame().GetWorld().TraceMove(trace,null);
   return trace;
  }
  void Clear(string reason="MANUAL")
  {
   State=0;
   Target=null;
   Point=vector.Zero;
   TargetLocal=vector.Zero;
   LastValidated=-1;
   Feedback=reason;
   MessageUntil=Clock+3;
  }
  bool ToggleTrack(IEntity user)
  {
   if(!Crew(user)||!Active())return false;
   if(State!=0){
    Clear("MANUAL - CANCELLED");
    return true;
   }
   GEP_TurretController410 drive=GEP_TurretController410.Cast(GetOwner().FindComponent(TurretControllerComponent));
   if(drive&&drive.IsDriveFailed()){
    Clear("MANUAL - DRIVE FAULT");
    return false;
   }
   vector pose[4];
   float fov;
   if(!View(pose,fov)){
    Clear("MANUAL - SIGHT UNAVAILABLE");
    return false;
   }
   TraceParam trace=Ray(pose[3],pose[3]+pose[2].Normalized()*MAX_RANGE);
   IEntity hit=trace.TraceEnt;
   if(!hit){
    Clear("MANUAL - NO SIGHTED TARGET");
    return false;
   }
   IEntity root=hit.GetRootParent();
   if(!Alive(root)||(!Vehicle.Cast(root)&&!ChimeraCharacter.Cast(root))){
    Clear("MANUAL - AIM AT VEHICLE OR PERSON");
    return false;
   }
   // Repeat the same native ray for its exact fraction; never use entity centre as acquisition.
   float fraction=GetGame().GetWorld().TraceMove(trace,null);
   Point=trace.Start+(trace.End-trace.Start)*fraction;
   Target=root;
   TargetLocal=root.CoordToLocal(Point);
   AcquireStart=Clock;
   State=1;
   Feedback="ACQUIRING";
   if(!Validate()){
    return false;
   }
   return true;
  }
  bool Validate()
  {
   if(!Alive(Target)){
    Clear("LOST - TARGET REMOVED / MANUAL");
    return false;
   }
   vector pose[4];
   float fov;
   if(!View(pose,fov)){
    Clear("LOST - SIGHT / MANUAL");
    return false;
   }
   Point=Target.CoordToParent(TargetLocal);
   vector delta=Point-pose[3];
   float range=delta.Length();
   if(range<3||range>MAX_RANGE){
    Clear("LOST - RANGE / MANUAL");
    return false;
   }
   vector direction=delta.Normalized();
   if(vector.Dot(direction,pose[2].Normalized())<Math.Cos(fov*0.48*Math.DEG2RAD)){
    Clear("LOST - OUT OF SIGHT / MANUAL");
    return false;
   }
   TurretControllerComponent gun=TurretControllerComponent.Cast(GetOwner().FindComponent(TurretControllerComponent));
   if(!gun||!gun.GetTurretComponent()||gun.GetTurretComponent().GetAimingAngleExcess(Point).Length()>0.1){
    Clear("LOST - AIM LIMIT / MANUAL");
    return false;
   }
   TraceParam trace=Ray(pose[3],Point+direction*0.3);
   if(!trace.TraceEnt||trace.TraceEnt.GetRootParent()!=Target){
    Clear("LOST - OCCLUDED / MANUAL");
    return false;
   }
   LastValidated=Clock;
   return true;
  }
  bool ToggleNight(IEntity user)
  {
   if(!Crew(user)||!Active())return false;
   Night=!Night;
   if(!Night)ReleaseNight();
   return true;
  }
  float BaseEV()
  {
   IEntity mode=GetGame().GetGameMode();
   if(!mode)return 0;
   SCR_NightModeGameModeComponent nightMode=SCR_NightModeGameModeComponent.Cast(mode.FindComponent(SCR_NightModeGameModeComponent));
   if(nightMode)return nightMode.GEP_OpticalBaseEV444();
   return 0;
  }
  void ReleaseNight()
  {
   if(NightCamera>=0&&GetGame()&&GetGame().GetWorld())
   {
    GetGame().GetWorld().SetCameraPostProcessEffect(NightCamera,NIGHT_PRIORITY,PostProcessEffectType.None,"");
    GetGame().GetWorld().AdjustCameraEV(NightCamera,BaseEV());
   }
   NightCamera=-1;
   Night=false;
  }
  void NightFrame()
  {
   if(!Night)return;
   CameraBase camera=GetGame().GetCameraManager().CurrentCamera();
   if(!camera){
    ReleaseNight();
    return;
   }
   int index=camera.GetCameraIndex();
   if(NightCamera!=index)
   {
    bool enabled=Night;
    ReleaseNight();
    Night=enabled;
    NightCamera=index;
    GetGame().GetWorld().SetCameraPostProcessEffect(index,NIGHT_PRIORITY,PostProcessEffectType.Colors,"{D561AD08932E8444}Assets/Gepard/Optics/GEP_Night444.emat");
   }
   // Camera-local monochrome light amplification, a labelled game approximation.
   // No lights, thermal imagery or global time/weather changes.
   GetGame().GetWorld().AdjustCameraEV(index,BaseEV()+5);
  }
  void OnNight(float value,EActionTrigger reason){
   ToggleNight(User());
  }
  void OnTrack(float value,EActionTrigger reason){
   ToggleTrack(User());
  }
  void OnCancel(float value,EActionTrigger reason){
   if(Active())Clear("MANUAL - CANCELLED");
  }
  void Bind(bool wanted)
  {
   if(Bound==wanted)return;
   InputManager input=GetGame().GetInputManager();
   if(!input)return;
   if(wanted){
    input.AddActionListener("GEPOpticalNight",EActionTrigger.DOWN,OnNight);
    input.AddActionListener("GEPOpticalTrack",EActionTrigger.DOWN,OnTrack);
    input.AddActionListener("GEPOpticalCancel",EActionTrigger.DOWN,OnCancel);
   }
   else {
    input.RemoveActionListener("GEPOpticalNight",EActionTrigger.DOWN,OnNight);
    input.RemoveActionListener("GEPOpticalTrack",EActionTrigger.DOWN,OnTrack);
    input.RemoveActionListener("GEPOpticalCancel",EActionTrigger.DOWN,OnCancel);
   }
   Bound=wanted;
  }
  void PrepareControls(ActionManager am,float dt,bool player)
  {
   if(!player||!am||State!=2||!Active()||Clock-LastValidated>0.12)return;
   // Real manual aim immediately takes precedence over optional assistance.
   if(Math.AbsFloat(am.GetActionValue("TurretAimLeft"))+Math.AbsFloat(am.GetActionValue("TurretAimRight"))+Math.AbsFloat(am.GetActionValue("TurretAimUp"))+Math.AbsFloat(am.GetActionValue("TurretAimDown"))+Math.AbsFloat(am.GetActionValue("TurretAimGamepadLeft"))+Math.AbsFloat(am.GetActionValue("TurretAimGamepadRight"))+Math.AbsFloat(am.GetActionValue("TurretAimGamepadUp"))+Math.AbsFloat(am.GetActionValue("TurretAimGamepadDown"))>0.001){
    Clear("MANUAL - AIM OVERRIDE");
    return;
   }
   vector pose[4];
   float fov;
   if(!View(pose,fov))return;
   vector delta=(Point-pose[3]).Normalized();
   float forward=vector.Dot(delta,pose[2]);
   float yaw=Math.Atan2(vector.Dot(delta,pose[0]),forward)*Math.RAD2DEG;
   float pitch=Math.Atan2(vector.Dot(delta,pose[1]),forward)*Math.RAD2DEG;
   TurretControllerComponent controller=TurretControllerComponent.Cast(GetOwner().FindComponent(TurretControllerComponent));
   SCR_2DOpticsComponent sight=SCR_2DOpticsComponent.Cast(controller.GetCurrentSights());
   float zoomScale=1;
   if(sight)zoomScale=Math.Max(1,sight.GetMagnification()/2);
   float x=Math.Clamp(yaw*dt*60*zoomScale,-2*zoomScale,2*zoomScale);
   float y=Math.Clamp(pitch*dt*60*zoomScale,-2*zoomScale,2*zoomScale);
   if(x>0)am.SetActionValue("TurretAimRight",x);
   else am.SetActionValue("TurretAimLeft",-x);
   if(y>0)am.SetActionValue("TurretAimUp",y);
   else am.SetActionValue("TurretAimDown",-y);
   GuidanceFrames++;
  }
  override void OnPostInit(IEntity owner){
   SetEventMask(owner,EntityEvent.FRAME|EntityEvent.POSTFRAME);
  }
  override void EOnFrame(IEntity owner,float timeSlice)
  {
   if(!GetGame().InPlayMode())return;
   Clock+=timeSlice;
   bool active=Active();
   Bind(active);
   if(!active){
    if(State!=0)Clear();
    if(Night||NightCamera>=0)ReleaseNight();
    return;
   }
   GetGame().GetInputManager().ActivateContext("GEPOpticalContext");
   GEP_TurretController410 drive=GEP_TurretController410.Cast(owner.FindComponent(TurretControllerComponent));
   if(drive&&drive.IsDriveFailed()&&State!=0)Clear("LOST - DRIVE FAULT / MANUAL");
   if(State!=0&&Validate()&&State==1&&Clock-AcquireStart>=0.35){
    State=2;
    Feedback="TRACK - OPTICAL ASSIST";
   }
   if(State==0&&Clock>MessageUntil)Feedback="MANUAL";
  }
  override void EOnPostFrame(IEntity owner,float timeSlice){
   if(Active())NightFrame();
  }
  override void OnDelete(IEntity owner){
   Bind(false);
   ReleaseNight();
   Clear();
  }
 }
 // Read native scenario exposure baseline; do not mutate global night mode.
 modded class SCR_NightModeGameModeComponent
 {
  float GEP_OpticalBaseEV444(){
   if(IsNightModeEnabled())return Math.Lerp(m_iEVValueNormalMode,m_iEVValueNightMode,m_fNightModeAlpha);
   return m_iEVValueNormalMode;
  }
 }
 class GEP_OpticalAction444 : ScriptedUserAction
 {
  [Attribute("0",UIWidgets.EditBox,"0 night, 1 acquire/cancel, 2 manual")]
  int Command;
  GEP_Optical444 Optic(){
   return GEP_Optical444.Cast(GetOwner().FindComponent(GEP_Optical444));
  }
  override bool HasLocalEffectOnlyScript(){
   return true;
  }
  override bool CanBeShownScript(IEntity user){
   GEP_Optical444 optic=Optic();
   return optic&&optic.Crew(user);
  }
  override bool CanBePerformedScript(IEntity user){
   GEP_Optical444 optic=Optic();
   if(!optic||!optic.Crew(user))return false;
   if(!optic.Active()){
    SetCannotPerformReason("Enter the gun sight first");
    return false;
   }
   return true;
  }
  override void PerformAction(IEntity pOwnerEntity,IEntity pUserEntity){
   if(!CanBePerformedScript(pUserEntity))return;
   GEP_Optical444 optic=Optic();
   if(Command==0)optic.ToggleNight(pUserEntity);
   if(Command==1)optic.ToggleTrack(pUserEntity);
   if(Command==2)optic.Clear("MANUAL - CANCELLED");
  }
  override bool GetActionNameScript(out string outName){
   GEP_Optical444 optic=Optic();
   if(Command==0){
    outName="Enable night optic";
    if(optic&&optic.Night)outName="Disable night optic";
   }
   if(Command==1){
    outName="Acquire target in sight";
    if(optic&&optic.State!=0)outName="Cancel optical track";
   }
   if(Command==2)outName="Return to manual aim";
   return true;
  }
 }

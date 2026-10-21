// QA diagnostic only. Native settled occupancy and public action callback, not physical input.
// No stock throw, salvo, rearm, driver input, pose/velocity/effect/fuse setter or process close.
class GEP_OneRoundProbe015Class : GenericEntityClass {}
class GEP_OneRoundProbe015 : GenericEntity
{
 static GEP_OneRoundProbe015 Active;
 IEntity Hull;IEntity Actor;IEntity Round;IEntity ProbeTurret;
 BaseCompartmentSlot Seat;GEP_StockSmoke015 Smoke;CameraBase Camera;
 float StartMs=-1;float Age;float DispatchMs=-1;float LastSample=-1;
 bool Staged;bool Bound;bool BoardRequested;bool Dispatched;bool RenderSetup;
 int BeforeCount=-1;int AfterCount=-1;int SampleCount;int FrameIndex;
 vector LaunchOrigin;vector FirstPosition;vector LatestPosition;
 float MaxDisplacement;bool Triggered;float FirstObservedMs=-1;
 string Reason="not_started";
 int m_iTerminalLatch015;int m_iTrace015;int m_iReadbackScheduled015;
 bool m_bActionCallback015;string m_sLifecycleCase015="launcher";
 int m_iValidPositionSamples015;vector m_vPreviousValidPosition015;float m_fValidPositionTravel015;
 void GEP_OneRoundProbe015(IEntitySource src,IEntity parent)
 {Active=this;SetEventMask(EntityEvent.FRAME);SetFlags(EntityFlags.ACTIVE,true);}
 IEntity Spawn(ResourceName prefab,vector position,float clearance)
 {
  Resource resource=Resource.Load(prefab);if(!resource || !resource.IsValid())return null;
  EntitySpawnParams params=new EntitySpawnParams();params.TransformMode=ETransformMode.WORLD;
  position[1]=GetGame().GetWorld().GetSurfaceY(position[0],position[2])+clearance;
  params.Transform[3]=position;return GetGame().SpawnEntityPrefab(resource,GetGame().GetWorld(),params);
 }
 BaseCompartmentSlot FindSeat(IEntity entity,int depth=0)
 {
  if(!entity || depth>16)return null;
  BaseCompartmentManagerComponent manager=BaseCompartmentManagerComponent.Cast(entity.FindComponent(BaseCompartmentManagerComponent));
  if(manager){array<BaseCompartmentSlot> seats={};manager.GetCompartments(seats);foreach(BaseCompartmentSlot slot:seats){if(TurretCompartmentSlot.Cast(slot))return slot;}}
  for(IEntity child=entity.GetChildren();child;child=child.GetSibling()){BaseCompartmentSlot found=FindSeat(child,depth+1);if(found)return found;}
  return null;
 }
 bool Settled()
 {
  if(!Actor || !Seat || !Smoke || !Hull)return false;
  SCR_CharacterControllerComponent life=SCR_CharacterControllerComponent.Cast(Actor.FindComponent(CharacterControllerComponent));
  CompartmentAccessComponent access=CompartmentAccessComponent.Cast(Actor.FindComponent(CompartmentAccessComponent));
  Physics physics=Hull.GetPhysics();
  return life && life.GetLifeState()==ECharacterLifeState.ALIVE && !life.IsUnconscious() && access && access.GetCompartment()==Seat && Seat.GetOccupant()==Actor && !access.IsGettingIn() && !access.IsGettingOut() && !access.IsSwitchingSeatsAnim() && physics && physics.GetVelocity().Length()<=0.25 && physics.GetAngularVelocity().Length()<=0.05;
 }
 void View(float timeSlice)
 {
  if(!Hull)return;
  if(!Camera)Camera=CameraBase.Cast(GetGame().SpawnEntity(CameraBase,GetGame().GetWorld()));
  if(!Camera)return;
  vector eye=Hull.CoordToParent(Vector(17,15,36));vector aim=Hull.CoordToParent(Vector(0,1,11));vector pose[4];
  Math3D.DirectionAndUpMatrix((aim-eye).Normalized(),vector.Up,pose);pose[3]=eye;
  Camera.SetWorldTransform(pose);Camera.SetFOVDegree(70);GetGame().GetCameraManager().SetCamera(Camera);Camera.ApplyTransform(timeSlice);
 }
 void Save(string phase)
 {
  JsonSaveContext result=new JsonSaveContext();
  result.WriteValue("scope","DIAGNOSTIC_NATIVE_LIFECYCLE");result.WriteValue("case",m_sLifecycleCase015);result.WriteValue("phase",phase);result.WriteValue("reason",Reason);
  result.WriteValue("ageSeconds",Age);result.WriteValue("worldTimeMs",GetWorld().GetWorldTime());
  result.WriteValue("scriptedBoardingRequested",BoardRequested);result.WriteValue("settledCrew",Settled());
  result.WriteValue("actionCallbackIssued",m_bActionCallback015);result.WriteValue("ammoBefore",BeforeCount);result.WriteValue("ammoAfter",AfterCount);
  result.WriteValue("samples",SampleCount);result.WriteValue("firstObservedWorldMs",FirstObservedMs);
  result.WriteValue("dispatchWorldMs",DispatchMs);result.WriteValue("muzzleWorld",LaunchOrigin);
  result.WriteValue("latestRoundWorld",LatestPosition);result.WriteValue("maxDistanceFromRequestedMuzzleDiagnostic",MaxDisplacement);result.WriteValue("validPositionSamples",m_iValidPositionSamples015);result.WriteValue("observedValidPositionTravel",m_fValidPositionTravel015);
  result.WriteValue("triggeredObserved",Triggered);result.WriteValue("screenshotsRequested",FrameIndex);
  result.WriteValue("ordinaryStockThrowControl","NOT_TESTED");result.WriteValue("visibleCloud","NEEDS_PIXEL_REVIEW");
  result.WriteValue("concealmentDurationRearmMP","NOT_TESTED");result.WriteValue("physicalInput","NOT_TESTED");
  result.SaveToFile("$profile:GEP015-auto-result.json");
 }
 void Stop(string reason)
 {
  if(m_iTerminalLatch015!=0)return;
  m_iTerminalLatch015=1;Reason=reason;
  ClearEventMask(EntityEvent.FRAME);
  GetGame().GetCallqueue().Remove(DelayedSample015);
  if(Active==this)Active=null;
  Save("terminal");
  PrintFormat("GEP015_LIFE_TERMINAL scene=%1 reason=%2 callback=%3 traces=%4 frameMaskCleared=1 state=%5",GetID(),reason,m_bActionCallback015,m_iTrace015,m_iTerminalLatch015);
 }
 string Identity015(IEntity entity)
 {
  if(!entity)return "null";
  if(entity.IsDeleted())return string.Format("%1:deleted",entity.GetID());
  return string.Format("%1:%2:%3",entity.GetID(),entity.Type().ToString(),entity.GetOrigin());
 }
 void ScheduleReadbacks015(IEntity round,int tube)
 {
  if(m_iReadbackScheduled015!=0)return;m_iReadbackScheduled015=1;
  GetGame().GetCallqueue().CallLater(DelayedSample015,0,false,"queued0ms",round,tube);
  GetGame().GetCallqueue().CallLater(DelayedSample015,33,false,"queued33ms",round,tube);
  GetGame().GetCallqueue().CallLater(DelayedSample015,100,false,"queued100ms",round,tube);
  GetGame().GetCallqueue().CallLater(DelayedSample015,250,false,"queued250ms",round,tube);
  GetGame().GetCallqueue().CallLater(DelayedSample015,500,false,"queued500ms",round,tube);
  GetGame().GetCallqueue().CallLater(DelayedSample015,1000,false,"queued1000ms",round,tube);
  GetGame().GetCallqueue().CallLater(DelayedSample015,1500,false,"queued1500ms",round,tube);
  GetGame().GetCallqueue().CallLater(DelayedSample015,2000,false,"queued2000ms",round,tube);
  GetGame().GetCallqueue().CallLater(DelayedSample015,3000,false,"queued3000ms",round,tube);
 }
 void DelayedSample015(string phase,IEntity round,int tube)
 {if(m_iTerminalLatch015==0)ObserveLifecycle015(phase,round,tube);}
 void ObserveRound(IEntity round,int tube)
 {ObserveLifecycle015("original5sCallback",round,tube);}
 void Sample()
 {ObserveLifecycle015("frameReadback",Round,0);}
 void ObserveLifecycle015(string phase,IEntity round,int tube)
 {
  if(m_iTerminalLatch015!=0 || tube!=0 || m_iTrace015>=80)return;
  m_iTrace015++;float now=GetWorld().GetWorldTime();LastSample=now;
  JsonSaveContext record=new JsonSaveContext();
  record.WriteValue("case",m_sLifecycleCase015);record.WriteValue("phase",phase);record.WriteValue("sequence",m_iTrace015);
  record.WriteValue("scene",Identity015(this));record.WriteValue("worldTimeMs",now);record.WriteValue("elapsedSinceDispatchSeconds",(now-DispatchMs)/1000.0);
  record.WriteValue("requestedMuzzle",LaunchOrigin);record.WriteValue("entityPresent",round!=null);
  bool enabled=Smoke && Smoke.NativeSimulationRequested015();if(m_sLifecycleCase015=="stock_helper_drop_no_launch")enabled=true;record.WriteValue("enableSimulationRequested",enabled);
  if(round)
  {
   bool deleted=round.IsDeleted();record.WriteValue("entity",Identity015(round));record.WriteValue("deleted",deleted);
   if(!deleted)
   {
    if(Round && Round!=round){Stop("unexpected_multiple_rounds");return;}Round=round;
    vector matrix[4];round.GetWorldTransform(matrix);LatestPosition=round.GetOrigin();
    float distance=vector.Distance(LaunchOrigin,LatestPosition);
    if(distance<200){if(m_iValidPositionSamples015>0)m_fValidPositionTravel015+=vector.Distance(m_vPreviousValidPosition015,LatestPosition);m_iValidPositionSamples015++;m_vPreviousValidPosition015=LatestPosition;MaxDisplacement=Math.Max(MaxDisplacement,distance);}
    if(FirstObservedMs<0){FirstObservedMs=now;FirstPosition=LatestPosition;}
    record.WriteValue("origin",LatestPosition);record.WriteValue("worldMatrixTranslation",matrix[3]);
    record.WriteValue("distanceFromRequestedMuzzle",distance);record.WriteValue("nearRequestedMuzzle",distance<200);
    record.WriteValue("parent",Identity015(round.GetParent()));record.WriteValue("rootParent",Identity015(round.GetRootParent()));
    record.WriteValue("flags",round.GetFlags());record.WriteValue("eventMask",round.GetEventMask());record.WriteValue("contactHistory","NOT_INSTRUMENTED");record.WriteValue("sameWorld",round.GetWorld()==GetWorld());
    string prefab="";if(round.GetPrefabData())prefab=round.GetPrefabData().GetPrefabName();record.WriteValue("prefab",prefab);
    GrenadeMoveComponent move=GrenadeMoveComponent.Cast(round.FindComponent(GrenadeMoveComponent));
    TimerTriggerComponent fuse=TimerTriggerComponent.Cast(round.FindComponent(TimerTriggerComponent));
    RplComponent rpl=RplComponent.Cast(round.FindComponent(RplComponent));Physics physics=round.GetPhysics();
    record.WriteValue("movePresent",move!=null);record.WriteValue("fusePresent",fuse!=null);record.WriteValue("physicsPresent",physics!=null);record.WriteValue("rplPresent",rpl!=null);
    if(move){record.WriteValue("moveLookupEntity",Identity015(round));record.WriteValue("moveParentProjectile",Identity015(move.GetParentProjectile()));record.WriteValue("projectileVelocity",move.GetVelocity());}
    if(fuse){Triggered=Triggered || fuse.WasTriggered();record.WriteValue("fuseOwner",Identity015(fuse.GetOwner()));record.WriteValue("fuseParentProjectile",Identity015(fuse.GetParentProjectile()));record.WriteValue("timerLocal",fuse.GetTimer());record.WriteValue("triggered",fuse.WasTriggered());}
    if(rpl)record.WriteValue("isProxy",rpl.IsProxy());
    if(physics){record.WriteValue("physicsActive",physics.IsActive());record.WriteValue("physicsDynamic",physics.IsDynamic());record.WriteValue("simulationState",physics.GetSimulationState());record.WriteValue("rigidBodyVelocity",physics.GetVelocity());}
    PrintFormat("GEP015_LIFE_ENTITY seq=%1 phase=%2 elapsed=%3 entity=%4 worldPosition=%5 parent=%6 physics=%7",m_iTrace015,phase,(now-DispatchMs)/1000.0,Identity015(round),matrix[3],Identity015(round.GetParent()),physics!=null);
    if(move && fuse)PrintFormat("GEP015_LIFE_COMPONENT seq=%1 moveLookupEntity=%2 projectile=%3 velocity=%4 fuse=%5 timer=%6 triggered=%7",m_iTrace015,Identity015(round),Identity015(move.GetParentProjectile()),move.GetVelocity(),Identity015(fuse.GetOwner()),fuse.GetTimer(),fuse.WasTriggered());
   }
  }
  SampleCount=m_iTrace015;record.SaveToFile(string.Format("$profile:GEP015-life-%1.json",m_iTrace015));
  if(phase=="post_live")ScheduleReadbacks015(round,tube);
 }
 override void EOnFrame(IEntity owner,float timeSlice)
 {
  if(!GetGame().InPlayMode() || System.IsConsoleApp() || RplSession.Mode()!=RplMode.None)return;
  float now=GetWorld().GetWorldTime();if(StartMs<0)StartMs=now;Age=(now-StartMs)/1000.0;
  if(m_iTerminalLatch015!=0)return;
  if(!Staged && Age>1)
  {
   Staged=true;Hull=Spawn("{B3495C8E7A0D2001}Prefabs/Vehicles/GEP_MobileCarrier_StockSmoke015.et","600 0 2600",0.7);
   Actor=Spawn("{26A9756790131354}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Rifleman.et","605 0 2595",0.1);
   if(!Hull || !Actor){Stop("spawn_failed");return;}
  }
  PlayerController player=GetGame().GetPlayerController();
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  SCR_RespawnSystemComponent respawn=SCR_RespawnSystemComponent.GetInstance();
  if(Staged && !Bound && Age>3 && player && mode && mode.IsRunning() && respawn)
  {
   player.SetControlledEntity(Actor);respawn.DestroyLoadingPlaceholder();Bound=true;
   player.SetCharacterCameraRenderActive(false);SCR_HintManagerComponent.HideHint();RenderSetup=true;
   Print("GEP015_AUTO_BOUND scriptedFixture=1 physicalInput=NOT_TESTED ordinaryStockThrow=NOT_TESTED");
  }
  if(Bound && !BoardRequested && Age>5)
  {
   Seat=FindSeat(Hull);if(!Seat){Stop("gunner_seat_missing");return;}
   ProbeTurret=Seat.GetOwner();Smoke=GEP_StockSmoke015.Cast(ProbeTurret.FindComponent(GEP_StockSmoke015));
   CompartmentAccessComponent access=CompartmentAccessComponent.Cast(Actor.FindComponent(CompartmentAccessComponent));
   BoardRequested=true;
   if(!Smoke || !access || !access.GetInVehicle(Seat.GetOwner(),Seat,true,-1,ECloseDoorAfterActions.INVALID,true)){Stop("native_boarding_rejected");return;}
  }
  View(timeSlice);
  if(BoardRequested && !Dispatched && Age>10 && Settled())
  {
   BeforeCount=Smoke.Count();bool authority=Smoke.Authority();bool operational=Smoke.Operational();bool crew=Smoke.Crew(Actor);bool canFire=Smoke.CanFire(Actor);
   PrintFormat("GEP015_AUTO_PREFLIGHT authority=%1 operational=%2 crew=%3 canFire=%4 count=%5",authority,operational,crew,canFire,BeforeCount);
   if(!authority || !operational || !crew || !canFire || BeforeCount!=8){Stop("smoke_preflight_failed");return;}
   BaseActionsManagerComponent actions=BaseActionsManagerComponent.Cast(ProbeTurret.FindComponent(BaseActionsManagerComponent));
   if(!actions){Stop("actions_manager_missing");return;}
   array<BaseUserAction> choices={};actions.GetActionsList(choices);GEP_StockSmokeFire015 selected;
   foreach(BaseUserAction action:choices){GEP_StockSmokeFire015 fire=GEP_StockSmokeFire015.Cast(action);if(fire && fire.m_bSingleRound){selected=fire;break;}}
   if(!selected || !selected.CanBeShown(Actor) || !selected.CanBePerformed(Actor)){Stop("single_round_action_ineligible");return;}
   vector muzzle[4];if(!Smoke.Muzzle(0,muzzle)){Stop("muzzle_missing");return;}LaunchOrigin=muzzle[3];
   Dispatched=true;m_bActionCallback015=true;DispatchMs=now;Reason="one_callback_issued";
   selected.OnActionStart(Actor);selected.PerformAction(ProbeTurret,Actor);AfterCount=Smoke.Count();
   PrintFormat("GEP015_AUTO_CALLBACK countBefore=%1 countAfter=%2 callback=public_action nativeFlight=NOT_YET_MEASURED",BeforeCount,AfterCount);
   Save("callback_issued");if(AfterCount!=7){Stop("one_round_count_failed_no_retry");return;}
  }
  if(Dispatched)
  {
   float elapsed=(now-DispatchMs)/1000.0;
   if(Round && now-LastSample>=500)Sample();
   float nextFrame=0.25;if(FrameIndex==1)nextFrame=1;if(FrameIndex==2)nextFrame=3;if(FrameIndex==3)nextFrame=5;if(FrameIndex==4)nextFrame=8;if(FrameIndex==5)nextFrame=12;
   if(FrameIndex<6 && elapsed>=nextFrame){FrameIndex++;bool requested=System.MakeScreenshot(string.Format("$profile:GEP015-auto-%1",FrameIndex));PrintFormat("GEP015_AUTO_FRAME index=%1 elapsed=%2 requested=%3",FrameIndex,elapsed,requested);}
   if(elapsed>=13){Stop("diagnostic_complete_acceptance_pending");return;}
  }
  if(Age>=35)Stop("preflight_timeout_no_retry");
 }
}
// Read-only observation hook: retains original dispatch, launch, fuse and inspection behavior.
modded class GEP_StockSmoke015
{
 override protected void TraceLifecycle015(string phase, IEntity round, int tube)
 {super.TraceLifecycle015(phase,round,tube);if(GEP_OneRoundProbe015.Active)GEP_OneRoundProbe015.Active.ObserveLifecycle015(phase,round,tube);}
 override protected void InspectRound(IEntity round,int tube)
 {super.InspectRound(round,tube);if(GEP_OneRoundProbe015.Active)GEP_OneRoundProbe015.Active.ObserveRound(round,tube);}
}

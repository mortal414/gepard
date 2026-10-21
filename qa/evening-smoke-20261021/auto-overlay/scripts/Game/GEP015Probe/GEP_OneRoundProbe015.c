// QA diagnostic only. Native settled occupancy and public action callback, not physical input.
// No stock throw, salvo, rearm, driver input, pose/velocity/effect/fuse setter or process close.
class GEP_OneRoundProbe015Class : GenericEntityClass {}
class GEP_OneRoundProbe015 : GenericEntity
{
 static GEP_OneRoundProbe015 Active;
 IEntity Hull;IEntity Actor;IEntity Round;IEntity Turret;
 BaseCompartmentSlot Seat;GEP_StockSmoke015 Smoke;CameraBase Camera;
 float StartMs=-1;float Age;float DispatchMs=-1;float LastSample=-1;
 bool Staged;bool Bound;bool BoardRequested;bool Dispatched;bool Finished;bool RenderSetup;
 int BeforeCount=-1;int AfterCount=-1;int SampleCount;int FrameIndex;
 vector LaunchOrigin;vector FirstPosition;vector LatestPosition;
 float MaxDisplacement;bool Triggered;float FirstObservedMs=-1;
 string Reason="not_started";
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
 void View(float dt)
 {
  if(!Hull)return;
  if(!Camera)Camera=CameraBase.Cast(GetGame().SpawnEntity(CameraBase,GetGame().GetWorld()));
  if(!Camera)return;
  vector eye=Hull.CoordToParent(Vector(17,15,36));vector aim=Hull.CoordToParent(Vector(0,1,11));vector pose[4];
  Math3D.DirectionAndUpMatrix((aim-eye).Normalized(),vector.Up,pose);pose[3]=eye;
  Camera.SetWorldTransform(pose);Camera.SetFOVDegree(70);GetGame().GetCameraManager().SetCamera(Camera);Camera.ApplyTransform(dt);
 }
 void Save(string phase)
 {
  JsonSaveContext result=new JsonSaveContext();
  result.WriteValue("scope","DIAGNOSTIC_ONE_ROUND_PUBLIC_ACTION_CALLBACK");result.WriteValue("phase",phase);result.WriteValue("reason",Reason);
  result.WriteValue("ageSeconds",Age);result.WriteValue("worldTimeMs",GetWorld().GetWorldTime());
  result.WriteValue("scriptedBoardingRequested",BoardRequested);result.WriteValue("settledCrew",Settled());
  result.WriteValue("actionCallbackIssued",Dispatched);result.WriteValue("ammoBefore",BeforeCount);result.WriteValue("ammoAfter",AfterCount);
  result.WriteValue("samples",SampleCount);result.WriteValue("firstObservedWorldMs",FirstObservedMs);
  result.WriteValue("dispatchWorldMs",DispatchMs);result.WriteValue("muzzleWorld",LaunchOrigin);
  result.WriteValue("latestRoundWorld",LatestPosition);result.WriteValue("maxDistanceFromMuzzle",MaxDisplacement);
  result.WriteValue("triggeredObserved",Triggered);result.WriteValue("screenshotsRequested",FrameIndex);
  result.WriteValue("ordinaryStockThrowControl","NOT_TESTED");result.WriteValue("visibleCloud","NEEDS_PIXEL_REVIEW");
  result.WriteValue("concealmentDurationRearmMP","NOT_TESTED");result.WriteValue("physicalInput","NOT_TESTED");
  result.SaveToFile("$profile:GEP015-auto-result.json");
 }
 void Stop(string reason)
 {if(Finished)return;Finished=true;Reason=reason;Save("terminal");PrintFormat("GEP015_AUTO_TERMINAL reason=%1 callback=%2 countBefore=%3 countAfter=%4 samples=%5 displacement=%6 triggered=%7 stockThrow=NOT_TESTED visibleCloud=NEEDS_PIXEL_REVIEW",reason,Dispatched,BeforeCount,AfterCount,SampleCount,MaxDisplacement,Triggered);}
 void ObserveRound(IEntity round,int tube)
 {
  if(!Dispatched || tube!=0 || !round || Finished)return;
  if(Round && Round!=round){Stop("unexpected_multiple_rounds");return;}
  Round=round;Sample();
 }
 void Sample()
 {
  if(!Round || Round.IsDeleted())return;
  GrenadeMoveComponent move=GrenadeMoveComponent.Cast(Round.FindComponent(GrenadeMoveComponent));
  TimerTriggerComponent fuse=TimerTriggerComponent.Cast(Round.FindComponent(TimerTriggerComponent));
  RplComponent rpl=RplComponent.Cast(Round.FindComponent(RplComponent));
  if(!move || !fuse || !rpl){Stop("round_components_missing");return;}
  float now=GetWorld().GetWorldTime();LatestPosition=Round.GetOrigin();
  float distance=vector.Distance(LaunchOrigin,LatestPosition);MaxDisplacement=Math.Max(MaxDisplacement,distance);
  if(FirstObservedMs<0){FirstObservedMs=now;FirstPosition=LatestPosition;}
  Triggered=Triggered || fuse.WasTriggered();SampleCount++;LastSample=now;
  PrintFormat("GEP015_AUTO_ROUND id=%1 elapsed=%2 position=%3 velocity=%4 timer=%5 triggered=%6 proxy=%7 distanceFromMuzzle=%8",Round.GetID(),(now-DispatchMs)/1000.0,LatestPosition,move.GetVelocity(),fuse.GetTimer(),fuse.WasTriggered(),rpl.IsProxy(),distance);
  Save("observing");
 }
 override void EOnFrame(IEntity owner,float dt)
 {
  if(!GetGame().InPlayMode() || System.IsConsoleApp() || RplSession.Mode()!=RplMode.None)return;
  float now=GetWorld().GetWorldTime();if(StartMs<0)StartMs=now;Age=(now-StartMs)/1000.0;
  if(Finished){View(dt);return;}
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
   Turret=Seat.GetOwner();Smoke=GEP_StockSmoke015.Cast(Turret.FindComponent(GEP_StockSmoke015));
   CompartmentAccessComponent access=CompartmentAccessComponent.Cast(Actor.FindComponent(CompartmentAccessComponent));
   BoardRequested=true;
   if(!Smoke || !access || !access.GetInVehicle(Seat.GetOwner(),Seat,true,-1,ECloseDoorAfterActions.INVALID,true)){Stop("native_boarding_rejected");return;}
  }
  View(dt);
  if(BoardRequested && !Dispatched && Age>10 && Settled())
  {
   BeforeCount=Smoke.Count();bool authority=Smoke.Authority();bool operational=Smoke.Operational();bool crew=Smoke.Crew(Actor);bool canFire=Smoke.CanFire(Actor);
   PrintFormat("GEP015_AUTO_PREFLIGHT authority=%1 operational=%2 crew=%3 canFire=%4 count=%5",authority,operational,crew,canFire,BeforeCount);
   if(!authority || !operational || !crew || !canFire || BeforeCount!=8){Stop("smoke_preflight_failed");return;}
   BaseActionsManagerComponent actions=BaseActionsManagerComponent.Cast(Turret.FindComponent(BaseActionsManagerComponent));
   if(!actions){Stop("actions_manager_missing");return;}
   array<BaseUserAction> choices={};actions.GetActionsList(choices);GEP_StockSmokeFire015 selected;
   foreach(BaseUserAction action:choices){GEP_StockSmokeFire015 fire=GEP_StockSmokeFire015.Cast(action);if(fire && fire.m_bSingleRound){selected=fire;break;}}
   if(!selected || !selected.CanBeShown(Actor) || !selected.CanBePerformed(Actor)){Stop("single_round_action_ineligible");return;}
   vector muzzle[4];if(!Smoke.Muzzle(0,muzzle)){Stop("muzzle_missing");return;}LaunchOrigin=muzzle[3];
   Dispatched=true;DispatchMs=now;Reason="one_callback_issued";
   selected.OnActionStart(Actor);selected.PerformAction(Turret,Actor);AfterCount=Smoke.Count();
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
 override protected void InspectRound(IEntity round,int tube)
 {super.InspectRound(round,tube);if(GEP_OneRoundProbe015.Active)GEP_OneRoundProbe015.Active.ObserveRound(round,tube);}
}

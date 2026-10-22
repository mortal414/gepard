// PRIVATE C-only V11 recovery delta; SOURCE-ONLY, native compile pending.
// Explicitly opted-in, standalone virtual GAME fixture. No health/damage setters.
// Fresh process/subject per case. Scripted actions are never physical acceptance.
class GEP_DamageZone683
{
 string EntityPrefab, EntityName, Zone;
 float Health, Scaled, Maximum;
 int State;
 bool Write(SaveContext ctx)
 {
  if(!ctx.StartObject())return false;
  bool ok=true;
  ok=ctx.WriteValue("entityPrefab",EntityPrefab) && ok;ok=ctx.WriteValue("entityName",EntityName) && ok;ok=ctx.WriteValue("zone",Zone) && ok;
  ok=ctx.WriteValue("health",Health) && ok;ok=ctx.WriteValue("scaled",Scaled) && ok;ok=ctx.WriteValue("maximum",Maximum) && ok;ok=ctx.WriteValue("state",State) && ok;
  return ctx.EndObject() && ok;
 }
}
class GEP_DamageScenario683Class : GenericEntityClass {}
class GEP_DamageScenario683 : GenericEntity
{
 string m_ImpactNonce688;
 ref GEP_CoreWheeledCourse20261022 m_WheelCourseQA;

 // Private workflow regression successor. Native action input, never aim/health setters.
 float m_RightRequestQA,m_FireRequestQA,m_UpRequestQA;
 float m_UpMotionQA,m_DownMotionQA,m_RightYawQA,m_LeftYawQA;
 bool m_BurstFrame1QA,m_BurstFrame2QA;
 TurretControllerComponent m_ControlQA;
 MuzzleComponent m_LeftQA,m_RightQA;
 vector m_AimStartQA;
 float m_RightMotionQA,m_LeftMotionQA;
 int m_LeftStartQA,m_RightStartQA,m_LeftUsedQA,m_RightUsedQA,m_ReLeftUsedQA,m_ReRightUsedQA;
 bool m_OperationStartedQA,m_ReentryRequestedQA,m_ReentryCompletedQA;

 bool m_OperationObserved688,m_CrewConscious688,m_CrewSeated688,m_DriveFailed688;
 int m_WeaponSlots688,m_LoadedMuzzles688;
 ref array<int> m_Ammo688={};
 string m_Id,m_Nonce,m_Case,m_Vehicle,m_Prefab,m_Mode,m_Resource,m_Position,m_TargetLocal,m_Incoming,m_CrewRole;
 float m_Range=25,m_Standoff=2,m_Settle=10,m_Sample=3,m_Max=60,m_Interval=0.3;
 int m_Count=1,m_Seed=-1;
 bool m_RequestEgress,m_Capture=true,m_Loaded,m_Finished,m_EgressAttempted,m_EgressCompleted;
 bool m_EnteredPlay,m_StimulusObserved,m_CrewSetupFailed,m_BeforeFrame,m_AfterFrame,m_RenderSetup,m_CloseRequested;
 string m_Status="NOT_TESTED",m_Reason,m_EgressStatus="NOT_TESTED",m_EgressReason="not_requested";
 float m_Time,m_SpawnTime,m_StageTime,m_LastIssue=-100,m_FinishTime;
 int m_Stage,m_Issued;
 IEntity m_Subject,m_Crew;
 BaseCompartmentSlot m_Seat;
 CameraBase m_Camera;
 vector m_Aim,m_Source,m_Outward,m_StartPosition;
 ref array<ref GEP_DamageZone683> m_Before={},m_After={};
 ref array<ref GEP_DamageRecordSubscription683> m_Watches={};
 ref array<ref GEP_NativeStimulus683> m_Stimuli={};
 ref array<string> m_Evidence={};

 void GEP_DamageScenario683(IEntitySource src,IEntity parent)
 {
  SetEventMask(EntityEvent.FRAME | EntityEvent.POSTFRAME);SetFlags(EntityFlags.ACTIVE,true);
 }
 bool Load()
 {
  JsonLoadContext ctx=new JsonLoadContext();int schema;
  if(!ctx.LoadFromFile("$profile:DamageHarness-job683.json"))return false;
  if(!ctx.ReadValue("schema",schema) || schema!=1)return false;
  if(!ctx.ReadValue("runId",m_Id) || !ctx.ReadValue("nonce",m_Nonce))return false;
  if(!ctx.ReadValue("caseId",m_Case) || !ctx.ReadValue("vehicleId",m_Vehicle) || !ctx.ReadValue("prefab",m_Prefab))return false;
  if(!ctx.ReadValue("stimulus",m_Mode) || !ctx.ReadValue("resource",m_Resource))return false;
  if(!ctx.ReadValue("position",m_Position) || !ctx.ReadValue("targetLocal",m_TargetLocal) || !ctx.ReadValue("incoming",m_Incoming))return false;
  ctx.ReadValue("range",m_Range);ctx.ReadValue("standoff",m_Standoff);ctx.ReadValue("settleSeconds",m_Settle);
  ctx.ReadValue("sampleSeconds",m_Sample);ctx.ReadValue("maxGameSeconds",m_Max);ctx.ReadValue("shotCount",m_Count);
  ctx.ReadValue("shotInterval",m_Interval);ctx.ReadValue("requestScriptedEgress",m_RequestEgress);
  ctx.ReadValue("crewRole",m_CrewRole);ctx.ReadValue("seedRequest",m_Seed);ctx.ReadValue("capture",m_Capture);
  if(m_Mode!="none" && m_Mode!="native-projectile" && m_Mode!="native-blast")return false;
  if(m_Count<1 || m_Count>16 || m_Range<2 || m_Range>250 || m_Standoff<0.2 || m_Standoff>30)return false;
  if(m_Settle<5 || m_Settle>25 || m_Sample<1 || m_Sample>15 || m_Max<20 || m_Max>120)return false;
  if(m_Interval<0.15 || m_Interval>3)return false;
  if(m_CrewRole!="none" && m_CrewRole!="driver" && m_CrewRole!="gunner")return false;
  // Explicit V11-only fixture: a stale baseline job must fail before spawning.
  if(m_Prefab!="{2150FC6F1E081188}Prefabs/ArtApproved20261022/Camo/GEP_MobileCarrier.et" && m_Prefab!="{8095474EC9806D0D}Prefabs/ArtApproved20261022/Charcoal/GEP_MobileCarrier.et")return false;
  if(m_RequestEgress && m_CrewRole=="none")return false;
  if(m_Seed!=-1)return false; // No exposed native projectile/fragment RNG seed.
  return m_Id!="" && m_Nonce!="";
 }
 string Prefab(IEntity entity)
 {
  if(!entity || entity.IsDeleted() || !entity.GetPrefabData())return "";
  return entity.GetPrefabData().GetPrefabName();
 }
 IEntity Spawn(ResourceName prefab,vector position)
 {
  Resource resource=Resource.Load(prefab);if(!resource || !resource.IsValid())return null;
  EntitySpawnParams params=new EntitySpawnParams();params.TransformMode=ETransformMode.WORLD;params.Transform[3]=position;
  return GetGame().SpawnEntityPrefab(resource,GetGame().GetWorld(),params);
 }
 void WatchTree(IEntity entity,int depth=0)
 {
  if(!entity || entity.IsDeleted() || depth>16 || m_Watches.Count()>=64)return;
  SCR_DamageManagerComponent dm=SCR_DamageManagerComponent.Cast(entity.FindComponent(SCR_DamageManagerComponent));
  if(dm){GEP_DamageRecordSubscription683 watch=new GEP_DamageRecordSubscription683(dm);watch.Limit683=64;m_Watches.Insert(watch);}
  for(IEntity child=entity.GetChildren();child;child=child.GetSibling())WatchTree(child,depth+1);
 }
 void CaptureZones(IEntity entity,array<ref GEP_DamageZone683> rows)
 {
  if(!entity || entity.IsDeleted())return;
  SCR_DamageManagerComponent dm=SCR_DamageManagerComponent.Cast(entity.FindComponent(SCR_DamageManagerComponent));
  if(!dm)return;
  array<HitZone> zones={};dm.GetAllHitZonesInHierarchy(zones);
  foreach(HitZone zone:zones)
  {
   if(!zone || rows.Count()>=512)break;
   GEP_DamageZone683 row=new GEP_DamageZone683();
   SCR_HitZone scripted=SCR_HitZone.Cast(zone);IEntity owner=entity;if(scripted && scripted.GetOwner())owner=scripted.GetOwner();
   row.EntityPrefab=Prefab(owner);row.EntityName=owner.GetName();row.Zone=zone.GetName();
   row.Health=zone.GetHealth();row.Scaled=zone.GetHealthScaled();row.Maximum=zone.GetMaxHealth();row.State=zone.GetDamageState();rows.Insert(row);
  }
 }
 void Snapshot(array<ref GEP_DamageZone683> rows)
 {
  CaptureZones(m_Subject,rows);if(m_Crew)CaptureZones(m_Crew,rows);
 }
 int EventCount()
 {
  int count=0;
  foreach(GEP_DamageRecordSubscription683 watch:m_Watches)
  {
   count+=watch.Seen683;
  }
  return count;
 }
 BaseCompartmentSlot FindSeat(IEntity entity,int depth=0)
 {
  if(!entity || depth>16)return null;
  BaseCompartmentManagerComponent manager=BaseCompartmentManagerComponent.Cast(entity.FindComponent(BaseCompartmentManagerComponent));
  if(manager)
  {
   array<BaseCompartmentSlot> seats={};manager.GetCompartments(seats);
   foreach(BaseCompartmentSlot seat:seats)
   {
    if(m_CrewRole=="driver" && PilotCompartmentSlot.Cast(seat))return seat;
    if(m_CrewRole=="gunner" && TurretCompartmentSlot.Cast(seat))return seat;
   }
  }
  for(IEntity child=entity.GetChildren();child;child=child.GetSibling()){BaseCompartmentSlot found=FindSeat(child,depth+1);if(found)return found;}
  return null;
 }
 void PrepareCrew()
 {
  if(m_CrewRole=="none")return;
  m_Seat=FindSeat(m_Subject);if(!m_Seat){m_CrewSetupFailed=true;return;}
  vector position=m_Subject.GetOrigin()+Vector(5,0,5);position[1]=GetGame().GetWorld().GetSurfaceY(position[0],position[2])+0.1;
  m_Crew=Spawn("{26A9756790131354}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Rifleman.et",position);
  if(!m_Crew){m_CrewSetupFailed=true;return;}
  // Bind the local player before native boarding so native input ownership is
  // established through the actual compartment transition, not after damage.
  PlayerController fixturePlayerQA=GetGame().GetPlayerController();
  if(fixturePlayerQA)fixturePlayerQA.SetControlledEntity(m_Crew);

  WatchTree(m_Crew);
  CompartmentAccessComponent access=CompartmentAccessComponent.Cast(m_Crew.FindComponent(CompartmentAccessComponent));
  if(!access || !access.GetInVehicle(m_Seat.GetOwner(),m_Seat,true,-1,ECloseDoorAfterActions.INVALID,true))m_CrewSetupFailed=true;
  m_Evidence.Insert("crew_setup=scripted_native_GetInVehicle physicalAcceptance=false");
 }
 bool CrewReady()
 {
  if(m_CrewRole=="none")return true;
  if(m_CrewSetupFailed || !m_Crew || !m_Seat)return false;
  CompartmentAccessComponent access=CompartmentAccessComponent.Cast(m_Crew.FindComponent(CompartmentAccessComponent));
  return access && access.GetCompartment()==m_Seat && m_Seat.GetOccupant()==m_Crew && !access.IsGettingIn() && !access.IsGettingOut() && !access.IsSwitchingSeatsAnim();
 }
 void Camera(float dt)
 {
  if(!m_Subject || m_Subject.IsDeleted())return;
  if(!m_Camera)m_Camera=CameraBase.Cast(GetGame().SpawnEntity(CameraBase,GetGame().GetWorld()));
  if(!m_Camera)return;
  vector aim=m_Subject.CoordToParent(Vector(0,1.3,0));vector eye=m_Subject.CoordToParent(Vector(12,7,12));vector pose[4];
  Math3D.DirectionAndUpMatrix((aim-eye).Normalized(),vector.Up,pose);pose[3]=eye;
  m_Camera.SetWorldTransform(pose);m_Camera.SetFOVDegree(55);GetGame().GetCameraManager().SetCamera(m_Camera);m_Camera.ApplyTransform(dt);
  if(!m_RenderSetup)
  {
   PlayerController player=GetGame().GetPlayerController();
   SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
   SCR_RespawnSystemComponent respawn=SCR_RespawnSystemComponent.GetInstance();
   if(player && mode && mode.IsRunning() && respawn)
   {
    respawn.DestroyLoadingPlaceholder();
    player.SetCharacterCameraRenderActive(false);
    SCR_HintManagerComponent.HideHint();
    m_RenderSetup=true;
    PrintFormat("GEP_DAMAGE683_RENDER_SETUP|%1|%2|nativeFreeCamera=1|scriptedFixture=1|physical=0",m_Id,m_Nonce);
   }
  }
 }
 bool Screenshot(string phase)
 {
  if(!m_Capture){return true;}
  if(!m_Camera){return false;}
  if(!m_RenderSetup || ArmaReforgerLoadingAnim.IsOpen() || GetGame().GetMenuManager().IsAnyMenuOpen()){return false;}
  string file="$profile:DamageHarness-"+phase;
  bool wrote=System.MakeScreenshot(file);
  PrintFormat("GEP_DAMAGE683_FRAME|%1|%2|%3|requested=%4|cameraSelected=%5|file=%6",m_Id,m_Nonce,phase,wrote,GetGame().GetCameraManager().CurrentCamera()==m_Camera,file);
  return wrote && GetGame().GetCameraManager().CurrentCamera()==m_Camera;
 }
 string Payload()
 {
  if(m_Resource=="{A7846B9D39014801}Prefabs/QA/GEP688_M855Observed.et")return "m855_688";
  if(m_Resource=="{A7846B9D39014802}Prefabs/QA/GEP688_M80Observed.et")return "m80_688";
  if(m_Resource=="{A7846B9D39014803}Prefabs/QA/GEP688_M33Observed.et")return "m33_688";
  if(m_Resource.Contains("GEP_35_HEIT_"))return "gep_35mm";
  if(m_Resource.Contains("Warhead_HEIT_M792") || m_Resource.Contains("Ammo_25x137_HEIT_M792"))return "m792_25mm";
  if(m_Resource.Contains("Ammo_127x99_Ball_M33"))return "m33";
  if(m_Resource.Contains("Ammo_127x99_AP_M2"))return "m2_ap";
  if(m_Resource.Contains("Warhead_Grenade_RGD5"))return "rgd5";
  if(m_Resource.Contains("Warhead_Grenade_M67"))return "m67";
  return "";
 }
 bool PrepareGeometry()
 {
  m_Aim=m_Subject.CoordToParent(m_TargetLocal.ToVector());vector local=m_Incoming.ToVector();
  if(local.LengthSq()<0.5)return false;local.Normalize();
  m_Outward=m_Subject.VectorToParent(local);m_Outward.Normalize();
  float distance=m_Range;if(m_Mode=="native-blast")distance=m_Standoff;
  m_Source=m_Aim+m_Outward*distance;
  TraceParam trace=new TraceParam();trace.Start=m_Aim+m_Outward*m_Range;trace.End=m_Aim-m_Outward*2;
  trace.Flags=TraceFlags.ENTS|TraceFlags.WORLD|TraceFlags.ANY_CONTACT;trace.LayerMask=32;
  float fraction=GetGame().GetWorld().TraceMove(trace,null);
  string hitPrefab=Prefab(trace.TraceEnt);
  m_Evidence.Insert(string.Format("native_preflight fraction=%1 collider=%2 hitPrefab=%3 node=%4",fraction,trace.ColliderName,hitPrefab,trace.NodeIndex));
  PrintFormat("GEP_DAMAGE683_PREFLIGHT|%1|%2|fraction=%3|collider=%4|hit=%5|origin=%6|target=%7",m_Id,m_Nonce,fraction,trace.ColliderName,trace.TraceEnt,m_Source,m_Aim);
  if(m_Mode=="native-projectile" && (!trace.TraceEnt || trace.TraceEnt.GetRootParent()!=m_Subject))return false;
  return true;
 }
 bool WriteZones(SaveContext ctx,string name,array<ref GEP_DamageZone683> rows)
 {
  int count=rows.Count();if(!ctx.StartArray(name,count))return false;
  foreach(GEP_DamageZone683 row:rows)if(!row.Write(ctx))return false;
  return ctx.EndArray();
 }
 bool Check(SaveContext ctx,string name,string status,string actual)
 {
  bool allWritten=true;
  if(!ctx.StartObject())return false;
  allWritten=ctx.WriteValue("name",name) && allWritten;
  allWritten=ctx.WriteValue("status",status) && allWritten;
  allWritten=ctx.WriteValue("actual",actual) && allWritten;
  return ctx.EndObject() && allWritten;
 }

 void Finish(string status,string reason)
 {
  if(m_Finished)return;
  InputQA(0,0);m_UpRequestQA=0;
  if(m_WheelCourseQA)m_WheelCourseQA.Release();
  GetGame().GetInputManager().SetActionValue("TurretAimRight",0);
  GetGame().GetInputManager().SetActionValue("TurretAimLeft",0);
  GetGame().GetInputManager().SetActionValue("TurretAimUp",0);
  GetGame().GetInputManager().SetActionValue("TurretAimDown",0);
  GetGame().GetInputManager().SetActionValue("TurretFire",0);
  GetGame().GetInputManager().SetActionValue("TurretADSHold",0);
  if(m_ControlQA)m_ControlQA.SetWeaponADS(false);
  m_Finished=true;m_FinishTime=m_Time;m_Status=status;m_Reason=reason;m_StimulusObserved=EventCount()>0;
  if(m_After.Count()==0)Snapshot(m_After);
  GEP688_ImpactInbox.Close(m_ImpactNonce688);
  GEP688_ImpactReport finalImpact688=GEP688_ImpactInbox.Snapshot(m_ImpactNonce688);
  if(finalImpact688 && finalImpact688.ValidSubjectCallbacks>0)m_StimulusObserved=true;
  if(m_Camera){GetGame().GetCameraManager().SetCamera(m_Camera);m_Camera.ApplyTransform(0);}
  m_AfterFrame=Screenshot("after");
  if(m_Capture && (!m_BeforeFrame || !m_AfterFrame) && m_Status=="PASS")
  {
   m_Status="NOT_TESTED";m_Reason="requested_native_capture_incomplete";
  }
  JsonSaveContext ctx=new JsonSaveContext();ctx.SetMaxDecimalPlaces(8);bool ok=true;
  ok=ctx.WriteValue("schema",1) && ok;ok=ctx.WriteValue("runId",m_Id) && ok;ok=ctx.WriteValue("nonce",m_Nonce) && ok;
  ok=ctx.WriteValue("caseId",m_Case) && ok;ok=ctx.WriteValue("vehicleId",m_Vehicle) && ok;ok=ctx.WriteValue("status",m_Status) && ok;ok=ctx.WriteValue("reason",m_Reason) && ok;
  ok=ctx.WriteValue("enteredPlay",m_EnteredPlay) && ok;ok=ctx.WriteValue("nativeStimulusObserved",m_StimulusObserved) && ok;ok=ctx.WriteValue("healthSettersUsed",false) && ok;
  ok=ctx.WriteValue("scriptedEgressCompleted",m_EgressCompleted) && ok;ok=ctx.WriteValue("physicalAcceptance",false) && ok;
  int checkCount=6;ok=ctx.StartArray("checks",checkCount) && ok;
  ok=Check(ctx,"gameplay", "PASS","InPlayMode; standalone fixture") && ok;
  ok=Check(ctx,"damageFixture",m_Status,m_Reason) && ok;
  ok=Check(ctx,"scriptedEgress",m_EgressStatus,m_EgressReason) && ok;
  ok=Check(ctx,"physicalHumanAcceptance","NOT_TESTED","All actions are declared scripted GAME QA") && ok;
  ok=Check(ctx,"nativeDamageSeed","NOT_TESTED","No exposed native projectile/fragment RNG seed; authored speed variation retained") && ok;
  ok=Check(ctx,"realWorldFidelity","NOT_TESTED","Virtual game model calibration only") && ok;ok=ctx.EndArray() && ok;
  ok=ctx.StartObject("measurements") && ok;
  ok=ctx.WriteValue("subjectPrefab",m_Prefab) && ok;ok=ctx.WriteValue("stimulus",m_Mode) && ok;ok=ctx.WriteValue("configuredSourcePrefab",m_Resource) && ok;
  ok=ctx.WriteValue("issued",m_Issued) && ok;ok=ctx.WriteValue("damageCallbacks",EventCount()) && ok;ok=ctx.WriteValue("elapsedGameSeconds",m_Time) && ok;
  ok=ctx.WriteValue("subjectExistsAfter",m_Subject && !m_Subject.IsDeleted()) && ok;ok=ctx.WriteValue("crewExistsAfter",m_Crew && !m_Crew.IsDeleted()) && ok;
  ok=ctx.WriteValue("seedRequest",m_Seed) && ok;ok=ctx.WriteValue("seedApplied",false) && ok;ok=ctx.WriteValue("deterministicLayout",true) && ok;ok=ctx.WriteValue("deterministicDamage",false) && ok;
  ok=ctx.WriteValue("source",m_Source) && ok;ok=ctx.WriteValue("aimPoint",m_Aim) && ok;ok=ctx.WriteValue("evidence",m_Evidence) && ok;
  ok=ctx.WriteValue("captureRequested",m_Capture) && ok;ok=ctx.WriteValue("beforeFrameRequested",m_BeforeFrame) && ok;ok=ctx.WriteValue("afterFrameRequested",m_AfterFrame) && ok;
  ok=ctx.WriteValue("settledRoot",m_StartPosition) && ok;
  ok=ctx.WriteValue("componentOperationObserved688",m_OperationObserved688) && ok;
  ok=ctx.WriteValue("crewConsciousAfter688",m_CrewConscious688) && ok;
  ok=ctx.WriteValue("crewSeatedAfterStimulus688",m_CrewSeated688) && ok;
  ok=ctx.WriteValue("turretDriveFailed688",m_DriveFailed688) && ok;
  ok=ctx.WriteValue("weaponSlots688",m_WeaponSlots688) && ok;
  ok=ctx.WriteValue("loadedMuzzles688",m_LoadedMuzzles688) && ok;
  ok=ctx.WriteValue("ammo688",m_Ammo688) && ok;
  ok=ctx.WriteValue("operationStartedQA",m_OperationStartedQA) && ok;
  ok=ctx.WriteValue("rightTraverseDegreesQA",m_RightMotionQA) && ok;
  ok=ctx.WriteValue("rightYawDegreesQA",m_RightYawQA) && ok;ok=ctx.WriteValue("leftYawDegreesQA",m_LeftYawQA) && ok;
  ok=ctx.WriteValue("upElevationDegreesQA",m_UpMotionQA) && ok;ok=ctx.WriteValue("downElevationDegreesQA",m_DownMotionQA) && ok;
  ok=ctx.WriteValue("burstFrame1RequestedQA",m_BurstFrame1QA) && ok;ok=ctx.WriteValue("burstFrame2RequestedQA",m_BurstFrame2QA) && ok;
  ok=ctx.WriteValue("recoilVisualAcceptance","NOT_TESTED") && ok;
  ok=ctx.WriteValue("wheeledCourseSamplesCompletedQA",m_WheelCourseQA && m_WheelCourseQA.Completed) && ok;
  ok=ctx.WriteValue("artNativeCookAcceptance","NOT_TESTED") && ok;
  ok=ctx.WriteValue("leftTraverseDegreesQA",m_LeftMotionQA) && ok;
  ok=ctx.WriteValue("leftRoundsUsedQA",m_LeftUsedQA) && ok;
  ok=ctx.WriteValue("rightRoundsUsedQA",m_RightUsedQA) && ok;
  ok=ctx.WriteValue("publicReentryRequestedQA",m_ReentryRequestedQA) && ok;
  ok=ctx.WriteValue("publicReentryCompletedQA",m_ReentryCompletedQA) && ok;
  ok=ctx.WriteValue("reentryLeftRoundsUsedQA",m_ReLeftUsedQA) && ok;
  ok=ctx.WriteValue("reentryRightRoundsUsedQA",m_ReRightUsedQA) && ok;
  ok=WriteImpacts688(ctx,finalImpact688) && ok;
  ok=WriteZones(ctx,"before",m_Before) && ok;ok=WriteZones(ctx,"after",m_After) && ok;
  int managerCount=m_Watches.Count();ok=ctx.StartArray("damageManagers",managerCount) && ok;
  foreach(GEP_DamageRecordSubscription683 watch:m_Watches)
  {
   ok=ctx.StartObject() && ok;ok=ctx.WriteValue("seen",watch.Seen683) && ok;ok=ctx.WriteValue("dropped",watch.Dropped683) && ok;ok=watch.Write683(ctx,"events") && ok;ok=ctx.EndObject() && ok;
  }
  ok=ctx.EndArray() && ok;ok=ctx.EndObject() && ok;
  bool saved;
  if(ok)saved=ctx.SaveToFile("$profile:DamageHarness-result683.json");
  else ctx.SaveToFile("$profile:DamageHarness-result683.partial.json");
  foreach(GEP_DamageRecordSubscription683 close:m_Watches)close.Close683();
  PrintFormat("GEP_DAMAGE683_RESULT status=%1 reason=%2 saved=%3 callbacks=%4 physical=0 healthSetters=0",m_Status,m_Reason,saved,EventCount());
  if(saved)PrintFormat("GEP_DAMAGE683_DONE|%1|%2",m_Id,m_Nonce);
  else PrintFormat("GEP_DAMAGE683_FAIL|%1|%2|result_save_failed",m_Id,m_Nonce);
 }

 void ObserveOperation688()
 {
  if(!m_Crew || !m_Seat)return;
  CharacterControllerComponent life=CharacterControllerComponent.Cast(m_Crew.FindComponent(CharacterControllerComponent));
  CompartmentAccessComponent access=CompartmentAccessComponent.Cast(m_Crew.FindComponent(CompartmentAccessComponent));
  m_CrewConscious688=life && life.GetLifeState()==ECharacterLifeState.ALIVE && !life.IsUnconscious();
  m_CrewSeated688=access && access.GetCompartment()==m_Seat && m_Seat.GetOccupant()==m_Crew;
  TurretControllerComponent control=TurretControllerComponent.Cast(m_Seat.GetController());
  if(!control)control=m_Seat.GetAttachedTurret();
  GEP_TurretController410 drive=GEP_TurretController410.Cast(control);
  if(!control || !drive || !control.GetWeaponManager())return;
  m_DriveFailed688=drive.IsDriveFailed();
  array<WeaponSlotComponent> slots={};control.GetWeaponManager().GetWeaponsSlots(slots);
  m_WeaponSlots688=slots.Count();
  foreach(WeaponSlotComponent slot:slots)
  {
   MuzzleComponent muzzle=MuzzleComponent.Cast(slot.GetCurrentMuzzle());
   int ammo=-1;if(muzzle)ammo=muzzle.GetAmmoCount();
   m_Ammo688.Insert(ammo);
   if(muzzle && muzzle.GetMagazine() && ammo>0)m_LoadedMuzzles688++;
  }
  m_OperationObserved688=true;
  PrintFormat("GEP688_OPERATION crewConscious=%1 seated=%2 driveFailed=%3 slots=%4 loaded=%5 actualPostDamageDriving=NOT_TESTED actualPostDamageFiring=NOT_TESTED",m_CrewConscious688,m_CrewSeated688,m_DriveFailed688,m_WeaponSlots688,m_LoadedMuzzles688);
 }
 bool WriteImpacts688(SaveContext ctx,GEP688_ImpactReport report)
 {
  bool ok=true;
  if(!ctx.StartObject("impactEvidence688"))return false;
  ok=ctx.WriteValue("available",report!=null) && ok;
  if(report)
  {
   ok=ctx.WriteValue("registered",report.RegisteredProjectiles) && ok;
   ok=ctx.WriteValue("validSubjectCallbacks",report.ValidSubjectCallbacks) && ok;
   ok=ctx.WriteValue("otherCallbacks",report.OtherCallbacks) && ok;
   ok=ctx.WriteValue("unregisteredCallbacks",report.UnregisteredCallbacks) && ok;
   ok=ctx.WriteValue("invalidCallbacks",report.InvalidSubjectCallbacks) && ok;
   ok=ctx.WriteValue("dropped",report.DroppedSubjectReceipts) && ok;
   ok=ctx.WriteValue("rejectedRegistrations",report.RejectedRegistrations) && ok;
   ok=ctx.WriteValue("expiredCallbacks",report.ExpiredCallbacks) && ok;
   int count=report.Receipts.Count();ok=ctx.StartArray("receipts",count) && ok;
   foreach(GEP688_ImpactReceipt receipt:report.Receipts)
   {
    ok=ctx.StartObject() && ok;
    ok=ctx.WriteValue("sourceId",receipt.SourceID) && ok;ok=ctx.WriteValue("sourcePrefab",receipt.SourcePrefab) && ok;
    ok=ctx.WriteValue("hitId",receipt.HitID) && ok;ok=ctx.WriteValue("hitPrefab",receipt.HitPrefab) && ok;
    ok=ctx.WriteValue("collider",receipt.Collider) && ok;ok=ctx.WriteValue("position",receipt.Position) && ok;
    ok=ctx.WriteValue("direction",receipt.Direction) && ok;ok=ctx.WriteValue("normal",receipt.Normal) && ok;
    ok=ctx.WriteValue("speed",receipt.Speed) && ok;ok=ctx.WriteValue("milliseconds",receipt.MillisecondsFromOpen) && ok;
    ok=ctx.EndObject() && ok;
   }
   ok=ctx.EndArray() && ok;
  }
  return ctx.EndObject() && ok;
 }

 void BeginEgress()
 {
  m_EgressAttempted=true;m_StageTime=m_Time;
  if(!m_Crew || !m_Seat || !m_Seat.GetOwner() || m_Seat.GetOwner().IsDeleted()){m_EgressReason="crew_or_original_seat_not_available";return;}
  CharacterControllerComponent life=CharacterControllerComponent.Cast(m_Crew.FindComponent(CharacterControllerComponent));
  if(!life || life.GetLifeState()!=ECharacterLifeState.ALIVE || life.IsUnconscious()){m_EgressReason="crew_not_conscious_survivor";return;}
  SCR_CompartmentUserAction action=SCR_CompartmentUserAction.Cast(m_Seat.GetGetOutAction());
  if(!action || !action.CanBeShown(m_Crew) || !action.CanBePerformed(m_Crew)){m_EgressStatus="FAIL";m_EgressReason="native_public_exit_ineligible";return;}
  action.OnActionStart(m_Crew);action.PerformAction(m_Seat.GetOwner(),m_Crew);m_EgressReason="native_public_exit_requested_once";
 }

 bool ResolveQA()
 {
  if(!m_Seat || !m_Crew)return false;
  m_ControlQA=TurretControllerComponent.Cast(m_Seat.GetController());
  if(!m_ControlQA)m_ControlQA=m_Seat.GetAttachedTurret();
  if(!m_ControlQA || !m_ControlQA.GetWeaponManager())return false;
  array<WeaponSlotComponent> slots={};m_ControlQA.GetWeaponManager().GetWeaponsSlots(slots);
  foreach(WeaponSlotComponent slot:slots)
  {
   if(slot.GetWeaponSlotIndex()==0)m_LeftQA=MuzzleComponent.Cast(slot.GetCurrentMuzzle());
   if(slot.GetWeaponSlotIndex()==1)m_RightQA=MuzzleComponent.Cast(slot.GetCurrentMuzzle());
  }
  return m_LeftQA && m_RightQA;
 }
 void TraceCompatPoseQA(string phase)
 {
  if(!m_Subject || !m_Seat || !m_Seat.GetOwner() || !m_ControlQA)return;
  vector hullPose[4],seatPose[4],sightsPose[4];float fov;
  m_Subject.GetWorldTransform(hullPose);m_Seat.GetOwner().GetWorldTransform(seatPose);
  PrintFormat("GEP_CORE_COMPAT_HULL case=%1 nonce=%2 phase=%3 owner=%4 x=%5 y=%6 z=%7 pos=%8",m_Case,m_Nonce,phase,m_Subject.GetID(),hullPose[0],hullPose[1],hullPose[2],hullPose[3]);
  PrintFormat("GEP_CORE_COMPAT_SEAT_ENTITY case=%1 nonce=%2 phase=%3 owner=%4 x=%5 y=%6 z=%7 pos=%8",m_Case,m_Nonce,phase,m_Seat.GetOwner().GetID(),seatPose[0],seatPose[1],seatPose[2],seatPose[3]);
  if(!m_ControlQA.GetCurrentSightsCameraTransform(sightsPose,fov))return;
  vector local=m_Subject.VectorToLocal(sightsPose[2]).Normalized();
  float yaw=Math.Atan2(local[0],local[2])*Math.RAD2DEG;
  float elevation=Math.Asin(Math.Clamp(local[1],-1,1))*Math.RAD2DEG;
  PrintFormat("GEP_CORE_COMPAT_SIGHTS case=%1 nonce=%2 phase=%3 x=%4 y=%5 z=%6 pos=%7 yawHull=%8 elevationHull=%9",m_Case,m_Nonce,phase,sightsPose[0],sightsPose[1],sightsPose[2],sightsPose[3],yaw,elevation);
 }
 void InputQA(float right,float fire)
 {
  m_RightRequestQA=right;m_FireRequestQA=fire;
 }
 override void EOnFrame(IEntity owner,float timeSlice)
 {
  if(m_WheelCourseQA && !m_Finished && m_Stage==20)
  {m_WheelCourseQA.BeforePhysics();return;}
  if(!GetGame().InPlayMode() || m_Finished || !m_ControlQA)return;
  float right=m_RightRequestQA,fire=m_FireRequestQA;
  float ads=0;if((m_Stage>=4 && m_Stage<=7) || m_Stage==9 || m_Stage==10 || m_Stage==11 || m_Stage==12)ads=1;
  GetGame().GetInputManager().SetActionValue("TurretADSHold",ads);
  float rightInput=0,leftInput=0;
  if(right>0)rightInput=right;else leftInput=-right;
  GetGame().GetInputManager().SetActionValue("TurretAimRight",rightInput);
  GetGame().GetInputManager().SetActionValue("TurretAimLeft",leftInput);
  GetGame().GetInputManager().SetActionValue("TurretAimUp",Math.Max(0,m_UpRequestQA));
  GetGame().GetInputManager().SetActionValue("TurretAimDown",Math.Max(0,-m_UpRequestQA));
  GetGame().GetInputManager().SetActionValue("TurretFire",fire);
 }
 vector DirectionQA()
 {
  vector pose[4];float fov;
  if(m_ControlQA && m_ControlQA.GetCurrentSightsCameraTransform(pose,fov))return pose[2].Normalized();
  return vector.Zero;
 }
 float MotionQA()
 {
  vector now=DirectionQA();if(now.LengthSq()<0.5 || m_AimStartQA.LengthSq()<0.5)return -1;
  return Math.Acos(Math.Clamp(vector.Dot(m_AimStartQA,now),-1,1))*Math.RAD2DEG;
 }
 void TraceReentryQA(SCR_CompartmentUserAction entry)
 {
  IEntity actionOwner=entry.GetOwner();
  BaseCompartmentManagerComponent doors=BaseCompartmentManagerComponent.Cast(actionOwner.FindComponent(BaseCompartmentManagerComponent));
  CompartmentAccessComponent access=CompartmentAccessComponent.Cast(m_Crew.FindComponent(CompartmentAccessComponent));
  array<int> indices={};m_Seat.GetAvailableDoorIndices(indices);int door=-1;
  if(indices.Count()==1)door=indices[0];else door=m_Seat.PickDoorIndexForPoint(m_Crew.GetOrigin());
  IEntity doorUser;if(doors && door>=0)doorUser=doors.GetDoorUser(door);
  IEntity main=SCR_EntityHelper.GetMainParent(actionOwner,true);
  bool allowed=false;if(access && doors && main && door>=0)allowed=access.CanAccessDoor(main,doors,door);
  PrintFormat("QA_REENTRY_ADMISSION ownerMatches=%1 slotMatches=%2 door=%3 doorUser=%4 doorAccess=%5 gettingIn=%6 gettingOut=%7 switching=%8",actionOwner==m_Seat.GetOwner(),entry.GetCompartmentSlot()==m_Seat,door,doorUser,allowed,access.IsGettingIn(),access.IsGettingOut(),access.IsSwitchingSeatsAnim());
  PrintFormat("QA_REENTRY_GEOMETRY actor=%1 seatOwner=%2 actionOwner=%3 main=%4 localPosition=%5",m_Crew.GetOrigin(),m_Seat.GetOwner(),actionOwner,main,actionOwner.CoordToLocal(m_Crew.GetOrigin()));
 }
 // Horizontal and vertical readback are separated. Native sight direction is
 // transformed into actual hull axes; no turret angle or animation setter.
 float YawMotionQA()
 {
  vector a=m_Subject.VectorToLocal(m_AimStartQA),b=m_Subject.VectorToLocal(DirectionQA());
  if(a.LengthSq()<0.5 || b.LengthSq()<0.5)return -1;
  float value=Math.AbsFloat((Math.Atan2(a[0],a[2])-Math.Atan2(b[0],b[2]))*Math.RAD2DEG);
  if(value>180)value=360-value;return value;
 }
 float ElevationMotionQA()
 {
  vector a=m_Subject.VectorToLocal(m_AimStartQA).Normalized(),b=m_Subject.VectorToLocal(DirectionQA()).Normalized();
  if(a.LengthSq()<0.5 || b.LengthSq()<0.5)return -1;
  return (Math.Asin(Math.Clamp(b[1],-1,1))-Math.Asin(Math.Clamp(a[1],-1,1)))*Math.RAD2DEG;
 }
 bool CaptureBurstQA(string phase)
 {
  PlayerController player=GetGame().GetPlayerController();
  if(!m_Capture || !player || player.GetControlledEntity()!=m_Crew || !player.GetPlayerCamera())return false;
  if(GetGame().GetCameraManager().CurrentCamera()!=player.GetPlayerCamera() || ArmaReforgerLoadingAnim.IsOpen() || GetGame().GetMenuManager().IsAnyMenuOpen())return false;
  bool requested=System.MakeScreenshot("$profile:CoreQA-"+phase);
  PrintFormat("GEP_CORE_FRAME case=%1 nonce=%2 phase=%3 requested=%4 recoilAcceptance=NOT_TESTED",m_Case,m_Nonce,phase,requested);
  return requested;
 }
 void OperationQA()
 {
  if(m_Stage==4)
  {
   if(!ResolveQA()){Finish("NOT_TESTED","native_controller_or_muzzles_missing_QA");return;}
   PlayerController player=GetGame().GetPlayerController();if(!player)return;
   if(player.GetControlledEntity()!=m_Crew){player.SetControlledEntity(m_Crew);return;}
   player.SetCharacterCameraRenderActive(true);
   if(player.GetPlayerCamera())GetGame().GetCameraManager().SetCamera(player.GetPlayerCamera());
   m_ControlQA.SetWeaponADS(true);InputQA(0,0);
   if(m_Time-m_StageTime<2)return;
   if(!m_ControlQA.GetCurrentSightsADS())
   {
    if(m_Time-m_StageTime>5)Finish("NOT_TESTED","ordinary_native_ADS_input_not_ready_QA");
    return;
   }

   PrintFormat("QA_INPUT_READY controlled=%1 camera=%2 ads=%3 turretContext=%4",player.GetControlledEntity()==m_Crew,player.GetPlayerCamera()!=null,m_ControlQA.GetCurrentSightsADS(),GetGame().GetInputManager().IsContextActive("TurretContext"));
   m_AimStartQA=DirectionQA();m_OperationStartedQA=true;m_Stage=5;m_StageTime=m_Time;
  }
  if(m_Stage==5)
  {
   InputQA(10,0);if(m_Time-m_StageTime<2)return;
   m_RightMotionQA=MotionQA();m_RightYawQA=YawMotionQA();m_AimStartQA=DirectionQA();InputQA(0,0);m_Stage=6;m_StageTime=m_Time;
   PrintFormat("QA_POSTHIT_RIGHT degrees=%1 nativeAction=TurretAimRight physicalInput=0",m_RightMotionQA);
  }
  if(m_Stage==6)
  {
   InputQA(-10,0);if(m_Time-m_StageTime<2)return;
   m_LeftMotionQA=MotionQA();m_LeftYawQA=YawMotionQA();InputQA(0,0);
   m_AimStartQA=DirectionQA();m_Stage=11;m_StageTime=m_Time;
   PrintFormat("QA_POSTHIT_LEFT degrees=%1 nativeAction=TurretAimRight physicalInput=0",m_LeftMotionQA);
  }
  if(m_Stage==11)
  {
   InputQA(0,0);m_UpRequestQA=2;if(m_Time-m_StageTime<0.7)return;
   m_UpMotionQA=ElevationMotionQA();m_UpRequestQA=0;m_AimStartQA=DirectionQA();m_Stage=12;m_StageTime=m_Time;
   PrintFormat("GEP_CORE_ELEVATION case=%1 nonce=%2 phase=up degrees=%3 physical=0",m_Case,m_Nonce,m_UpMotionQA);
  }
  if(m_Stage==12)
  {
   InputQA(0,0);m_UpRequestQA=-2;if(m_Time-m_StageTime<0.7)return;
   m_DownMotionQA=ElevationMotionQA();m_UpRequestQA=0;
   PrintFormat("GEP_CORE_ELEVATION case=%1 nonce=%2 phase=down degrees=%3 physical=0",m_Case,m_Nonce,m_DownMotionQA);
   m_LeftStartQA=m_LeftQA.GetAmmoCount();m_RightStartQA=m_RightQA.GetAmmoCount();m_Stage=7;m_StageTime=m_Time;
  }
  if(m_Stage==7)
  {
   InputQA(0,1);
   if(!m_BurstFrame1QA && m_Time-m_StageTime>=0.12)m_BurstFrame1QA=CaptureBurstQA("burst-012");
   if(!m_BurstFrame2QA && m_Time-m_StageTime>=0.32)m_BurstFrame2QA=CaptureBurstQA("burst-032");
   if(m_Time-m_StageTime<0.6)return;
   InputQA(0,0);m_LeftUsedQA=m_LeftStartQA-m_LeftQA.GetAmmoCount();m_RightUsedQA=m_RightStartQA-m_RightQA.GetAmmoCount();
   TraceCompatPoseQA("first_burst_complete");
   PrintFormat("QA_POSTHIT_FIRE leftUsed=%1 rightUsed=%2 physicalInput=0",m_LeftUsedQA,m_RightUsedQA);
   GetGame().GetInputManager().SetActionValue("TurretADSHold",0);m_ControlQA.SetWeaponADS(false);
   m_Stage=3;BeginEgress();
  }
  if(m_Stage==8)
  {
   // Wait for the completed public exit to settle before one entry request.
   if(m_Time-m_StageTime<2)return;
   if(m_Time-m_StageTime>25){Finish("FAIL","public_reentry_timeout_QA");return;}
   if(CrewReady())
   {
    m_ReentryCompletedQA=true;m_ControlQA.SetWeaponADS(true);InputQA(0,0);
    PlayerController restoredPlayerQA=GetGame().GetPlayerController();
    restoredPlayerQA.SetCharacterCameraRenderActive(true);
    if(restoredPlayerQA.GetPlayerCamera())GetGame().GetCameraManager().SetCamera(restoredPlayerQA.GetPlayerCamera());
    m_Stage=10;m_StageTime=m_Time;return;
   }
   if(m_ReentryRequestedQA)return;
   SCR_CompartmentUserAction entry=SCR_CompartmentUserAction.Cast(m_Seat.GetGetInAction());
   BaseActionsManagerComponent actions=BaseActionsManagerComponent.Cast(m_Seat.GetOwner().FindComponent(BaseActionsManagerComponent));
   if(!entry || !actions){Finish("NOT_TESTED","public_entry_action_context_missing_QA");return;}
   UserActionContext context=actions.GetContext("gunner_entry");if(!context)return;
   entry.SetActiveContext(context);
   if(!entry.CanBeShown(m_Crew) || !entry.CanBePerformed(m_Crew))return;
   TraceReentryQA(entry);
   m_ReentryRequestedQA=true;entry.OnActionStart(m_Crew);entry.PerformAction(m_Seat.GetOwner(),m_Crew);
  }
  if(m_Stage==10)
  {
   m_ControlQA.SetWeaponADS(true);InputQA(0,0);
   if(m_Time-m_StageTime<2)return;
   bool restoredADSQA=m_ControlQA.GetCurrentSightsADS();
   bool restoredContextQA=GetGame().GetInputManager().IsContextActive("TurretContext");
   if(!restoredADSQA || !restoredContextQA)
   {
    if(m_Time-m_StageTime>5)
    {
     PrintFormat("QA_REENTRY_INPUT ads=%1 context=%2 occupied=%3",restoredADSQA,restoredContextQA,CrewReady());
     Finish("NOT_TESTED","reentry_native_input_not_ready_QA");
    }
    return;
   }
   PrintFormat("QA_REENTRY_INPUT ads=%1 context=%2 occupied=%3",restoredADSQA,restoredContextQA,CrewReady());
   m_LeftStartQA=m_LeftQA.GetAmmoCount();m_RightStartQA=m_RightQA.GetAmmoCount();m_Stage=9;m_StageTime=m_Time;
  }
  if(m_Stage==9)
  {
   InputQA(0,1);if(m_Time-m_StageTime<1.5)return;
   InputQA(0,0);m_ReLeftUsedQA=m_LeftStartQA-m_LeftQA.GetAmmoCount();m_ReRightUsedQA=m_RightStartQA-m_RightQA.GetAmmoCount();
   TraceCompatPoseQA("reentry_burst_complete");
   PrintFormat("QA_REENTRY_FIRE leftUsed=%1 rightUsed=%2 physicalInput=0",m_ReLeftUsedQA,m_ReRightUsedQA);
   if(m_RightYawQA<1 || m_LeftYawQA<1 || m_UpMotionQA<0.5 || m_DownMotionQA>-0.5 || m_LeftUsedQA<1 || m_RightUsedQA<1 || m_ReLeftUsedQA<1 || m_ReRightUsedQA<1)
   {Finish("FAIL","measured_posthit_function_failed_QA");return;}
   Finish("PASS","native_yaw_elevation_both_feeds_exit_reentry_fire_completed_QA");
  }
 }
 override void EOnPostFrame(IEntity owner,float timeSlice)
 {
  float dt=timeSlice;
  if(!GetGame().InPlayMode() || RplSession.Mode()!=RplMode.None)return;
  m_EnteredPlay=true;m_Time+=dt;
  // The coordinator wrapper owns normal closure. Never close a human session.
  if(m_Finished)return;
  if(!m_Loaded){m_Loaded=true;if(!Load()){Finish("FAIL","invalid_or_missing_job");return;}}
  if(m_Time>m_Max){Finish("FAIL","bounded_game_timeout");return;}
  if(!m_Subject && m_Time>3)
  {
   vector p=m_Position.ToVector();p[1]=GetGame().GetWorld().GetSurfaceY(p[0],p[2])+p[1];
   m_Subject=Spawn(m_Prefab,p);if(!m_Subject){Finish("FAIL","subject_spawn_failed");return;}
   m_SpawnTime=m_Time;GetGame().BeginPreload(GetGame().GetWorld(),p,100,20);WatchTree(m_Subject);PrepareCrew();
   PrintFormat("GEP_DAMAGE683_SUBJECT|%1|%2|%3|%4",m_Id,m_Nonce,m_Prefab,m_Subject.GetOrigin());
  }
  if(!m_Subject){return;}
  if(m_Stage<4)Camera(dt);
  if(m_Stage==20)
  {
   m_WheelCourseQA.AfterPhysics();
   if(m_WheelCourseQA.IsCourseFinishedQA())
   {Finish("NOT_TESTED",m_WheelCourseQA.Reason);return;}
  }
  else if(m_Stage>=4)OperationQA();
  if(m_Stage==0 && m_Time-m_SpawnTime>=m_Settle)
  {
   if(!CrewReady()){Finish("FAIL","scripted_crew_setup_incomplete");return;}
   Physics body=m_Subject.GetPhysics();if(body && body.GetVelocity().Length()>0.25){Finish("NOT_TESTED","subject_not_settled");return;}
   if(!PrepareGeometry()){Finish("NOT_TESTED","native_preflight_missed_subject");return;}
   SCR_DamageManagerComponent fresh=SCR_DamageManagerComponent.Cast(m_Subject.FindComponent(SCR_DamageManagerComponent));
   if(!fresh || !fresh.GetDefaultHitZone() || fresh.GetDefaultHitZone().GetHealthScaled()<0.9999){Finish("FAIL","fresh_subject_predamaged_during_setup");return;}
   // Separate startup/settling notifications from the measured test window.
   foreach(GEP_DamageRecordSubscription683 reset:m_Watches){reset.Records683.Clear();reset.Seen683=0;reset.Dropped683=0;}
   m_ImpactNonce688=m_Nonce+"/"+m_Case;
   if(!GEP688_ImpactInbox.Open(m_ImpactNonce688,m_Subject)){Finish("FAIL","impact_case_open_failed688");return;}
   Snapshot(m_Before);m_StartPosition=m_Subject.GetOrigin();m_BeforeFrame=Screenshot("before");m_Stage=1;m_StageTime=m_Time;
   PrintFormat("GEP_DAMAGE683_READY|%1|%2",m_Id,m_Nonce);
  }
  if(m_Stage==1)
  {
   foreach(GEP_NativeStimulus683 stimulus:m_Stimuli)stimulus.Tick(dt);
   foreach(GEP_NativeStimulus683 checked:m_Stimuli)if(checked.Error()!=""){Finish("FAIL",checked.Error());return;}
   if(m_Mode=="none"){m_Stage=2;m_StageTime=m_Time;}
   else if(m_Issued<m_Count && m_Time-m_LastIssue>=m_Interval)
   {
    GEP_NativeStimulus683 next=new GEP_NativeStimulus683();
    if(!next.Begin(true,m_Mode=="native-blast",Payload(),this,m_Source,m_Aim,m_ImpactNonce688)){Finish("FAIL","native_stimulus_setup_failed");return;}
    m_Stimuli.Insert(next);m_Issued++;m_LastIssue=m_Time;
   }
   else if(m_Issued==m_Count)
   {
    bool allDone=true;foreach(GEP_NativeStimulus683 done:m_Stimuli)if(!done.Done() || !done.CommandIssued())allDone=false;
    if(allDone){m_Stage=2;m_StageTime=m_Time;}
   }
  }
  if(m_Stage==2 && m_Time-m_StageTime>=m_Sample)
  {
   Snapshot(m_After);ObserveOperation688();
   if(m_Mode=="none")
   {
    if(m_After.Count()!=m_Before.Count()){Finish("FAIL","baseline_zone_count_changed");return;}
    for(int i=0;i<m_Before.Count();i++)if(Math.AbsFloat(m_Before[i].Health-m_After[i].Health)>0.0001){Finish("FAIL","baseline_health_changed");return;}
    if(EventCount()>0){Finish("FAIL","baseline_damage_event");return;}
   }
   else if(EventCount()==0)
   {
    GEP688_ImpactReport actualImpact688=GEP688_ImpactInbox.Snapshot(m_ImpactNonce688);
    if(!actualImpact688 || actualImpact688.ValidSubjectCallbacks==0){Finish("NOT_TESTED","no_subject_damage_or_registered_collision_receipt688");return;}
   }
   if(m_CrewRole=="driver")
   {
    m_WheelCourseQA=new GEP_CoreWheeledCourse20261022();
    if(!m_WheelCourseQA.Begin(m_Subject,m_Crew,m_Seat,m_Case,m_Nonce))
    {Finish("NOT_TESTED","wheeled_course_setup_unqualified_QA");return;}
    m_Stage=20;m_StageTime=m_Time;return;
   }
   if(m_RequestEgress){m_Stage=4;m_StageTime=m_Time;}

   else Finish("PASS","native_fixture_measured_no_armor_balance_claim");
  }
  if(m_Stage==3)
  {
   if(m_EgressReason!="native_public_exit_requested_once"){Finish(m_EgressStatus,"damage_measured_required_egress_"+m_EgressReason);return;}
   CompartmentAccessComponent access=CompartmentAccessComponent.Cast(m_Crew.FindComponent(CompartmentAccessComponent));
   CharacterControllerComponent survivor=CharacterControllerComponent.Cast(m_Crew.FindComponent(CharacterControllerComponent));
   bool conscious=survivor && survivor.GetLifeState()==ECharacterLifeState.ALIVE && !survivor.IsUnconscious();
   if(conscious && access && !access.GetCompartment() && m_Seat.GetOccupant()!=m_Crew && !access.IsGettingIn() && !access.IsGettingOut() && !access.IsSwitchingSeatsAnim())
   {
    m_EgressCompleted=true;m_EgressStatus="PASS";m_EgressReason="native_compartment_transition_completed_scripted";
    m_Stage=8;m_StageTime=m_Time;return;
   }
   if(m_Time-m_StageTime>8){m_EgressStatus="FAIL";m_EgressReason="native_exit_transition_timeout";Finish("FAIL","damage_measured_required_egress_failed");}
  }
 }
 void ~GEP_DamageScenario683()
 {
  foreach(GEP_DamageRecordSubscription683 watch:m_Watches)watch.Close683();
 }
}

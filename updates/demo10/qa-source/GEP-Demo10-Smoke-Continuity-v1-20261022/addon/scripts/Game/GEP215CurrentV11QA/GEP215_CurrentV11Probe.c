// Original bounded QA: public WCS program entry, actual settled crew, read-only observations.
// No WCS implementation copied, no custom projectile/arming/physics/particle state mutation.
[BaseContainerProps(category:"Respawn")]
class GEP215_CurrentV11SpawnLogic : SCR_AutoSpawnLogic
{
 override protected void DoSpawn_S(int playerId)
 {if(RplSession.Mode()==RplMode.None)return;super.DoSpawn_S(playerId);}
}
class GEP215_CurrentV11ProbeClass : GenericEntityClass {}
class GEP215_CurrentV11Probe : GenericEntity
{
 [Attribute("1")] int ExpectedShots;
 [Attribute("{2150FC6F1E081188}Prefabs/ArtApproved20261022/Camo/GEP_MobileCarrier.et", UIWidgets.ResourceNamePicker, "Exact original test vehicle", "et")] ResourceName VehiclePrefab;
 [Attribute("0")] float FixtureYaw;
 [Attribute("0")] bool CaptureEnabled;
 bool CaptureUnavailable;bool CaptureViewReady;bool CaptureSetupLogged;
 string CaptureFailureReason;
 bool BaselineCaptured;bool CapturePending;bool CaptureFar;float CaptureReadyMs;vector CaptureEye;vector CaptureAim;
 static const ResourceName PAYLOAD="{F8EE668DCA4ACE4E}Prefabs/Weapons/Ammo/WCS/Ammo_Smoke_CROWS.et";
 IEntity Hull;IEntity Actor;IEntity ProbeTurret;BaseCompartmentSlot Seat;
 CameraBase m_SmokeCaptureCamera215;
 WCS_Armament_DispenserManagerComponent Manager;
 ref array<IEntity> Rounds={};ref array<vector> Previous={};ref array<float> Travel={};
 float StartMs=-1;float Age;float ProgramMs=-1;float LastSample=-1;
 bool Bound;bool BoardRequested;bool ProgramRequested;bool EmptyRequested;int Terminal;int SmokeSampleSequence;int FrameIndex;
 vector LastCloudTarget;bool HaveCloudTarget;
 void GEP215_CurrentV11Probe(IEntitySource src,IEntity parent)
 {SetEventMask(EntityEvent.FRAME);SetFlags(EntityFlags.ACTIVE,true);}
 IEntity SpawnFixture(ResourceName prefab,vector position,float clearance)
 {
  Resource resource=Resource.Load(prefab);if(!resource || !resource.IsValid())return null;
  EntitySpawnParams params=new EntitySpawnParams();params.TransformMode=ETransformMode.WORLD;
  position[1]=GetWorld().GetSurfaceY(position[0],position[2])+clearance;params.Transform[3]=position;
  if(prefab==VehiclePrefab)Math3D.AnglesToMatrix(Vector(FixtureYaw,0,0),params.Transform);
  params.Transform[3]=position;
  return GetGame().SpawnEntityPrefab(resource,GetWorld(),params);
 }
 BaseCompartmentSlot FindSeat(IEntity entity,int depth=0)
 {
  if(!entity || depth>16)return null;
  BaseCompartmentManagerComponent compartments=BaseCompartmentManagerComponent.Cast(entity.FindComponent(BaseCompartmentManagerComponent));
  if(compartments){array<BaseCompartmentSlot> slots={};compartments.GetCompartments(slots);foreach(BaseCompartmentSlot slot:slots)if(TurretCompartmentSlot.Cast(slot))return slot;}
  for(IEntity child=entity.GetChildren();child;child=child.GetSibling()){BaseCompartmentSlot found=FindSeat(child,depth+1);if(found)return found;}return null;
 }
 bool Settled()
 {
  if(!Actor || !Seat || !Hull || !Manager)return false;
  SCR_CharacterControllerComponent life=SCR_CharacterControllerComponent.Cast(Actor.FindComponent(CharacterControllerComponent));
  CompartmentAccessComponent access=CompartmentAccessComponent.Cast(Actor.FindComponent(CompartmentAccessComponent));Physics body=Hull.GetPhysics();
  return life && life.GetLifeState()==ECharacterLifeState.ALIVE && !life.IsUnconscious() && access && access.GetCompartment()==Seat && Seat.GetOccupant()==Actor && !access.IsGettingIn() && !access.IsGettingOut() && !access.IsSwitchingSeatsAnim() && body && body.GetVelocity().Length()<=0.25 && body.GetAngularVelocity().Length()<=0.05;
 }
 int Ammo()
 {
  if(!Manager)return -1;
  int smokeAmmo=Manager.GetCurrentProjectileAmount(WCS_Armament_SmokeDispenserComponent);
  return smokeAmmo;
 }
 string CurrentV11EntityId(IEntity entity)
 {
  if(!entity)
  {
   return "NULL";
  }
  EntityID fullNativeId=entity.GetID();
  return string.Format("%1",fullNativeId);
 }
 protected ref array<string> m_aCurrentV11BankKeys={};
 protected IEntity m_CurrentV11LeftBankOwner;
 protected IEntity m_CurrentV11RightBankOwner;
 protected int m_nCurrentV11BankNodes;
 protected int m_nCurrentV11BankCapacity;
 protected int m_nCurrentV11BankCurrent;
 protected bool m_bCurrentV11BankReadValid;
 void ReadCurrentV11Banks(IEntity entity,string phase,int depth=0)
 {
  if(!entity || entity.IsDeleted())return;
  if(depth>16 || m_nCurrentV11BankNodes>=512)
  {
   m_bCurrentV11BankReadValid=false;
   return;
  }
  m_nCurrentV11BankNodes++;
  array<Managed> components={};
  entity.FindComponents(WCS_Armament_SmokeDispenserComponent,components);
  foreach(Managed componentEntry:components)
  {
   WCS_Armament_DispenserComponent dispenser=WCS_Armament_DispenserComponent.Cast(componentEntry);
   if(!dispenser)
   {
    m_bCurrentV11BankReadValid=false;
    continue;
   }
   int bankId=dispenser.GetId();
   string bankOwnerId=CurrentV11EntityId(entity);
   string bankKey=string.Format("%1|%2",bankOwnerId,bankId);
   if(bankId<0 || bankId>3 || m_aCurrentV11BankKeys.Find(bankKey)>=0)m_bCurrentV11BankReadValid=false;
   m_aCurrentV11BankKeys.Insert(bankKey);
   int capacity=dispenser.GetMaxProjectileAmount();
   int current=dispenser.GetCurrentProjectileAmount();
   m_nCurrentV11BankCapacity=m_nCurrentV11BankCapacity+capacity;
   m_nCurrentV11BankCurrent=m_nCurrentV11BankCurrent+current;
   if(capacity!=1 || current<0 || current>capacity)m_bCurrentV11BankReadValid=false;
   string ownerPrefab="NULL";
   if(entity.GetPrefabData())ownerPrefab=entity.GetPrefabData().GetPrefabName();
   if(ownerPrefab=="{7E0150C026101503}Prefabs/Weapons/GEP_WCSSmokeBankLeft215.et")
   {
    if(m_CurrentV11LeftBankOwner && m_CurrentV11LeftBankOwner!=entity)m_bCurrentV11BankReadValid=false;
    m_CurrentV11LeftBankOwner=entity;
   }
   else if(ownerPrefab=="{7E0150C026101504}Prefabs/Weapons/GEP_WCSSmokeBankRight215.et")
   {
    if(m_CurrentV11RightBankOwner && m_CurrentV11RightBankOwner!=entity)m_bCurrentV11BankReadValid=false;
    m_CurrentV11RightBankOwner=entity;
   }
   else
   {
    m_bCurrentV11BankReadValid=false;
   }
   PrintFormat("GEP215_CURRENT_BANK phase=%1 id=%2 owner=%3 current=%4 capacity=%5 ownerPrefab=%6",phase,bankId,CurrentV11EntityId(entity),current,capacity,ownerPrefab);
   // These are bank ENTITY world axes only. They are NOT actual tube fire matrices.
   PrintFormat("GEP215_CURRENT_BANK_OWNER phase=%1 id=%2 origin=%3 axisX=%4 axisY=%5 axisZ=%6 actualTubeFireMatrix=NOT_AVAILABLE",phase,bankId,entity.GetOrigin(),entity.GetWorldTransformAxis(0),entity.GetWorldTransformAxis(1),entity.GetWorldTransformAxis(2));
  }
  for(IEntity child=entity.GetChildren();child;child=child.GetSibling())
  {
   ReadCurrentV11Banks(child,phase,depth+1);
  }
 }
 bool ReadCurrentV11Compatibility(string phase)
 {
  if(!Hull || Hull.IsDeleted() || !Hull.GetPrefabData() || !Manager)return false;
  ResourceName actual=Hull.GetPrefabData().GetPrefabName();
  bool resourceMatches=actual==VehiclePrefab;
  bool approvedVariant=VehiclePrefab=="{2150FC6F1E081188}Prefabs/ArtApproved20261022/Camo/GEP_MobileCarrier.et" || VehiclePrefab=="{8095474EC9806D0D}Prefabs/ArtApproved20261022/Charcoal/GEP_MobileCarrier.et";
  PrintFormat("GEP215_CURRENT_RESOURCE phase=%1 requested=%2 actual=%3 matches=%4 approvedVariant=%5 hull=%6 turret=%7 authoredHullYaw=%8",phase,VehiclePrefab,actual,resourceMatches,approvedVariant,CurrentV11EntityId(Hull),CurrentV11EntityId(ProbeTurret),FixtureYaw);
  PrintFormat("GEP215_CURRENT_POSE phase=%1 hullOrigin=%2 hullNativeAngles=%3 turretOrigin=%4 turretNativeAngles=%5",phase,Hull.GetOrigin(),Hull.GetAngles(),ProbeTurret.GetOrigin(),ProbeTurret.GetAngles());
  m_aCurrentV11BankKeys.Clear();
  m_CurrentV11LeftBankOwner=null;
  m_CurrentV11RightBankOwner=null;
  m_nCurrentV11BankNodes=0;
  m_nCurrentV11BankCapacity=0;
  m_nCurrentV11BankCurrent=0;
  m_bCurrentV11BankReadValid=true;
  ReadCurrentV11Banks(Hull,phase);
  PrintFormat("GEP215_CURRENT_BANK_TOTAL phase=%1 count=%2 capacity=%3 current=%4 valid=%5",phase,m_aCurrentV11BankKeys.Count(),m_nCurrentV11BankCapacity,m_nCurrentV11BankCurrent,m_bCurrentV11BankReadValid);
  WorkspaceWidget workspace=GetGame().GetWorkspace();
  Widget originalSight;Widget plainSight;
  if(workspace)
  {
   originalSight=workspace.FindAnyWidget("GepardOriginalScreen583");
   plainSight=workspace.FindAnyWidget("GepardPlainSight680");
  }
  bool originalVisible=false;bool plainVisible=false;
  if(originalSight)originalVisible=originalSight.IsVisible();
  if(plainSight)plainVisible=plainSight.IsVisible();
  TurretControllerComponent currentControl=TurretControllerComponent.Cast(ProbeTurret.FindComponent(TurretControllerComponent));
  bool actualADS=false;
  if(currentControl)actualADS=currentControl.GetCurrentSightsADS();
  PrintFormat("GEP215_CURRENT_HUD phase=%1 workspace=%2 originalSight=%3 originalVisible=%4 plainSight=%5 plainVisible=%6 actualADS=%7 observationOnly=1",phase,workspace!=null,originalSight!=null,originalVisible,plainSight!=null,plainVisible,actualADS);
  if(!resourceMatches || !approvedVariant || FixtureYaw!=0)return false;
  if(!m_bCurrentV11BankReadValid || m_aCurrentV11BankKeys.Count()!=8 || m_nCurrentV11BankCapacity!=8)return false;
  if(!m_CurrentV11LeftBankOwner || !m_CurrentV11RightBankOwner || m_CurrentV11LeftBankOwner==m_CurrentV11RightBankOwner)return false;
  if(phase=="preflight" && m_nCurrentV11BankCurrent!=8)return false;
  return true;
 }
 bool ObserveCandidate(IEntity entity)
 {
  if(!entity || entity.IsDeleted() || !entity.GetPrefabData() || entity.GetPrefabData().GetPrefabName()!=PAYLOAD)return true;
  WCS_Armament_DispenserProjectileComponent state=WCS_Armament_DispenserProjectileComponent.Cast(entity.FindComponent(WCS_Armament_DispenserProjectileComponent));
  if(!state || !state.HasLaunched())return true;
  if(Rounds.Find(entity)<0){ProjectileMoveComponent move=ProjectileMoveComponent.Cast(entity.FindComponent(ProjectileMoveComponent));vector velocity;if(move)velocity=move.GetVelocity();
   PrintFormat("GEP215_WCS_LAUNCH_VECTOR id=%1 forward=%2 actualVelocity=%3 hull=%4",entity.GetID(),entity.GetTransformAxis(2),velocity,Hull.GetOrigin());
   Rounds.Insert(entity);Previous.Insert(entity.GetOrigin());Travel.Insert(0);PrintFormat("GEP215_WCS_ENTITY discovered=%1 id=%2 actualOrigin=%3 launched=%4 launchAge=%5",Rounds.Count(),string.Format("%1",entity.GetID()),entity.GetOrigin(),state.HasLaunched(),state.GetLaunchAge());}
  return true;
 }
 void Observe()
 {
  LastSample=GetWorld().GetWorldTime();GetWorld().QueryEntitiesBySphere(Hull.GetOrigin(),120,ObserveCandidate);
  if(Rounds.Count()>ExpectedShots){Stop("more_payload_entities_than_expected_no_retry");return;}
  SmokeSampleSequence++;JsonSaveContext row=new JsonSaveContext();row.WriteValue("scope","WCS_DEPENDENCY_ENGINEERING_DIAGNOSTIC");row.WriteValue("sequence",SmokeSampleSequence);row.WriteValue("ageSeconds",Age);row.WriteValue("elapsedSinceProgram",(LastSample-ProgramMs)/1000.0);row.WriteValue("ammo",Ammo());row.WriteValue("knownPayloadEntities",Rounds.Count());
  array<string> identities={};array<vector> origins={};array<float> travels={};array<float> launchAges={};
  foreach(int i,IEntity round:Rounds)
  {
   if(!round || round.IsDeleted())continue;
   vector actual=round.GetOrigin();if(vector.Distance(Hull.GetOrigin(),actual)>200)continue;
   Travel[i]=Travel[i]+vector.Distance(Previous[i],actual);Previous[i]=actual;
   identities.Insert(string.Format("%1",round.GetID()));origins.Insert(actual);travels.Insert(Travel[i]);
   WCS_Armament_DispenserProjectileComponent state=WCS_Armament_DispenserProjectileComponent.Cast(round.FindComponent(WCS_Armament_DispenserProjectileComponent));
   if(state)launchAges.Insert(state.GetLaunchAge());else launchAges.Insert(-1);
   if(i==0){LastCloudTarget=actual;HaveCloudTarget=true;}
  }
  row.WriteValue("entityIds",identities);row.WriteValue("actualOrigins",origins);row.WriteValue("travelObserved",travels);row.WriteValue("launchAges",launchAges);
  row.WriteValue("fuseContract","NATIVE_SHELL_IMPACT_EFFECT_NO_ANM8_TIMER_EXPECTED");row.WriteValue("visibleCloud","NEEDS_PIXEL_REVIEW");row.WriteValue("rearm","NOT_TESTED");row.WriteValue("physicalInput","NOT_TESTED");
  row.SaveToFile(string.Format("$profile:GEP215-wcs-life-%1.json",SmokeSampleSequence));PrintFormat("GEP215_WCS_SAMPLE seq=%1 ammo=%2 entities=%3 positions=%4 travel=%5",SmokeSampleSequence,Ammo(),Rounds.Count(),origins,travels);
 }
 void DisableCapture(string reason)
 {
  CaptureUnavailable=true;
  CaptureViewReady=false;
  CapturePending=false;
  CaptureFailureReason=reason;
  PlayerController smokePlayer215=GetGame().GetPlayerController();
  if(Bound && smokePlayer215 && smokePlayer215.GetControlledEntity()==Actor)
  {
   smokePlayer215.SetCharacterCameraRenderActive(true);
  }
  PrintFormat("GEP215_WCS_CAPTURE_UNAVAILABLE reason=%1 mechanismContinues=1",reason);
 }
 bool View(float timeSlice)
 {
  CaptureViewReady=false;
  if(!CaptureEnabled || CaptureUnavailable || !Bound)
  {
   return false;
  }
  if(!Hull || Hull.IsDeleted())
  {
   DisableCapture("hull_unavailable");
   return false;
  }
  CameraManager smokeCameraManager215=GetGame().GetCameraManager();
  if(!smokeCameraManager215)
  {
   DisableCapture("camera_manager_missing");
   return false;
  }
  if(!m_SmokeCaptureCamera215)
  {
   Print("GEP215_WCS_CAPTURE_SETUP stage=spawn_after_player_bound");
   m_SmokeCaptureCamera215=CameraBase.Cast(GetGame().SpawnEntity(CameraBase,GetGame().GetWorld()));
  }
  CameraBase smokeCamera215=m_SmokeCaptureCamera215;
  if(!smokeCamera215 || smokeCamera215.IsDeleted())
  {
   DisableCapture("camera_spawn_or_lifetime_failed");
   return false;
  }
  vector aim=Hull.GetOrigin()+Vector(0,1,6);if(HaveCloudTarget)aim=LastCloudTarget+Vector(0,1,0);
  vector eye=aim+Vector(10,5,-10);if(CaptureFar){eye=Hull.GetOrigin()+Hull.GetTransformAxis(2)*60+Vector(0,4,0);aim=Hull.GetOrigin()+Vector(0,1.5,0);}CaptureEye=eye;CaptureAim=aim;vector pose[4];Math3D.DirectionAndUpMatrix((aim-eye).Normalized(),vector.Up,pose);pose[3]=eye;
  if(!CaptureSetupLogged)Print("GEP215_WCS_CAPTURE_SETUP stage=before_world_transform");
  smokeCamera215.SetWorldTransform(pose);
  if(!CaptureSetupLogged)Print("GEP215_WCS_CAPTURE_SETUP stage=before_fov");
  smokeCamera215.SetFOVDegree(70);
  if(!CaptureSetupLogged)Print("GEP215_WCS_CAPTURE_SETUP stage=before_select");
  bool selected=smokeCameraManager215.SetCamera(smokeCamera215);
  if(!selected)
  {
   DisableCapture("camera_selection_rejected");
   return false;
  }
  if(!smokeCamera215 || smokeCamera215.IsDeleted() || !m_SmokeCaptureCamera215)
  {
   DisableCapture("camera_lost_during_selection");
   return false;
  }
  if(smokeCameraManager215.CurrentCamera()!=smokeCamera215)
  {
   DisableCapture("selected_camera_not_current");
   return false;
  }
  if(!CaptureSetupLogged)Print("GEP215_WCS_CAPTURE_SETUP stage=before_apply");
  smokeCamera215.ApplyTransform(timeSlice);
  if(!CaptureSetupLogged)Print("GEP215_WCS_CAPTURE_SETUP stage=ready");
  CaptureSetupLogged=true;
  CaptureViewReady=true;
  return true;
 }
 void Stop(string reason)
 {
  if(Terminal!=0)return;Terminal=1;ClearEventMask(EntityEvent.FRAME);
  if(Hull && ProbeTurret && Manager)ReadCurrentV11Compatibility("terminal");
  JsonSaveContext result=new JsonSaveContext();result.WriteValue("scope","WCS_DEPENDENCY_ENGINEERING_DIAGNOSTIC");result.WriteValue("reason",reason);result.WriteValue("programRequested",ProgramRequested);result.WriteValue("emptyProgramRequested",EmptyRequested);result.WriteValue("settledCrew",Settled());result.WriteValue("ammo",Ammo());result.WriteValue("knownPayloadEntities",Rounds.Count());result.WriteValue("screenshotsRequested",FrameIndex/2);result.WriteValue("pairedViewRequests",FrameIndex);result.WriteValue("captureEnabled",CaptureEnabled);result.WriteValue("captureUnavailable",CaptureUnavailable);result.WriteValue("captureFailureReason",CaptureFailureReason);result.WriteValue("vehiclePrefab",VehiclePrefab);result.WriteValue("fixtureYaw",FixtureYaw);result.WriteValue("visibleCloud","NEEDS_PIXEL_REVIEW");result.WriteValue("rearmCooldownRolesMPPhysicalInput","NOT_TESTED");result.SaveToFile("$profile:GEP215-wcs-result.json");
  PrintFormat("GEP215_WCS_TERMINAL reason=%1 ammo=%2 rounds=%3",reason,Ammo(),Rounds.Count());
 }
 override void EOnFrame(IEntity owner,float timeSlice)
 {
  if(Terminal!=0 || !GetGame().InPlayMode() || System.IsConsoleApp() || RplSession.Mode()!=RplMode.None)return;
  float now=GetWorld().GetWorldTime();if(StartMs<0)StartMs=now;Age=(now-StartMs)/1000.0;
  if(!Hull && Age>1){Hull=SpawnFixture(VehiclePrefab,"600 0 2600",0.7);Actor=SpawnFixture("{26A9756790131354}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Rifleman.et","605 0 2595",0.1);if(!Hull || !Actor){Stop("fixture_spawn_failed");return;}}
  PlayerController player=GetGame().GetPlayerController();SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());SCR_RespawnSystemComponent respawn=SCR_RespawnSystemComponent.GetInstance();
  if(Hull && !Bound && Age>3 && player && mode && mode.IsRunning() && respawn){player.SetControlledEntity(Actor);respawn.DestroyLoadingPlaceholder();player.SetCharacterCameraRenderActive(!CaptureEnabled);SCR_HintManagerComponent.HideHint();Bound=true;PrintFormat("GEP215_WCS_CAPTURE_MODE enabled=%1",CaptureEnabled);}
  if(Bound && !BoardRequested && Age>5)
  {
   Seat=FindSeat(Hull);if(!Seat){Stop("gunner_seat_missing");return;}ProbeTurret=Seat.GetOwner();Manager=WCS_Armament_DispenserManagerComponent.Cast(ProbeTurret.FindComponent(WCS_Armament_DispenserManagerComponent));
   CompartmentAccessComponent access=CompartmentAccessComponent.Cast(Actor.FindComponent(CompartmentAccessComponent));BoardRequested=true;
   if(!Manager || !access || !access.GetInVehicle(ProbeTurret,Seat,true,-1,ECloseDoorAfterActions.INVALID,true)){Stop("manager_or_native_boarding_rejected");return;}
  }
  if(CaptureEnabled && !CaptureUnavailable && BoardRequested && !ProgramRequested && Age>8 && !BaselineCaptured)
  {
   CaptureFar=true;
   bool baselineViewReady215=View(timeSlice);
   if(Age>9 && baselineViewReady215){BaselineCaptured=true;System.MakeScreenshot("$profile:GEP215-wcs-baseline-far");PrintFormat("GEP215_WCS_BASELINE eye=%1 aim=%2",CaptureEye,CaptureAim);}
  }
  if(BoardRequested && !ProgramRequested && Age>10 && Settled())
  {
   int capacity=Manager.GetMaxProjectileAmount(WCS_Armament_SmokeDispenserComponent);
   PrintFormat("GEP215_WCS_PREFLIGHT crew=settled seatName=%1 ammo=%2 capacity=%3",Seat.GetCompartmentUniqueName(),Ammo(),capacity);
   // Full-only private QA admission: read serialized value before any program.
   PrintFormat("GEP215_WCS_FULL_SCOPE expectedShots=%1 captureEnabled=%2 programRequested=%3",ExpectedShots,CaptureEnabled,ProgramRequested);
   if(ExpectedShots!=8){Stop("full_fixture_expected_shots_not_eight_no_program");return;}
   if(Ammo()!=8 || capacity!=8){Stop("initial_ammo_or_registration_not_eight");return;}
   if(!ReadCurrentV11Compatibility("preflight")){Stop("current_v11_resource_bank_or_scope_not_qualified_no_program");return;}
   ProgramRequested=true;ProgramMs=now;PrintFormat("GEP215_WCS_PROGRAM callback=RunProgramFromController_S requested=1 shotsExpected=%1 physicalInput=NOT_TESTED",ExpectedShots);
   Manager.RunProgramFromController_S(Actor,WCS_Armament_EDispenserType.SMOKE);
  }
  if(ProgramRequested)
  {
   float elapsed=(now-ProgramMs)/1000.0;float sampleMS=500;if(elapsed<6)sampleMS=100;if(elapsed<1)sampleMS=25;if(now-LastSample>=sampleMS)Observe();if(Terminal!=0)return;
   if(elapsed>3 && Ammo()!=8-ExpectedShots){Stop("ammo_change_did_not_match_expected_program");return;}
   if(ExpectedShots==8 && !EmptyRequested && elapsed>6){if(Ammo()!=0){Stop("unexpected_ammo_before_empty_probe");return;}EmptyRequested=true;Manager.RunProgramFromController_S(Actor,WCS_Armament_EDispenserType.SMOKE);Print("GEP215_WCS_EMPTY requested=1 expectedNoAdditionalPayload=1");}
   // Two views at each retained time. Camera changes are applied before a
   // screenshot request, with at least 100ms render dwell; retain actual pose/time.
   if(CaptureEnabled && !CaptureUnavailable)
   {
   int pair=FrameIndex/2;float frameTime=0.25;if(pair==1)frameTime=1;if(pair==2)frameTime=3;if(pair==3)frameTime=5;if(pair==4)frameTime=12;if(pair==5)frameTime=20;if(pair==6)frameTime=30;if(pair==7)frameTime=40;
   if(FrameIndex<16 && !CapturePending && elapsed>=frameTime){CaptureFar=(FrameIndex%2)==1;CapturePending=true;CaptureReadyMs=now+100;}
   if(CapturePending && CaptureViewReady && now>=CaptureReadyMs)
   {
    string view="near";if(CaptureFar)view="far";
    bool requested=System.MakeScreenshot(string.Format("$profile:GEP215-wcs-pair-%1-%2",pair+1,view));
    PrintFormat("GEP215_WCS_CAPTURE pair=%1 view=%2 elapsed=%3 requested=%4 eye=%5 aim=%6",pair+1,view,elapsed,requested,CaptureEye,CaptureAim);
    // Retain old near filenames for the existing analyzer's capture contract.
    if(!CaptureFar)PrintFormat("GEP215_WCS_FRAME index=%1 elapsed=%2 requested=%3 target=%4",pair+1,elapsed,requested,LastCloudTarget);
    if(!CaptureFar)System.MakeScreenshot(string.Format("$profile:GEP215-wcs-frame-%1",pair+1));
    FrameIndex++;CapturePending=false;
   }
   }
   if(elapsed>=42){Stop("diagnostic_complete_acceptance_pending");return;}
  }
  if(CaptureEnabled && !CaptureUnavailable)View(timeSlice);
  if(Age>=65)Stop("fixture_deadline_no_retry");
 }
}

// Original bounded normal-AI admission. No forced seat, player-control switch,
// existing entity transform, velocity, health, input or authority setter.
// Separate v3 explicitly prepares native streaming tiles then waits; v2 preserved.
class GPG_SoloDriver708
{
 protected IEntity m_Hull624,m_Turret624,m_Driver624,m_Gunner624;
 protected BaseCompartmentSlot m_GunnerSeat624;
 protected BaseSightsComponent m_GunnerSight624;
 protected SCR_AIGroup m_Group624;
 protected SCR_BoardingEntityWaypoint m_GetIn624;
 protected AIWaypoint m_Move624;
 protected ref array<AIWaypoint> m_Moves708={
}
;
 protected ref array<vector> m_Route708={
}
;
 protected vector m_DriverPoint708;
 protected IEntity m_ObserverHull708;
 protected SCR_AIVehicleUsageComponent m_Usage624;
 protected string m_Run624,m_Nonce624,m_Reason624="not_started";
 protected int m_State624,m_Samples624,m_NavFrames624;
 protected bool m_NavRequested624;
 protected float m_NavLogMs624=-1000;
 protected bool m_Done624,m_Moved624,m_Closed624,m_RetainRoute624;
 protected float m_StartMs624,m_LastSampleMs624=-1000;
 protected vector m_StartPosition624,m_MovePoint624;
 protected BaseCompartmentSlot m_SettlingPilot624;
 protected float m_PilotSettledMs624=-1;
 protected float m_PilotLogMs624=-1000;
 protected int m_PreviousPilotFlags624=-1;
 protected void ReadNav624(string phase624,vector point624)
 {
  AIWorld world624=GetGame().GetAIWorld();
  if(!world624)
  {
   return;
  }
  array<string> projects624=
  {
   "Soldiers","BTRlike","LowRes"
  }
  ;
  foreach(string project624:projects624)
  {
   NavmeshWorldComponent nav624=world624.GetNavmeshWorldComponent(project624);
   if(!nav624)
   {
    PrintFormat("GEP_MOVING624_AI_NAV phase=%1 project=%2 component=NULL",phase624,project624);
    continue;
   }
   PrintFormat("GEP_MOVING624_AI_NAV phase=%1 project=%2 component=%3 point=%4 tileLoaded=%5 tileValid=%6 tileRequested=%7",phase624,project624,nav624,point624,nav624.IsTileLoaded(point624),nav624.IsTileValid(point624),nav624.IsTileRequested(point624));
  }
 }
 protected IEntity Spawn624(ResourceName prefab624,vector position624)
 {
  Resource resource624=Resource.Load(prefab624);
  if(!resource624 || !resource624.IsValid())
  {
   return null;
  }
  EntitySpawnParams params624=new EntitySpawnParams();
  params624.TransformMode=ETransformMode.WORLD;
  params624.Transform[3]=position624;
  return GetGame().SpawnEntityPrefab(resource624,GetGame().GetWorld(),params624);
 }
 protected bool OriginalGunner624()
 {
  if(!m_Hull624 || m_Hull624.IsDeleted() || !m_Turret624 || m_Turret624.IsDeleted())
  {
   return false;
  }
  IEntity user624;
  BaseCompartmentSlot seat624;
  BaseSightsComponent sight624;
  string reason624;
  return m_Turret624.GetRootParent()==m_ObserverHull708 && GPG_SoloRead708.ReadScope624(m_Turret624,user624,seat624,sight624,reason624) && user624==m_Gunner624 && seat624==m_GunnerSeat624 && sight624==m_GunnerSight624;
 }
 protected void Done624(string reason624)
 {
  if(m_Done624)
  {
   return;
  }
  m_Done624=true;
  m_Reason624=reason624;
  PrintFormat("GEP_MOVING624_AI_TERMINAL run=%1 nonce=%2 stage=%3 reason=%4 moved=%5 samples=%6 normalAI=1 movingStabilization=NOT_TESTED",m_Run624,m_Nonce624,m_State624,reason624,m_Moved624,m_Samples624);
  Close624();
 }
 void Configure708(vector driverPoint,array<vector> route)
 {
  if(m_Hull624)return;
m_DriverPoint708=driverPoint;
  foreach(vector point:route)m_Route708.Insert(point);
  m_RetainRoute624=true;
 }
 void Stop708(){
Done624("explicit_station_stop_REQUEST_NOT_HALT_PROOF");
}
 bool Closed708(){
return m_Closed624;
}
 void DisposeOwned708()
 {
  // Coordinator calls only after no human/unknown occupant, observed rest and player distance guards.
  Close624();
  if(m_Driver624 && !m_Driver624.IsDeleted())SCR_EntityHelper.DeleteEntityAndChildren(m_Driver624);
  if(m_Group624 && !m_Group624.IsDeleted())SCR_EntityHelper.DeleteEntityAndChildren(m_Group624);
  if(m_GetIn624 && !m_GetIn624.IsDeleted())SCR_EntityHelper.DeleteEntityAndChildren(m_GetIn624);
  foreach(AIWaypoint move708:m_Moves708)if(move708 && !move708.IsDeleted())SCR_EntityHelper.DeleteEntityAndChildren(move708);
 }
 bool Begin624(string run624,string nonce624,IEntity hull624,IEntity turret624)
 {
  if(m_Hull624 || !hull624 || !turret624 || run624=="" || nonce624=="")
  {
   return false;
  }
  IEntity user624;
  BaseCompartmentSlot seat624;
  BaseSightsComponent sight624;
  string reason624;
  if(!GPG_SoloRead708.ReadScope624(turret624,user624,seat624,sight624,reason624))
  {
   return false;
  }
  if(m_Route708.Count()==0 || m_Route708.Count()>8 || !turret624.GetRootParent())return false;
  m_ObserverHull708=turret624.GetRootParent();
  m_Run624=run624;
  m_Nonce624=nonce624;
  m_Hull624=hull624;
  m_Turret624=turret624;
  m_Gunner624=user624;
  m_GunnerSeat624=seat624;
  m_GunnerSight624=sight624;
  m_StartMs624=GetGame().GetWorld().GetWorldTime();
  m_StartPosition624=hull624.GetOrigin();
  // Root-selected physical corridor. Navmesh is deliberately NOT assumed.
  vector driverPoint624=m_DriverPoint708;
  driverPoint624[1]=GetGame().GetWorld().GetSurfaceY(driverPoint624[0],driverPoint624[2]);
  m_MovePoint624=m_Route708[0];
  m_MovePoint624[1]=GetGame().GetWorld().GetSurfaceY(m_MovePoint624[0],m_MovePoint624[2]);
  AIWorld world624=GetGame().GetAIWorld();
  if(!world624 || !world624.GetNavmeshWorldComponent("Soldiers") || !world624.GetNavmeshWorldComponent("BTRlike") || !world624.GetNavmeshWorldComponent("LowRes"))
  {
   Done624("AIWorld_or_required_navmesh_project_missing");
   return false;
  }
  PrintFormat("GEP_MOVING624_AI_WORLD world=%1 activeAIs=%2 canActivate=%3 originalScope=%4",world624,world624.GetCurrentNumOfActiveAIs(),world624.CanAIBeActivated(),OriginalGunner624());
  ReadNav624("driver-start",driverPoint624);
  ReadNav624("hull-start",m_StartPosition624);
  ReadNav624("route-target",m_MovePoint624);
  m_Group624=SCR_AIGroup.Cast(Spawn624("{EACD97CF4A702FAE}Prefabs/Groups/BLUFOR/Group_US_Base.et",driverPoint624));
  m_Driver624=Spawn624("{5D16D103D4C78A78}Prefabs/Characters/Campaign/Final/BLUFOR/US_army/Regular/Campaign_US_Player_Driver.et",driverPoint624);
  if(!m_Group624 || !m_Driver624)
  {
   Done624("group_or_driver_spawn_missing");
   return false;
  }
  PrintFormat("GEP_MOVING624_AI_BEGIN run=%1 nonce=%2 hull=%3 originalGunner=%4 driver=%5 group=%6 driverSpawn=%7 move=%8 navmesh=NOT_VERIFIED",run624,nonce624,GPG_SoloRead708.NativeID624(hull624),GPG_SoloRead708.NativeID624(user624),GPG_SoloRead708.NativeID624(m_Driver624),m_Group624,driverPoint624,m_MovePoint624);
  m_Reason624="waiting_group_initialization";
  return true;
 }
 bool Tick624()
 {
  if(m_Done624)
  {
   return true;
  }
  if(!OriginalGunner624())
  {
   Done624("original_ADS_gunner_scope_changed");
   return true;
  }
  float now624=GetGame().GetWorld().GetWorldTime();
  float elapsed624=(now624-m_StartMs624)/1000;
  if(GetGame().GetWorld().GetTimeScale()!=1)
  {
   Done624("timeScale_not_one");
   return true;
  }
  if(elapsed624>240 || (m_State624<2 && elapsed624>45))
  {
   Done624("AI_admission_timeout_"+m_Reason624);
   return true;
  }
  if(!m_Group624 || m_Group624.IsDeleted() || !m_Driver624 || m_Driver624.IsDeleted())
  {
   Done624("owned_group_or_driver_missing");
   return true;
  }
  if(m_State624==0)
  {
   if(m_Group624.IsInitializing() || !m_Group624.GetGroupUtilityComponent() || !m_Group624.GetGroupUtilityComponent().m_VehicleMgr)
   {
    return false;
   }
   if(m_Group624.GetAgentsCount()!=0)
   {
    Done624("fresh_group_not_empty");
    return true;
   }
   if(!m_Group624.AddAIEntityToGroup(m_Driver624))
   {
    Done624("normal_AI_add_rejected");
    return true;
   }
   m_State624=10;
   m_Reason624="waiting_exact_AI_membership";
   return false;
  }
  if(m_State624==10)
  {
   AIControlComponent ai624=AIControlComponent.Cast(m_Driver624.FindComponent(AIControlComponent));
   AIAgent agent624;
   if(ai624)
   {
    agent624=ai624.GetControlAIAgent();
   }
   if(agent624 && agent624.GetParentGroup() && agent624.GetParentGroup()!=m_Group624)
   {
    Done624("AI_joined_wrong_group");
    return true;
   }
   if(m_Group624.GetAgentsCount()>1)
   {
    Done624("group_has_extra_agents");
    return true;
   }
   if(!ai624 || !ai624.IsAIActivated() || !agent624 || agent624.GetParentGroup()!=m_Group624 || agent624.GetControlledEntity()!=m_Driver624 || m_Group624.GetAgentsCount()!=1)
   {
    return false;
   }
   AIPathfindingComponent path624=AIPathfindingComponent.Cast(m_Driver624.FindComponent(AIPathfindingComponent));
   if(!path624 || !path624.GetNavmeshComponent())
   {
    m_Reason624="waiting_actual_driver_pathfinding_project";
    return false;
   }
   AIWorld world624=GetGame().GetAIWorld();
   if(!world624 || path624.GetNavmeshComponent()!=world624.GetNavmeshWorldComponent("Soldiers"))
   {
    Done624("actual_driver_navmesh_project_not_Soldiers");
    return true;
   }
   NavmeshWorldComponent soldiers624=path624.GetNavmeshComponent();
   m_NavFrames624++;
   m_Reason624="waiting_requested_Soldiers_tiles_and_nearest_points";
   if(!m_NavRequested624)
   {
    m_NavRequested624=true;
    array<vector> required624=
    {
     m_Driver624.GetOrigin(),m_Hull624.GetOrigin(),m_MovePoint624
    }
    ;
    foreach(vector routePoint708:m_Route708)required624.Insert(routePoint708);
    int pointIndex624;
    foreach(vector requiredPoint624:required624)
    {
     bool loaded624=soldiers624.IsTileLoaded(requiredPoint624),pending624=soldiers624.IsTileRequested(requiredPoint624),request624;
     if(!loaded624 && !pending624)
     {
      request624=soldiers624.LoadTileIn(requiredPoint624);
     }
     PrintFormat("GEP_MOVING624_AI_NAV_REQUEST pointIndex=%1 point=%2 project=Soldiers loadedBefore=%3 requestedBefore=%4 nativeLoadCalled=%5 nativeReturn=%6 oncePerPoint=1",pointIndex624,requiredPoint624,loaded624,pending624,!loaded624 && !pending624,request624);
     array<string> vehicleProjects708={
"BTRlike","LowRes"}
;
     foreach(string vehicleProject708:vehicleProjects708)
     {
      NavmeshWorldComponent nav708=world624.GetNavmeshWorldComponent(vehicleProject708);
      if(nav708 && !nav708.IsTileLoaded(requiredPoint624) && !nav708.IsTileRequested(requiredPoint624))nav708.LoadTileIn(requiredPoint624);
     }
     pointIndex624++;
    }
   }
   vector closestDriver624,closestHull624,closestRoute624;
   bool driverOnNav624=path624.GetClosestPositionOnNavmesh(m_Driver624.GetOrigin(),"2 2 2",closestDriver624);
   bool hullOnNav624=path624.GetClosestPositionOnNavmesh(m_Hull624.GetOrigin(),"2 2 2",closestHull624);
   bool routeOnNav624=path624.GetClosestPositionOnNavmesh(m_MovePoint624,"2 2 2",closestRoute624);
   bool tilesReady624=soldiers624.IsTileLoaded(m_Driver624.GetOrigin()) && soldiers624.IsTileValid(m_Driver624.GetOrigin()) && soldiers624.IsTileLoaded(m_Hull624.GetOrigin()) && soldiers624.IsTileValid(m_Hull624.GetOrigin()) && soldiers624.IsTileLoaded(m_MovePoint624) && soldiers624.IsTileValid(m_MovePoint624);
   if(now624-m_NavLogMs624>=500)
   {
    m_NavLogMs624=now624;
    PrintFormat("GEP_MOVING624_AI_PATH driver=%1 component=%2 actualProject=Soldiers driverNear=%3 hullNear=%4 routeNear=%5",GPG_SoloRead708.NativeID624(m_Driver624),soldiers624,driverOnNav624,hullOnNav624,routeOnNav624);
    PrintFormat("GEP_MOVING624_AI_NAV_WAIT frames=%1 elapsed=%2 tilesReady=%3 requireMoreThanFrames=100 requestPhaseComplete=%4",m_NavFrames624,elapsed624,tilesReady624,m_NavRequested624);
    PrintFormat("GEP_MOVING624_AI_PATH_POINTS driver=%1 hull=%2 route=%3 correctedPointsReadOnly=1 vehicleSelectedProject=NOT_TESTED",closestDriver624,closestHull624,closestRoute624);
    ReadNav624("requested-driver",m_Driver624.GetOrigin());
    ReadNav624("requested-hull",m_Hull624.GetOrigin());
    ReadNav624("requested-route",m_MovePoint624);
   }
   bool allRouteReady708=true;
   foreach(vector extraPoint708:m_Route708)
   {
    vector nearest708;
    if(!path624.GetClosestPositionOnNavmesh(extraPoint708,"2 2 2",nearest708))allRouteReady708=false;
    array<string> projects708={
"Soldiers","BTRlike","LowRes"}
;
    foreach(string project708:projects708)
    {
     NavmeshWorldComponent routeNav708=world624.GetNavmeshWorldComponent(project708);
     if(!routeNav708 || !routeNav708.IsTileLoaded(extraPoint708) || !routeNav708.IsTileValid(extraPoint708))allRouteReady708=false;
    }
   }
   if(m_NavFrames624<=100 || !tilesReady624 || !driverOnNav624 || !hullOnNav624 || !routeOnNav624 || !allRouteReady708)
   {
    return false;
   }
   PrintFormat("GEP_MOVING624_AI_NAV_READY frames=%1 actualProject=Soldiers allTilesLoadedValid=1 allNearestQueries=1 coverageBeyondThreePoints=NOT_TESTED",m_NavFrames624);
   IEntity usageOwner624;
   m_Usage624=SCR_AIVehicleUsageComponent.FindOnNearestParent(m_Hull624,usageOwner624);
   if(!m_Usage624 || usageOwner624!=m_Hull624)
   {
    Done624("vehicle_usage_wrong_owner");
    return true;
   }
   PrintFormat("GPG_SOLO_AI_PILOTABLE actual=%1 owner=%2 method=native_readback",m_Usage624.CanBePiloted(),GPG_SoloRead708.NativeID624(usageOwner624));
   if(!m_Usage624.CanBePiloted())
   {
    Done624("native_vehicle_not_pilotable_no_driving_substitute");
    return true;
   }
   m_Group624.GetGroupUtilityComponent().AddUsableVehicle(m_Usage624);
   m_State624=11;
   m_Reason624="waiting_exact_hull_group_registration";
   return false;
  }
  if(m_State624==11)
  {
   if(!m_Group624.GetGroupUtilityComponent().IsUsableVehicle(m_Hull624) || !m_Group624.GetGroupUtilityComponent().m_VehicleMgr.FindVehicle(m_Hull624))
   {
    return false;
   }
   m_GetIn624=SCR_BoardingEntityWaypoint.Cast(Spawn624("{712F4795CF8B91C7}Prefabs/AI/Waypoints/AIWaypoint_GetIn.et",m_Hull624.GetOrigin()));
   if(!m_GetIn624)
   {
    Done624("native_GetIn_waypoint_missing");
    return true;
   }
   m_GetIn624.SetEntity(m_Hull624);
   m_GetIn624.SetAllowance(true,false,false);
   if(m_GetIn624.GetEntity()!=m_Hull624)
   {
    Done624("boarding_target_wrong_hull");
    return true;
   }
   m_Group624.AddWaypoint(m_GetIn624);
   // Queue normal movement behind boarding. Native GetIn completion owns advancement.
   m_Move624=AIWaypoint.Cast(Spawn624("{750A8D1695BD6998}Prefabs/AI/Waypoints/AIWaypoint_Move.et",m_MovePoint624));
   if(!m_Move624)
   {
    Done624("native_Move_waypoint_missing");
    return true;
   }
   m_Group624.AddWaypoint(m_Move624);
m_Moves708.Insert(m_Move624);
   for(int next708=1;next708<m_Route708.Count();next708++)
   {
    AIWaypoint move708=AIWaypoint.Cast(Spawn624("{750A8D1695BD6998}Prefabs/AI/Waypoints/AIWaypoint_Move.et",m_Route708[next708]));
    if(!move708){
Done624("additional_owned_move_waypoint_missing");
return true;
}
    m_Moves708.Insert(move708);
m_Group624.AddWaypoint(move708);
   }
   array<AIWaypoint> queued624=
   {
   }
   ;
   m_Group624.GetWaypoints(queued624);
   int getInIndex624=queued624.Find(m_GetIn624),moveIndex624=queued624.Find(m_Move624);
   if(getInIndex624<0 || moveIndex624<=getInIndex624)
   {
    Done624("normal_GetIn_Move_queue_order_not_observed");
    return true;
   }
   m_State624=1;
   m_Reason624="waiting_native_GetIn_completion_and_settled_PILOT";
   PrintFormat("GEP_MOVING624_AI_QUEUE getInIndex=%1 moveIndex=%2 count=%3 nativeCompletionRequired=1 manualRemove=0 manualComplete=0",getInIndex624,moveIndex624,queued624.Count());
   PrintFormat("GEP_MOVING624_AI_REGISTERED hull=%1 driver=%2 agents=%3 registration=1 allowanceDriver=1 allowanceGunner=0 allowanceCargo=0",GPG_SoloRead708.NativeID624(m_Hull624),GPG_SoloRead708.NativeID624(m_Driver624),m_Group624.GetAgentsCount());
  }
  CompartmentAccessComponent access624=CompartmentAccessComponent.Cast(m_Driver624.FindComponent(CompartmentAccessComponent));
  BaseCompartmentSlot pilot624;
  if(access624)
  {
   pilot624=access624.GetCompartment();
  }
  bool boarded624=PilotCompartmentSlot.Cast(pilot624) && pilot624.GetOccupant()==m_Driver624 && pilot624.GetOwner()==m_Hull624;
  if(pilot624 && !boarded624)
  {
   Done624("AI_in_wrong_compartment");
   return true;
  }
  bool inside624=access624 && access624.IsInCompartment();
  bool entering624=access624 && access624.IsGettingIn();
  bool leaving624=access624 && access624.IsGettingOut();
  bool stable624=boarded624 && inside624 && !entering624 && !leaving624;
  if(m_State624==1)
  {
   if(stable624)
   {
    if(m_PilotSettledMs624<0 || m_SettlingPilot624!=pilot624)
    {
     m_PilotSettledMs624=now624;
     m_SettlingPilot624=pilot624;
    }
   }
   else
   {
    m_PilotSettledMs624=-1;
    m_SettlingPilot624=null;
   }
   m_Reason624="waiting_native_GetIn_completion_and_settled_PILOT";
  }
  float settledSeconds624;
  if(m_PilotSettledMs624>=0)
  {
   settledSeconds624=(now624-m_PilotSettledMs624)/1000;
  }
  array<AIWaypoint> remaining624=
  {
  }
  ;
  m_Group624.GetWaypoints(remaining624);
  bool getInQueued624=remaining624.Find(m_GetIn624)>=0;
  if(m_State624==2 && remaining624.Count()==0){
Done624("ordinary_route_naturally_complete_REVIEW_REQUIRED");
return true;
}
  bool naturalMove624=m_Moves708.Find(m_Group624.GetCurrentWaypoint())>=0 && !getInQueued624;
  IEntity pilotOwner624;
  if(pilot624)
  {
   pilotOwner624=pilot624.GetOwner();
  }
  int pilotFlags624=0;
  if(boarded624)
  {
   pilotFlags624+=1;
  }
  if(inside624)
  {
   pilotFlags624+=2;
  }
  if(entering624)
  {
   pilotFlags624+=4;
  }
  if(leaving624)
  {
   pilotFlags624+=8;
  }
  if(pilotFlags624!=m_PreviousPilotFlags624 || now624-m_PilotLogMs624>=500)
  {
   m_PreviousPilotFlags624=pilotFlags624;
   m_PilotLogMs624=now624;
   PrintFormat("GEP_MOVING624_AI_TRANSITION stage=%1 elapsed=%2 exactPilot=%3 inCompartment=%4 gettingIn=%5 gettingOut=%6 settledSeconds=%7 requiredSettleSeconds=0.5",m_State624,elapsed624,boarded624,inside624,entering624,leaving624,settledSeconds624);
   PrintFormat("GEP_MOVING624_AI_WAYPOINT current=%1 GetInStillQueued=%2 naturalCurrentMove=%3 pilot=%4 pilotOwner=%5 driver=%6",m_Group624.GetCurrentWaypoint(),getInQueued624,naturalMove624,pilot624,pilotOwner624,m_Driver624);
  }
  if(m_State624==1 && stable624 && settledSeconds624>=0.5 && naturalMove624)
  {
   m_State624=2;
   m_Reason624="waiting_native_vehicle_motion";
   PrintFormat("GEP_MOVING624_AI_PILOT hull=%1 driver=%2 pilot=%3 normalBoarding=1 originalGunnerUnchanged=%4 settledSeconds=%5 naturallyAdvancedCurrentMove=1 nativeGettingIn=0 nativeGettingOut=0",GPG_SoloRead708.NativeID624(m_Hull624),GPG_SoloRead708.NativeID624(m_Driver624),pilot624,OriginalGunner624(),settledSeconds624);
  }
  if(m_State624==2 && !stable624)
  {
   Done624("AI_lost_exact_PILOT");
   return true;
  }
  Physics body624=m_Hull624.GetPhysics();
  if(!body624)
  {
   Done624("native_hull_physics_missing");
   return true;
  }
  if(vector.Distance(m_Hull624.GetOrigin(),m_StartPosition624)>160)
  {
   Done624("left_bounded_station_area_REQUEST_STOP");
return true;
  }
  vector displacement624=m_Hull624.GetOrigin()-m_StartPosition624;
  vector route624=(m_MovePoint624-m_StartPosition624).Normalized();
  float progress624=vector.Dot(displacement624,route624);
  vector velocity624=body624.GetVelocity();
  if(now624-m_LastSampleMs624>=500)
  {
   m_LastSampleMs624=now624;
   m_Samples624++;
   PrintFormat("GEP_MOVING624_AI_SAMPLE run=%1 nonce=%2 sample=%3 stage=%4 elapsed=%5 boarded=%6 position=%7 velocity=%8 routeProgressMetres=%9",m_Run624,m_Nonce624,m_Samples624,m_State624,elapsed624,boarded624,m_Hull624.GetOrigin(),velocity624,progress624);
   PrintFormat("GEP_MOVING624_AI_SCOPE originalGunner=%1 originalSeat=%2 originalSight=%3 ADSscope=%4 groupAgents=%5 currentWaypoint=%6",GPG_SoloRead708.NativeID624(m_Gunner624),m_GunnerSeat624,m_GunnerSight624,OriginalGunner624(),m_Group624.GetAgentsCount(),m_Group624.GetCurrentWaypoint());
   vector hullBasis708[4];
m_Hull624.GetWorldTransform(hullBasis708);
   PrintFormat("GPG_SOLO_NATIVE_BASIS run=%1 nonce=%2 worldMs=%3 right=%4 up=%5 forward=%6 origin=%7",m_Run624,m_Nonce624,now624,hullBasis708[0],hullBasis708[1],hullBasis708[2],hullBasis708[3]);
   VehicleWheeledSimulation nativeSim708=VehicleWheeledSimulation.Cast(m_Hull624.FindComponent(VehicleWheeledSimulation));
   if(nativeSim708)
   {
    int contacts708;
    for(int wheel708=0;wheel708<nativeSim708.WheelCount();wheel708++)if(nativeSim708.WheelHasContact(wheel708))contacts708++;
    PrintFormat("GPG_SOLO_NATIVE_POWER run=%1 nonce=%2 engineOn=%3 rpm=%4 load=%5 gear=%6 throttle=%7 contacts=%8",m_Run624,m_Nonce624,nativeSim708.EngineIsOn(),nativeSim708.EngineGetRPM(),nativeSim708.EngineGetLoad(),nativeSim708.GetGear(),nativeSim708.GetThrottle(),contacts708);
   }
  }
  if(m_State624==2 && boarded624 && progress624>=2 && velocity624.Length()>0.25 && m_Moves708.Find(m_Group624.GetCurrentWaypoint())>=0)
  {
   if(!m_Moved624)
   {
    PrintFormat("GEP_MOVING624_AI_FIRST_2M run=%1 nonce=%2 worldMs=%3 position=%4 velocity=%5 routeProgressMetres=%6 retainRoute=%7",m_Run624,m_Nonce624,now624,m_Hull624.GetOrigin(),velocity624,progress624,m_RetainRoute624);
   }
   m_Moved624=true;
   if(!m_RetainRoute624)
   {
    Done624("normal_AI_PILOT_motion_observed_review_required");
    return true;
   }
   m_Reason624="normal_motion_retained_for_bounded_capture";
  }
  return false;
 }
 // Original QA-only opt-in before Begin; Tick retains its existing guard/time budget.
 void RetainRouteForCapture624()
 {
  if(!m_Hull624)
  {
   m_RetainRoute624=true;
  }
 }
 IEntity Driver624()
 {
  return m_Driver624;
 }
 SCR_AIGroup Group624()
 {
  return m_Group624;
 }
 AIWaypoint MoveWaypoint624()
 {
  return m_Move624;
 }
 float ElapsedSeconds624()
 {
  return (GetGame().GetWorld().GetWorldTime()-m_StartMs624)/1000;
 }
 bool StableMotionStage624()
 {
  return m_State624==2 && !m_Done624 && !m_Closed624;
 }
 string Reason624()
 {
  return m_Reason624;
 }
 bool Moved624()
 {
  return m_Moved624;
 }
 void Close624()
 {
  // Removing own tasks requests normal AI cleanup; it is NOT proof of a halt.
  if(m_Closed624)
  {
   return;
  }
  m_Closed624=true;
  if(m_Group624 && !m_Group624.IsDeleted())
  {
   array<AIWaypoint> ownedWaypoints624=
   {
   }
   ;
   m_Group624.GetWaypoints(ownedWaypoints624);
   if(m_GetIn624 && !m_GetIn624.IsDeleted() && ownedWaypoints624.Find(m_GetIn624)>=0)
   {
    m_Group624.RemoveWaypoint(m_GetIn624);
   }
   foreach(AIWaypoint move708:m_Moves708)
   {
    if(move708 && !move708.IsDeleted() && ownedWaypoints624.Find(move708)>=0)m_Group624.RemoveWaypoint(move708);
   }
  }
 }
 void ~GPG_SoloDriver708()
 {
  Close624();
 }
}

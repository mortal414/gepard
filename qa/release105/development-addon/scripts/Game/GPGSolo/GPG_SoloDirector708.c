// Original private solo fixture. Native boarding/AI waypoints move the vehicles.
// Public instant ACTOR seating is labeled scenario setup, never exterior boarding acceptance.
// No vehicle transform, synthetic driving, aim, servo, weapon, health or ammo writes.
class GPG_SoloRead708
{
 static string NativeID624(IEntity entity624)
 {
  if(!entity624)
  {
   return "null";
  }
  if(entity624.IsDeleted())
  {
   return "deleted";
  }
  return string.Format("%1",entity624.GetID());
 }
 static bool ReadScope624(IEntity turret,out IEntity user,out BaseCompartmentSlot seat,out BaseSightsComponent sight,out string reason)
 {
  user=null;
seat=null;
sight=null;
  if(!GEP_GunnerSnapshot583.Evaluate679(turret,reason))return false;
  PlayerController pc=GetGame().GetPlayerController();
if(!pc)return false;
  user=pc.GetControlledEntity();
if(!user)return false;
  CompartmentAccessComponent access=CompartmentAccessComponent.Cast(user.FindComponent(CompartmentAccessComponent));
  TurretControllerComponent control=TurretControllerComponent.Cast(turret.FindComponent(TurretControllerComponent));
  if(!access || !control)return false;
  seat=access.GetCompartment();
sight=control.GetCurrentSights();
  return seat && sight && seat.GetOccupant()==user;
 }
}
class GPG_SoloBoard708Class : ScriptComponentClass {
}
class GPG_SoloBoard708 : ScriptComponent {
string Station708;
string Label710;
}
class GPG_SoloAction708 : ScriptedUserAction
{
 [Attribute("read",UIWidgets.EditBox,"read/start/stop/reset")]
 string Operation;
 override bool HasLocalEffectOnlyScript(){
return true;
}
 override bool CanBeShownScript(IEntity user){
return GPG_SoloDirector708.Solo && GetOwner().FindComponent(GPG_SoloBoard708);
}
 override bool CanBePerformedScript(IEntity user)
 {
  return RplSession.Mode()==RplMode.None && user && GPG_SoloDirector708.Solo && vector.Distance(user.GetOrigin(),GetOwner().GetOrigin())<12;
 }
 override bool GetActionNameScript(out string outName)
 {
  GPG_SoloBoard708 board710=GPG_SoloBoard708.Cast(GetOwner().FindComponent(GPG_SoloBoard708));
  if(!board710)return super.GetActionNameScript(outName);
  string verb710="Read / identify vehicle";
  if(Operation=="start")verb710="START then enter gunner";
  else if(Operation=="stop")verb710="STOP owned AI route";
  else if(Operation=="reset")verb710="RESET only when empty / parked / clear";
  else if(Operation=="gunner")verb710="START GUNNER TEST / scenario seating";
  else if(Operation=="inspect")verb710="INSPECT exterior entry / read only";
  outName=verb710+" | "+board710.Label710;
  return true;
 }
 override void PerformAction(IEntity pOwnerEntity,IEntity pUserEntity)
 {
  if(!CanBePerformedScript(pUserEntity))return;
  GPG_SoloBoard708 board=GPG_SoloBoard708.Cast(pOwnerEntity.FindComponent(GPG_SoloBoard708));
  if(board)GPG_SoloDirector708.Solo.Command708(board.Station708,Operation,pUserEntity);
 }
}
class GPG_SoloDirector708Class : GPG_DirectorClass {
}
class GPG_SoloDirector708 : GPG_Director
{
 static GPG_SoloDirector708 Solo;
 protected ref array<ref GPG_Owned> SoloObjects708={
}
;
 protected ref GPG_SoloDriver708 Driver708;
 protected IEntity Driven708;
 protected string ActiveStation708,ArmedStation708;
 protected float ArmedAt708,Poll708,LastSoloReset708=-30;
 protected bool SoloStaged708;
 protected IEntity ElevatedLandmark713;
 protected ref GPG_SoloAcquisition713 Acquisition713=new GPG_SoloAcquisition713();
 [Attribute("target",UIWidgets.EditBox,"target or motion; selects a focused test start")]
 string InitialScenario712;
 protected bool AutoSetupRequested712,SetupPending712,SetupAnnounced712;
 protected string SetupStation712;
 protected ref GPG_SoloGunnerSetup712 GunnerSetup712;
 void GPG_SoloDirector708(IEntitySource src,IEntity parent){
Solo=this;
}
 GPG_Owned AddSolo708(string id,string station,ResourceName prefab,vector pad,float yaw=0)
 {
  GPG_Owned item=new GPG_Owned();
item.Id=id;
item.Station=station;
item.Prefab=prefab;
item.Pad=pad;
item.Yaw=yaw;
  item.Entity=Spawn(prefab,pad,yaw,0.05);
if(item.Entity)item.Entity.SetName("GPGSolo_"+id);
  PrintFormat("GPG_SOLO_OBJECT id=%1 station=%2 pad=%3 nativeId=%4 clearance=0.05 initialPlacementOnly=1",id,station,Ground(pad,0.05),GPG_SoloRead708.NativeID624(item.Entity));
  SoloObjects708.Insert(item);
return item;
 }
 void Board708(string station,vector position,string label710)
 {
  IEntity board=Spawn("{A76A420024150202}Prefabs/GPG_SoloBoard708.et",position,180);
  if(!board)return;
  GPG_SoloBoard708 component=GPG_SoloBoard708.Cast(board.FindComponent(GPG_SoloBoard708));
  if(component)
  {
   component.Station708=station;
   component.Label710=label710;
  }
  PrintFormat("GPG_SOLO_BOARD station=%1 label=%2 position=%3",station,label710,Ground(position));
 }
 bool PadReady710(string id710,vector pad710)
 {
  vector center710=Ground(pad710,0);
  float minimum710=center710[1];
  float maximum710=center710[1];
  array<vector> offsets710={"-5 0 -3","-5 0 3","5 0 -3","5 0 3"};
  foreach(vector offset710:offsets710)
  {
   vector corner710=Ground(pad710+offset710,0);
   minimum710=Math.Min(minimum710,corner710[1]);
   maximum710=Math.Max(maximum710,corner710[1]);
  }
  bool level710=maximum710-minimum710<=0.25 && minimum710>=38.75 && maximum710<=39.35;
  PrintFormat("GPG_SOLO_PAD_CHECK id=%1 center=%2 minimum=%3 maximum=%4 level=%5 method=native_GetSurfaceY_not_offline_acceptance",id710,center710,minimum710,maximum710,level710);
  return level710;
 }
 void StageSolo708()
 {
  bool motion712=InitialScenario712=="motion";
  if(!motion712 && InitialScenario712!="target")
  {
   Failed=true;
   Print("GPG_SOLO_FATAL reason=unknown_initial_scenario");
   return;
  }
  bool player712=PadReady710("PLAYER","663 0 2689.8");
  if(motion712)
  {
   bool ownPad712=PadReady710("MOTION_CONFIRMED_CENTER","705 0 2720");
   if(!player712 || !ownPad712)
   {
    Failed=true;
    Hint("Required own-motion pad was refused. No movement test was admitted.","SOLO PLACEMENT REFUSED");
    Print("GPG_SOLO_FATAL reason=motion_required_pad_failed");
    return;
   }
   AddSolo708("MOVING-TRACKING-CAMO","motion","{A76A420024150221}Prefabs/GPG_SoloAIDrive712.et","705 0 2720",90);
   Board708("motion","703 0 2716.5","MOVING OWN GEPARD / DIRECT GUNNER START");
   Board708("motion","688 0 2720","MOTION / CLEAR RESET POINT");
  }
  else
  {
   bool targetPad712=PadReady710("TARGET_CONFIRMED_FOOTPRINT","665 0 2695");
   bool truckPad712=PadReady710("TRUCK_CONFIRMED_DRIVE_MARKER_CENTER","730 0 2695");
   if(!player712 || !targetPad712 || !truckPad712)
   {
    Failed=true;
    Hint("The truck is required for this test. No incomplete moving-target demonstration was admitted.","SOLO PLACEMENT REFUSED");
    Print("GPG_SOLO_FATAL reason=target_or_required_truck_pad_failed");
    return;
   }
   AddSolo708("TARGET-TRACKING-CAMO","target","{A76A420024150106}Prefabs/FinishStabilization/Camo/GEP_Tracking.et","665 0 2695",90);
   AddSolo708("AI-TARGET-TRUCK","target",GPG_Resources.RepairTruck(),"730 0 2695",0);
   Board708("target","663 0 2691.5","MOVING TRUCK / DIRECT GUNNER START");
   Board708("target","665 0 2678","TARGET / CLEAR RESET POINT");
  }
  if(motion712)Print("GPG_OWN714_PANEL_PENDING initialPlacementAfterFirstActualADS=1 beforeAnyDesignationOrAI=1");
  PrintFormat("GPG_SOLO_STAGED scenario=%1 requiredSubjectsPresent=1 actualAIMovement=NOT_TESTED automaticActorSeatingIsSetupOnly=1",InitialScenario712);
 }
 override void Service()
 {
  Print("GPG_SOLO_SERVICE_OMITTED reason=focused_stabilization_test no_error_filtering_or_catalog_override=1");
 }
 bool InitialLandmark714(IEntity turret714)
 {
  if(ElevatedLandmark713)return true;
  if(Failed || !turret714)return false;
  TurretControllerComponent control714=TurretControllerComponent.Cast(turret714.FindComponent(TurretControllerComponent));
  GEP_NextStabilizationConsumer625 stab714=GEP_NextStabilizationConsumer625.Cast(turret714.FindComponent(GEP_NextStabilizationConsumer625));
  PlayerController player714=GetGame().GetPlayerController();
  CameraManager cameras714=GetGame().GetCameraManager();
  if(!control714 || !stab714 || !player714 || !cameras714 || !player714.GetPlayerCamera() || cameras714.CurrentCamera()!=player714.GetPlayerCamera())return false;
  BaseSightsComponent sights714=control714.GetCurrentSights();
  if(!control714.GetCurrentSightsADS() || !sights714 || !sights714.IsSightADSActive())return false;
  // This is initial fixture placement, once, before any lock or servo demand.
  vector requested714;
  if(stab714.GetMode()!=WCS_EStabilizationMode.OFF || stab714.GetStabilizationEnabled() || stab714.TryGetRequestedAim(requested714))return false;
  IEntity hull714=turret714.GetRootParent();
  if(!hull714 || !hull714.GetPhysics() || hull714.GetPhysics().GetVelocity().Length()>0.25)return false;
  vector camera714[4];float fov714;
  if(!control714.GetCurrentSightsCameraTransform(camera714,fov714) || camera714[2].Length()<0.9)return false;
  vector forward714=camera714[2].Normalized();
  if(vector.Dot(forward714,"1 0 0")<0.99999)
  {
   Failed=true;
   PrintFormat("GPG_OWN714_PANEL_REFUSED reason=not_initial_east_sight_line forward=%1 no_aim_setter_or_marker_retry=1",forward714);
   return false;
  }
  vector initialPoint714=camera714[3]+forward714*200;
  // Same native-observed stock panel, yaw270/scale3. Its crossbar contact offset
  // was (-0.306,3.8754,-0.43) from the root. This places that physical crossbar
  // on the initial ray; it never changes aim or moves an existing marker.
  vector markerOrigin714=initialPoint714-Vector(-0.306,3.8754,-0.43);
  float minimumTerrainClearance714=1000;
  for(int step714=1;step714<=40;step714++)
  {
   vector sample714=camera714[3]+forward714*(step714*5);
   float terrain714=GetGame().GetWorld().GetSurfaceY(sample714[0],sample714[2]);
   minimumTerrainClearance714=Math.Min(minimumTerrainClearance714,sample714[1]-terrain714);
  }
  if(minimumTerrainClearance714<0.5)
  {
   Failed=true;
   PrintFormat("GPG_OWN714_PANEL_REFUSED reason=initial_line_terrain_occluded minimumSampledClearance=%1 no_hidden_reposition=1",minimumTerrainClearance714);
   return false;
  }
  float clearance714=markerOrigin714[1]-GetGame().GetWorld().GetSurfaceY(markerOrigin714[0],markerOrigin714[2]);
  ElevatedLandmark713=Spawn(GPG_Resources.VehicleTarget(),markerOrigin714,270,clearance714);
  if(!ElevatedLandmark713){Failed=true;return false;}
  ElevatedLandmark713.SetName("GPGSolo_INITIAL_RAY_STATIC_LANDMARK714");
  ElevatedLandmark713.SetScale(3);
  ElevatedLandmark713.Update();
  ArmedAt708=Clock;
  PrintFormat("GPG_OWN714_PANEL_PLACED cameraOrigin=%1 cameraForward=%2 initialRayPoint=%3 markerOrigin=%4 minimumSampledTerrainClearance=%5 initialPlacementOnly=1 noLaterTransformWrites=1",camera714[3],forward714,initialPoint714,ElevatedLandmark713.GetOrigin(),minimumTerrainClearance714);
  Hint("The static panel is now on your initial sight ray, beyond the driving route. Hold Space ON, then tap its crossbar. There is no acquisition countdown in this human session. Native AI waits for a genuine panel designation. No mouse adjustment or aim setter is required.","OWN-MOTION PANEL READY");
  return true;
 }
 IEntity Expected713(string station713)
 {
  if(station713=="motion")return ElevatedLandmark713;
  foreach(GPG_Owned item713:SoloObjects708)
  {
   if(item713.Station=="target" && item713.Id=="AI-TARGET-TRUCK")return item713.Entity;
  }
  return null;
 }
 IEntity Subject712(string station712)
 {
  foreach(GPG_Owned item712:SoloObjects708)
  {
   if(item712.Station==station712 && item712.Id!="AI-TARGET-TRUCK")return item712.Entity;
  }
  return null;
 }
 bool StartGunner712(string station712)
 {
  if(SetupPending712 || Driver708 || !Actor || RplSession.Mode()!=RplMode.None)return false;
  IEntity hull712=Subject712(station712);
  if(!hull712)
  {
   Hint("That station is not staged in this focused scenario. Choose the other scene at launch.","GUNNER SETUP REFUSED");
   return false;
  }
  Acquisition713.Reset713();
  GunnerSetup712=new GPG_SoloGunnerSetup712();
  SetupStation712=station712;
  SetupAnnounced712=false;
  SetupPending712=GunnerSetup712.Begin712(Actor,hull712);
  if(!SetupPending712)Hint(GunnerSetup712.Reason712(),"GUNNER SETUP REFUSED");
  return SetupPending712;
 }
 void TickGunnerSetup712()
 {
  if(!SetupPending712 || !GunnerSetup712)return;
  int state712=GunnerSetup712.Tick712();
  if(state712==1)return;
  SetupPending712=false;
  if(state712!=2)
  {
   Hint(GunnerSetup712.Reason712(),"GUNNER SETUP FAILED");
   return;
  }
  ArmedStation708=SetupStation712;
  ArmedAt708=Clock;
  SetupAnnounced712=true;
  Hint("Scenario seating completed through public native GetInVehicle; exterior boarding remains untested. Enter the ordinary sight with RMB. Hold Space ON/OFF; tap designates/clears. The route waits for your exact truck entity lock or position designation on the elevated panel. Native AI then boards and drives; there is no acquisition race. No aim, servo or vehicle-motion setter was used.","DIRECT GUNNER TEST READY / "+SetupStation712);
 }
 IEntity PlayerTurret708(IEntity user,out IEntity hull)
 {
  hull=null;
if(!user)return null;
  CompartmentAccessComponent access=CompartmentAccessComponent.Cast(user.FindComponent(CompartmentAccessComponent));
  if(!access || access.IsGettingIn() || access.IsGettingOut() || access.IsSwitchingSeatsAnim())return null;
  BaseCompartmentSlot seat=access.GetCompartment();
if(!TurretCompartmentSlot.Cast(seat))return null;
  TurretControllerComponent control=TurretControllerComponent.Cast(seat.GetController());
if(!control)control=seat.GetAttachedTurret();
  if(!control)return null;
  IEntity turret=control.GetOwner();
string reason;
  if(!GEP_GunnerSnapshot583.Evaluate679(turret,reason))return null;
  hull=turret.GetRootParent();
return turret;
 }
 bool OwnedGepard708(IEntity hull,string station)
 {
  foreach(GPG_Owned item:SoloObjects708)
  {
   if(item.Station==station && item.Id!="AI-TARGET-TRUCK" && item.Entity==hull)return true;
  }
  return false;
 }
 void Explain708(string station)
 {
  if(station=="target")Hint("This board identifies TARGET / TRACKING CAMO immediately north-east. Use START GUNNER TEST for explicitly labeled native scenario seating, then enter the sight with ordinary RMB. Exterior boarding is a separate gate. The visible truck stays parked until its exact entity lock is observed, with native line-of-sight verification; only then does native AI board and drive it. Hold Space ON/OFF; tap the real truck to designate, tap again to clear. STABILIZED also allows manual following. No firing. Reset from CLEAR RESET POINT only after exit, real rest and pad return.","TARGET / TRACKING CAMO");
  else Hint("This board identifies MOVING OWN GEPARD / TRACKING CAMO immediately north-east. Use START GUNNER TEST for explicitly labeled native scenario seating, then enter the sight with ordinary RMB. Exterior boarding is a separate gate. Designate the elevated static panel first. After native point/collision verification, an AI driver boards and drives the closed route. Compare Space OFF/ON while holding the fixed east landmark. Initial actor seating is scenario setup only; vehicle movement is real native driving. Reset from CLEAR RESET POINT only after exit, real rest and pad return.","MOVING OWN GEPARD / TRACKING CAMO");
 }
 void Stop708(string reason)
 {
  ArmedStation708="";
  if(Driver708)Driver708.Stop708();
  PrintFormat("GPG_SOLO_STOP reason=%1 station=%2 normalTaskRemovalOnly=1 haltProven=0",reason,ActiveStation708);
 }
 void Command708(string station,string operation,IEntity user)
 {
  if(RplSession.Mode()!=RplMode.None || user!=Actor || (station!="target" && station!="motion"))return;
  if(operation=="gunner")
  {
   StartGunner712(station);
   return;
  }
  if(operation=="inspect")
  {
   GPG_SoloGunnerSetup712.InspectEntry712(Actor,Subject712(station));
   return;
  }
  if(operation=="read"){
Explain708(station);
return;
}
  if(operation=="stop"){
Stop708("user");
return;
}
  if(operation=="reset"){
ResetSolo708(station);
return;
}
  if(operation!="start")return;
  if(Driver708){
Hint("STOP, then reset the empty parked station before another route. No active owned driver is replaced.","SOLO START REFUSED");
return;
}
  ArmedStation708=station;
ArmedAt708=Clock;
Explain708(station);
  PrintFormat("GPG_SOLO_ARMED station=%1 waitForNativeDesignationSeconds=UNTIMED_HUMAN forcedSeat=0",station);
 }
 void Admit708()
 {
  if(ArmedStation708=="")return;
  IEntity userHull;
IEntity turret=PlayerTurret708(Actor,userHull);
  if(!turret || !OwnedGepard708(userHull,ArmedStation708))return;
  if(ArmedStation708=="motion" && !InitialLandmark714(turret))return;
  IEntity target=userHull;
vector driverPoint;
array<vector> route={
}
;
  if(ArmedStation708=="target")
  {
   foreach(GPG_Owned item:SoloObjects708)if(item.Id=="AI-TARGET-TRUCK")target=item.Entity;
   if(!target || target==userHull)
   {
    ArmedStation708="";
    Hint("Truck pad was not admitted. Ordinary stabilization controls can still be checked; moving-target tracking remains unavailable.","TARGET ROUTE NOT ADMITTED");
    Print("GPG_SOLO_ROUTE_REFUSED reason=truck_not_admitted");
    return;
   }
   driverPoint=target.GetOrigin()+Vector(-4,0,-4);
   vector truckStart711=target.GetOrigin();
   route.Insert(truckStart711+Vector(0,0,25));
   route.Insert(truckStart711+Vector(-40,0,25));
   route.Insert(truckStart711+Vector(-40,0,5));
   route.Insert(truckStart711+Vector(0,0,5));
   route.Insert(truckStart711+Vector(0,0,25));
   route.Insert(truckStart711+Vector(-40,0,25));
   route.Insert(truckStart711+Vector(-40,0,5));
   route.Insert(truckStart711+Vector(0,0,5));
  }
  else
  {
   vector start=userHull.GetOrigin();
driverPoint=start+Vector(-4,0,-4);
   route.Insert(start+Vector(60,0,0));
   route.Insert(start+Vector(60,0,-25));
   route.Insert(start+Vector(0,0,-25));
   route.Insert(start+Vector(0,0,5));
  }
  // Read-only success detection. Only a genuine native designation starts AI.
  // Does not set WCS mode, target, aim, servo, driving inputs or vehicle physics.
  if(!Acquisition713.Ready713(ArmedStation708,turret,Expected713(ArmedStation708)))return;
  driverPoint=Ground(driverPoint,0.05);
  for(int i=0;i<route.Count();i++)route[i]=Ground(route[i],0.05);
  GPG_SoloDriver708 fresh=new GPG_SoloDriver708();
fresh.Configure708(driverPoint,route);
  string run="solo708-"+System.GetTickCount().ToString();
  if(!fresh.Begin624(run,run,target,turret)){
fresh.Stop708();
Driver708=fresh;
Driven708=target;
ActiveStation708=ArmedStation708;
ArmedStation708="";
Hint("Native AI setup was rejected. STOP/reset after observed rest; source/native logs retain the reason.","SOLO NOT STARTED");
return;
}
  Acquisition713.Started713(ArmedStation708,turret,Expected713(ArmedStation708));
  Driver708=fresh;
Driven708=target;
ActiveStation708=ArmedStation708;
ArmedStation708="";
 }
 bool ResetOccupants708(IEntity entity,IEntity allowedDriver)
 {
  if(!entity)return true;
  BaseCompartmentManagerComponent manager=BaseCompartmentManagerComponent.Cast(entity.FindComponent(BaseCompartmentManagerComponent));
  if(manager)
  {
   array<BaseCompartmentSlot> seats={
}
;
manager.GetCompartments(seats);
   foreach(BaseCompartmentSlot seat:seats)
   {
    if(!seat)
{
 continue;
}
IEntity occupant=seat.GetOccupant();
    if(occupant && occupant!=allowedDriver)return false;
   }
  }
  for(IEntity child=entity.GetChildren();child;child=child.GetSibling())if(!ResetOccupants708(child,allowedDriver))return false;
  return true;
 }
 bool SoloResetIdentityReady708(string station)
 {
  foreach(GPG_Owned check:SoloObjects708)
  {
   if(check.Station!=station)continue;
   // Missing roots retain CanReset's existing replacement behavior.
   if(!check.Entity || check.Entity.IsDeleted())continue;
   EntityID nativeId=check.Entity.GetID();
   if(GetGame().GetWorld().FindEntityByID(nativeId)!=check.Entity)return false;
   foreach(GPG_Owned other:SoloObjects708)
   {
    if(other==check || !other.Entity || other.Entity.IsDeleted())continue;
    if(other.Entity.GetID()==nativeId)return false;
   }
  }
  return true;
 }
 void ResetSolo708(string station)
 {
  if(Clock-LastSoloReset708<15){
Hint("Wait15s between explicit resets.","SOLO RESET PAUSED");
return;
}
  if(Driver708 && (!Driver708.Closed708() || ActiveStation708!=station)){
Hint("STOP this owned route first. Other station and driver are preserved.","SOLO RESET REFUSED");
return;
}
  IEntity allowedDriver;
if(Driver708)allowedDriver=Driver708.Driver624();
  if(allowedDriver)
  {
   CompartmentAccessComponent access=CompartmentAccessComponent.Cast(allowedDriver.FindComponent(CompartmentAccessComponent));
   if(!access || access.IsGettingIn() || access.IsGettingOut() || access.IsSwitchingSeatsAnim()){
Hint("Wait until the owned AI transition finishes.","SOLO RESET REFUSED");
return;
}
  }
  foreach(GPG_Owned check:SoloObjects708)
  {
   if(check.Station!=station || !check.Entity)continue;
   if(!ResetOccupants708(check.Entity,allowedDriver) || !check.Entity.GetPhysics() || check.Entity.GetPhysics().GetVelocity().Length()>0.25 || vector.Distance(check.Entity.GetOrigin(),Ground(check.Pad))>18 || (Actor && vector.Distance(Actor.GetOrigin(),check.Entity.GetOrigin())<12))
   {
Hint("Reset requires empty/owned-AI-only vehicles, observed rest, within18m of original pads, and you at least12m clear. Unknown occupants and moved vehicles are preserved. Return to pads or restart normally.","SOLO RESET REFUSED");
return;
}
  }
  if(!SoloResetIdentityReady708(station))
  {
   Hint("Owned native identity is ambiguous; reset refused. Restart the scenario.","SOLO RESET REFUSED");
   return;
  }
  PlayerController resetController708=GetGame().GetPlayerController();
  if(allowedDriver && (GetGame().GetWorld().FindEntityByID(allowedDriver.GetID())!=allowedDriver || (resetController708 && resetController708.GetControlledEntity()==allowedDriver)))
  {
   Hint("Driver identity changed or is player controlled; reset refused.","SOLO RESET REFUSED");
   return;
  }
  if(Driver708){
Driver708.DisposeOwned708();
Driver708=null;
Driven708=null;
ActiveStation708="";
}
  foreach(GPG_Owned item:SoloObjects708)
  {
   if(item.Station!=station)continue;
   if(item.Entity)SCR_EntityHelper.DeleteEntityAndChildren(item.Entity);
   item.Entity=Spawn(item.Prefab,item.Pad,item.Yaw,0.05);
item.Epoch++;
   if(item.Entity)item.Entity.SetName("GPGSolo_"+item.Id);
   PrintFormat("GPG_SOLO_RESET station=%1 id=%2 epoch=%3 freshOwnedSubject=1",station,item.Id,item.Epoch);
  }
  Acquisition713.Reset713();
  LastSoloReset708=Clock;
 }

 protected bool ProbeBound709;
 protected bool ProbeStarted709;
 protected bool ProbeClosed709;
 protected float ProbeStartMs709;
 protected float ProbeLastMs709=-1000;
 protected int ProbeSequence709,ProbeHoldDown709,ProbeHoldUp709,ProbeTap709;
 protected IEntity ProbeLastTurret709;

 // This focused child never instantiates the optional resupply fixtures.
 // The original full proving ground and its diagnostics remain unchanged.
 override void SmokeService()
 {
  Print("GPG_SOLO_OPTIONAL_SUPPORT_ISOLATED reason=stabilization_focus no_catalog_error_suppression=1");
 }
 protected bool ProbeScope709()
 {
  IEntity hull709;
  return PlayerTurret708(Actor,hull709)!=null;
 }
 protected void ProbeHoldDownEvent709(float value,EActionTrigger reason)
 {
  if(ProbeScope709())ProbeHoldDown709++;
 }
 protected void ProbeHoldUpEvent709(float value,EActionTrigger reason)
 {
  if(ProbeScope709())ProbeHoldUp709++;
 }
 protected void ProbeTapEvent709(float value,EActionTrigger reason)
 {
  if(ProbeScope709())ProbeTap709++;
 }
 protected void CloseProbe709()
 {
  if(!ProbeBound709)return;
  InputManager input709=GetGame().GetInputManager();
  if(input709)
  {
   input709.RemoveActionListener("WCS_Armament_TurretStabilizationHold",EActionTrigger.DOWN,ProbeHoldDownEvent709);
   input709.RemoveActionListener("WCS_Armament_TurretStabilizationHold",EActionTrigger.UP,ProbeHoldUpEvent709);
   input709.RemoveActionListener("WCS_Armament_TurretStabilizationToggle",EActionTrigger.DOWN,ProbeTapEvent709);
  }
  ProbeBound709=false;
 }
 protected void ObserveControls709()
 {
  if(ProbeClosed709)return;
  float now709=GetGame().GetWorld().GetWorldTime();
  if(!ProbeStarted709)
  {
   ProbeStarted709=true;
   ProbeStartMs709=now709;
  }
  if(now709-ProbeStartMs709>300000)
  {
   ProbeClosed709=true;
   CloseProbe709();
   Print("GPG_SOLO_CONTROL_OBSERVER_STOP boundedSeconds=300 gameplay_continues=1");
   return;
  }
  if(!ProbeBound709)
  {
   InputManager input709=GetGame().GetInputManager();
   if(!input709)return;
   input709.AddActionListener("WCS_Armament_TurretStabilizationHold",EActionTrigger.DOWN,ProbeHoldDownEvent709);
   input709.AddActionListener("WCS_Armament_TurretStabilizationHold",EActionTrigger.UP,ProbeHoldUpEvent709);
   input709.AddActionListener("WCS_Armament_TurretStabilizationToggle",EActionTrigger.DOWN,ProbeTapEvent709);
   ProbeBound709=true;
  }
  if(now709-ProbeLastMs709<250)return;
  ProbeLastMs709=now709;
  ProbeSequence709++;
  if(Actor && ProbeSequence709%4==0)
  {
   PrintFormat("GPG_SOLO_PLAYER_POSITION seq=%1 actor=%2 position=%3 ypr=%4 method=read_only",ProbeSequence709,GPG_SoloRead708.NativeID624(Actor),Actor.GetOrigin(),Actor.GetYawPitchRoll());
  }
  IEntity hull709;
  IEntity turret709=PlayerTurret708(Actor,hull709);
  bool scope709=turret709!=null;
  if(turret709)ProbeLastTurret709=turret709;
  GEP_NextStabilizationConsumer625 stab709;
  if(ProbeLastTurret709 && !ProbeLastTurret709.IsDeleted())
  {
   stab709=GEP_NextStabilizationConsumer625.Cast(ProbeLastTurret709.FindComponent(GEP_NextStabilizationConsumer625));
  }
  PrintFormat("GPG_SOLO_CONTROL|seq=%1|worldMs=%2|scope=%3|holdDown=%4|holdUp=%5|tap=%6",ProbeSequence709,now709,scope709,ProbeHoldDown709,ProbeHoldUp709,ProbeTap709);
  if(!stab709)return;
  vector demand709;
  bool requested709=stab709.TryGetRequestedAim(demand709);
  PrintFormat("GPG_SOLO_CONTROL_MODE|seq=%1|mode=%2|enabled=%3|tracking=%4|requested=%5|generation=%6|delivered=%7|forwarded=%8",ProbeSequence709,stab709.GetMode(),stab709.GetStabilizationEnabled(),stab709.IsTracking(),requested709,stab709.Generation625(),stab709.Delivered625(),stab709.Forwarded625());
  IEntity target709=stab709.GetStabilizationTarget();
  PrintFormat("GPG_SOLO_CONTROL_TARGET|seq=%1|target=%2|demand=%3|method=PASSIVE_OBSERVATION|inputInjectedByProbe=0",ProbeSequence709,GPG_SoloRead708.NativeID624(target709),demand709);
  Acquisition713.Observe713(InitialScenario712,ProbeLastTurret709,Expected713(InitialScenario712));
 }

 override void EOnFrame(IEntity owner,float timeSlice)
 {
  super.EOnFrame(owner,timeSlice);
  if(!Ready || RplSession.Mode()!=RplMode.None)return;
ObserveControls709();
  if(!SoloStaged708){
SoloStaged708=true;
StageSolo708();
}
  Poll708+=timeSlice;
if(Poll708<0.1)return;
Poll708=0;
  if(!AutoSetupRequested712)
  {
   AutoSetupRequested712=true;
   if(!Failed)StartGunner712(InitialScenario712);
  }
  TickGunnerSetup712();
  Admit708();
  if(Driver708)Driver708.Tick624();
 }
 void ~GPG_SoloDirector708()
 {
CloseProbe709();
  // Never force-delete an occupied/moving human test vehicle on teardown.
  if(Driver708)Driver708.Stop708();
  if(Solo==this)Solo=null;
 }
}

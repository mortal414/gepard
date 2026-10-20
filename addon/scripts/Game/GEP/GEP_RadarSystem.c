// Original sensor adapter, ported from our frozen LAV systems. Native vehicles
// are dependencies. Entity association is authority-only; UI/lead use samples.
class GEP_AirContact
{
 IEntity Association;

 int ID;

 int Hits;

 vector Position;

 vector Velocity;

 float Seen;

}
enum GEP_RadarCommand { SEARCH_POWER, SEARCH_RANGE, NEXT_CONTACT, PREVIOUS_CONTACT, TRACK_POWER, TRACK_SELECTED, TRACK_SIGHTED, UNTRACK }
class GEP_RadarSystemClass : ScriptComponentClass {}
class GEP_RadarSystem : ScriptComponent
{
 // Functional milestone: sensing stays dormant; deployment is a crew action.
 static const bool SENSING_ENABLED=false;
 bool Deployed;
 float Deployment;
 IEntity SearchBase;
 IEntity SearchYawSupport;
 vector SearchYawRest[4];
 int CosmeticSequence;
 int CosmeticLastReceived=-1;
 int CosmeticPreviousPhase=-1;
 bool CosmeticSynced;
 bool CosmeticWasAuthority;
 vector SearchBaseRest[4];
 ref GEP_EngagementModel Model=new GEP_EngagementModel();

 ref array<ref GEP_AirContact> Contacts={};

 ref array<IEntity> Candidates={};

 float Range=3000;

 float Sweep;

 float Clock;

 float ScanTimer;

 float TrackTimer;

 float NetworkTimer;

 float LogTimer;

 int Serial;

 int ScanCount;

 float ScanSum;

 float ScanPeak;

 string Feedback="Search off. Tracking radar off.";

 IEntity SearchHead;

 IEntity TrackerHead;

 IEntity TrackerBase;

 IEntity ActiveAssociation;

 IEntity Rig;

 TurretControllerComponent Gun;

 vector SearchRest[4];

 vector TrackerRest[4];

 bool PartsReady;

 float TrackerYaw;

 float TrackerPitch;

 ref GEP_RadarDisplay Display;

 bool ShowDisplay=false;
 bool InputAudited;




 bool Authority()
 {
  RplComponent replication=RplComponent.Cast(GetOwner().FindComponent(RplComponent));

  return !replication || !replication.IsProxy();

 }
 static GEP_RadarSystem Find(IEntity entity)
 {
  for(IEntity parent=entity;parent;parent=parent.GetParent())
  {
   GEP_RadarSystem system=GEP_RadarSystem.Cast(parent.FindComponent(GEP_RadarSystem));

   if(system) return system;

  }
  return null;

 }
 bool Crew(IEntity user)
 {
  if(!user) return false;

  CharacterControllerComponent life=CharacterControllerComponent.Cast(user.FindComponent(CharacterControllerComponent));

  CompartmentAccessComponent access=CompartmentAccessComponent.Cast(user.FindComponent(CompartmentAccessComponent));

  if(!life || life.GetLifeState()!=ECharacterLifeState.ALIVE || !access || access.IsGettingIn() || access.IsGettingOut() || access.IsSwitchingSeatsAnim()) return false;

  BaseCompartmentSlot seat=access.GetCompartment();

  return seat && seat.IsOccupied() && Find(seat.GetVehicle())==this;

 }
 bool Gunner(IEntity user)
 {
  if(!Crew(user)) return false;

  CompartmentAccessComponent access=CompartmentAccessComponent.Cast(user.FindComponent(CompartmentAccessComponent));

  BaseCompartmentSlot seat=access.GetCompartment();

  TurretControllerComponent controller=TurretControllerComponent.Cast(seat.GetController());
  if(!controller) controller=seat.GetAttachedTurret();

  return TurretCompartmentSlot.Cast(seat) && controller && controller==Gun;

 }
 bool Alive(IEntity entity)
 {
  if(!entity) return false;

  SCR_DamageManagerComponent damage=SCR_DamageManagerComponent.Cast(entity.FindComponent(SCR_DamageManagerComponent));

  return !damage || damage.GetDefaultHitZone().GetHealthScaled()>0;

 }
 void FindParts(IEntity entity)
 {
  string name;
  if(entity.GetPrefabData()) name=entity.GetPrefabData().GetPrefabName();

  if(name.Contains("GepardSearchBase.et")) SearchBase=entity;
  if(name.Contains("GepardSearchHead.et")) SearchHead=entity;
  if(name.Contains("GepardSearchYawSupport.et")) SearchYawSupport=entity;

  if(name.Contains("GepardTrackingHead.et")) TrackerHead=entity;

  if(name.Contains("GepardTrackingBase.et")) TrackerBase=entity;

  TurretControllerComponent controller=TurretControllerComponent.Cast(entity.FindComponent(TurretControllerComponent));

  if(controller && entity.FindComponent(SCR_FireModeManagerComponent)) {Gun=controller;
  Rig=entity;
  }
  for(IEntity child=entity.GetChildren();child;child=child.GetSibling()) FindParts(child);

 }
 vector AntennaOrigin()
 {
  if(SearchHead) return SearchHead.GetOrigin();

  return GetOwner().CoordToParent(Vector(0,3.7,-1.6));

 }
 vector TrackerOrigin()
 {
  if(TrackerHead) return TrackerHead.GetOrigin();

  return AntennaOrigin();

 }
 bool Candidate(IEntity entity)
 {
  IEntity root=entity.GetRootParent();

  if(root==GetOwner() || Candidates.Contains(root)) return true;

  if(root.FindComponent(VehicleHelicopterSimulation) && Candidates.Count()<48) Candidates.Insert(root);

  return true;

 }
 bool Eligible(IEntity entity,vector origin,float maxRange)
 {
  if(!Alive(entity) || !entity.FindComponent(VehicleHelicopterSimulation)) return false;

  vector point=entity.GetOrigin()+Vector(0,2,0);

  float distance=(point-origin).Length();

  return distance>=60 && distance<=maxRange && point[1]-GetGame().GetWorld().GetSurfaceY(point[0],point[2])>=12;

 }
 bool ClearLOS(vector origin,vector point,IEntity target)
 {
  TraceParam trace=new TraceParam();
  trace.Start=origin;
  trace.End=point;

  trace.Flags=TraceFlags.WORLD|TraceFlags.ENTS|TraceFlags.ANY_CONTACT;

  ref array<IEntity> exclude={GetOwner(),target};
  if(Rig) exclude.Insert(Rig);

  trace.ExcludeArray=exclude;

  float hit=GetGame().GetWorld().TraceMove(trace,null);

  return hit>=0.999 || (trace.TraceEnt && trace.TraceEnt.GetRootParent()==target);

 }
 GEP_AirContact Selected()
 {
  foreach(GEP_AirContact contact:Contacts) if(contact.ID==Model.SelectedContact) return contact;

  return null;

 }
 vector Predict(GEP_AirContact contact) {return contact.Position+contact.Velocity*Math.Clamp(Clock-contact.Seen,0,7);
  }
 bool AssistedAim(out vector solution,out float flight)
 {
  if(!Model.CanShowLead(Clock) || !Gun || !Gun.GetWeaponManager() || !Gun.GetWeaponManager().GetCurrentWeapon()) return false;

  BaseWeaponManagerComponent weapons=Gun.GetWeaponManager();
  vector muzzle[4];
  if(!weapons.GetCurrentMuzzleTransform(muzzle)) return false;

  float speed=weapons.GetCurrentWeapon().GetInitialProjectileSpeed();
  if(speed<100) return false;

  vector target=Model.Predict(Clock);
  float distance=(target-muzzle[3]).Length();
  if(distance>1800) return false;

  vector ownVelocity;
  Physics body=GetOwner().GetPhysics();
  if(body) ownVelocity=body.GetVelocity();

  flight=distance/speed;

  for(int i=0;i<3;i++) {solution=target+(Model.EstimatedVelocity-ownVelocity)*(flight+0.05)+Vector(0,4.905*flight*flight,0);
  flight=(solution-muzzle[3]).Length()/speed;
  }
  // Explicit game assistance: constant-velocity/vacuum-drop estimate, not a
  // calibrated drag solution. This never moves the turret or pulls its trigger.
  return Gun.GetTurretComponent() && Gun.GetTurretComponent().GetAimingAngleExcess(solution).Length()<0.1;

 }
 void SearchScan()
 {
  int start=System.GetTickCount();
  Candidates.Clear();
  vector origin=AntennaOrigin();

  GetGame().GetWorld().QueryEntitiesBySphere(origin,Range,Candidate,null,EQueryEntitiesFlags.DYNAMIC);

  vector parentPose[4];
  SearchHead.GetParent().GetWorldTransform(parentPose);

  float angle=Sweep*Math.DEG2RAD;
  vector beam=parentPose[2]*Math.Cos(angle)+parentPose[0]*Math.Sin(angle);

  foreach(IEntity entity:Candidates)
  {
   if(!Eligible(entity,origin,Range)) continue;

   vector truth=entity.GetOrigin()+Vector(0,2,0);
  vector delta=truth-origin;
  vector flat=delta;
  flat[1]=0;

   if(flat.Length()<1) continue;
  flat.Normalize();

   if(vector.Dot(flat,beam)<0.9703 || !ClearLOS(origin,truth,entity)) continue;

   vector measured=truth+Vector(Math.RandomFloatInclusive(-5,5),Math.RandomFloatInclusive(-3,3),Math.RandomFloatInclusive(-5,5));

   GEP_AirContact contact;
  foreach(GEP_AirContact known:Contacts) if(known.Association==entity) {contact=known;
  break;
  }
   if(!contact)
   {
    if(Contacts.Count()>=16) continue;

    contact=new GEP_AirContact();
  contact.Association=entity;
  contact.ID=++Serial;

    if(entity==ActiveAssociation) contact.ID=Model.ActiveTarget;

    contact.Position=measured;
  contact.Seen=Clock;
  contact.Hits=1;
  Contacts.Insert(contact);

   }
   else if(Clock-contact.Seen>0.65)
   {
    float dt=Clock-contact.Seen;
  vector residual=measured-(contact.Position+contact.Velocity*dt);

    contact.Position=contact.Position+contact.Velocity*dt+residual*0.65;

    contact.Velocity=contact.Velocity+residual*(0.28/dt);

    if(contact.Velocity.Length()>180) contact.Velocity=contact.Velocity.Normalized()*180;

    contact.Seen=Clock;
  contact.Hits++;

   }
  }
  float cost=System.GetTickCount()-start;
  ScanCount++;
  ScanSum+=cost;
  ScanPeak=Math.Max(ScanPeak,cost);

 }
 bool InTrackerLimits(vector point)
 {
  if(!TrackerBase || !Gun || !Gun.GetTurretComponent()) return false;

  vector local=TrackerBase.VectorToLocal(point-TrackerOrigin());

  float yaw=Math.Atan2(local[0],local[2])*Math.RAD2DEG;

  float pitch=Math.Atan2(local[1],Math.Sqrt(local[0]*local[0]+local[2]*local[2]))*Math.RAD2DEG;

  return Math.AbsFloat(yaw)<=25 && pitch>=-20 && pitch<=20 && Gun.GetTurretComponent().GetAimingAngleExcess(point).Length()<0.1;

 }
 void MeasureTracking()
 {
  if(!Model.TrackingPowered || Model.ActiveTarget<0 || Model.Invalidated) return;

  if(!Alive(ActiveAssociation)) {Model.Invalidate(Model.ActiveTarget,GEP_LossReason.TARGET_DESTROYED);
  Feedback="Track lost: target destroyed or removed";
  return;
  }
  vector origin=TrackerOrigin();
  vector truth=ActiveAssociation.GetOrigin()+Vector(0,2,0);

  bool qualified=Eligible(ActiveAssociation,origin,4500) && InTrackerLimits(truth) && ClearLOS(origin,truth,ActiveAssociation);

  if(!qualified) {Model.AcceptTrackingMeasurement(Model.ActiveTarget,vector.Zero,vector.Zero,Clock,false);
  return;
  }
  vector measured=truth+Vector(Math.RandomFloatInclusive(-0.8,0.8),Math.RandomFloatInclusive(-0.5,0.5),Math.RandomFloatInclusive(-0.8,0.8));

  vector position=measured;
  vector velocity;

  bool restart=Model.MeasurementCount==0 || Clock-Model.LastMeasurement>GEP_EngagementModel.FRESH_SECONDS;

  if(restart) {Model.MeasurementCount=0;
  Model.AcquisitionStarted=Clock;
  }
  else
  {
   float dt=Clock-Model.LastMeasurement;
  if(dt<0.01) return;

   vector residual=measured-(Model.MeasuredPosition+Model.EstimatedVelocity*dt);

   position=Model.MeasuredPosition+Model.EstimatedVelocity*dt+residual*0.65;

   velocity=Model.EstimatedVelocity+residual*(0.22/dt);

   if(velocity.Length()>180) velocity=velocity.Normalized()*180;

  }
  Model.AcceptTrackingMeasurement(Model.ActiveTarget,position,velocity,Clock,true);

 }
 bool AcquireSighted()
 {
  if(!Gun || !Gun.GetWeaponManager() || !Gun.IsWeaponADS()) {Feedback="Enter the gun sights before sighted acquisition";
  return false;
  }
  vector muzzle[4];
  if(!Gun.GetWeaponManager().GetCurrentMuzzleTransform(muzzle)) return false;

  Candidates.Clear();
  GetGame().GetWorld().QueryEntitiesBySphere(muzzle[3],4500,Candidate,null,EQueryEntitiesFlags.DYNAMIC);

  IEntity best;
  float bestDot=0.9986295;
   // deliberate three-degree sight cone
  foreach(IEntity entity:Candidates)
  {
   vector point=entity.GetOrigin()+Vector(0,2,0);

   if(!Eligible(entity,muzzle[3],4500) || !InTrackerLimits(point) || !ClearLOS(muzzle[3],point,entity)) continue;

   vector direction=(point-muzzle[3]).Normalized();
  float dot=vector.Dot(direction,muzzle[2]);

   if(dot>bestDot) {bestDot=dot;
  best=entity;
  }
  }
  if(!best) {Feedback="No visible airborne helicopter within the sight cone";
  return false;
  }
  int id=++Serial;
  foreach(GEP_AirContact contact:Contacts) if(contact.Association==best) id=contact.ID;

  if(best==ActiveAssociation) id=Model.ActiveTarget;

  ActiveAssociation=best;
  Model.AcquireOptical(id,true,true,true,true,Clock);

  Feedback="Sighted acquisition: tracking radar on; search unchanged";
  return true;

 }
 bool Command(IEntity user,int command)
 {
  if(!Authority() || command<0 || command>7 || !Crew(user) || !Alive(GetOwner())) return false;

  if(!SENSING_ENABLED)
  {
   if(command!=0) return false;
   Deployed=!Deployed;
   CosmeticSnapshot(true);
   Feedback="Radar stowing; sensing unavailable";
   if(Deployed) Feedback="Radar deploying; sensing unavailable";
   Broadcast();
   PrintFormat("GEP_RADAR_DEPLOY requested=%1 sensing=0 display=0",Deployed);
   return true;
  }
  if(command>=2 && !Gunner(user)) {Feedback="Use the gunner station for contact and tracking controls";
  return false;
  }
  bool accepted=true;

  if(command==GEP_RadarCommand.SEARCH_POWER)
  {
   Model.SetSearchPower(!Model.SearchPowered);
  if(!Model.SearchPowered) Contacts.Clear();

   Feedback="Search off; active tracking unchanged";
  if(Model.SearchPowered) Feedback="Search sweeping; wait for qualified returns";

  }
  if(command==GEP_RadarCommand.SEARCH_RANGE)
  {
   if(Range<2000) Range=3000;
  else if(Range<4000) Range=4500;
  else Range=1500;

   Contacts.Clear();
  Model.SelectedContact=-1;
  Feedback="Search range changed; active tracking unchanged";

  }
  if(command==GEP_RadarCommand.NEXT_CONTACT || command==GEP_RadarCommand.PREVIOUS_CONTACT)
  {
   if(!Model.SearchPowered || Contacts.IsEmpty()) {Model.SelectedContact=-1;
  Feedback="No search return to select";
  accepted=false;
  }
   else
   {
    int index=-1;
  for(int i=0;i<Contacts.Count();i++) if(Contacts[i].ID==Model.SelectedContact) index=i;

    if(command==2) index++;
  else index--;
  if(index<0) index=Contacts.Count()-1;
  if(index>=Contacts.Count()) index=0;

    Model.SelectContact(Contacts[index].ID);
  Feedback="Contact selected; aim toward it and acquire deliberately";

   }
  }
  if(command==GEP_RadarCommand.TRACK_POWER)
  {
   Model.SetTrackingPower(!Model.TrackingPowered,Clock);
  Feedback="Tracking radar off; search unchanged";

   if(Model.TrackingPowered) Feedback="Tracking radar ready; search unchanged";

  }
  if(command==GEP_RadarCommand.TRACK_SELECTED)
  {
   GEP_AirContact selected=Selected();

   if(!selected || selected.Hits<2 || Clock-selected.Seen>GEP_EngagementModel.FRESH_SECONDS) {Feedback="Select a fresh confirmed search return";
  accepted=false;
  }
   else if(!InTrackerLimits(Predict(selected))) {Feedback="Train turret toward selected bearing; tracker limit +/-25 az / +/-20 el";
  accepted=false;
  }
   else
   {
    Model.SetTrackingPower(true,Clock);

    accepted=Model.AcquireSelected(true,true,Clock-selected.Seen,Clock);

    if(accepted) {ActiveAssociation=selected.Association;
  Feedback="Selected return acquiring on tracking radar";
  }
   }
  }
  if(command==GEP_RadarCommand.TRACK_SIGHTED) accepted=AcquireSighted();

  if(command==GEP_RadarCommand.UNTRACK) {Model.ClearActive();
  ActiveAssociation=null;
  Feedback="Untracked; search and radar power unchanged";
  }
  PrintFormat("GEP_RADAR_COMMAND command=%1 accepted=%2 search=%3 tracking=%4 selected=%5 active=%6 feedback=%7",command,accepted,Model.SearchPowered,Model.TrackingPowered,Model.SelectedContact,Model.ActiveTarget,Feedback);

  return accepted;

 }
 void Broadcast()
 {
  int flags;
  if(Deployed) flags+=16;
  if(Model.SearchPowered) flags+=1;
  if(Model.TrackingPowered) flags+=2;
  if(Model.Invalidated) flags+=4;
  if(Model.TrackingObstructed) flags+=8;

  array<int> ids={Model.SelectedContact,Model.ActiveTarget,Model.State,Model.AcquisitionSource,Model.LossReason,Model.MeasurementCount,flags};

  array<float> data={Range,Sweep,Clock-Model.LastMeasurement,Clock-Model.AcquisitionStarted};

  for(int p=0;p<3;p++) data.Insert(Model.MeasuredPosition[p]);
  for(int v=0;v<3;v++) data.Insert(Model.EstimatedVelocity[v]);

  foreach(GEP_AirContact contact:Contacts)
  {
   ids.Insert(contact.ID);
  ids.Insert(contact.Hits);

   for(int a=0;a<3;a++) data.Insert(contact.Position[a]);
  for(int b=0;b<3;b++) data.Insert(contact.Velocity[b]);
  data.Insert(Clock-contact.Seen);

  }
  Rpc(Receive,ids,data,Feedback);

 }
 [RplRpc(RplChannel.Unreliable,RplRcver.Broadcast)]
 void Receive(array<int> ids,array<float> data,string feedback)
 {
  if(Authority() || ids.Count()<7 || data.Count()<10) return;

  int count=(ids.Count()-7)/2;
  if(count>16 || ids.Count()!=7+count*2 || data.Count()!=10+count*7) return;

  Model.SelectedContact=ids[0];
  Model.ActiveTarget=ids[1];
  Model.State=ids[2];
  Model.AcquisitionSource=ids[3];
  Model.LossReason=ids[4];
  Model.MeasurementCount=ids[5];

  if(SENSING_ENABLED) Deployed=(ids[6]&16)!=0;
  Model.SearchPowered=(ids[6]&1)!=0;
  Model.TrackingPowered=(ids[6]&2)!=0;
  Model.Invalidated=(ids[6]&4)!=0;
  Model.TrackingObstructed=(ids[6]&8)!=0;

  Range=data[0];
  if(SENSING_ENABLED) Sweep=data[1];
  Model.LastMeasurement=Clock-data[2];
  Model.AcquisitionStarted=Clock-data[3];

  Model.MeasuredPosition=Vector(data[4],data[5],data[6]);
  Model.EstimatedVelocity=Vector(data[7],data[8],data[9]);
  Feedback=feedback;
  Contacts.Clear();

  for(int i=0;i<count;i++)
  {
   int offset=10+i*7;
  GEP_AirContact contact=new GEP_AirContact();
  contact.ID=ids[7+i*2];
  contact.Hits=ids[8+i*2];

   contact.Position=Vector(data[offset],data[offset+1],data[offset+2]);
  contact.Velocity=Vector(data[offset+3],data[offset+4],data[offset+5]);
  contact.Seen=Clock-data[offset+6];
  Contacts.Insert(contact);

  }
 }
 bool InputBound;
 void RouteInput(int command)
 {
  SCR_PlayerController player=SCR_PlayerController.Cast(GetGame().GetPlayerController());
  if(!player || !Crew(player.GetControlledEntity()) || GetGame().GetMenuManager().IsAnyMenuOpen()) return;
  if(command==8) ShowDisplay=!ShowDisplay;
  else player.GEP_RequestRadar(command);
 }
 void OnSearchPower(float value,EActionTrigger reason) {RouteInput(0);}
 void OnSearchRange(float value,EActionTrigger reason) {RouteInput(1);}
 void OnNextContact(float value,EActionTrigger reason) {RouteInput(2);}
 void OnPreviousContact(float value,EActionTrigger reason) {RouteInput(3);}
 void OnTrackingPower(float value,EActionTrigger reason) {RouteInput(4);}
 void OnTrackSelected(float value,EActionTrigger reason) {RouteInput(5);}
 void OnTrackSighted(float value,EActionTrigger reason) {RouteInput(6);}
 void OnUntrack(float value,EActionTrigger reason) {RouteInput(7);}
 void OnDisplay(float value,EActionTrigger reason) {RouteInput(8);}
 void BindInput(bool bind)
 {
  if(InputBound==bind) return;
  InputManager input=GetGame().GetInputManager();
  if(!input) return;
  if(bind)
  {
   input.AddActionListener("GEPSearchPower",EActionTrigger.DOWN,OnSearchPower);
   input.AddActionListener("GEPSearchRange",EActionTrigger.DOWN,OnSearchRange);
   input.AddActionListener("GEPNextContact",EActionTrigger.DOWN,OnNextContact);
   input.AddActionListener("GEPPreviousContact",EActionTrigger.DOWN,OnPreviousContact);
   input.AddActionListener("GEPTrackingPower",EActionTrigger.DOWN,OnTrackingPower);
   input.AddActionListener("GEPTrackSelected",EActionTrigger.DOWN,OnTrackSelected);
   input.AddActionListener("GEPTrackSighted",EActionTrigger.DOWN,OnTrackSighted);
   input.AddActionListener("GEPUntrack",EActionTrigger.DOWN,OnUntrack);
   input.AddActionListener("GEPRadarDisplay",EActionTrigger.DOWN,OnDisplay);
  }
  else
  {
   input.RemoveActionListener("GEPSearchPower",EActionTrigger.DOWN,OnSearchPower);
   input.RemoveActionListener("GEPSearchRange",EActionTrigger.DOWN,OnSearchRange);
   input.RemoveActionListener("GEPNextContact",EActionTrigger.DOWN,OnNextContact);
   input.RemoveActionListener("GEPPreviousContact",EActionTrigger.DOWN,OnPreviousContact);
   input.RemoveActionListener("GEPTrackingPower",EActionTrigger.DOWN,OnTrackingPower);
   input.RemoveActionListener("GEPTrackSelected",EActionTrigger.DOWN,OnTrackSelected);
   input.RemoveActionListener("GEPTrackSighted",EActionTrigger.DOWN,OnTrackSighted);
   input.RemoveActionListener("GEPUntrack",EActionTrigger.DOWN,OnUntrack);
   input.RemoveActionListener("GEPRadarDisplay",EActionTrigger.DOWN,OnDisplay);
  }
  InputBound=bind;
 }
 void LocalInput(float dt)
 {
  if(!SENSING_ENABLED) {BindInput(false); if(Display) Display.Visible(false); return;}
  SCR_PlayerController player=SCR_PlayerController.Cast(GetGame().GetPlayerController());
  IEntity user;
  if(player) user=player.GetControlledEntity();
  bool active=Crew(user) && !GetGame().GetMenuManager().IsAnyMenuOpen();
  BindInput(active);
  if(!active)
  {
   if(Display) Display.Visible(false);
   return;
  }
  InputManager input=GetGame().GetInputManager();
  if(input)
  {
   input.ActivateContext("GEPRadarContext");
   if(!InputAudited)
   {
    InputAudited=true;
    bool found;
    for(int a=0;a<input.GetActionCount();a++) if(input.GetActionName(a)=="GEPTrackingPower") found=true;
    PrintFormat("GEP_RADAR_INPUT_CONFIG actionFound=%1 contextActive=%2 listenerBound=%3",found,input.IsContextActive("GEPRadarContext"),InputBound);
   }
  }
  if(!Display) Display=new GEP_RadarDisplay();
  Display.Visible(ShowDisplay);
  if(ShowDisplay) Display.Update(this,user,dt);
 }
 void Visuals(float dt)
 {
  vector turn[4],pose[4];
  Math3D.AnglesToMatrix(Vector(Sweep,0,0),turn);
  Math3D.MatrixMultiply3(SearchRest,turn,pose);
  pose[3]=SearchRest[3];
  SearchHead.SetLocalTransform(pose);
  SearchHead.Update();

  float desiredYaw;
  float desiredPitch;

  if(Model.TrackingPowered && Model.ActiveTarget>=0 && !Model.Invalidated && Model.MeasurementCount>0 && Model.State!=GEP_TrackState.LOST)
  {
   vector local=TrackerBase.VectorToLocal(Model.Predict(Clock)-TrackerOrigin());

   desiredYaw=Math.Clamp(Math.Atan2(local[0],local[2])*Math.RAD2DEG,-25,25);

   desiredPitch=Math.Clamp(Math.Atan2(local[1],Math.Sqrt(local[0]*local[0]+local[2]*local[2]))*Math.RAD2DEG,-20,20);

  }
  TrackerYaw=Math.Lerp(TrackerYaw,desiredYaw,Math.Min(dt*5,1));
  TrackerPitch=Math.Lerp(TrackerPitch,desiredPitch,Math.Min(dt*5,1));

  Math3D.AnglesToMatrix(Vector(TrackerYaw,TrackerPitch,0),turn);
  Math3D.MatrixMultiply3(TrackerRest,turn,pose);
  pose[3]=TrackerRest[3];
  TrackerHead.SetLocalTransform(pose);
  TrackerHead.Update();

 }

 // Cosmetic-only V1. Requires the coordinated yaw-support/head art hierarchy.
 // No sensor, aiming, contact, weapon or input ownership is added.
 float CosmeticWrap(float angle)
 {
  while(angle>=360) angle-=360;
  while(angle<0) angle+=360;
  return angle;
 }
 float CosmeticIndexDelta(float yaw)
 {
  float delta=180-yaw;
  while(delta>=180) delta-=360;
  while(delta< -180) delta+=360;
  return delta;
 }
 int CosmeticPhase(bool requested589,float progress589,float yaw589)
 {
  if(requested589 && progress589<1.0)
  {
   return 1; // DEPLOYING
  }
  if(requested589)
  {
   return 4; // SPINNING
  }
  if(progress589<=0.0)
  {
   return 0; // STOWED
  }
  if(progress589>=1.0 && Math.AbsFloat(CosmeticIndexDelta(yaw589))>0.00001)
  {
   return 3; // INDEXING
  }
  return 2; // FOLDING
 }
 void CosmeticStep(float dt)
 {
  if(dt<=0) return;
  // At most two boundaries per constant-target step; remainder time is consumed.
  for(int boundary=0;boundary<4 && dt>0;boundary++)
  {
   float duration;
   if(Deployed)
   {
    if(Deployment>=1) {Sweep=CosmeticWrap(Sweep+180*dt);return;}
    Sweep=180;duration=(1-Deployment)*3;
    if(dt>=duration) {dt-=duration;Deployment=1;continue;}
    Deployment+=dt/3;return;
   }
   float delta=CosmeticIndexDelta(Sweep);
   if(Deployment>=1 && Math.AbsFloat(delta)>0.00001)
   {
    duration=Math.AbsFloat(delta)/180;
    if(dt>=duration) {dt-=duration;Sweep=180;continue;}
    float sign=1;if(delta<0) sign=-1;
    Sweep=CosmeticWrap(Sweep+sign*180*dt);return;
   }
   Sweep=180;
   if(Deployment<=0) {Deployment=0;return;}
   duration=Deployment*3;
   if(dt>=duration) {dt-=duration;Deployment=0;continue;}
   Deployment-=dt/3;return;
  }
 }
 void CosmeticSnapshot(bool reliable)
 {
  if(!Authority() || !PartsReady) return;
  if(CosmeticSequence<CosmeticLastReceived) CosmeticSequence=CosmeticLastReceived;
  CosmeticSequence++;
  int phase=CosmeticPhase(Deployed,Deployment,Sweep);
  if(reliable) Rpc(ReceiveCosmeticReliable,1,CosmeticSequence,Deployed,Deployment,Sweep,phase);
  else Rpc(ReceiveCosmeticUnreliable,1,CosmeticSequence,Deployed,Deployment,Sweep,phase);
 }
 void AcceptCosmeticSnapshot(int version,int sequence,bool target,float progress,float yaw,int phase)
 {
  if(Authority() || version!=1 || sequence<=CosmeticLastReceived) return;
  // Reject NaNs, range violations and states outside the reachable manifold.
  if(progress!=progress || yaw!=yaw || progress<0 || progress>1 || yaw<0 || yaw>=360) return;
  if(progress<1 && Math.AbsFloat(CosmeticIndexDelta(yaw))>0.00001) return;
  if(CosmeticPhase(target,progress,yaw)!=phase) return;
  // Atomic state correction. Do not independently lerp yaw and pitch across phases.
  CosmeticLastReceived=sequence;CosmeticSynced=true;
  Deployed=target;Deployment=progress;Sweep=yaw;
 }
 [RplRpc(RplChannel.Reliable,RplRcver.Broadcast)]
 void ReceiveCosmeticReliable(int version,int sequence,bool target,float progress,float yaw,int phase)
 {
  AcceptCosmeticSnapshot(version,sequence,target,progress,yaw,phase);
 }
 [RplRpc(RplChannel.Unreliable,RplRcver.Broadcast)]
 void ReceiveCosmeticUnreliable(int version,int sequence,bool target,float progress,float yaw,int phase)
 {
  AcceptCosmeticSnapshot(version,sequence,target,progress,yaw,phase);
 }
 void DeploymentFrame(float dt)
 {
  bool authority=Authority();
  if(authority && !Alive(GetOwner())) Deployed=false;
  if(authority && !CosmeticWasAuthority) CosmeticSnapshot(true);
  CosmeticWasAuthority=authority;
  if(Authority() || CosmeticSynced) CosmeticStep(dt);
  float smooth=Deployment*Deployment*(3-2*Deployment);
  vector turn[4],pose[4];
  // The registered base stays upright, retaining its attachment to GEP_TurretYaw.
  SearchBase.SetLocalTransform(SearchBaseRest);SearchBase.Update();
  Math3D.AnglesToMatrix(Vector(Sweep,0,0),turn);
  Math3D.MatrixMultiply3(SearchYawRest,turn,pose);pose[3]=SearchYawRest[3];
  SearchYawSupport.SetLocalTransform(pose);SearchYawSupport.Update();
  Math3D.AnglesToMatrix(Vector(0,-90*(1-smooth),0),turn);
  Math3D.MatrixMultiply3(SearchRest,turn,pose);pose[3]=SearchRest[3];
  SearchHead.SetLocalTransform(pose);SearchHead.Update();
  if(Authority())
  {
   int phase=CosmeticPhase(Deployed,Deployment,Sweep);
   NetworkTimer+=dt;
   if(phase!=CosmeticPreviousPhase) {CosmeticPreviousPhase=phase;CosmeticSnapshot(true);NetworkTimer=0;}
   else if(NetworkTimer>=0.25) {NetworkTimer=0;CosmeticSnapshot(false);}
  }
 }

 override void OnPostInit(IEntity owner) {SetEventMask(owner,EntityEvent.FRAME);
  }
 override void EOnFrame(IEntity owner,float timeSlice)
 {
  if(!GetGame().InPlayMode()) return;
  Clock+=timeSlice;

  if(!PartsReady)
  {
   FindParts(owner);
  if(!SearchBase || !SearchYawSupport || !SearchHead || !TrackerHead || !TrackerBase || !Gun) return;

   SearchBase.GetLocalTransform(SearchBaseRest);
   SearchHead.GetLocalTransform(SearchRest);
   SearchYawSupport.GetLocalTransform(SearchYawRest);
   if(!CosmeticSynced) {Deployment=0;Sweep=180;}
  TrackerHead.GetLocalTransform(TrackerRest);
  PartsReady=true;

   Print("GEP_RADAR_READY deploymentOnly=1 sensing=0 display=0 nativeGunUnmodified=1");

  }
  if(!SearchBase || !SearchYawSupport || !SearchHead || !Gun) {return;} // Destroyed slotted parts can disappear.
  if(!SENSING_ENABLED) {DeploymentFrame(timeSlice);return;}
  if(Authority())
  {
   if(!Alive(owner)) {Model.SetSearchPower(false);
  Model.SetTrackingPower(false,Clock);
  Model.ClearActive();
  ActiveAssociation=null;
  Contacts.Clear();
  }
   if(Model.SearchPowered)
   {
    Sweep+=timeSlice*180;
  while(Sweep>=360) Sweep-=360;
  ScanTimer+=timeSlice;
  if(ScanTimer>=0.1){ScanTimer=0;
  SearchScan();
  }
   }
   for(int j=Contacts.Count()-1;j>=0;j--) if(Clock-Contacts[j].Seen>7)
   {if(Model.SelectedContact==Contacts[j].ID) Model.SelectedContact=-1;
  Contacts.Remove(j);
  }
   TrackTimer+=timeSlice;
  if(TrackTimer>=0.2) {TrackTimer=0;
  MeasureTracking();
  }Model.Tick(Clock);

   NetworkTimer+=timeSlice;
  if(NetworkTimer>=0.25){NetworkTimer=0;
  Broadcast();
  }
   LogTimer+=timeSlice;
  if(LogTimer>=5)
   {
    LogTimer=0;
  float mean;
  if(ScanCount>0) mean=ScanSum/ScanCount;

    PrintFormat("GEP_RADAR_STATE search=%1 trackPower=%2 contacts=%3 selected=%4 active=%5 state=%6 measurements=%7 age=%8 lead=%9",Model.SearchPowered,Model.TrackingPowered,Contacts.Count(),Model.SelectedContact,Model.ActiveTarget,Model.State,Model.MeasurementCount,Clock-Model.LastMeasurement,Model.CanShowLead(Clock));

    PrintFormat("GEP_RADAR_COST meanMs=%1 peakMs=%2",mean,ScanPeak);

   }
  }
  else {if(Model.SearchPowered) {Sweep+=timeSlice*180;
  while(Sweep>=360) Sweep-=360;
  }Model.Tick(Clock);
  }
  Visuals(timeSlice);
  LocalInput(timeSlice);

 }
 override void OnDelete(IEntity owner) {BindInput(false);
 if(Display) Display.Destroy();
  }
}

// Reliable requests bind to the actual requesting player's controller/seat.
modded class SCR_PlayerController
{
 void GEP_RequestRadar(int command)
 {
  if(RplSession.Mode()==RplMode.None) {GEP_ServerRadar(command);
  return;
  }
  Rpc(GEP_ServerRadar,command);

 }
 [RplRpc(RplChannel.Reliable,RplRcver.Server)]
 void GEP_ServerRadar(int command)
 {
  if(command<0 || command>7) return;

  IEntity user=GetControlledEntity();

  if(!user) return;

  GEP_RadarSystem radar=GEP_RadarSystem.Find(CompartmentAccessComponent.GetVehicleIn(user));
  if(radar) radar.Command(user,command);

 }
}

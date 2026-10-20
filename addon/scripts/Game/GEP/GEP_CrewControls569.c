// Native hatch states and native seat actions remain the network authority path.
// No locomotion, fake radar sensing, camera takeover, or synthetic driving inputs.
// Source-only594: native compartment requests remain the replicated authority.
// Inherits SCR_CompartmentUserAction's local-only execution contract.
class GEP_GetInHatch569 : SCR_GetInUserAction
{
 override bool GetActionNameScript(out string outName)
 {
  if(CargoCompartmentSlot.Cast(GetCompartmentSlot())){outName="Get in commander";return true;}
  return super.GetActionNameScript(outName);
 }

 protected IEntity m_HatchOwner594;
 protected IEntity m_HatchUser594;
 protected BaseCompartmentSlot m_HatchSeat594;
 protected int m_HatchDoor594=-1;
 protected int m_HatchPhase594; // 0 idle, 1 open/clearance, 2 boarding, 3 closing
 protected int m_HatchToken594;
 protected int m_HatchPolls594;
 protected bool m_HatchSeenOpen594;
 protected bool m_HatchCloseRequested594;
 protected vector m_HatchApproachLocal594;
 const int HATCH_MAX_POLLS594=60; // 100ms polls; every attempt is finite
 const float HATCH_OPEN_READY594=0.95;
 const float HATCH_CLOSED_READY594=0.05;
 const float HATCH_APPROACH_DRIFT594=1.5;

 int ResolveHatchDoor594(IEntity user)
 {
  BaseCompartmentSlot seat=GetCompartmentSlot();if(!user||!seat)return -1;
  array<int> doors={};seat.GetAvailableDoorIndices(doors);
  int door=-1;
  if(doors.Count()==1)door=doors[0];
  else door=seat.PickDoorIndexForPoint(user.GetOrigin());
  if(door<0||!doors.Contains(door))return -1;
  SetCompartmentDoorInfoIndex(door);return door;
 }
 override bool CanBePerformedScript(IEntity user)
 {
  if(m_HatchPhase594!=0){SetCannotPerformReason("Hatch entry in progress");return false;}
  if(ResolveHatchDoor594(user)<0)return false;
  // Keep native lock/faction/occupancy/reservation/obstruction/supply checks.
  // A closed hatch is no longer an additional custom veto.
  return super.CanBePerformedScript(user);
 }
 bool NativeHatchEntryAllowed594()
 {
  SetCompartmentDoorInfoIndex(m_HatchDoor594);
  return CanBeShown(m_HatchUser594)&&super.CanBePerformedScript(m_HatchUser594);
 }
 float HatchProgress594()
 {
  if(PilotCompartmentSlot.Cast(m_HatchSeat594))
  {
   GEP_DriverStation driver=GEP_DriverStation.Cast(m_HatchOwner594.FindComponent(GEP_DriverStation));
   if(driver&&driver.GetHatchVisual())return driver.GetHatchProgress();
   return -1;
  }
  GEP_TurretHatches569 lids=GEP_TurretHatches569.Cast(m_HatchOwner594.FindComponent(GEP_TurretHatches569));
  if(!lids||!lids.Ready)return -1;
  if(m_HatchDoor594==0)return lids.GunnerProgress;
  if(m_HatchDoor594==1)return lids.CommanderProgress;
  return -1;
 }
 void EndHatchEntry594(string reason)
 {
  
  m_HatchToken594++;
  if(GetGame())GetGame().GetCallqueue().Remove(PollHatchEntry594);
  m_HatchPhase594=0;m_HatchOwner594=null;m_HatchUser594=null;m_HatchSeat594=null;
  m_HatchDoor594=-1;m_HatchPolls594=0;m_HatchSeenOpen594=false;m_HatchCloseRequested594=false;
 }
 void ScheduleHatchPoll594()
 {
  GetGame().GetCallqueue().CallLater(PollHatchEntry594,100,false,m_HatchToken594);
 }
 bool HatchActorAndVehicleAlive594()
 {
  if(!m_HatchOwner594||!m_HatchUser594||m_HatchOwner594.IsDeleted()||m_HatchUser594.IsDeleted())return false;
  if(!m_HatchSeat594||GetOwner()!=m_HatchOwner594||GetCompartmentSlot()!=m_HatchSeat594)return false;
  CharacterControllerComponent life=CharacterControllerComponent.Cast(m_HatchUser594.FindComponent(CharacterControllerComponent));
  if(!life||life.GetLifeState()!=ECharacterLifeState.ALIVE)return false;
  SCR_DamageManagerComponent ownDamage=SCR_DamageManagerComponent.Cast(m_HatchOwner594.FindComponent(SCR_DamageManagerComponent));
  if(ownDamage&&ownDamage.GetState()==EDamageState.DESTROYED)return false;
  SCR_DamageManagerComponent rootDamage=SCR_DamageManagerComponent.Cast(m_HatchOwner594.GetRootParent().FindComponent(SCR_DamageManagerComponent));
  return !rootDamage||rootDamage.GetState()!=EDamageState.DESTROYED;
 }
 override void PerformAction(IEntity pOwnerEntity,IEntity pUserEntity)
 {
  if(!pOwnerEntity||!pUserEntity||pOwnerEntity!=GetOwner())return;
  if(!CanBeShown(pUserEntity)||!CanBePerformed(pUserEntity))return;
  BaseCompartmentSlot seat=GetCompartmentSlot();
  CompartmentAccessComponent access=CompartmentAccessComponent.Cast(pUserEntity.FindComponent(CompartmentAccessComponent));
  BaseCompartmentManagerComponent manager=BaseCompartmentManagerComponent.Cast(pOwnerEntity.FindComponent(BaseCompartmentManagerComponent));
  int door=ResolveHatchDoor594(pUserEntity);
  if(!seat||!access||!manager||door<0)return;
  if(access.GetCompartment()||access.IsGettingIn()||access.IsGettingOut()||access.IsSwitchingSeatsAnim())return;
  IEntity doorUser=manager.GetDoorUser(door);if(doorUser&&doorUser!=pUserEntity)return;
  IEntity vehicle=SCR_EntityHelper.GetMainParent(pOwnerEntity,true);
  if(!vehicle||!access.CanAccessDoor(vehicle,manager,door))return;
  m_HatchOwner594=pOwnerEntity;m_HatchUser594=pUserEntity;m_HatchSeat594=seat;m_HatchDoor594=door;
  m_HatchApproachLocal594=pOwnerEntity.CoordToLocal(pUserEntity.GetOrigin());
  m_HatchToken594++;m_HatchPolls594=0;m_HatchPhase594=1;
  m_HatchSeenOpen594=manager.IsDoorOpen(door);m_HatchCloseRequested594=false;
  if(!HatchActorAndVehicleAlive594()){EndHatchEntry594("initial_actor_or_vehicle_invalid");return;}
  if(!m_HatchSeenOpen594)
  {
   // Request once. A rejection leaves the native state alone and ends this attempt.
   if(!access.OpenDoor(pOwnerEntity,ECharacterDoorAnimType.NO_CHARACTER_ANIM,door))
   {EndHatchEntry594("open_request_rejected");return;}
  }
  ScheduleHatchPoll594();
 }
 void PollHatchEntry594(int token)
 {
  if(token!=m_HatchToken594||m_HatchPhase594==0)return;
  m_HatchPolls594++;
  if(m_HatchPolls594>HATCH_MAX_POLLS594){EndHatchEntry594("timeout");return;}
  if(!HatchActorAndVehicleAlive594()){EndHatchEntry594("owner_or_actor_lost");return;}
  CompartmentAccessComponent access=CompartmentAccessComponent.Cast(m_HatchUser594.FindComponent(CompartmentAccessComponent));
  BaseCompartmentManagerComponent manager=BaseCompartmentManagerComponent.Cast(m_HatchOwner594.FindComponent(BaseCompartmentManagerComponent));
  if(!access||!manager||m_HatchDoor594<0){EndHatchEntry594("native_access_lost");return;}
  IEntity doorUser=manager.GetDoorUser(m_HatchDoor594);
  if(doorUser&&doorUser!=m_HatchUser594){EndHatchEntry594("other_door_user");return;}
  if(access.IsGettingOut()||access.IsSwitchingSeatsAnim()){EndHatchEntry594("other_transition");return;}
  if(m_HatchPhase594==1)
  {
   if(access.GetCompartment()){EndHatchEntry594("another_entry");return;}
   if(m_HatchSeat594.GetOccupant()||(m_HatchSeat594.IsReserved()&&!m_HatchSeat594.IsReservedBy(m_HatchUser594)))
   {EndHatchEntry594("seat_no_longer_available");return;}
   // This is a cancellation bound relative to the starting approach point,
   // not a replacement for native door access/obstruction eligibility.
   if(vector.Distance(m_HatchUser594.GetOrigin(),m_HatchOwner594.CoordToParent(m_HatchApproachLocal594))>HATCH_APPROACH_DRIFT594)
   {EndHatchEntry594("walked_away");return;}
   bool open=manager.IsDoorOpen(m_HatchDoor594);
   if(!open&&m_HatchSeenOpen594){EndHatchEntry594("door_closed_during_wait");return;}
   if(open)m_HatchSeenOpen594=true;
   // An accepted native OpenDoor still owns its queue until it releases the
   // door user. Wait; do not treat that temporary native busy state as denial.
   if(access.IsGettingIn()||!open||doorUser||HatchProgress594()<HATCH_OPEN_READY594)
   {ScheduleHatchPoll594();return;}
   if(!NativeHatchEntryAllowed594()){EndHatchEntry594("native_entry_denied");return;}
   m_HatchPhase594=2;
   if(!access.GetInVehicle(m_HatchOwner594,m_HatchSeat594,false,m_HatchDoor594,ECloseDoorAfterActions.CLOSE_DOOR,false))
   {EndHatchEntry594("boarding_request_rejected");return;}
   ScheduleHatchPoll594();return;
  }
  if(m_HatchPhase594==2)
  {
   BaseCompartmentSlot actual=access.GetCompartment();
   if(actual&&actual!=m_HatchSeat594){EndHatchEntry594("different_seat");return;}
   if(access.IsGettingIn()||!actual){ScheduleHatchPoll594();return;}
   if(actual.GetOccupant()!=m_HatchUser594){EndHatchEntry594("occupant_mismatch");return;}
   m_HatchPhase594=3;
  }
  if(m_HatchPhase594==3)
  {
   if(access.IsGettingIn()||access.GetCompartment()!=m_HatchSeat594||m_HatchSeat594.GetOccupant()!=m_HatchUser594)
   {EndHatchEntry594("occupancy_lost");return;}
   // Never compete with the still-finishing native boarding/door queue.
   if(doorUser){ScheduleHatchPoll594();return;}
   if(manager.IsDoorOpen(m_HatchDoor594))
   {
    if(!m_HatchCloseRequested594)
    {
     IEntity vehicle=SCR_EntityHelper.GetMainParent(m_HatchOwner594,true);
     if(!access.CanAccessDoor(vehicle,manager,m_HatchDoor594))
     {EndHatchEntry594("native_close_denied");return;}
     if(!access.CloseDoor(m_HatchOwner594,ECharacterDoorAnimType.NO_CHARACTER_ANIM,m_HatchDoor594))
     {EndHatchEntry594("close_request_rejected");return;}
     m_HatchCloseRequested594=true;
    }
   }
   else
   {
    float progress=HatchProgress594();
    if(progress>=0&&progress<=HATCH_CLOSED_READY594){EndHatchEntry594("complete");return;}
   }
   ScheduleHatchPoll594();
  }
 }
 void CancelManualHatch594(IEntity owner,IEntity user,int door)
 {
  if(m_HatchPhase594!=0&&m_HatchOwner594==owner&&m_HatchDoor594==door)
   EndHatchEntry594("manual_hatch_override");
 }
 static void CancelPendingHatch594(IEntity owner,IEntity user,int door)
 {
  if(!owner||!user)return;
  BaseActionsManagerComponent manager=BaseActionsManagerComponent.Cast(owner.FindComponent(BaseActionsManagerComponent));
  if(!manager)return;
  array<UserActionContext> contexts={};manager.GetContextList(contexts);
  foreach(UserActionContext context:contexts)
  {
   array<BaseUserAction> actions={};context.GetActionsList(actions);
   foreach(BaseUserAction action:actions)
   {
    GEP_GetInHatch569 entry=GEP_GetInHatch569.Cast(action);
    if(entry)entry.CancelManualHatch594(owner,user,door);
   }
  }
 }
 void ~GEP_GetInHatch569()
 {
  if(GetGame())GetGame().GetCallqueue().Remove(PollHatchEntry594);
 }
}

// Original hatch motion is procedural. Stock LAV reach animations do not match
// these hull/roof portals; retain native access checks and replicated door state.
class GEP_OpenHatch569 : SCR_OpenVehicleDoorUserAction
{
 override void PerformAction(IEntity pOwnerEntity,IEntity pUserEntity)
 {
  IEntity owner=pOwnerEntity;IEntity user=pUserEntity;
  if(!CanBeShown(user)||!CanBePerformed(user))return;
  CompartmentAccessComponent access=CompartmentAccessComponent.Cast(user.FindComponent(CompartmentAccessComponent));
  if(access)
  {
   GEP_GetInHatch569.CancelPendingHatch594(owner,user,GetDoorIndex());
   access.OpenDoor(owner,ECharacterDoorAnimType.NO_CHARACTER_ANIM,GetDoorIndex());
  }
 }
}
class GEP_CloseHatch569 : SCR_CloseVehicleDoorUserAction
{
 override void PerformAction(IEntity pOwnerEntity,IEntity pUserEntity)
 {
  IEntity owner=pOwnerEntity;IEntity user=pUserEntity;
  if(!CanBeShown(user)||!CanBePerformed(user))return;
  CompartmentAccessComponent access=CompartmentAccessComponent.Cast(user.FindComponent(CompartmentAccessComponent));
  if(access)
  {
   GEP_GetInHatch569.CancelPendingHatch594(owner,user,GetDoorIndex());
   access.CloseDoor(owner,ECharacterDoorAnimType.NO_CHARACTER_ANIM,GetDoorIndex());
  }
 }
}
class GEP_TurretHatches569Class : ScriptComponentClass {}
class GEP_TurretHatches569 : ScriptComponent
{
 IEntity GunnerLid;
 IEntity CommanderLid;
 vector GunnerRest[4];
 vector CommanderRest[4];
 float GunnerProgress;
 float CommanderProgress;
 bool Ready;
 // Native hierarchy pivot follows turret yaw; local lid motion is hinge-only.
 void Pose(IEntity lid,vector rest[4],float amount)
 {
  if(!lid)return;
  float eased=amount*amount*(3-2*amount);
  vector turn[4],pose[4];
  Math3D.AnglesToMatrix(Vector(0,105*eased,0),turn);
  Math3D.MatrixMultiply3(rest,turn,pose);pose[3]=rest[3];
  lid.SetLocalTransform(pose);lid.Update();
 }
 override void OnPostInit(IEntity owner){SetEventMask(owner,EntityEvent.POSTFRAME);}
 override void EOnPostFrame(IEntity owner,float timeSlice)
 {
  float dt=timeSlice;
  if(!GetGame().InPlayMode())return;
  if(!Ready)
  {
   for(IEntity child=owner.GetChildren();child;child=child.GetSibling())
   {
    if(!child.GetPrefabData())continue;
    string name=child.GetPrefabData().GetPrefabName();
    if(name.Contains("GepardGunnerHatch569.et"))GunnerLid=child;
    if(name.Contains("GepardCommanderHatch569.et"))CommanderLid=child;
   }
   if(!GunnerLid||!CommanderLid)return;
   GunnerLid.GetLocalTransform(GunnerRest);CommanderLid.GetLocalTransform(CommanderRest);Ready=true;
  }
  BaseCompartmentManagerComponent manager=BaseCompartmentManagerComponent.Cast(owner.FindComponent(BaseCompartmentManagerComponent));
  if(!manager)return;
  float gunnerTarget;float commanderTarget;
  if(manager.IsDoorOpen(0))gunnerTarget=1;
  if(manager.IsDoorOpen(1))commanderTarget=1;
  GunnerProgress=Math.Lerp(GunnerProgress,gunnerTarget,Math.Min(dt*4,1));
  CommanderProgress=Math.Lerp(CommanderProgress,commanderTarget,Math.Min(dt*4,1));
  Pose(GunnerLid,GunnerRest,GunnerProgress);Pose(CommanderLid,CommanderRest,CommanderProgress);
 }
}
class GEP_CrewControls569Class : ScriptComponentClass {}
class GEP_CrewControls569 : ScriptComponent
{
 bool Bound;
 GEP_RadarSystem Radar;
 Widget Root;
 TextWidget Hint;
 float Timer;
 IEntity User()
 {
  SCR_PlayerController player=SCR_PlayerController.Cast(GetGame().GetPlayerController());
  if(!player||!Radar||!Radar.Alive(GetOwner())||!Radar.Crew(player.GetControlledEntity())||GetGame().GetMenuManager().IsAnyMenuOpen())return null;
  return player.GetControlledEntity();
 }
 void RadarInput(float value,EActionTrigger reason)
 {
  if(!User())return;
  SCR_PlayerController player=SCR_PlayerController.Cast(GetGame().GetPlayerController());
  player.GEP_RequestRadar(0);
 }
 void ViewInput(float value,EActionTrigger reason)
 {
  IEntity user=User();if(!user)return;
  GEP_DriverStation station=GEP_DriverStation.Cast(GetOwner().FindComponent(GEP_DriverStation));
  if(station&&station.IsDriver(user))station.SelectView(user,(station.GetViewMode(user)+1)%4);
 }
 void CabinInput(float value,EActionTrigger reason)
 {
  IEntity user=User();if(!user)return;
  GEP_DriverStation station=GEP_DriverStation.Cast(GetOwner().FindComponent(GEP_DriverStation));
  if(station&&station.IsDriver(user))station.SelectView(user,0);
 }
 void HatchInput(float value,EActionTrigger reason)
 {
  IEntity user=User();if(!user)return;
  CompartmentAccessComponent access=CompartmentAccessComponent.Cast(user.FindComponent(CompartmentAccessComponent));
  if(!access)return;
  BaseCompartmentSlot seat=access.GetCompartment();if(!seat)return;
  array<int> doors={};seat.GetAvailableDoorIndices(doors);
  BaseActionsManagerComponent manager=BaseActionsManagerComponent.Cast(seat.GetOwner().FindComponent(BaseActionsManagerComponent));
  if(!manager)return;
  array<UserActionContext> contexts={};manager.GetContextList(contexts);
  foreach(UserActionContext context:contexts)
  {
   array<BaseUserAction> actions={};context.GetActionsList(actions);
   foreach(BaseUserAction action:actions)
   {
    VehicleDoorUserAction door=VehicleDoorUserAction.Cast(action);
    if(!door||!doors.Contains(door.GetDoorIndex()))continue;
    if(!GEP_OpenHatch569.Cast(action)&&!GEP_CloseHatch569.Cast(action))continue;
    action.SetActiveContext(context);
    if(!action.CanBeShown(user)||!action.CanBePerformed(user))continue;
    door.PerformAction(seat.GetOwner(),user);return;
   }
  }
 }
 void Bind(bool enabled)
 {
  if(Bound==enabled)return;
  InputManager input=GetGame().GetInputManager();if(!input)return;
  if(enabled)
  {
   input.AddActionListener("GEPSearchPower",EActionTrigger.DOWN,RadarInput);
   input.AddActionListener("GEPDriverView569",EActionTrigger.DOWN,ViewInput);
   input.AddActionListener("GEPDriverCabin569",EActionTrigger.DOWN,CabinInput);
   input.AddActionListener("GEPHatch569",EActionTrigger.DOWN,HatchInput);
  }
  else
  {
   input.RemoveActionListener("GEPSearchPower",EActionTrigger.DOWN,RadarInput);
   input.RemoveActionListener("GEPDriverView569",EActionTrigger.DOWN,ViewInput);
   input.RemoveActionListener("GEPDriverCabin569",EActionTrigger.DOWN,CabinInput);
   input.RemoveActionListener("GEPHatch569",EActionTrigger.DOWN,HatchInput);
  }
  Bound=enabled;
 }
 override void OnPostInit(IEntity owner){SetEventMask(owner,EntityEvent.FRAME);}
 override void EOnFrame(IEntity owner,float timeSlice)
 {
  float dt=timeSlice;
  if(!GetGame().InPlayMode()||System.IsConsoleApp())return;
  if(!Radar)Radar=GEP_RadarSystem.Cast(owner.FindComponent(GEP_RadarSystem));
  IEntity user=User();Bind(user!=null);
  // Controls remain registered native bindings. No free-text crew/driver HUD.
  if(Root)Root.SetVisible(false);
  if(!user)return;
  GetGame().GetInputManager().ActivateContext("GEPCrewContext569");
  Timer+=dt;if(Timer<0.2)return;Timer=0;
  // Optional passive evidence only. Never inject actions or change assistance.
 }
 override void OnDelete(IEntity owner){Bind(false);if(Root)Root.RemoveFromHierarchy();Root=null;}
}

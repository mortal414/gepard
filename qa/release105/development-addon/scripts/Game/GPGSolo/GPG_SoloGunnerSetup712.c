// Original QA setup only. Public native actor seating is not exterior boarding
// acceptance. No vehicle transforms, drive inputs, aim, servo or damage writes.
class GPG_SoloGunnerSetup712
{
 protected IEntity m_Actor712,m_Hull712;
 protected BaseCompartmentSlot m_Seat712;
 protected float m_StartMs712,m_SettledMs712=-1;
 protected int m_State712;
 protected string m_Reason712="not_started";

 static void Collect712(IEntity entity712,IEntity hull712,array<BaseCompartmentSlot> result712,int depth712=0)
 {
  if(!entity712 || depth712>16)return;
  BaseCompartmentManagerComponent manager712=BaseCompartmentManagerComponent.Cast(entity712.FindComponent(BaseCompartmentManagerComponent));
  if(manager712)
  {
   array<BaseCompartmentSlot> slots712={};
   manager712.GetCompartments(slots712);
   foreach(BaseCompartmentSlot slot712:slots712)
   {
    if(!TurretCompartmentSlot.Cast(slot712) || result712.Find(slot712)>=0)continue;
    TurretControllerComponent control712=TurretControllerComponent.Cast(slot712.GetController());
    if(!control712)control712=slot712.GetAttachedTurret();
    if(control712 && control712.GetOwner().GetRootParent()==hull712)result712.Insert(slot712);
   }
  }
  for(IEntity child712=entity712.GetChildren();child712;child712=child712.GetSibling())Collect712(child712,hull712,result712,depth712+1);
 }
 static BaseCompartmentSlot Find712(IEntity hull712)
 {
  array<BaseCompartmentSlot> found712={};
  Collect712(hull712,hull712,found712);
  PrintFormat("GPG_SETUP_GUNNER_SLOTS hull=%1 uniqueSlots=%2",GPG_SoloRead708.NativeID624(hull712),found712.Count());
  if(found712.Count()!=1)return null;
  return found712[0];
 }
 static void InspectEntry712(IEntity actor712,IEntity hull712)
 {
  if(!actor712 || !hull712)return;
  BaseCompartmentSlot seat712=Find712(hull712);
  if(!seat712 || !seat712.GetOwner())return;
  IEntity owner712=seat712.GetOwner();
  CompartmentAccessComponent access712=CompartmentAccessComponent.Cast(actor712.FindComponent(CompartmentAccessComponent));
  BaseCompartmentManagerComponent doors712=BaseCompartmentManagerComponent.Cast(owner712.FindComponent(BaseCompartmentManagerComponent));
  BaseActionsManagerComponent actions712=BaseActionsManagerComponent.Cast(owner712.FindComponent(BaseActionsManagerComponent));
  array<int> available712={};
  seat712.GetAvailableDoorIndices(available712);
  int door712=-1;
  if(available712.Count()==1)door712=available712[0];
  else door712=seat712.PickDoorIndexForPoint(actor712.GetOrigin());
  IEntity main712=SCR_EntityHelper.GetMainParent(owner712,true);
  bool doorAccess712=false;
  if(access712 && doors712 && main712 && door712>=0)doorAccess712=access712.CanAccessDoor(main712,doors712,door712);
  PrintFormat("GPG_ENTRY_GEOMETRY actor=%1 hull=%2 seatOwner=%3 actorLocalToSeatOwner=%4 door=%5 doorAccess=%6",actor712.GetOrigin(),GPG_SoloRead708.NativeID624(hull712),GPG_SoloRead708.NativeID624(owner712),owner712.CoordToLocal(actor712.GetOrigin()),door712,doorAccess712);
  if(!actions712)
  {
   Print("GPG_ENTRY_CONTEXT status=missing_actions_manager");
   return;
  }
  UserActionContext context712=actions712.GetContext("gunner_entry");
  if(!context712)
  {
   Print("GPG_ENTRY_CONTEXT status=missing_gunner_entry_context");
   return;
  }
  vector world712[4];
  bool valid712=context712.GetTransformationWorld(world712);
  PrintFormat("GPG_ENTRY_CONTEXT name=%1 validWorld=%2 origin=%3 radius=%4 height=%5 actorDistance=%6 lineOfSightRequired=%7 actorOriginInVisibilityAngle=%8",context712.GetContextName(),valid712,context712.GetOrigin(),context712.GetRadius(),context712.GetHeight(),vector.Distance(actor712.GetOrigin(),context712.GetOrigin()),context712.ShouldCheckLineOfSight(),context712.IsInVisibilityAngle(actor712.GetOrigin()));
  array<BaseUserAction> choices712={};
  context712.GetActionsList(choices712);
  int entryCount712;
  foreach(BaseUserAction action712:choices712)
  {
   GEP_GetInHatch569 entry712=GEP_GetInHatch569.Cast(action712);
   if(!entry712)continue;
   entryCount712++;
   PrintFormat("GPG_ENTRY_ACTION slotMatches=%1 ownerMatches=%2 shown=%3 performedEligible=%4 method=predicates_only_no_PerformAction",entry712.GetCompartmentSlot()==seat712,entry712.GetOwner()==owner712,entry712.CanBeShown(actor712),entry712.CanBePerformed(actor712));
  }
  PrintFormat("GPG_ENTRY_ACTION_COUNT nativeEntryActions=%1 uiFocusAndRaySelection=NOT_TESTED",entryCount712);
 }
 protected void Fail712(string reason712)
 {
  m_State712=-1;
  m_Reason712=reason712;
  PrintFormat("GPG_SETUP_GUNNER_FAILED reason=%1 actor=%2 hull=%3 no_retry_or_forced_egress=1",reason712,GPG_SoloRead708.NativeID624(m_Actor712),GPG_SoloRead708.NativeID624(m_Hull712));
 }
 bool Begin712(IEntity actor712,IEntity hull712)
 {
  if(m_State712!=0 || RplSession.Mode()!=RplMode.None || !actor712 || !hull712)return false;
  m_Actor712=actor712;
  m_Hull712=hull712;
  PlayerController player712=GetGame().GetPlayerController();
  CharacterControllerComponent life712=CharacterControllerComponent.Cast(actor712.FindComponent(CharacterControllerComponent));
  CompartmentAccessComponent access712=CompartmentAccessComponent.Cast(actor712.FindComponent(CompartmentAccessComponent));
  if(!player712 || player712.GetControlledEntity()!=actor712 || !life712 || life712.GetLifeState()!=ECharacterLifeState.ALIVE || !access712)
  {
   Fail712("wrong_player_or_dead_or_missing_access");
   return false;
  }
  if(access712.GetCompartment() || access712.IsGettingIn() || access712.IsGettingOut() || access712.IsSwitchingSeatsAnim())
  {
   Fail712("already_seated_or_transitioning");
   return false;
  }
  Physics body712=hull712.GetPhysics();
  SCR_DamageManagerComponent damage712=SCR_DamageManagerComponent.Cast(hull712.FindComponent(SCR_DamageManagerComponent));
  if(hull712.IsDeleted() || GetGame().GetWorld().FindEntityByID(hull712.GetID())!=hull712 || !body712 || body712.GetVelocity().Length()>0.25 || !damage712 || damage712.GetState()==EDamageState.DESTROYED)
  {
   Fail712("hull_not_fresh_identifiable_alive_and_at_rest");
   return false;
  }
  m_Seat712=Find712(hull712);
  if(!m_Seat712 || m_Seat712.GetOccupant() || m_Seat712.IsReserved())
  {
   Fail712("gunner_slot_missing_ambiguous_occupied_or_reserved");
   return false;
  }
  WCS_Armament_TurretStabilizationComponent stab712=WCS_Armament_TurretStabilizationComponent.Cast(m_Seat712.GetOwner().FindComponent(WCS_Armament_TurretStabilizationComponent));
  if(!stab712 || stab712.GetStabilizationEnabled() || stab712.GetMode()!=0)
  {
   Fail712("fresh_stabilizer_OFF_not_observed_no_servo_reset_performed");
   return false;
  }
  InspectEntry712(actor712,hull712);
  m_StartMs712=GetGame().GetWorld().GetWorldTime();
  // forceTeleport applies ONLY to the initial actor seating. It never moves
  // the vehicle, supplies ADS input or enables stabilization.
  bool accepted712=access712.GetInVehicle(m_Seat712.GetOwner(),m_Seat712,true,-1,ECloseDoorAfterActions.INVALID,false);
  PrintFormat("GPG_SETUP_GUNNER_REQUEST accepted=%1 actor=%2 hull=%3 seatOwner=%4 actorForceTeleportForScenarioSetup=1 exteriorBoardingAcceptance=NOT_TESTED servoWrites=0",accepted712,GPG_SoloRead708.NativeID624(actor712),GPG_SoloRead708.NativeID624(hull712),GPG_SoloRead708.NativeID624(m_Seat712.GetOwner()));
  if(!accepted712)
  {
   Fail712("public_engine_entry_rejected");
   return false;
  }
  m_State712=1;
  m_Reason712="waiting_exact_native_compartment";
  return true;
 }
 int Tick712()
 {
  if(m_State712!=1)return m_State712;
  float now712=GetGame().GetWorld().GetWorldTime();
  if(!m_Actor712 || !m_Hull712 || m_Actor712.IsDeleted() || m_Hull712.IsDeleted() || !m_Seat712)
  {
   Fail712("setup_identity_lost");
   return m_State712;
  }
  if(now712-m_StartMs712>15000)
  {
   Fail712("exact_compartment_setup_timeout15s");
   return m_State712;
  }
  CompartmentAccessComponent access712=CompartmentAccessComponent.Cast(m_Actor712.FindComponent(CompartmentAccessComponent));
  if(!access712 || (access712.GetCompartment() && access712.GetCompartment()!=m_Seat712))
  {
   Fail712("missing_access_or_different_compartment");
   return m_State712;
  }
  bool settled712=access712.GetCompartment()==m_Seat712 && m_Seat712.GetOccupant()==m_Actor712 && !access712.IsGettingIn() && !access712.IsGettingOut() && !access712.IsSwitchingSeatsAnim();
  if(!settled712)
  {
   m_SettledMs712=-1;
   return m_State712;
  }
  if(m_SettledMs712<0)m_SettledMs712=now712;
  if(now712-m_SettledMs712<500)return m_State712;
  m_State712=2;
  m_Reason712="exact_gunner_seat_settled_setup_only";
  PrintFormat("GPG_SETUP_GUNNER_READY actor=%1 hull=%2 seatOwner=%3 exactOccupant=1 settledSeconds=0.5 ADSandSpaceRequireOrdinaryInput=1 exteriorBoardingAcceptance=NOT_TESTED",GPG_SoloRead708.NativeID624(m_Actor712),GPG_SoloRead708.NativeID624(m_Hull712),GPG_SoloRead708.NativeID624(m_Seat712.GetOwner()));
  return m_State712;
 }
 string Reason712(){return m_Reason712;}
}

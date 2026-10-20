// Original station glue around installed native access/light/weapon APIs.
// Native compartment queues, light manager and weapon-group requests own state.
class GEP_GetInDriver579 : GEP_GetInHatch569
{
 override bool CanBePerformedScript(IEntity user)
 {
  if(!PilotCompartmentSlot.Cast(GetCompartmentSlot()))return false;
  return super.CanBePerformedScript(user);
 }
}

class GEP_StationLight579 : LightUserAction
{
 [Attribute("0",UIWidgets.EditBox,"0 actual driver, 1 actual gunner, 2 actual commander")]
 int Station;
 BaseLightManagerComponent StationManager()
 {
  return BaseLightManagerComponent.Cast(GetOwner().FindComponent(BaseLightManagerComponent));
 }
 bool Eligible(IEntity user)
 {
  if(!user || !StationManager())return false;
  CharacterControllerComponent life=CharacterControllerComponent.Cast(user.FindComponent(CharacterControllerComponent));
  CompartmentAccessComponent access=CompartmentAccessComponent.Cast(user.FindComponent(CompartmentAccessComponent));
  if(!life || life.GetLifeState()!=ECharacterLifeState.ALIVE || !access || access.IsGettingIn() || access.IsGettingOut() || access.IsSwitchingSeatsAnim())return false;
  BaseCompartmentSlot seat=access.GetCompartment();
  if(!seat || seat.GetOccupant()!=user || seat.GetOwner()!=GetOwner())return false;
  SCR_DamageManagerComponent damage=SCR_DamageManagerComponent.Cast(GetOwner().GetRootParent().FindComponent(SCR_DamageManagerComponent));
  if(damage && damage.GetState()==EDamageState.DESTROYED)return false;
  if(Station==0)return PilotCompartmentSlot.Cast(seat)!=null && seat.IsPiloting();
  if(Station==1)return TurretCompartmentSlot.Cast(seat)!=null;
  // This authored turret has exactly one cargo slot: Commander. No attached
  // turret controller is required to operate the commander's local lamp.
  if(Station==2)return CargoCompartmentSlot.Cast(seat)!=null;
  return false;
 }
 override bool CanBeShownScript(IEntity user){return Eligible(user);}
 override bool CanBePerformedScript(IEntity user){return Eligible(user);}
 override bool GetActionNameScript(out string outName)
 {
  string name;
  UIInfo info=GetUIInfo();name="Lights";if(info)name=info.GetName();
  BaseLightManagerComponent manager=StationManager();
  if(manager && manager.GetLightsState(m_eLightType,m_iLightSide))name+=" : off";
  else name+=" : on";
  outName=name;return true;
 }
 override void PerformAction(IEntity pOwnerEntity,IEntity pUserEntity)
 {
  IEntity owner=pOwnerEntity;IEntity user=pUserEntity;
  if(!Eligible(user))return;
  BaseLightManagerComponent manager=StationManager();
  bool state=manager.GetLightsState(m_eLightType,m_iLightSide);
  // Match installed native LightUserAction's client/server toggle contract.
  if(RplSession.Mode()!=RplMode.Client)state=!state;
  manager.SetLightsState(m_eLightType,state,m_iLightSide);
  PlaySound(owner,state);
 }
}

class GEP_FireModeManager579Class : SCR_FireModeManagerComponentClass {}
class GEP_FireModeManager579 : SCR_FireModeManagerComponent
{
 override protected void SetUpWeaponGroupActionListeners()
 {
  // GEP_GunInputContext579 owns the seat-scoped binding and calls the same
  // public native request used by SCR_VehicleWeaponGroupSelectAction.
 }
 // All three authored groups are fixed SALVO, quantity1. V click/hold and R
 // ripple listeners must not also react to the custom group chord.
 override protected void SetUpFireModeActionListeners(){}
 override protected void SetUpRippleQuantityActionListeners(){}
}
class GEP_GunInputContext579Class : ScriptComponentClass {}
class GEP_GunInputContext579 : ScriptComponent
{
 bool Bound;
 bool Eligible()
 {
  if(!GetGame().InPlayMode() || System.IsConsoleApp() || GetGame().GetMenuManager().IsAnyMenuOpen())return false;
  PlayerController player=GetGame().GetPlayerController();if(!player)return false;
  GEP_Optical444 optic=GEP_Optical444.Cast(GetOwner().FindComponent(GEP_Optical444));
  return optic && optic.Crew(player.GetControlledEntity());
 }
 void NextGroup(float value,EActionTrigger reason)
 {
  if(!Eligible())return;
  SCR_FireModeManagerComponent groups=SCR_FireModeManagerComponent.Cast(GetOwner().FindComponent(SCR_FireModeManagerComponent));
  if(groups)groups.NextWeaponsGroup();
 }
 void Bind(bool wanted)
 {
  if(Bound==wanted)return;
  InputManager input=GetGame().GetInputManager();if(!input)return;
  if(wanted)input.AddActionListener("GEPNextGunGroup579",EActionTrigger.DOWN,NextGroup);
  else input.RemoveActionListener("GEPNextGunGroup579",EActionTrigger.DOWN,NextGroup);
  Bound=wanted;
 }
 override void OnPostInit(IEntity owner){SetEventMask(owner,EntityEvent.FRAME);}
 override void EOnFrame(IEntity owner,float timeSlice)
 {
  bool wanted=Eligible();Bind(wanted);if(!wanted)return;
  // Only this actual local gunner activates the chord. No mouse/aim actions
  // appear in this context, and no contexts are forced outside this vehicle.
  GetGame().GetInputManager().ActivateContext("GEPGunControls579");
 }
 override void OnDelete(IEntity owner){Bind(false);}
}

// Original READ-ONLY sight presentation eligibility. NEVER use this helper to
// authorize weapons, range, tracking, stabilization, doors or compartment actions.
// Operational Evaluate679/Crew679 remain unchanged in their existing callers.
class GEP_Presentation680
{
 static bool ActualOccupant(IEntity owner,out IEntity user,out BaseCompartmentSlot seat,out TurretControllerComponent controller,out string reason)
 {
  user=null;seat=null;controller=null;
  reason="game";if(!GetGame() || !GetGame().InPlayMode() || System.IsConsoleApp())return false;
  reason="owner";if(!owner || owner.IsDeleted())return false;
  reason="player";PlayerController player=GetGame().GetPlayerController();if(!player)return false;
  reason="user";user=player.GetControlledEntity();if(!user || user.IsDeleted())return false;
  CharacterControllerComponent life=CharacterControllerComponent.Cast(user.FindComponent(CharacterControllerComponent));
  reason="life";if(!life || life.IsUnconscious() || life.GetLifeState()!=ECharacterLifeState.ALIVE)return false;
  reason="menu";if(GetGame().GetMenuManager().IsAnyMenuOpen())return false;
  CompartmentAccessComponent access=CompartmentAccessComponent.Cast(user.FindComponent(CompartmentAccessComponent));
  reason="access";if(!access || access.IsGettingIn() || access.IsGettingOut() || access.IsSwitchingSeatsAnim())return false;
  reason="seat";seat=access.GetCompartment();if(!seat || !TurretCompartmentSlot.Cast(seat))return false;
  reason="occupant";if(seat.GetOccupant()!=user)return false;
  reason="seatOwner";if(seat.GetOwner()!=owner)return false;
  controller=TurretControllerComponent.Cast(seat.GetController());
  if(!controller)controller=seat.GetAttachedTurret();
  TurretControllerComponent expected=TurretControllerComponent.Cast(owner.FindComponent(TurretControllerComponent));
  reason="controller";if(!controller || !expected || controller!=expected || controller.GetOwner()!=owner)return false;
  reason="occupant";return true;
 }

 static bool Evaluate(IEntity owner,out IEntity user,out BaseCompartmentSlot seat,out SCR_2DSightsComponent sight,out string reason)
 {
  sight=null;TurretControllerComponent controller;
  if(!ActualOccupant(owner,user,seat,controller,reason))return false;
  reason="sight";sight=SCR_2DSightsComponent.Cast(controller.GetCurrentSights());
  if(!sight || sight.GetOwner()!=owner)return false;
  reason="ADS";if(!controller.GetCurrentSightsADS() || !sight.IsSightADSActive())return false;
  PlayerController player=GetGame().GetPlayerController();
  CameraManager cameras=GetGame().GetCameraManager();
  reason="camera";if(!cameras || !player.GetPlayerCamera() || cameras.CurrentCamera()!=player.GetPlayerCamera())return false;
  reason="present";return true;
 }

 static bool MaskWeaponInfo(IEntity owner)
 {
  IEntity user;BaseCompartmentSlot seat;SCR_2DSightsComponent sight;string reason;
  if(!Evaluate(owner,user,seat,sight,reason))return false;
  GEP_GunStatus instruments=GEP_GunStatus.Cast(owner.FindComponent(GEP_GunStatus));
  // Suppress native ammunition information only after a complete replacement
  // was actually presented for this exact occupant, seat and sight.
  return instruments && instruments.ReplacesWeaponInfo702(user,seat,sight);
 }

 static bool ExactSight(SCR_2DOpticsComponent comp)
 {
  if(!comp || !comp.GetOwner() || comp.GetOwner().IsDeleted())return false;
  if(!comp.GetOwner().FindComponent(GEP_GunStatus))return false;
  IEntity user;BaseCompartmentSlot seat;SCR_2DSightsComponent current;string reason;
  return Evaluate(comp.GetOwner(),user,seat,current,reason) && current==comp;
 }

 // Read-only fault label. A visible camera image is not operational permission.
 static string DisabledLabel(IEntity owner,string operationalReason)
 {
  if(!owner || owner.IsDeleted())return "VISIER NICHT VERFUEGBAR";
  IEntity root=owner.GetRootParent();
  SCR_DamageManagerComponent hull;
  if(root && !root.IsDeleted())hull=SCR_DamageManagerComponent.Cast(root.FindComponent(SCR_DamageManagerComponent));
  if(hull && hull.GetState()==EDamageState.DESTROYED)return "WANNE ZERSTOERT";
  SCR_DamageManagerComponent turret=SCR_DamageManagerComponent.Cast(owner.FindComponent(SCR_DamageManagerComponent));
  if(turret && turret.GetState()==EDamageState.DESTROYED)return "TURM ZERSTOERT";
  return "VISIER NICHT VERFUEGBAR";
 }
}

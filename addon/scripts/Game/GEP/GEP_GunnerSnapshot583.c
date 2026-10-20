// Original read-only presentation data. No input, camera, weapon or simulation writes.
class GEP_GunnerFeed583
{
 int State; // 0 unavailable slot/muzzle, 1 no magazine, 2 actual loaded magazine
 int Count=-1;
 int Capacity=-1;
 string Ammunition="UNAVAILABLE";
 void Reset()
 {
  State=0;Count=-1;Capacity=-1;Ammunition="UNAVAILABLE";
 }
 void Read(WeaponSlotComponent slot)
 {
  Reset();if(!slot)return;
  BaseMuzzleComponent muzzle=slot.GetCurrentMuzzle();if(!muzzle)return;
  BaseMagazineComponent magazine=muzzle.GetMagazine();
  if(!magazine){State=1;Ammunition="NO MAGAZINE";return;}
  State=2;Count=muzzle.GetAmmoCount();Capacity=magazine.GetMaxAmmoCount();Ammunition="UNKNOWN";
  IEntity item=magazine.GetOwner();
  if(item && item.GetPrefabData() && item.GetPrefabData().GetPrefabName()=="{D561AD08932E4020}Prefabs/Weapons/GEP_35_HEIT_Belt320.et")Ammunition="HEI-T";
 }
 string Rounds()
 {
  if(State==0)return "-- / --";
  if(State==1)return "EMPTY";
  string loaded="--";string maximum="--";
  if(Count>=0)loaded=Count.ToString();
  if(Capacity>0)maximum=Capacity.ToString();
  return loaded+" / "+maximum;
 }
}

class GEP_GunnerSnapshot583
{
 ref GEP_GunnerFeed583 Left=new GEP_GunnerFeed583();
 ref GEP_GunnerFeed583 Right=new GEP_GunnerFeed583();
 bool Crew;
 bool Presentation680;
 string OperationalReason680;
 bool ADS;
 bool OpticsValid;
 bool Night;
 bool DriveValid;
 bool DriveFault;
 bool ZoomValid;
 bool ViewValid;
 bool AimValid;
 int Group=-1;
 float Magnification;
 float Bearing;
 float SightElevation;
 float HullAzimuth;
 float GunElevation;
 string Feedback="UNAVAILABLE";
 int RangeState676=EGEP_Range676State.UNAVAILABLE;
 float RangeMetres676=-1;
 float RangeAge676=-1;
 static bool IsLocalGunner(IEntity owner)
 {
  string reason679;return Evaluate679(owner,reason679);
 }
 // Same ordered eligibility predicates; a named result makes the first rejection
 // observable without widening the role or operational safety gate.
 static bool Evaluate679(IEntity owner,out string reason679)
 {
  reason679="game";if(!GetGame())return false;
  reason679="notInPlay";if(!GetGame().InPlayMode())return false;
  reason679="console";if(System.IsConsoleApp())return false;
  reason679="ownerMissing";if(!owner)return false;
  reason679="ownerDeleted";if(owner.IsDeleted())return false;
  reason679="playerMissing";PlayerController player=GetGame().GetPlayerController();if(!player)return false;
  reason679="userMissing";IEntity user=player.GetControlledEntity();if(!user)return false;
  CharacterControllerComponent life=CharacterControllerComponent.Cast(user.FindComponent(CharacterControllerComponent));
  reason679="lifeMissing";if(!life)return false;
  reason679="unconscious";if(life.IsUnconscious())return false;
  reason679="lifeNotAlive";if(life.GetLifeState()!=ECharacterLifeState.ALIVE)return false;
  CompartmentAccessComponent access=CompartmentAccessComponent.Cast(user.FindComponent(CompartmentAccessComponent));
  reason679="accessMissing";if(!access)return false;
  reason679="gettingIn";if(access.IsGettingIn())return false;
  reason679="gettingOut";if(access.IsGettingOut())return false;
  reason679="switchingSeat";if(access.IsSwitchingSeatsAnim())return false;
  reason679="seatMissing";BaseCompartmentSlot seat=access.GetCompartment();if(!seat)return false;
  reason679="notTurretSeat";if(!TurretCompartmentSlot.Cast(seat))return false;
  TurretControllerComponent control=TurretControllerComponent.Cast(seat.GetController());
  if(!control)control=seat.GetAttachedTurret();
  reason679="controllerMissing";if(!control)return false;
  reason679="controllerOwnerMismatch";if(control.GetOwner()!=owner)return false;
  GEP_Optical444 optical=GEP_Optical444.Cast(owner.FindComponent(GEP_Optical444));
  if(optical && !optical.Crew679(user,reason679))return false;
  SCR_DamageManagerComponent damage=SCR_DamageManagerComponent.Cast(owner.GetRootParent().FindComponent(SCR_DamageManagerComponent));
  reason679="rootDestroyed";if(damage && damage.GetState()==EDamageState.DESTROYED)return false;
  reason679="menu";if(GetGame().GetMenuManager().IsAnyMenuOpen())return false;
  reason679="ready";return true;
 }
 void Reset()
 {
  Left.Reset();Right.Reset();Crew=false;Presentation680=false;OperationalReason680="";ADS=false;OpticsValid=false;Night=false;DriveValid=false;DriveFault=false;
  RangeState676=EGEP_Range676State.UNAVAILABLE;RangeMetres676=-1;RangeAge676=-1;
  ZoomValid=false;ViewValid=false;AimValid=false;Group=-1;Magnification=0;Bearing=0;SightElevation=0;HullAzimuth=0;GunElevation=0;Feedback="UNAVAILABLE";
 }
 void Sample(IEntity owner)
 {
  Reset();
  IEntity viewUser680;BaseCompartmentSlot viewSeat680;SCR_2DSightsComponent viewSight680;string viewReason680;
  Presentation680=GEP_Presentation680.Evaluate(owner,viewUser680,viewSeat680,viewSight680,viewReason680);
  if(!Presentation680)return;
  Crew=Evaluate679(owner,OperationalReason680);
  TurretControllerComponent gun=TurretControllerComponent.Cast(owner.FindComponent(TurretControllerComponent));if(!gun)return;
  SCR_2DSightsComponent sight=SCR_2DSightsComponent.Cast(gun.GetCurrentSights());
  ADS=sight && gun.GetCurrentSightsADS() && sight.IsSightADSActive();
  GEP_Optical444 optical=GEP_Optical444.Cast(owner.FindComponent(GEP_Optical444));
  if(Crew && optical){OpticsValid=true;Night=optical.Night;Feedback=optical.Feedback;}
  GEP_FireControl fireControl=GEP_FireControl.Cast(owner.FindComponent(GEP_FireControl));
  if(Crew && fireControl){string status=fireControl.GetStatus();if(status!="")Feedback=status;}
  GEP_TurretController410 drive=GEP_TurretController410.Cast(gun);
  if(Crew && drive){DriveValid=true;DriveFault=drive.IsDriveFailed();}
  if(!Crew)Feedback=GEP_Presentation680.DisabledLabel(owner,OperationalReason680);
  SCR_FireModeManagerComponent groups=SCR_FireModeManagerComponent.Cast(owner.FindComponent(SCR_FireModeManagerComponent));
  if(groups)
  {
   string displayName;int index=groups.GetWeaponGroupID(displayName);
   // Exact authored group order: Both{0,1}, Left{0}, Right{1}. Display text is not an identifier.
   if(index>=0 && index<=2)Group=index;
  }
  BaseWeaponManagerComponent weapons=gun.GetWeaponManager();
  if(weapons)
  {
   array<WeaponSlotComponent> slots={};weapons.GetWeaponsSlots(slots);
   foreach(WeaponSlotComponent slot:slots)
   {
    if(!slot)continue;
    if(slot.GetWeaponSlotIndex()==0)Left.Read(slot);
    if(slot.GetWeaponSlotIndex()==1)Right.Read(slot);
   }
  }
  if(!ADS)return;
  GEP_Range676 range676=GEP_Range676.Cast(owner.FindComponent(GEP_Range676));
  if(Crew && range676)
  {
   RangeState676=range676.GetState676();
   vector measuredPoint676;
   if(!range676.Read676(measuredPoint676,RangeMetres676,RangeAge676) && RangeState676==EGEP_Range676State.VALID)RangeState676=EGEP_Range676State.UNAVAILABLE;
  }
  Magnification=sight.GetMagnification();ZoomValid=Magnification>0;
  vector camera[4];float fov;
  if(gun.GetCurrentSightsCameraTransform(camera,fov))
  {
   vector forward=camera[2].Normalized();
   if(forward.Length()>0.9)
   {
    ViewValid=true;Bearing=Math.Atan2(forward[0],forward[2])*Math.RAD2DEG;
    if(Bearing<0)Bearing+=360;
    if(Bearing>=359.95)Bearing=0;
    SightElevation=Math.Asin(Math.Clamp(forward[1],-1,1))*Math.RAD2DEG;
   }
  }
  AimingComponent turret=gun.GetTurretComponent();
  if(turret)
  {
   vector angles=turret.GetAimingRotation();AimValid=true;HullAzimuth=angles[0];GunElevation=angles[1];
   while(HullAzimuth>180)HullAzimuth-=360;
   while(HullAzimuth<-180)HullAzimuth+=360;
  }
 }
 string GroupName()
 {
  if(Group==0)return "BOTH";
  if(Group==1)return "LEFT";
  if(Group==2)return "RIGHT";
  return "UNAVAILABLE";
 }
}

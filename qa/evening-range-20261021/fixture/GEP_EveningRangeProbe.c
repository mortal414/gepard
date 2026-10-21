// PRIVATE QA HELPER. Not attached to a prefab, no frame callbacks or inputs.
// Invoke explicitly from an addon-owned isolated fixture after an observed action.
// Adapted from the project's original range676 passive table diagnostic.
class GEP_EveningRangeProbe
{
 static void Sample(string caseId, IEntity turret, bool queryTable = false)
 {
  if(!GetGame() || !GetGame().InPlayMode() || System.IsConsoleApp() || !turret || turret.IsDeleted())return;
  IEntity user;BaseCompartmentSlot seat;SCR_2DSightsComponent sight;string reason;
  bool presentation=GEP_Presentation680.Evaluate(turret,user,seat,sight,reason);
  string operationalReason;bool operational=GEP_GunnerSnapshot583.Evaluate679(turret,operationalReason);
  GEP_Range676 range=GEP_Range676.Cast(turret.FindComponent(GEP_Range676));
  int state=EGEP_Range676State.UNAVAILABLE;bool valid;vector point;float metres=-1;float age=-1;
  if(range){state=range.GetState676();valid=range.Read676(point,metres,age);}
  PrintFormat("GEP_EVENING_RANGE case=%1 presentation=%2 reason=%3 operational=%4 operationalReason=%5",caseId,presentation,reason,operational,operationalReason);
  PrintFormat("GEP_EVENING_SAMPLE case=%1 state=%2 valid=%3 metres=%4 age=%5 point=%6",caseId,state,valid,metres,age,point);
  PlayerController player=GetGame().GetPlayerController();CameraManager cameras=GetGame().GetCameraManager();
  if(player && cameras && player.GetPlayerCamera() && cameras.CurrentCamera()==player.GetPlayerCamera())
  {
   vector camera[4];cameras.CurrentCamera().GetWorldCameraTransform(camera);
   PrintFormat("GEP_EVENING_CAMERA case=%1 origin=%2 forward=%3",caseId,camera[3],camera[2]);
  }
  Animation animation=turret.GetAnimation();vector localPose[4];
  if(animation && animation.GetBoneMatrix(animation.GetBoneIndex("GEP_GunElevation"),localPose))
  {
   float pitch=GEP_FireControlGeometry.PitchDegrees(localPose[2]);
   PrintFormat("GEP_EVENING_BARREL case=%1 localBoneForward=%2 physicalPitchDegrees=%3",caseId,localPose[2],pitch);
  }
  // A table query is diagnostic only, never a substitute for impacts or actuation.
  if(!queryTable || !presentation || !operational || !valid)return;
  TurretControllerComponent controller=TurretControllerComponent.Cast(turret.FindComponent(TurretControllerComponent));
  BaseWeaponManagerComponent manager;if(controller)manager=controller.GetWeaponManager();
  BaseWeaponComponent weapon;if(manager)weapon=manager.GetCurrentWeapon();
  BaseMuzzleComponent muzzle;BaseMagazineComponent magazine;
  if(weapon){muzzle=weapon.GetCurrentMuzzle();if(muzzle)magazine=muzzle.GetMagazine();}
  if(!muzzle || !magazine || muzzle.GetAmmoCount()<=0)
  {PrintFormat("GEP_EVENING_TABLE case=%1 result=NO_LOADED_NEXT_PROJECTILE aimSolution=0",caseId);return;}
  vector muzzlePose[4];if(!manager.GetCurrentMuzzleTransform(muzzlePose))
  {PrintFormat("GEP_EVENING_TABLE case=%1 result=NO_MUZZLE_TRANSFORM aimSolution=0",caseId);return;}
  float muzzleMetres=vector.Distance(muzzlePose[3],point);
  if(!(muzzleMetres>0 && muzzleMetres<5000))
  {PrintFormat("GEP_EVENING_TABLE case=%1 result=RANGE_REJECTED aimSolution=0",caseId);return;}
  WeaponSlotComponent slot=manager.GetCurrentSlot();int slotIndex=-1;if(slot)slotIndex=slot.GetWeaponSlotIndex();
  float seconds;float height=BallisticTable.GetAimHeightOfNextProjectile(muzzleMetres,seconds,muzzle,true);
  string result="INVALID_TABLE_RESULT";
  if(seconds<0)result="OUT_OF_RANGE";
  else if(seconds>0 && seconds<1000000 && height>=0 && height<10000000)result="TABLE_SAMPLE_ONLY";
  PrintFormat("GEP_EVENING_TABLE case=%1 result=%2 selectedSlot=%3 ammo=%4 muzzleMetres=%5 seconds=%6 height=%7 aimSolution=0",caseId,result,slotIndex,magazine.GetAmmoType(),muzzleMetres,seconds,height);
 }
}

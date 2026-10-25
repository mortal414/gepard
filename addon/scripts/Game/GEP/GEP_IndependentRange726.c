// Original PRIVATE consumer integration. Calls exposed dependency contracts only.
// GEP676 remains the sole local C listener and measured-range/HUD owner.
// WCS owns all assistance actuation; the disabled GEP prototype stays disabled.
class GEP_NextAimingBinding726
{
 IEntity User726;
 BaseCompartmentSlot Seat726;
 BaseSightsComponent Sight726;
 BaseWeaponComponent Weapon726;
 BaseMuzzleComponent Muzzle726;
 BaseMagazineComponent Magazine726;
 WeaponSlotComponent Slot726;
 RplComponent Rpl726;
 IEntity Hull726;
 int Group726=-1;
 ref array<WeaponSlotComponent> Feeds726={};
 ref array<BaseMuzzleComponent> FeedMuzzles726={};
 ref array<BaseMagazineComponent> FeedMagazines726={};
 ref array<ResourceName> FeedAmmo726={};
 ref array<float> FeedSpeed726={};
 bool CaptureFeeds726(IEntity owner)
 {
  SCR_FireModeManagerComponent groups726=SCR_FireModeManagerComponent.Cast(owner.FindComponent(SCR_FireModeManagerComponent));
  if(!groups726)return false;
  string name726;Group726=groups726.GetWeaponGroupID(name726);
  if(Group726<0 || Group726>2)return false;
  GEP_TurretController410 control726=GEP_TurretController410.Cast(owner.FindComponent(TurretControllerComponent));
  array<WeaponSlotComponent> slots726={};control726.GetWeaponManager().GetWeaponsSlots(slots726);
  foreach(WeaponSlotComponent feed726:slots726)
  {
   int index726=feed726.GetWeaponSlotIndex();
   if((Group726==1 && index726!=0) || (Group726==2 && index726!=1) || index726<0 || index726>1)continue;
   IEntity weaponEntity726=feed726.GetWeaponEntity();
   if(!weaponEntity726)return false;
   BaseWeaponComponent weapon726=BaseWeaponComponent.Cast(weaponEntity726.FindComponent(BaseWeaponComponent));
   if(!weapon726 || !weapon726.GetCurrentMuzzle() || !weapon726.GetCurrentMagazine())return false;
   BaseMuzzleComponent muzzle726=weapon726.GetCurrentMuzzle();
   BaseMagazineComponent magazine726=weapon726.GetCurrentMagazine();
   if(muzzle726.GetAmmoCount()<=0 || magazine726.GetAmmoType().IsEmpty())return false;
   Feeds726.Insert(feed726);FeedMuzzles726.Insert(muzzle726);FeedMagazines726.Insert(magazine726);
   FeedAmmo726.Insert(magazine726.GetAmmoType());FeedSpeed726.Insert(muzzle726.GetBulletInitSpeedCoef());
  }
  int expected726=1;if(Group726==0)expected726=2;
  if(Feeds726.Count()!=expected726)return false;
  // Both guns share one elevation axis. Current HEIT feeds have identical native
  // projectile/table and speed coefficient; reject incompatible mixed feeds.
  for(int i726=0;i726<Feeds726.Count();i726++)
   if(FeedAmmo726[i726]!=Magazine726.GetAmmoType() || Math.AbsFloat(FeedSpeed726[i726]-Muzzle726.GetBulletInitSpeedCoef())>0.00001)return false;
  return true;
 }
 bool MatchFeeds726(IEntity owner)
 {
  SCR_FireModeManagerComponent groups726=SCR_FireModeManagerComponent.Cast(owner.FindComponent(SCR_FireModeManagerComponent));
  string name726;if(!groups726 || groups726.GetWeaponGroupID(name726)!=Group726)return false;
  for(int i726=0;i726<Feeds726.Count();i726++)
  {
   IEntity entity726=Feeds726[i726].GetWeaponEntity();if(!entity726)return false;
   BaseWeaponComponent weapon726=BaseWeaponComponent.Cast(entity726.FindComponent(BaseWeaponComponent));
   if(!weapon726 || weapon726.GetCurrentMuzzle()!=FeedMuzzles726[i726] || weapon726.GetCurrentMagazine()!=FeedMagazines726[i726])return false;
   if(FeedMuzzles726[i726].GetAmmoCount()<=0 || FeedMagazines726[i726].GetAmmoType()!=FeedAmmo726[i726] || Math.AbsFloat(FeedMuzzles726[i726].GetBulletInitSpeedCoef()-FeedSpeed726[i726])>0.00001)return false;
  }
  return true;
 }
 void TraceFeeds726(float range726)
 {
  for(int i726=0;i726<Feeds726.Count();i726++)
  {
   float flight726;float height726=BallisticTable.GetAimHeightOfNextProjectile(range726,flight726,FeedMuzzles726[i726]);
   PrintFormat("GEP_RANGE726_FEED group=%1 slot=%2 ammo=%3 speedCoef=%4 range=%5 nativeHeight=%6 flight=%7",Group726,Feeds726[i726].GetWeaponSlotIndex(),FeedAmmo726[i726],FeedSpeed726[i726],range726,height726,flight726);
  }
 }

 static bool Read726(IEntity owner, out IEntity user, out BaseCompartmentSlot seat, out BaseSightsComponent sight)
 {
  user=null;
  seat=null;
  sight=null;
  string reason726;
  if(!GEP_GunnerSnapshot583.Evaluate679(owner,reason726))
  {
   return false;
  }
  GEP_TurretController410 control726=GEP_TurretController410.Cast(owner.FindComponent(TurretControllerComponent));
  if(!control726 || control726.GetOwner()!=owner || control726.IsDriveFailed())
  {
   return false;
  }
  PlayerController player726=GetGame().GetPlayerController();
  user=player726.GetControlledEntity();
  CompartmentAccessComponent access726=CompartmentAccessComponent.Cast(user.FindComponent(CompartmentAccessComponent));
  if(!access726)
  {
   return false;
  }
  seat=access726.GetCompartment();
  sight=control726.GetCurrentSights();
  SCR_2DSightsComponent optic726=SCR_2DSightsComponent.Cast(sight);
  if(!seat || seat.GetOccupant()!=user || seat.GetOwner()!=owner || !sight || sight.GetOwner()!=owner || !optic726 || !control726.GetCurrentSightsADS() || !optic726.IsSightADSActive())
  {
   return false;
  }
  CameraManager cameras726=GetGame().GetCameraManager();
  if(!cameras726 || !player726.GetPlayerCamera() || cameras726.CurrentCamera()!=player726.GetPlayerCamera())
  {
   return false;
  }
  GEP_Optical444 optical726=GEP_Optical444.Cast(owner.FindComponent(GEP_Optical444));
  RplComponent rpl726=RplComponent.Cast(owner.FindComponent(RplComponent));
  if(!optical726 || optical726.State!=0 || !rpl726 || !rpl726.IsOwner() || rpl726.IsProxy() || rpl726.Role()!=RplRole.Authority)
  {
   return false;
  }
  return true;
 }
 bool Capture726(IEntity owner)
 {
  if(!Read726(owner,User726,Seat726,Sight726))
  {
   return false;
  }
  GEP_TurretController410 control726=GEP_TurretController410.Cast(owner.FindComponent(TurretControllerComponent));
  BaseWeaponManagerComponent manager726=control726.GetWeaponManager();
  if(!manager726)
  {
   return false;
  }
  Weapon726=manager726.GetCurrentWeapon();
  Slot726=manager726.GetCurrentSlot();
  if(!Weapon726 || !Slot726)
  {
   return false;
  }
  Muzzle726=Weapon726.GetCurrentMuzzle();
  if(!Muzzle726 || Muzzle726.GetAmmoCount()<=0)
  {
   return false;
  }
  Magazine726=Muzzle726.GetMagazine();
  Rpl726=RplComponent.Cast(owner.FindComponent(RplComponent));
  Hull726=owner.GetRootParent();
  return Magazine726 && Hull726 && Hull726!=owner && CaptureFeeds726(owner);
 }
 bool Matches726(IEntity owner)
 {
  IEntity user726;
  BaseCompartmentSlot seat726;
  BaseSightsComponent sight726;
  if(!Read726(owner,user726,seat726,sight726) || user726!=User726 || seat726!=Seat726 || sight726!=Sight726 || owner.GetRootParent()!=Hull726 || owner.FindComponent(RplComponent)!=Rpl726)
  {
   return false;
  }
  GEP_TurretController410 control726=GEP_TurretController410.Cast(owner.FindComponent(TurretControllerComponent));
  BaseWeaponManagerComponent manager726=control726.GetWeaponManager();
  return manager726 && manager726.GetCurrentWeapon()==Weapon726 && manager726.GetCurrentSlot()==Slot726 && Weapon726.GetCurrentMuzzle()==Muzzle726 && Muzzle726.GetMagazine()==Magazine726 && Muzzle726.GetAmmoCount()>0 && MatchFeeds726(owner);
 }
}

class GEP_NextFireControlConsumer726Class : WCS_Armament_FireControlComponentClass {}
class GEP_NextFireControlConsumer726 : WCS_Armament_FireControlComponent
{
 protected ref GEP_NextAimingBinding726 m_Binding726;
 protected bool m_Revoked726=true,m_Cleanup726,m_Dispatching726,m_Pending726,m_ServerConsumed726,m_FreshValid726;
 protected int m_Generation726,m_PreviousRevision726,m_Nonclear726,m_Clears726,m_ServerNonclear726,m_Rejected726;
 protected vector m_GEPPoint726;
 protected float m_GEPDistance726=-1,m_RequestWorldMs726=-1;

 bool Live726()
 {
  return !m_Revoked726 && m_Binding726 && m_Binding726.Matches726(GetOwner());
 }
 // Read the real inherited fields and native state, never presentation substitutes.
 void TraceCancelState726(string phase726)
 {
  IEntity owner726=GetOwner();
  if(!owner726 || owner726.IsDeleted())
  {
   return;
  }
  GEP_TurretController410 control726=GEP_TurretController410.Cast(owner726.FindComponent(TurretControllerComponent));
  if(!control726)
  {
   return;
  }
  AimingComponent aim726=control726.GetTurretComponent();
  if(!aim726)
  {
   return;
  }
  PrintFormat("GEP_ATOMIC_CANCEL_STATE phase=%1 generation=%2 targetCorrectionRAW=%3 appliedCorrectionRAW=%4 adjustmentVelocityRAW=%5 IsAdjusting=%6 rangeResult=%7 revision=%8",phase726,m_Generation726,m_vCorrection,m_vAppliedCorrection,m_vAdjustmentVelocity,IsAdjusting(),GetRangeResult(),GetMeasurementRevision());
  PrintFormat("GEP_ATOMIC_CANCEL_NATIVE phase=%1 currentDegrees=%2 wantedDegrees=%3 rawModelRadians=%4 worldDirection=%5",phase726,aim726.GetAimingRotation(),aim726.GetAimingRotationWanted(),aim726.GetRawAimingRotation(),aim726.GetAimingDirectionWorld());
  PrintFormat("GEP_ATOMIC_CANCEL_SIGHT phase=%1 gunElevation=%2 gunTraverse=%3 cursorElevation=%4 cursorTraverse=%5 appliedElevation=%6 appliedTraverse=%7",phase726,GetGunElevation(),GetGunTraverse(),GetCursorElevation(),GetCursorTraverse(),GetElevation(),GetTraverse());
  PlayerController player726=GetGame().GetPlayerController();
  if(player726 && player726.GetPlayerCamera())
  {
   vector camera726[4];
   player726.GetPlayerCamera().GetWorldCameraTransform(camera726);
   PrintFormat("GEP_ATOMIC_CANCEL_CAMERA phase=%1 row0=%2 row1=%3 row2=%4 row3=%5",phase726,camera726[0],camera726[1],camera726[2],camera726[3]);
  }
 }
 void Invalidate726(string reason726)
 {
  m_Revoked726=true;
  m_Pending726=false;
  m_FreshValid726=false;
  if(m_Cleanup726)
  {
   return;
  }
  m_Cleanup726=true;
  RplComponent rpl726=RplComponent.Cast(GetOwner().FindComponent(RplComponent));
  if(rpl726 && rpl726.Role()==RplRole.Authority)
  {
   // This is normal solution clearing, NOT a claimed immediate cancel/flush.
   // Its inherited smoothing state remains visible in real public readbacks.
   TraceCancelState726("before_normal_clear");
   ClearSolution(WCS_EArmamentRangeResult.NONE);
   // ORIGINAL version-pinned teardown of two protected local bookkeeping fields.
   // Normal ClearSolution owns replicated solution/revision/monitor teardown.
   // No native aiming setter, inverse correction or getter override occurs here.
   m_vAppliedCorrection=vector.Zero;
   m_vAdjustmentVelocity=vector.Zero;
   TraceCancelState726("after_original_local_teardown");
  }
  PrintFormat("GEP_CONSUMER_FC_REVOKE generation=%1 reason=%2 result=%3 revision=%4 adjusting=%5 appliedElevation=%6 appliedTraverse=%7",m_Generation726,reason726,GetRangeResult(),GetMeasurementRevision(),IsAdjusting(),GetElevation(),GetTraverse());
 }
 void Guard726()
 {
  if(m_Revoked726)
  {
   return;
  }
  if(!Live726())
  {
   Invalidate726("scope_or_weapon_changed");
   return;
  }
  if(m_Pending726 && GetMeasurementRevision()!=m_PreviousRevision726)
  {
   m_Pending726=false;
   m_FreshValid726=GetRangeResult()==WCS_EArmamentRangeResult.VALID && GetRange()>0 && GetRange()<5000;
   PrintFormat("GEP_CONSUMER_FC_ACK generation=%1 oldRevision=%2 revision=%3 result=%4 nativeRange=%5 GEPRange=%6 GEPPoint=%7 freshValid=%8",m_Generation726,m_PreviousRevision726,GetMeasurementRevision(),GetRangeResult(),GetRange(),m_GEPDistance726,m_GEPPoint726,m_FreshValid726);
  }
  if(m_Pending726 && GetGame().GetWorld().GetWorldTime()-m_RequestWorldMs726>5000)
  {
   Invalidate726("ack_timeout");
  }
 }
 bool AcceptedC726(vector sampledWorldPoint)
 {
  // Called only by OUR existing accepted GEP676 success seam below, not an input listener.
  if(m_Pending726)
  {
   m_Rejected726++;
   return false;
  }
  GEP_Range676 range726=GEP_Range676.Cast(GetOwner().FindComponent(GEP_Range676));
  vector actualPoint726;
  float actualDistance726,age726;
  ref GEP_NextAimingBinding726 fresh726=new GEP_NextAimingBinding726();
  if(!range726 || !range726.Read676(actualPoint726,actualDistance726,age726) || !(age726>=0 && age726<=0.05) || vector.Distance(actualPoint726,sampledWorldPoint)>0.001 || !fresh726.Capture726(GetOwner()))
  {
   m_Rejected726++;
   return false;
  }
  // Capture checked the actual selected left/right/both feed identities. The
  // dependency solves through the current native muzzle; both matched HEIT
  // feeds use the same shared axis and engine table, without changing selection.
  fresh726.TraceFeeds726(actualDistance726);
  m_Binding726=fresh726;
  m_GEPPoint726=actualPoint726;
  m_GEPDistance726=actualDistance726;
  m_Generation726++;
  m_PreviousRevision726=GetMeasurementRevision();
  m_RequestWorldMs726=GetGame().GetWorld().GetWorldTime();
  m_Revoked726=false;
  m_Cleanup726=false;
  m_ServerConsumed726=false;
  m_Pending726=true;
  m_FreshValid726=false;
  m_Dispatching726=true;
  RequestMeasurement(false);
  m_Dispatching726=false;
  return true;
 }
 override void RequestMeasurement(bool clear=false)
 {
  if(clear)
  {
   m_Clears726++;
   Invalidate726("ordinary_clear_or_sight_lifecycle");
   return;
  }
  if(!m_Dispatching726 || !m_Pending726 || !Live726())
  {
   m_Rejected726++;
   return;
  }
  m_Nonclear726++;
  PrintFormat("GEP_CONSUMER_FC_REQUEST generation=%1 nonclear=%2 beforeRevision=%3 GEPRange=%4 GEPPoint=%5 normalPublicRoute=1",m_Generation726,m_Nonclear726,m_PreviousRevision726,m_GEPDistance726,m_GEPPoint726);
  super.RequestMeasurement(clear);
 }
 override void MeasureFromController_S(IEntity controlled,bool clear)
 {
  if(clear)
  {
   Invalidate726("server_clear");
   return;
  }
  if(!Live726() || !m_Pending726 || m_ServerConsumed726 || controlled!=m_Binding726.User726)
  {
   m_Rejected726++;
   return;
  }
  m_ServerConsumed726=true;
  m_ServerNonclear726++;
  super.MeasureFromController_S(controlled,clear);
 }
 override protected void UpdateAdjustment(float timeSlice)
 {
  Guard726();
  if(!Live726() || m_Pending726 || !m_FreshValid726)
  {
   return;
  }
  super.UpdateAdjustment(timeSlice);
 }
 override void EOnFrame(IEntity owner,float timeSlice)
 {
  Guard726();
  if(!Live726() || m_Pending726 || !m_FreshValid726)
  {
   return;
  }
  super.EOnFrame(owner,timeSlice);
 }
 override void EOnFixedFrame(IEntity owner,float timeSlice)
 {
  Guard726();
  if(!Live726() || m_Pending726 || !m_FreshValid726)
  {
   return;
  }
  super.EOnFixedFrame(owner,timeSlice);
 }
 int Generation726(){return m_Generation726;}
 bool Pending726(){return m_Pending726;}
 bool FreshValid726(){return m_FreshValid726 && Live726();}
 int Nonclear726(){return m_Nonclear726;}
 int Clears726(){return m_Clears726;}
 int ServerNonclear726(){return m_ServerNonclear726;}
 int Rejected726(){return m_Rejected726;}
 vector GEPPoint726(){return m_GEPPoint726;}
 float GEPDistance726(){return m_GEPDistance726;}
}

// Existing GEP676 calls these original public methods exactly once per accepted C.
// Original prototype source/defaults remain unchanged and disabled.
modded class GEP_FireControl
{
 override void RequestCorrection(vector sampledWorldPoint)
 {
  GEP_NextFireControlConsumer726 consumer726=GEP_NextFireControlConsumer726.Cast(GetOwner().FindComponent(GEP_NextFireControlConsumer726));
  if(!consumer726)
  {
   super.RequestCorrection(sampledWorldPoint);
   return;
  }
  consumer726.AcceptedC726(sampledWorldPoint);
 }
 override void CancelCorrection(string status)
 {
  GEP_NextFireControlConsumer726 consumer726=GEP_NextFireControlConsumer726.Cast(GetOwner().FindComponent(GEP_NextFireControlConsumer726));
  if(consumer726)
  {
   consumer726.Invalidate726("accepted_range_cancel");
  }
  super.CancelCorrection(status);
 }
}


// Separate range cleanup; it never switches stabilization mode or clears target.
class GEP_RangeGuard726Class : ScriptComponentClass {}
class GEP_RangeGuard726 : ScriptComponent
{
 override void OnPostInit(IEntity owner){SetEventMask(owner,EntityEvent.FRAME);}
 override void EOnFrame(IEntity owner,float timeSlice)
 {
  GEP_NextFireControlConsumer726 range726=GEP_NextFireControlConsumer726.Cast(owner.FindComponent(GEP_NextFireControlConsumer726));
  if(range726)range726.Guard726();
 }
}

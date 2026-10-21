// Original stock-only GEP-015 prototype. No third-party classes or code.
// Native smoke/fuse/flight/replication qualification remains a release gate.
class GEP_StockSmoke015Class : ScriptComponentClass {}

class GEP_StockSmoke015 : ScriptComponent
{
 [Attribute("0")] protected bool m_bEnableNativeSimulation015;
 bool NativeSimulationRequested015() { return m_bEnableNativeSimulation015; }
 // Empty in runtime source; QA overlay records lifecycle without changing it.
 protected void TraceLifecycle015(string phase, IEntity round, int tube) {}
 // QA one-round deferred-launch transaction:0 unused,1 reserved,2 permanently consumed.
 protected int m_iDeferredStage015;
 protected IEntity m_DeferredRound015;
 protected IEntity m_DeferredUser015;
 protected IEntity m_DeferredRoot015;
 protected vector m_DeferredPose015[4];
 protected vector m_DeferredParentVelocity015;
 protected int m_iDeferredTube015=-1;
 protected float m_fDeferredQueuedMs015;
 protected bool m_bDeferredNativeIssued015;
 static const ResourceName ROUND = "{9DB69176CEF0EE97}Prefabs/Weapons/Grenades/Smoke_ANM8HC.et";
 [RplProp(onRplName: "OnAmmoChanged")]
 protected int m_iLoadedMask = 255;
 [RplProp(onRplName: "OnAmmoChanged")]
 protected bool m_bReloadPending;
 protected int m_iReloadSerial;
 protected int m_iReservedTube = -1;
 protected ref GEP_StockSmokeDelete015 m_DeleteCallback;
 protected RplComponent m_Rpl;
 protected bool m_bHasDispatched;
 protected WorldTimestamp m_LastDispatch;

 override void OnPostInit(IEntity owner)
 {
  m_Rpl = RplComponent.Cast(owner.FindComponent(RplComponent));
  SetEventMask(owner, EntityEvent.INIT);
 }
 override void EOnInit(IEntity owner) { OnAmmoChanged(); }
 bool Authority() { return m_Rpl && !m_Rpl.IsProxy(); }
 bool Operational()
 {
  IEntity root = GetOwner().GetRootParent();
  SCR_DamageManagerComponent hull = SCR_DamageManagerComponent.Cast(root.FindComponent(SCR_DamageManagerComponent));
  SCR_DamageManagerComponent turret = SCR_DamageManagerComponent.Cast(GetOwner().FindComponent(SCR_DamageManagerComponent));
  return hull && turret && hull.GetState() != EDamageState.DESTROYED && turret.GetState() != EDamageState.DESTROYED;
 }
 int Count()
 {
  int count;
  for (int i = 0; i < 8; i++) if ((m_iLoadedMask & (1 << i)) != 0) count++;
  return count;
 }
 bool Crew(IEntity user)
 {
  if (!user) return false;
  CharacterControllerComponent life = CharacterControllerComponent.Cast(user.FindComponent(CharacterControllerComponent));
  CompartmentAccessComponent access = CompartmentAccessComponent.Cast(user.FindComponent(CompartmentAccessComponent));
  if (!life || life.GetLifeState() != ECharacterLifeState.ALIVE || !access || access.IsGettingIn() || access.IsGettingOut() || access.IsSwitchingSeatsAnim()) return false;
  BaseCompartmentSlot seat = access.GetCompartment();
  if (!seat || seat.GetOccupant() != user || seat.GetOwner() != GetOwner()) return false;
  return TurretCompartmentSlot.Cast(seat) != null || CargoCompartmentSlot.Cast(seat) != null;
 }
 bool CanFire(IEntity user)
 {
  if (m_iDeferredStage015 != 0 || m_bEnableNativeSimulation015 || !Operational() || !Crew(user) || m_bReloadPending || Count() == 0) return false;
  return !m_bHasDispatched || GetOwner().GetWorld().GetTimestamp().DiffSeconds(m_LastDispatch) >= 2.5;
 }
 // Proposed muzzle frames only: native clearance and direction must be measured.
 bool Muzzle(int index, out vector worldPose[4])
 {
  if (index < 0 || index >= 8) return false;
  int ordinal = index % 4;
  float sign = -1;
  if (index >= 4) sign = 1;
  vector localPose[4];
  Math3D.AnglesToMatrix(Vector(sign * (10 + 10 * ordinal), -30, 0), localPose);
  localPose[3] = Vector(sign * 1.12, 0.35, -0.1 - ordinal * 0.25);
  PointInfo point = new PointInfo();
  point.Set(GetOwner(), "GEP_TurretYaw", localPose);
  if (point.GetNodeId() < 0) return false;
  point.GetWorldTransform(worldPose);
  return true;
 }
 // No client RPC is exposed. Native user-action routing supplies the acting user.
 // Return value is a reserved diagnostic round, not proof native launch occurred.
 int Dispatch(IEntity user, bool singleRound)
 {
  if (!singleRound) { Print("GEP015_DEFERRED_REJECT salvoDisabled=1 scope=one_round_QA"); return 0; }
  if (!Authority() || !CanFire(user)) return 0;
  Resource resource=Resource.Load(ROUND);if (!resource || !resource.IsValid()) return 0;
  int tube=-1;for(int i=0;i<8;i++)if((m_iLoadedMask & (1 << i))!=0){tube=i;break;}
  if(tube<0)return 0;
  vector pose[4];if(!Muzzle(tube,pose))return 0;
  IEntity root=GetOwner().GetRootParent();if(!root || root.IsDeleted())return 0;
  vector velocity;Physics physics=root.GetPhysics();if(physics)velocity=physics.GetVelocity();
  EntitySpawnParams params();params.TransformMode=ETransformMode.WORLD;Math3D.MatrixCopy(pose,params.Transform);
  IEntity round=GetGame().SpawnEntityPrefab(resource,GetOwner().GetWorld(),params);
  if(!round){TraceLifecycle015("spawn_failed",round,tube);return 0;}
  TraceLifecycle015("post_spawn",round,tube);
  GrenadeMoveComponent move=GrenadeMoveComponent.Cast(round.FindComponent(GrenadeMoveComponent));
  TimerTriggerComponent fuse=TimerTriggerComponent.Cast(round.FindComponent(TimerTriggerComponent));RplComponent rpl=RplComponent.Cast(round.FindComponent(RplComponent));
  if(!move || !fuse || !rpl || rpl.IsProxy()){RplComponent.DeleteRplEntity(round,false);return 0;}
  TraceLifecycle015("components_validated",round,tube);
  m_iDeferredStage015=1;m_DeferredRound015=round;m_DeferredUser015=user;m_DeferredRoot015=root;m_iDeferredTube015=tube;
  Math3D.MatrixCopy(pose,m_DeferredPose015);m_DeferredParentVelocity015=velocity;m_fDeferredQueuedMs015=GetOwner().GetWorld().GetWorldTime();
  m_iLoadedMask=m_iLoadedMask & ~(1 << tube);Replication.BumpMe();
  m_bHasDispatched=true;m_LastDispatch=GetOwner().GetWorld().GetTimestamp();
  TraceLifecycle015("deferred_reserved",round,tube);
  PrintFormat("GEP015_DEFERRED_RESERVED tube=%1 round=%2 queuedWorldMs=%3 capturedOrigin=%4 remaining=%5 nativeCallsIssued=0 oneShot=1",tube,round.GetID(),m_fDeferredQueuedMs015,m_DeferredPose015[3],Count());
  SetEventMask(GetOwner(),EntityEvent.FRAME);OnAmmoChanged();return 1;
 }
 protected void AbortDeferred015(string reason)
 {
  TraceLifecycle015("deferred_aborted_"+reason,m_DeferredRound015,m_iDeferredTube015);
  PrintFormat("GEP015_DEFERRED_ABORT reason=%1 nativeCallsIssued=%2 reservedTube=%3 remaining=%4 refund=0 retry=0",reason,m_bDeferredNativeIssued015,m_iDeferredTube015,Count());
  // No refund: reservation is deliberately spent for this one-shot diagnostic.
  // Delete request is issued only before our native launch/arming, never treated as an acknowledgement.
  if(!m_bDeferredNativeIssued015 && Authority() && m_DeferredRound015 && !m_DeferredRound015.IsDeleted())RplComponent.DeleteRplEntity(m_DeferredRound015,false);
  m_DeferredRound015=null;m_DeferredUser015=null;m_DeferredRoot015=null;
 }
 override void EOnFrame(IEntity owner,float timeSlice)
 {
  if(m_iDeferredStage015!=1)return;
  if(!owner || owner.IsDeleted()){m_iDeferredStage015=2;Print("GEP015_DEFERRED_ABORT reason=owner_missing_before_clock nativeCallsIssued=0 retry=0");return;}
  float now=owner.GetWorld().GetWorldTime();
  // A dynamic subscription may be visited in the current frame; reject that timestamp.
  if(now<=m_fDeferredQueuedMs015)return;
  m_iDeferredStage015=2;ClearEventMask(owner,EntityEvent.FRAME);
  TraceLifecycle015("deferred_next_frame",m_DeferredRound015,m_iDeferredTube015);
  if(!Authority() || !owner || owner.IsDeleted() || !m_DeferredRound015 || m_DeferredRound015.IsDeleted() || !m_DeferredUser015 || m_DeferredUser015.IsDeleted() || !m_DeferredRoot015 || m_DeferredRoot015.IsDeleted()) {AbortDeferred015("identity_or_authority");return;}
  if(owner.GetRootParent()!=m_DeferredRoot015 || !Operational() || !Crew(m_DeferredUser015) || m_bReloadPending){AbortDeferred015("crew_root_or_operational");return;}
  SCR_CharacterControllerComponent life=SCR_CharacterControllerComponent.Cast(m_DeferredUser015.FindComponent(CharacterControllerComponent));
  if(!life || life.GetLifeState()!=ECharacterLifeState.ALIVE || life.IsUnconscious()){AbortDeferred015("user_life");return;}
  if(!m_DeferredRound015.GetPrefabData() || m_DeferredRound015.GetPrefabData().GetPrefabName()!=ROUND){AbortDeferred015("prefab_identity");return;}
  GrenadeMoveComponent move=GrenadeMoveComponent.Cast(m_DeferredRound015.FindComponent(GrenadeMoveComponent));
  TimerTriggerComponent fuse=TimerTriggerComponent.Cast(m_DeferredRound015.FindComponent(TimerTriggerComponent));RplComponent rpl=RplComponent.Cast(m_DeferredRound015.FindComponent(RplComponent));
  if(!move || !fuse || !rpl || rpl.IsProxy() || fuse.WasTriggered()){AbortDeferred015("components_authority_or_triggered");return;}
  float positionError=vector.Distance(m_DeferredRound015.GetOrigin(),m_DeferredPose015[3]);
  // Diagnostic guard against the measured first-tick reset to world0; never repair position.
  if(positionError>2){AbortDeferred015("actual_origin_outside_2m_guard");return;}
  if(m_iDeferredTube015<0 || m_iDeferredTube015>=8 || (m_iLoadedMask & (1 << m_iDeferredTube015))!=0){AbortDeferred015("reservation_lost");return;}
  TraceLifecycle015("deferred_pre_launch",m_DeferredRound015,m_iDeferredTube015);
  m_bDeferredNativeIssued015=true;
  move.Launch(m_DeferredPose015[2],m_DeferredParentVelocity015,0.6666667,m_DeferredRound015,m_DeferredUser015,m_DeferredRoot015,null,null);
  TraceLifecycle015("deferred_post_launch",m_DeferredRound015,m_iDeferredTube015);
  fuse.SetLive();TraceLifecycle015("post_live",m_DeferredRound015,m_iDeferredTube015);
  PrintFormat("GEP015_STOCK_DISPATCH tube=%1 entity=%2 actualOrigin=%3 velocity=%4 timer=%5 remaining=%6 deferredFrameDeltaMs=%7",m_iDeferredTube015,m_DeferredRound015.GetID(),m_DeferredRound015.GetOrigin(),move.GetVelocity(),fuse.GetTimer(),Count(),now-m_fDeferredQueuedMs015);
  GetGame().GetCallqueue().CallLater(InspectRound,5000,false,m_DeferredRound015,m_iDeferredTube015);
  m_DeferredUser015=null;m_DeferredRoot015=null;
 }
 protected void InspectRound(IEntity round, int tube)
 {
  if (!Authority() || !round) return;
  TimerTriggerComponent fuse = TimerTriggerComponent.Cast(round.FindComponent(TimerTriggerComponent));
  GrenadeMoveComponent move = GrenadeMoveComponent.Cast(round.FindComponent(GrenadeMoveComponent));
  if (!fuse || !move) return;
  PrintFormat("GEP015_STOCK_OBSERVE tube=%1 entity=%2 position=%3 velocity=%4 timer=%5 triggered=%6", tube, round.GetID(), round.GetOrigin(), move.GetVelocity(), fuse.GetTimer(), fuse.WasTriggered());
 }
 bool ServiceUser(IEntity user)
 {
  if (!user || !Operational() || m_bReloadPending || Count() >= 8) return false;
  CharacterControllerComponent life = CharacterControllerComponent.Cast(user.FindComponent(CharacterControllerComponent));
  CompartmentAccessComponent access = CompartmentAccessComponent.Cast(user.FindComponent(CompartmentAccessComponent));
  if (!life || life.GetLifeState() != ECharacterLifeState.ALIVE || !access || access.GetCompartment() || access.IsGettingIn() || access.IsGettingOut() || access.IsSwitchingSeatsAnim()) return false;
  vector service[4];
  PointInfo point = new PointInfo();
  Math3D.MatrixIdentity4(service);
  service[3] = "-1.9 -0.8 -0.55";
  point.Set(GetOwner(), "GEP_TurretYaw", service);
  if (point.GetNodeId() < 0) return false;
  point.GetWorldTransform(service);
  if (vector.Distance(user.GetOrigin(), service[3]) > 2.5) return false;
  Physics physics = GetOwner().GetRootParent().GetPhysics();
  return physics && physics.GetVelocity().Length() <= 0.25 && physics.GetAngularVelocity().Length() <= 0.05;
 }
 IEntity CarriedRound(IEntity user)
 {
  if (!user) return null;
  InventoryStorageManagerComponent inventory = InventoryStorageManagerComponent.Cast(user.FindComponent(InventoryStorageManagerComponent));
  if (!inventory) return null;
  BaseWeaponManagerComponent weapons = BaseWeaponManagerComponent.Cast(user.FindComponent(BaseWeaponManagerComponent));
  BaseWeaponComponent held;
  if (weapons) held = weapons.GetCurrentWeapon();
  array<IEntity> items = {};
  inventory.GetItems(items);
  foreach (IEntity item : items)
  {
   if (!item || !item.GetPrefabData() || item.GetPrefabData().GetPrefabName() != ROUND) continue;
   if (held && held.GetOwner() == item) continue;
   TimerTriggerComponent fuse = TimerTriggerComponent.Cast(item.FindComponent(TimerTriggerComponent));
   if (fuse && !fuse.WasTriggered()) return item;
  }
  return null;
 }
 bool BeginReload(IEntity user)
 {
  if (!Authority() || !ServiceUser(user)) return false;
  IEntity round = CarriedRound(user);
  if (!round) return false;
  InventoryStorageManagerComponent inventory = InventoryStorageManagerComponent.Cast(user.FindComponent(InventoryStorageManagerComponent));
  if (!inventory) return false;
  int tube = -1;
  for (int i = 0; i < 8; i++) if ((m_iLoadedMask & (1 << i)) == 0) { tube = i; break; }
  if (tube < 0) return false;
  m_iReloadSerial++;
  m_iReservedTube = tube;
  m_bReloadPending = true;
  m_DeleteCallback = new GEP_StockSmokeDelete015();
  m_DeleteCallback.Target = this;
  m_DeleteCallback.Serial = m_iReloadSerial;
  Replication.BumpMe();
  if (!inventory.TryDeleteItem(round, m_DeleteCallback))
  {
   CompleteReload(m_iReloadSerial, false);
   return false;
  }
  return true;
 }
 void CompleteReload(int serial, bool deleted)
 {
  if (!Authority() || !m_bReloadPending || serial != m_iReloadSerial) return;
  bool credited = deleted && Operational() && m_iReservedTube >= 0 && m_iReservedTube < 8;
  if (credited) m_iLoadedMask = m_iLoadedMask | (1 << m_iReservedTube);
  m_bReloadPending = false;
  m_iReservedTube = -1;
  Replication.BumpMe();
  PrintFormat("GEP015_STOCK_RELOAD serial=%1 itemDeleted=%2 tubeCredited=%3 remaining=%4", serial, deleted, credited, Count());
  OnAmmoChanged();
 }
 protected void OnAmmoChanged()
 {
  PrintFormat("GEP015_STOCK_AMMO authority=%1 mask=%2 current=%3 capacity=8 pending=%4", Authority(), m_iLoadedMask, Count(), m_bReloadPending);
 }
 override void OnDelete(IEntity owner)
 {
  m_iDeferredStage015=2;m_DeferredUser015=null;m_DeferredRoot015=null;m_DeferredRound015=null;
  GetGame().GetCallqueue().Remove(InspectRound);
  if (m_DeleteCallback) m_DeleteCallback.Target = null;
 }
}

class GEP_StockSmokeDelete015 : ScriptedInventoryOperationCallback
{
 GEP_StockSmoke015 Target;
 int Serial;
 override protected void OnComplete() { if (Target) Target.CompleteReload(Serial, true); }
 override protected void OnFailed() { if (Target) Target.CompleteReload(Serial, false); }
}

class GEP_StockSmokeFire015 : ScriptedUserAction
{
 [Attribute("0")] bool m_bSingleRound;
 GEP_StockSmoke015 Smoke() { return GEP_StockSmoke015.Cast(GetOwner().FindComponent(GEP_StockSmoke015)); }
 override bool HasLocalEffectOnlyScript() { return false; }
 override bool CheckOnServerFirstScript() { return true; }
 override bool CanBeShownScript(IEntity user) { GEP_StockSmoke015 smoke = Smoke(); return smoke && smoke.Crew(user); }
 override bool CanBePerformedScript(IEntity user) { GEP_StockSmoke015 smoke = Smoke(); return smoke && smoke.CanFire(user); }
 override bool GetActionNameScript(out string outName)
 {
  GEP_StockSmoke015 smoke = Smoke();
  int count;
  if (smoke) count = smoke.Count();
  string mode = "Launch smoke salvo";
  if (m_bSingleRound) mode = "Launch one smoke round";
  outName = string.Format("%1 (%2/8)", mode, count);
  return true;
 }
 override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) { GEP_StockSmoke015 smoke = Smoke(); if (smoke) smoke.Dispatch(pUserEntity, m_bSingleRound); }
}

class GEP_StockSmokeReload015 : ScriptedUserAction
{
 GEP_StockSmoke015 Smoke() { return GEP_StockSmoke015.Cast(GetOwner().FindComponent(GEP_StockSmoke015)); }
 override bool HasLocalEffectOnlyScript() { return false; }
 override bool CheckOnServerFirstScript() { return true; }
 override bool CanBeShownScript(IEntity user) { GEP_StockSmoke015 smoke = Smoke(); return smoke && smoke.ServiceUser(user); }
 override bool CanBePerformedScript(IEntity user) { GEP_StockSmoke015 smoke = Smoke(); return smoke && smoke.ServiceUser(user) && smoke.CarriedRound(user) != null; }
 override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) { GEP_StockSmoke015 smoke = Smoke(); if (smoke) smoke.BeginReload(pUserEntity); }
}

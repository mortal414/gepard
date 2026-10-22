// PRIVATE functional V11 visual QA caller. No vehicle input/state setters; parent wrapper owns native close.
[BaseContainerProps(category:"Respawn")]
class GEP_ArtNativeSpawnLogic20261022 : SCR_AutoSpawnLogic
{
 override protected void DoSpawn_S(int playerId)
 {
  if (RplSession.Mode() == RplMode.None) return;
  super.DoSpawn_S(playerId);
 }
}
class GEP_ArtNativeSceneQA20261022Class : GenericEntityClass {}
class GEP_ArtNativeSceneQA20261022 : GenericEntity
{
 protected ref GEP_ArtNativeQualification20261022 m_Observer = new GEP_ArtNativeQualification20261022();
 protected ref array<ResourceName> m_Prefabs = {
  "{2150FC6F1E081188}Prefabs/ArtApproved20261022/Camo/GEP_MobileCarrier.et",
  "{8095474EC9806D0D}Prefabs/ArtApproved20261022/Charcoal/GEP_MobileCarrier.et"
 };
 protected ref array<string> m_Skins = {"Camo", "Charcoal"};
 protected ref array<string> m_Views = {"front3q", "rear3q", "left", "right", "guns", "reargrille", "rearfar"};
 protected ref array<vector> m_Positions = {"592 0 2600", "608 0 2620"};
 protected ref array<IEntity> m_Subjects = {};
 protected ref array<vector> m_PreviousOrigins = {};
 protected ref array<vector> m_PreviousUp = {};
 protected ref array<vector> m_PreviousForward = {};
 protected IEntity m_OwnedCameraActor;
 protected IEntity m_ViewSubject;
 protected vector m_ViewOrigin; protected vector m_ViewUp; protected vector m_ViewForward;
 protected float m_StartMs = -1; protected float m_Age; protected float m_FloorY;
 protected float m_LastSettleSample = -1; protected float m_ViewStartMs;
 protected int m_StableSamples; protected int m_Case; protected int m_BootstrapPhase;
 protected bool m_Staged; protected bool m_Prepared; protected bool m_PlayerReady;
 protected bool m_Audited; protected bool m_ViewStaged; protected bool m_Done;

 void GEP_ArtNativeSceneQA20261022(IEntitySource src, IEntity parent)
 { SetEventMask(EntityEvent.FRAME); SetFlags(EntityFlags.ACTIVE, true); }

 protected void Fail(string reason)
 {
  m_Done = true;
  PrintFormat("GEP_ART_SCENE terminal=NOT_TESTED reason=%1 completedViews=%2 worldSeconds=%3 automaticVehicleInput=0 wrapperOwnsClose=1", reason, m_Case, m_Age);
 }
 protected IEntity SpawnAt(ResourceName prefab, vector position)
 {
  Resource resource = Resource.Load(prefab);
  if (!resource || !resource.IsValid()) return null;
  EntitySpawnParams params = new EntitySpawnParams(); params.TransformMode = ETransformMode.WORLD;
  params.Transform[0] = "1 0 0"; params.Transform[1] = "0 1 0"; params.Transform[2] = "0 0 1"; params.Transform[3] = position;
  return GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params);
 }
 protected void Stage()
 {
  m_Staged = true; m_FloorY = GetGame().GetWorld().GetSurfaceY(600,2600);
  foreach (vector point : m_Positions)
  {
   if (Math.AbsFloat(GetGame().GetWorld().GetSurfaceY(point[0], point[2]) - m_FloorY) > 0.02)
   { Fail("uneven-ground"); return; }
   for (int sx = -1; sx <= 1; sx = sx + 2)
   {
    for (int sz = -1; sz <= 1; sz = sz + 2)
    {
     if (Math.AbsFloat(GetGame().GetWorld().GetSurfaceY(point[0] + sx * 2, point[2] + sz * 4.2) - m_FloorY) > 0.02)
     { Fail("uneven-footprint"); return; }
    }
   }
  }
  ChimeraWorld world = ChimeraWorld.CastFrom(GetGame().GetWorld());
  TimeAndWeatherManagerEntity weather;
  if (world) weather = world.GetTimeAndWeatherManager();
  bool noonIssued;
  if (weather) noonIssued = weather.SetTime(new TimeContainer(12,0,0));
  PrintFormat("GEP_ART_SCENE lightingNoonRequested=%1 actualDaylight=TO_INSPECT floorY=%2", noonIssued, m_FloorY);
  for (int i = 0; i < 2; i++)
  {
   vector pos = m_Positions[i]; pos[1] = m_FloorY + 0.25;
   IEntity subject = SpawnAt(m_Prefabs[i], pos);
   if (!subject) { Fail("functional-subject-spawn-failed"); return; }
   m_Subjects.Insert(subject); subject.SetName("ART_NATIVE_FUNCTIONAL_" + m_Skins[i]);
   vector tr[4]; subject.GetWorldTransform(tr);
   m_PreviousOrigins.Insert(tr[3]); m_PreviousUp.Insert(tr[1]); m_PreviousForward.Insert(tr[2]);
   PrintFormat("GEP_ART_SCENE spawnedSkin=%1 prefab=%2 origin=%3 vehicleStateWrites=0", m_Skins[i], m_Prefabs[i], tr[3]);
  }
 }
 protected void PreparePlayer()
 {
  bool previouslyReady = m_PlayerReady; m_PlayerReady = false;
  PlayerController player = GetGame().GetPlayerController();
  SCR_BaseGameMode mode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  SCR_RespawnSystemComponent respawn = SCR_RespawnSystemComponent.GetInstance();
  if (!player || !mode || !mode.IsRunning() || !respawn) return;
  IEntity actor = player.GetControlledEntity();
  if (!actor)
  {
   vector pos = "600 0 2580"; pos[1] = m_FloorY + 0.1;
   actor = SpawnAt("{26A9756790131354}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Rifleman.et", pos);
   if (!actor) { Fail("initial-camera-actor-spawn-failed"); return; }
   m_OwnedCameraActor = actor; player.SetControlledEntity(actor);
  }
  if (!m_Prepared) { respawn.DestroyLoadingPlaceholder(); m_Prepared = true; }
  bool render = player.SetCharacterCameraRenderActive(true);
  if (player.GetPlayerCamera()) GetGame().GetCameraManager().SetCamera(player.GetPlayerCamera());
  SCR_CharacterControllerComponent life = SCR_CharacterControllerComponent.Cast(actor.FindComponent(CharacterControllerComponent));
  bool alive = life && life.GetLifeState() == ECharacterLifeState.ALIVE && !life.IsUnconscious();
  m_PlayerReady = render && alive && player.GetPlayerCamera() && GetGame().GetCameraManager().CurrentCamera() == player.GetPlayerCamera() && !ArmaReforgerLoadingAnim.IsOpen() && !GetGame().GetMenuManager().IsAnyMenuOpen();
  if (m_PlayerReady && !previouslyReady) Print("GEP_ART_SCENE playerReady=1 loadingPlaceholderDestroyed=1 automaticVehicleInput=0");
 }
 protected bool Settled(float now)
 {
  if (m_LastSettleSample >= 0 && now - m_LastSettleSample < 1000) return false;
  bool stable = m_LastSettleSample >= 0;
  for (int i = 0; i < 2; i++)
  {
   vector tr[4]; m_Subjects[i].GetWorldTransform(tr);
   float originDelta = vector.Distance(tr[3], m_PreviousOrigins[i]);
   float upDelta = vector.Distance(tr[1], m_PreviousUp[i]);
   float forwardDelta = vector.Distance(tr[2], m_PreviousForward[i]);
   if (originDelta > 0.02 || upDelta > 0.005 || forwardDelta > 0.005) stable = false;
   PrintFormat("GEP_ART_SCENE settleSkin=%1 worldMs=%2 origin=%3 originDeltaM=%4 upDelta=%5 forwardDelta=%6", m_Skins[i], now, tr[3], originDelta, upDelta, forwardDelta);
   m_PreviousOrigins[i] = tr[3]; m_PreviousUp[i] = tr[1]; m_PreviousForward[i] = tr[2];
  }
  m_LastSettleSample = now;
  if (stable) m_StableSamples++; else m_StableSamples = 0;
  return m_StableSamples >= 3;
 }
 protected bool PlaceView(int skinIndex, int viewIndex)
 {
  PlayerController player = GetGame().GetPlayerController();
  if (!player || !player.GetPlayerCamera()) return false;
  m_ViewSubject = m_Subjects[skinIndex];
  vector subjectMatrix[4]; m_ViewSubject.GetWorldTransform(subjectMatrix);
  // Known QA spawn yaw is identity. Reject drift before using the prior world-offset camera mechanism.
  if (vector.Distance(subjectMatrix[0], "1 0 0") > 0.02 || vector.Distance(subjectMatrix[1], "0 1 0") > 0.02 || vector.Distance(subjectMatrix[2], "0 0 1") > 0.02) return false;
  vector offset = "7 0 10"; vector right = "-0.8192319 0 0.5734623"; vector up = "0 1 0"; vector forward = "-0.5734623 0 -0.8192319";
  if (viewIndex == 1) { offset = "7 0 -10"; right = "0.8192319 0 0.5734623"; forward = "-0.5734623 0 0.8192319"; }
  if (viewIndex == 2) { offset = "-9 0 0"; right = "0 0 -1"; forward = "1 0 0"; }
  if (viewIndex == 3) { offset = "9 0 0"; right = "0 0 1"; forward = "-1 0 0"; }
  if (viewIndex == 4) { offset = "4 0 6"; right = "-0.8320503 0 0.5547002"; up = "0.1482499 0.9636241 0.2223748"; forward = "-0.5345225 0.2672612 -0.8017837"; }
  if (viewIndex == 5) { offset = "3 0 -6"; right = "0.8944272 0 0.4472136"; forward = "-0.4472136 0 0.8944272"; }
  // Same rear3q basis, four times horizontal distance; avoids the adjacent other subject; image evidence only.
  if (viewIndex == 6) { offset = "28 0 -40"; right = "0.8192319 0 0.5734623"; up = "0 1 0"; forward = "-0.5734623 0 0.8192319"; }
  vector point = subjectMatrix[3] + offset;
  point[1] = GetGame().GetWorld().GetSurfaceY(point[0], point[2]) + 0.1;
  EntitySpawnParams params = new EntitySpawnParams(); params.TransformMode = ETransformMode.WORLD;
  params.Transform[0] = right; params.Transform[1] = up; params.Transform[2] = forward; params.Transform[3] = point;
  Resource resource = Resource.Load("{26A9756790131354}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Rifleman.et");
  if (!resource || !resource.IsValid()) return false;
  IEntity actor = GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params);
  if (!actor) return false;
  IEntity previousOwnedActor = m_OwnedCameraActor;
  player.SetControlledEntity(actor); m_OwnedCameraActor = actor;
  if (player.GetControlledEntity() != actor) return false;
  if (previousOwnedActor && previousOwnedActor != actor && previousOwnedActor != m_Subjects[0] && previousOwnedActor != m_Subjects[1] && previousOwnedActor.FindComponent(CharacterControllerComponent))
   SCR_EntityHelper.DeleteEntityAndChildren(previousOwnedActor);
  player.SetCharacterCameraRenderActive(true);
  if (player.GetPlayerCamera()) GetGame().GetCameraManager().SetCamera(player.GetPlayerCamera());
  m_ViewOrigin = subjectMatrix[3]; m_ViewUp = subjectMatrix[1]; m_ViewForward = subjectMatrix[2];
  bool staged = m_Observer.BeginView(m_ViewSubject, actor, m_Skins[skinIndex], m_Views[viewIndex]);
  PrintFormat("GEP_ART_SCENE viewStaged=%1 case=%2 skin=%3 view=%4 actorOrigin=%5 viewRight=%6 viewUp=%7 viewForward=%8", staged, m_Case, m_Skins[skinIndex], m_Views[viewIndex], point, right, up, forward);
  return staged;
 }
 protected bool Bootstrap()
 {
  SCR_BaseGameMode initialMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  if (!initialMode || !initialMode.IsRunning())
  {
   if (m_BootstrapPhase != 1) Print("GEP_ART_SCENE bootstrapPhase=WAIT_GAME clockStarted=0");
   m_BootstrapPhase = 1; return false;
  }
  if (!m_Staged)
  {
   m_BootstrapPhase = 2; Print("GEP_ART_SCENE bootstrapPhase=STAGE clockStarted=0 timeGate=NONE");
   Stage();
  }
  if (m_Done || !m_Staged) return false;
  // Fresh existing player/camera/loading readback on every bootstrap frame, even while world time is paused.
  PreparePlayer();
  if (!m_PlayerReady)
  {
   if (m_BootstrapPhase != 3) PrintFormat("GEP_ART_SCENE bootstrapPhase=PLAYER_CAMERA clockStarted=0 timeScale=%1", GetGame().GetWorld().GetTimeScale());
   m_BootstrapPhase = 3; return false;
  }
  if (GetGame().GetWorld().GetTimeScale() != 1)
  {
   if (m_BootstrapPhase != 4) PrintFormat("GEP_ART_SCENE bootstrapPhase=WAIT_TIMESCALE_ONE playerReady=1 clockStarted=0 timeScale=%1", GetGame().GetWorld().GetTimeScale());
   m_BootstrapPhase = 4; return false;
  }
  return true;
 }
 override void EOnFrame(IEntity owner, float timeSlice)
 {
  if (m_Done || !GetGame().InPlayMode() || RplSession.Mode() != RplMode.None) return;
  if (m_StartMs < 0)
  {
   if (!Bootstrap()) return;
   m_StartMs = GetGame().GetWorld().GetWorldTime(); m_BootstrapPhase = 5;
   PrintFormat("GEP_ART_SCENE bootstrapPhase=QUALIFIED playerReady=1 timeScale=%1 worldMs=%2 clockStarted=1", GetGame().GetWorld().GetTimeScale(), m_StartMs);
   Print("GEP_ART_SCENE begin=1 actualFunctionalSubjects=2 plannedViews=14 staticDisplays=0 vehicleInputs=0");
  }
  float now = GetGame().GetWorld().GetWorldTime();
  m_Age = (now - m_StartMs) / 1000.0;
  if (m_Age > 140) { Fail("world-deadline"); return; }
  if (GetGame().GetWorld().GetTimeScale() != 1) { Fail("timescale-not-one"); return; }
  if (m_Age < 12) return;
  if (!m_Audited)
  {
   if (!Settled(now)) return;
   bool camo = m_Observer.Inspect(m_Subjects[0], "Camo");
   bool charcoal = m_Observer.Inspect(m_Subjects[1], "Charcoal");
   if (!camo || !charcoal) { Fail("native-readout-incomplete"); return; }
   m_Audited = true; Print("GEP_ART_SCENE bothFunctionalReadoutsComplete=1 artAcceptance=PENDING");
  }
  if (!m_ViewStaged)
  {
   int skinIndex = m_Case / 7; int viewIndex = m_Case % 7;
   if (!PlaceView(skinIndex, viewIndex)) { Fail("view-placement-or-binding-failed"); return; }
   m_ViewStartMs = now; m_ViewStaged = true; return;
  }
  vector current[4]; m_ViewSubject.GetWorldTransform(current);
  if (vector.Distance(current[3], m_ViewOrigin) > 0.02 || vector.Distance(current[1], m_ViewUp) > 0.005 || vector.Distance(current[2], m_ViewForward) > 0.005)
  { Fail("subject-moved-during-view"); return; }
  if (now - m_ViewStartMs > 12000) { Fail("view-camera-or-capture-deadline"); return; }
  if (now - m_ViewStartMs < 5000) return;
  if (!m_Observer.TryCapture()) return;
  m_Case++; m_ViewStaged = false;
  if (m_Case == 14)
  {
   m_Done = true;
   PrintFormat("GEP_ART_SCENE terminal=CAPTURE_REQUESTS_COMPLETE actualFunctionalSubjects=2 completedViews=14 worldSeconds=%1 pixelsValidation=PENDING artAcceptance=PENDING dynamicGameplay=NOT_TESTED newPadsBound=0 wrapperOwnsClose=1", m_Age);
  }
 }
}

// Isolated untimed ordinary-control comparison. No automatic boarding/input.
[BaseContainerProps(category:"Respawn")]
class GEP_Approved705SpawnLogic : SCR_AutoSpawnLogic
{
 override protected void DoSpawn_S(int playerId)
 {
  if(RplSession.Mode()==RplMode.None){return;}
  super.DoSpawn_S(playerId);
 }
}
class GEP_Approved705SceneClass : GenericEntityClass {}
class GEP_Approved705Scene : GenericEntity
{
 float StartMs=-1;float Age;float ReadyAt;
 bool Staged;bool Prepared;bool PlayerReady;bool HintShown;bool BodyStartRequested;
 bool FrameOne;bool FrameTwo;
 IEntity Baseline;IEntity Candidate;IEntity Third;IEntity Actor;bool CompareConfigured;
 void GEP_Approved705Scene(IEntitySource src,IEntity parent){SetEventMask(EntityEvent.FRAME);SetFlags(EntityFlags.ACTIVE,true);}
 IEntity Spawn(ResourceName prefab,vector position,float clearance)
 {
  Resource resource=Resource.Load(prefab);
  if(!resource || !resource.IsValid()){return null;}
  EntitySpawnParams params=new EntitySpawnParams();params.TransformMode=ETransformMode.WORLD;
  position[1]=GetGame().GetWorld().GetSurfaceY(position[0],position[2])+clearance;params.Transform[3]=position;
  return GetGame().SpawnEntityPrefab(resource,GetGame().GetWorld(),params);
 }
 override void EOnFrame(IEntity owner,float timeSlice)
 {
  if(!GetGame().InPlayMode() || System.IsConsoleApp() || RplSession.Mode()!=RplMode.None){return;}
  float nowMs=GetGame().GetWorld().GetWorldTime();
  if(StartMs<0){StartMs=nowMs;}
  Age=(nowMs-StartMs)/1000.0;
  if(!Staged && Age>1)
  {
   Staged=true;
   Baseline=Spawn("{D561AD08932E5480}Prefabs/Vehicles/GEP_MobileCarrier.et","600 0 2600",0.7);
   Candidate=Baseline;Third=Baseline;CompareConfigured=Baseline!=null;
   if(Baseline)Baseline.SetName("GEPARD - genehmigtes Zielvisier");
   PrintFormat("GEP_UPDATE101_SCENE_STAGED baseline=%1 candidate=%2 automaticBoarding=0 inputWrites=0 timeLimit=none",Baseline!=null,Candidate!=null);
  }
  PlayerController player=GetGame().GetPlayerController();
  if(!PlayerReady && Staged && Age>3 && player)
  {
   SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
   SCR_RespawnSystemComponent respawn=SCR_RespawnSystemComponent.GetInstance();
   if(!mode || !mode.IsRunning() || !respawn){return;}
   Actor=player.GetControlledEntity();
   if(!Actor)
   {
    Actor=Spawn("{26A9756790131354}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Rifleman.et","600 0 2593",0.1);
    if(!Actor){return;}
    player.SetControlledEntity(Actor);
   }
   if(!Prepared)
   {
    respawn.DestroyLoadingPlaceholder();
    Prepared=true;
    Print("GEP_UPDATE101_LOADING_PLACEHOLDER clearedThroughPublicRespawnAPI=1");
   }
   bool renderEnabled=player.SetCharacterCameraRenderActive(true);
   if(player.GetPlayerCamera()){GetGame().GetCameraManager().SetCamera(player.GetPlayerCamera());}
   SCR_CharacterControllerComponent life=SCR_CharacterControllerComponent.Cast(Actor.FindComponent(CharacterControllerComponent));
   bool canInteract=life && life.CanInteract();
   bool alive=life && life.GetLifeState()==ECharacterLifeState.ALIVE && !life.IsUnconscious();
   bool loading=ArmaReforgerLoadingAnim.IsOpen();
   bool menu=GetGame().GetMenuManager().IsAnyMenuOpen();
   bool camera=player.GetPlayerCamera()!=null && GetGame().GetCameraManager().CurrentCamera()==player.GetPlayerCamera();
   PlayerReady=CompareConfigured && Baseline!=null && Candidate!=null && Third!=null && renderEnabled && camera && alive && canInteract && !loading && !menu;
   if(PlayerReady)
   {
    ReadyAt=Age;
    PrintFormat("GEP_UPDATE101_SCENE_PLAYER ready=1 alive=%1 canInteract=%2 loading=%3 menu=%4 playerCamera=%5 renderEnabled=%6 physicalInput=NOT_TESTED",alive,canInteract,loading,menu,camera,renderEnabled);
   }
  }
  if(PlayerReady && !HintShown && Age>ReadyAt+0.5)
  {
   HintShown=SCR_HintManagerComponent.ShowCustomHint("Neues Zielvisier: Darstellung bei allen Zoomstufen vergleichen. Umschalt+V: Waffengruppe. C: Entfernung. N: Tag/Restlicht. Munition und Turmanzeige bei echter Bedienung pruefen. Kein Zeitlimit; normal schliessen.","GEPARD - Zielvisier",20,true);
  }
  if(PlayerReady && !FrameOne && Age>ReadyAt+1)
  {
   FrameOne=System.MakeScreenshot("$profile:approved705-manual-ready-01");
   PrintFormat("GEP_UPDATE101_NATIVE_FRAME frame=1 requested=%1",FrameOne);
  }
  if(PlayerReady && !FrameTwo && Age>ReadyAt+3)
  {
   FrameTwo=System.MakeScreenshot("$profile:approved705-manual-ready-02");
   PrintFormat("GEP_UPDATE101_NATIVE_FRAME frame=2 requested=%1",FrameTwo);
  }
 }
}

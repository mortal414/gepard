// Untimed, on-foot ordinary-control acceptance scene; no automatic action/input.
[BaseContainerProps(category:"Respawn")]
class GEP_DemoSpawnLogic : SCR_AutoSpawnLogic
{
 override protected void DoSpawn_S(int playerId)
 {
  if(RplSession.Mode()==RplMode.None){return;}
  super.DoSpawn_S(playerId);
 }
}
class GEP_DemoSceneClass : GenericEntityClass {}
class GEP_DemoScene : GenericEntity
{
 float LastMs=-1;float Age;bool Staged;bool Ready;bool Hint;IEntity DemoVehicle;IEntity Actor;
 void GEP_DemoScene(IEntitySource src,IEntity parent){SetEventMask(EntityEvent.FRAME);SetFlags(EntityFlags.ACTIVE,true);}
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
  if(LastMs<0){LastMs=nowMs;}
  Age=Age+(nowMs-LastMs)/1000.0;
  LastMs=nowMs;
  if(!Staged && Age>1)
  {
   Staged=true;
   DemoVehicle=Spawn("{B3495C8E7A0D2001}Prefabs/Vehicles/GEP_MobileCarrier_StockSmoke015.et","600 0 2600",0.7);
   Spawn("{9DB69176CEF0EE97}Prefabs/Weapons/Grenades/Smoke_ANM8HC.et","603 0 2596",0.2);
   Spawn("{9DB69176CEF0EE97}Prefabs/Weapons/Grenades/Smoke_ANM8HC.et","604 0 2596",0.2);
   Spawn("{9DB69176CEF0EE97}Prefabs/Weapons/Grenades/Smoke_ANM8HC.et","602 0 2596",0.2);
   PrintFormat("GEP_DEMO_STAGED kind=smoke vehicle=%1 automaticBoarding=0 inputWrites=0",DemoVehicle!=null);
  }
  if(!Ready && Staged && Age>3)
  {
   PlayerController player=GetGame().GetPlayerController();
   if(!player){return;}
   Actor=player.GetControlledEntity();
   if(!Actor)
   {
    Actor=Spawn("{26A9756790131354}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Rifleman.et","603 0 2597",0.1);
    if(!Actor){return;}
    player.SetControlledEntity(Actor);
   }
   player.SetCharacterCameraRenderActive(true);
   if(player.GetPlayerCamera()){GetGame().GetCameraManager().SetCamera(player.GetPlayerCamera());}
   Ready=Actor!=null && player.GetPlayerCamera()!=null && GetGame().GetCameraManager().CurrentCamera()==player.GetPlayerCamera();
   if(Ready){Print("GEP_DEMO_READY kind=smoke physicalInput=NOT_TESTED");}
  }
  if(Ready && !Hint && Age>4)
  {
   Hint=SCR_HintManagerComponent.ShowCustomHint("Stock smoke gate: three unarmed AN-M8 grenades are on the ground ahead. First pick up and throw one normally; verify white smoke. Then enter gunner and Launch ONE smoke round. Require 8 to 7 plus real flight and white cloud. Stop if it fails; do not salvo. Launcher artwork is absent. Close normally when done.","Gepard smoke test",30,true);
  }
 }
}

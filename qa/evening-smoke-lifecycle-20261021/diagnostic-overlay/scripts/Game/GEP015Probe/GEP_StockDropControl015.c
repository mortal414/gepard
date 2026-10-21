// Engineering stock-helper control. This is NOT an ordinary character throw.
class GEP_StockDropControl015Class : GEP_OneRoundProbe015Class {}
class GEP_StockDropControl015 : GEP_OneRoundProbe015
{
 override void View(float timeSlice)
 {
  if(!Staged)return;
  if(!Camera)Camera=CameraBase.Cast(GetGame().SpawnEntity(CameraBase,GetGame().GetWorld()));if(!Camera)return;
  vector eye=LaunchOrigin+Vector(7,5,7);vector aim=LaunchOrigin-Vector(0,1,0);vector pose[4];
  Math3D.DirectionAndUpMatrix((aim-eye).Normalized(),vector.Up,pose);pose[3]=eye;
  Camera.SetWorldTransform(pose);Camera.SetFOVDegree(65);GetGame().GetCameraManager().SetCamera(Camera);Camera.ApplyTransform(timeSlice);
 }
 void ArmStockControl015()
 {
  if(m_iTerminalLatch015!=0 || !Round || Round.IsDeleted())return;
  TimerTriggerComponent fuse=TimerTriggerComponent.Cast(Round.FindComponent(TimerTriggerComponent));
  if(!fuse){Stop("drop_fuse_missing");return;}
  ObserveLifecycle015("stock_pre_deferred_live",Round,0);fuse.SetLive();ObserveLifecycle015("stock_post_deferred_live",Round,0);
 }
 override void EOnFrame(IEntity owner,float timeSlice)
 {
  if(!GetGame().InPlayMode() || System.IsConsoleApp() || RplSession.Mode()!=RplMode.None || m_iTerminalLatch015!=0)return;
  float now=GetWorld().GetWorldTime();if(StartMs<0)StartMs=now;Age=(now-StartMs)/1000.0;
  PlayerController player=GetGame().GetPlayerController();SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());SCR_RespawnSystemComponent respawn=SCR_RespawnSystemComponent.GetInstance();
  if(!Bound && Age>3 && player && mode && mode.IsRunning() && respawn)
  {
   Actor=Spawn("{26A9756790131354}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Rifleman.et","607 0 2594",0.1);
   if(!Actor){Stop("drop_actor_spawn_failed");return;}
   player.SetControlledEntity(Actor);respawn.DestroyLoadingPlaceholder();player.SetCharacterCameraRenderActive(false);SCR_HintManagerComponent.HideHint();Bound=true;
  }
  if(Bound && !Staged && Age>5)
  {
   m_sLifecycleCase015="stock_helper_drop_no_launch";Dispatched=true;DispatchMs=now;
   LaunchOrigin=Vector(603,0,2600);LaunchOrigin[1]=GetWorld().GetSurfaceY(LaunchOrigin[0],LaunchOrigin[2])+2;
   Round=Spawn("{9DB69176CEF0EE97}Prefabs/Weapons/Grenades/Smoke_ANM8HC.et",LaunchOrigin,2);Staged=true;
   ObserveLifecycle015("stock_post_spawn",Round,0);
   if(!Round){Stop("stock_drop_spawn_failed");return;}
   GrenadeMoveComponent move=GrenadeMoveComponent.Cast(Round.FindComponent(GrenadeMoveComponent));TimerTriggerComponent fuse=TimerTriggerComponent.Cast(Round.FindComponent(TimerTriggerComponent));RplComponent rpl=RplComponent.Cast(Round.FindComponent(RplComponent));
   if(!move || !fuse || !rpl || rpl.IsProxy()){Stop("stock_drop_components_or_authority_failed");return;}
   // Same public helper semantics: schedule one-second SetLive, then enable simulation.
   SCR_ScenarioFrameworkSystem.GetCallQueuePausable().CallLater(ArmStockControl015,1000,false);
   ObserveLifecycle015("stock_pre_enable",Round,0);move.EnableSimulation(Round);ObserveLifecycle015("stock_post_enable",Round,0);
   ScheduleReadbacks015(Round,0);Save("stock_drop_started");
  }
  View(timeSlice);
  if(Staged)
  {
   float elapsed=(now-DispatchMs)/1000.0;if(Round && now-LastSample>=500)Sample();
   float nextFrame=0.25;if(FrameIndex==1)nextFrame=1;if(FrameIndex==2)nextFrame=3;if(FrameIndex==3)nextFrame=5;if(FrameIndex==4)nextFrame=8;if(FrameIndex==5)nextFrame=12;
   if(FrameIndex<6 && elapsed>=nextFrame){FrameIndex++;bool requested=System.MakeScreenshot(string.Format("$profile:GEP015-stock-drop-%1",FrameIndex));PrintFormat("GEP015_STOCK_DROP_FRAME index=%1 elapsed=%2 requested=%3",FrameIndex,elapsed,requested);}
   if(elapsed>=13){Stop("stock_drop_diagnostic_complete_acceptance_pending");return;}
  }
  if(Age>=35)Stop("stock_drop_preflight_timeout");
 }
}

// QA engineering control through the real character inventory/equip/weapon manager.
// Exactly one native Throw callback; no custom projectile spawn/Launch/arming/physics changes.
class GEP_CharacterInventoryCallback015 : ScriptedInventoryOperationCallback
{
 GEP_CharacterThrowControl015 Target;
 override protected void OnComplete(){if(Target){Target.InventoryComplete015(true);Target=null;}}
 override protected void OnFailed(){if(Target){Target.InventoryComplete015(false);Target=null;}}
}
class GEP_CharacterThrowControl015Class : GEP_OneRoundProbe015Class {}
class GEP_CharacterThrowControl015 : GEP_OneRoundProbe015
{
 static const ResourceName ROUND015="{9DB69176CEF0EE97}Prefabs/Weapons/Grenades/Smoke_ANM8HC.et";
 ref GEP_CharacterInventoryCallback015 m_InventoryCallback015;
 ref array<IEntity> m_ExistingRounds015={};
 InventoryStorageManagerComponent m_Inventory015;
 BaseWeaponManagerComponent m_Weapons015;
 SCR_CharacterControllerComponent m_Character015;
 int m_iInventoryState015;bool m_bEquipRequested015;bool m_bThrowIssued015;float m_fEquipRequestedMs015;
 override void View(float timeSlice)
 {
  if(!Bound)return;
  if(!Camera)Camera=CameraBase.Cast(GetGame().SpawnEntity(CameraBase,GetGame().GetWorld()));if(!Camera)return;
  vector eye=Actor.GetOrigin()+Vector(12,10,22);vector aim=Actor.GetOrigin()+Vector(0,2,14);vector pose[4];
  Math3D.DirectionAndUpMatrix((aim-eye).Normalized(),vector.Up,pose);pose[3]=eye;
  Camera.SetWorldTransform(pose);Camera.SetFOVDegree(70);GetGame().GetCameraManager().SetCamera(Camera);Camera.ApplyTransform(timeSlice);
 }
 bool IsExactRound015(IEntity item)
 {return item && !item.IsDeleted() && item.GetPrefabData() && item.GetPrefabData().GetPrefabName()==ROUND015;}
 bool CharacterReady015()
 {
  if(!Actor || !m_Character015 || !m_Weapons015 || !m_Inventory015)return false;
  CompartmentAccessComponent access=CompartmentAccessComponent.Cast(Actor.FindComponent(CompartmentAccessComponent));
  return m_Character015.GetLifeState()==ECharacterLifeState.ALIVE && !m_Character015.IsUnconscious() && access && !access.GetCompartment() && !access.IsGettingIn() && !access.IsGettingOut() && !access.IsSwitchingSeatsAnim();
 }
 void InventoryComplete015(bool success)
 {
  if(m_iTerminalLatch015!=0 || m_iInventoryState015!=1)return;
  if(!success){m_iInventoryState015=-1;Stop("character_inventory_callback_failed");return;}
  array<IEntity> items={};m_Inventory015.GetItems(items);int freshCount;
  foreach(IEntity item:items){if(IsExactRound015(item) && m_ExistingRounds015.Find(item)<0){Round=item;freshCount++;}}
  if(freshCount!=1 || !Round){Stop("inventory_completed_without_one_unique_new_ANM8");return;}
  m_iInventoryState015=2;
  PrintFormat("GEP015_CHARACTER_INVENTORY complete=1 uniqueNewRound=%1 exactPrefab=1",Identity015(Round));
  ObserveLifecycle015("character_inventory_complete",Round,0);
 }
 override void EOnFrame(IEntity owner,float timeSlice)
 {
  if(!GetGame().InPlayMode() || System.IsConsoleApp() || RplSession.Mode()!=RplMode.None || m_iTerminalLatch015!=0)return;
  float now=GetWorld().GetWorldTime();if(StartMs<0)StartMs=now;Age=(now-StartMs)/1000.0;
  PlayerController player=GetGame().GetPlayerController();SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());SCR_RespawnSystemComponent respawn=SCR_RespawnSystemComponent.GetInstance();
  if(!Bound && Age>3 && player && mode && mode.IsRunning() && respawn)
  {
   Actor=Spawn("{26A9756790131354}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Rifleman.et","603 0 2595",0.1);
   if(!Actor){Stop("character_actor_spawn_failed");return;}
   player.SetControlledEntity(Actor);respawn.DestroyLoadingPlaceholder();player.SetCharacterCameraRenderActive(false);SCR_HintManagerComponent.HideHint();Bound=true;Staged=true;
   m_sLifecycleCase015="character_weapon_manager_throw";
   m_Character015=SCR_CharacterControllerComponent.Cast(Actor.FindComponent(CharacterControllerComponent));
   m_Weapons015=BaseWeaponManagerComponent.Cast(Actor.FindComponent(BaseWeaponManagerComponent));
   m_Inventory015=InventoryStorageManagerComponent.Cast(Actor.FindComponent(InventoryStorageManagerComponent));
   if(!CharacterReady015()){Stop("character_components_or_life_not_ready");return;}
   LaunchOrigin=Actor.GetOrigin()+Vector(0,1.6,0);DispatchMs=now;
  }
  View(timeSlice);
  if(Bound && Age>5 && m_iInventoryState015==0)
  {
   array<IEntity> priorItems={};m_Inventory015.GetItems(priorItems);foreach(IEntity prior:priorItems)if(IsExactRound015(prior))m_ExistingRounds015.Insert(prior);
   m_InventoryCallback015=new GEP_CharacterInventoryCallback015();m_InventoryCallback015.Target=this;m_iInventoryState015=1;
   bool scheduled=m_Inventory015.TrySpawnPrefabToStorage(ROUND015,null,-1,EStoragePurpose.PURPOSE_WEAPON_PROXY,m_InventoryCallback015,1);
   PrintFormat("GEP015_CHARACTER_INVENTORY requested=1 scheduled=%1 count=1 purpose=WEAPON_PROXY callbacksRequired=1",scheduled);
   if(!scheduled){Stop("character_inventory_schedule_rejected");return;}
  }
  if(m_iInventoryState015==2 && !m_bEquipRequested015 && CharacterReady015())
  {
   m_bEquipRequested015=true;m_fEquipRequestedMs015=now;
   bool accepted=m_Character015.TryEquipRightHandItem(Round,EEquipItemType.EEquipTypeWeapon,false);
   PrintFormat("GEP015_CHARACTER_EQUIP requested=1 accepted=%1 item=%2 readinessStillRequired=1",accepted,Identity015(Round));
   if(!accepted){Stop("character_equip_request_rejected");return;}
  }
  if(m_bEquipRequested015 && !m_bThrowIssued015 && now-m_fEquipRequestedMs015>500 && CharacterReady015())
  {
   BaseWeaponComponent current=m_Weapons015.GetCurrentWeapon();BaseWeaponComponent grenade=m_Weapons015.GetCurrentGrenade();WeaponSlotComponent slot=m_Weapons015.GetCurrentSlot();
   bool currentMatches=current && current.GetOwner()==Round;bool grenadeMatches=grenade && grenade.GetOwner()==Round;bool slotMatches=slot && slot.GetWeaponEntity()==Round;
   bool canThrow=m_Character015.GetCanThrow();
   if(currentMatches && grenadeMatches && slotMatches && canThrow)
   {
    m_bThrowIssued015=true;Dispatched=true;DispatchMs=now;LaunchOrigin=Actor.GetOrigin()+Vector(0,1.6,0);
    ObserveLifecycle015("character_pre_weapon_manager_throw",Round,0);
    vector direction=Vector(0,0.5,1).Normalized();
    PrintFormat("GEP015_CHARACTER_THROW requested=1 callbackCount=1 aliveConsciousOnFoot=1 currentMatches=%1 grenadeMatches=%2 slotMatches=%3 canThrow=%4 direction=%5 scale=1",currentMatches,grenadeMatches,slotMatches,canThrow,direction);
    m_Weapons015.Throw(direction,1);
    ObserveLifecycle015("character_post_weapon_manager_throw",Round,0);ScheduleReadbacks015(Round,0);Save("character_throw_callback_issued");
   }
  }
  if(m_bThrowIssued015)
  {
   float elapsed=(now-DispatchMs)/1000.0;if(Round && now-LastSample>=500)Sample();
   float nextFrame=0.25;if(FrameIndex==1)nextFrame=1;if(FrameIndex==2)nextFrame=3;if(FrameIndex==3)nextFrame=5;if(FrameIndex==4)nextFrame=8;if(FrameIndex==5)nextFrame=12;
   if(FrameIndex<6 && elapsed>=nextFrame){FrameIndex++;bool requested=System.MakeScreenshot(string.Format("$profile:GEP015-character-throw-%1",FrameIndex));PrintFormat("GEP015_CHARACTER_THROW_FRAME index=%1 elapsed=%2 requested=%3",FrameIndex,elapsed,requested);}
   if(elapsed>=13){Stop("character_throw_diagnostic_complete_acceptance_pending");return;}
  }
  if(!m_bThrowIssued015 && Age>=40){Stop("character_inventory_equip_or_throw_readiness_timeout_no_retry");return;}
 }
}

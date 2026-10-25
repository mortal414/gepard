// Isolated original manual long-range scene. No automated firing, correction,
// target movement, AI route or native process control. Public actor seating is setup.
class GPG_LongRangeDirector725Class : GPG_SoloDirector708Class {}
class GPG_LongRangeDirector725 : GPG_SoloDirector708
{
 protected float ReadTimer726;
 override void EOnFrame(IEntity owner,float timeSlice)
 {
  super.EOnFrame(owner,timeSlice);
  ReadTimer726+=timeSlice;if(ReadTimer726<0.5)return;ReadTimer726=0;
  PlayerController player726=GetGame().GetPlayerController();if(!player726)return;
  IEntity actor726=player726.GetControlledEntity();if(!actor726)return;
  CompartmentAccessComponent access726=CompartmentAccessComponent.Cast(actor726.FindComponent(CompartmentAccessComponent));
  if(!access726 || access726.IsGettingIn() || access726.IsGettingOut() || access726.IsSwitchingSeatsAnim())return;
  BaseCompartmentSlot seat726=access726.GetCompartment();if(!seat726 || seat726.GetOccupant()!=actor726)return;
  IEntity turret726=seat726.GetOwner();if(!turret726)return;
  GEP_TrackingStabilization707 stab726=GEP_TrackingStabilization707.Cast(turret726.FindComponent(GEP_TrackingStabilization707));
  GEP_NextFireControlConsumer726 fc726=GEP_NextFireControlConsumer726.Cast(turret726.FindComponent(GEP_NextFireControlConsumer726));
  TurretControllerComponent control726=TurretControllerComponent.Cast(turret726.FindComponent(TurretControllerComponent));
  SCR_FireModeManagerComponent groups726=SCR_FireModeManagerComponent.Cast(turret726.FindComponent(SCR_FireModeManagerComponent));
  if(!stab726 || !fc726 || !control726 || !groups726)return;
  AimingComponent aim726=control726.GetTurretComponent();if(!aim726)return;
  string name726;int group726=groups726.GetWeaponGroupID(name726);
  PrintFormat("GPG_COMBINED726_STATE time=%1 mode=%2 enabled=%3 tracking=%4 target=%5 group=%6 ADS=%7 range=%8 result=%9",GetGame().GetWorld().GetWorldTime(),stab726.GetMode(),stab726.GetStabilizationEnabled(),stab726.IsTracking(),GPG_SoloRead708.NativeID624(stab726.GetStabilizationTarget()),group726,control726.GetCurrentSightsADS(),fc726.GetRange(),fc726.GetRangeResult());
  PrintFormat("GPG_COMBINED726_NATIVE time=%1 current=%2 wanted=%3 correctionGeneration=%4 valid=%5 lockCue=%6 applied=%7",GetGame().GetWorld().GetWorldTime(),aim726.GetAimingRotation(),aim726.GetAimingRotationWanted(),fc726.Generation726(),fc726.FreshValid726(),stab726.HasConfirmedEntityLockCue715(),Vector(fc726.GetGunTraverse(),fc726.GetGunElevation(),0));
  array<WeaponSlotComponent> slots726={};control726.GetWeaponManager().GetWeaponsSlots(slots726);
  foreach(WeaponSlotComponent slot726:slots726)
  {
   IEntity gun726=slot726.GetWeaponEntity();if(!gun726)continue;
   BaseWeaponComponent weapon726=BaseWeaponComponent.Cast(gun726.FindComponent(BaseWeaponComponent));
   if(weapon726 && weapon726.GetCurrentMuzzle())PrintFormat("GPG_COMBINED726_AMMO time=%1 slot=%2 count=%3",GetGame().GetWorld().GetWorldTime(),slot726.GetWeaponSlotIndex(),weapon726.GetCurrentMuzzle().GetAmmoCount());
  }
 }
 override void StageSolo708()
 {
  InitialScenario712="target";
  if(!PadReady710("LONG_RANGE_PLAYER","663 0 2689.8") || !PadReady710("LONG_RANGE_GEPARD","665 0 2695"))
  {Failed=true;Hint("Known gunner pad refused. No test admitted.","LONG RANGE SOURCE CANDIDATE");return;}
  AddSolo708("TARGET-TRACKING-CAMO","target","{A76A420025072610}Prefabs/FinishStabilization/Camo/GEP_Combined726.et","665 0 2695",90);
  Board708("target","663 0 2691.5","EXPERIMENTAL NATIVE BALLISTICS / 1000m AND 1500m");
  array<vector> pads725={"1665 0 2680","2165 0 2715"};
  array<string> names725={"GPG_LongRange_1000m_LEFT","GPG_LongRange_1500m_RIGHT"};
  for(int i725=0;i725<2;i725++)
  {
   vector from725=Ground("665 0 2695",2.5);
   vector ground725=Ground(pads725[i725],0);
   float aimHeight725=ground725[1]+1.5;
   // Initial fixture placement only: elevate the static training target enough
   // for a sampled terrain-clear line. No later target, camera or gun writes.
   for(int step725=1;step725<=200;step725++)
   {
    float fraction725=step725/200.0;
    vector sample725=from725+(ground725-from725)*fraction725;
    float clearance725=GetGame().GetWorld().GetSurfaceY(sample725[0],sample725[2])+1.0;
    aimHeight725=Math.Max(aimHeight725,from725[1]+(clearance725-from725[1])/fraction725);
   }
   float lift725=Math.Max(0.05,aimHeight725-1.5-ground725[1]);
   IEntity target725=Spawn("{DFD494A96CA95D0B}Prefabs/Structures/Military/Training/TargetVehicles/TargetBTR_front.et",pads725[i725],90,lift725);
   if(!target725){Failed=true;continue;}
   target725.SetName(names725[i725]);
   vector to725=target725.GetOrigin()+Vector(0,1.5,0);
   TraceParam trace725=new TraceParam();trace725.Start=from725;trace725.End=to725;trace725.Flags=TraceFlags.WORLD;
   float terrain725=GetGame().GetWorld().TraceMove(trace725,null);
   vector flat725=pads725[i725]-Vector(665,0,2695);flat725[1]=0;
   PrintFormat("GPG_LONG_RANGE_TARGET name=%1 targetOrigin=%2 horizontal=%3 slantSurvey=%4 terrainFraction=%5 initialTargetHeightFromTerrainSurvey=1 noVisibilityAcceptance=1",names725[i725],target725.GetOrigin(),flat725.Length(),vector.Distance(from725,to725),terrain725);
  }
  Print("GPG_LONG_RANGE_SOURCE_READY nativeCompile=NOT_TESTED impacts=NOT_TESTED automaticShots=0 baseline707Unmodified=1");
 }
 override void Admit708() {} // Stationary impact comparison; no truck or own-motion route.
 override void Explain708(string station)
 {
  Hint("Combined707 tracking and native35mm correction. Gunner seating is setup only. RMB ADS; Shift+V selects Both/Left/Right. Aim then C to range and apply elevation with stabilization OFF or ON. C never changes stabilization mode. Hold Space toggles stabilization; tap Space designates/clears a target while ON. Mint outer reticle caps confirm entity tracking. 1000m target is slightly LEFT of east; 1500m is slightly RIGHT. Measured range is authoritative. ADS loss cancels the solution; manually re-aim/re-measure after return. Gun-group, ammo, sight or seat changes clear only the range solution; re-aim/re-range. Original707 rollback is preserved. Approximate long-range correction, not full ballistic accuracy qualification.","COMBINED PRIVATE PLAYTEST");
 }
}

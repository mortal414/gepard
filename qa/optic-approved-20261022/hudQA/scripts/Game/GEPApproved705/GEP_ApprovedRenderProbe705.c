// PRIVATE bounded visual qualification only. Scripted native boarding/ADS;
// screenshots prove rendered pixels, never ordinary user input or accuracy.
class GEP_ApprovedRenderProbe705Class : GEP_Approved705SceneClass {}
class GEP_ApprovedRenderProbe705 : GEP_Approved705Scene
{
 int ProbeCase;int ProbePhase;float ProbeAt;bool ProbeDone;
 BaseCompartmentSlot ProbeSeat;TurretControllerComponent ProbeControl;GEP_GunStatus ProbeHud;
 BaseCompartmentSlot FindProbeSeat(IEntity node,int depth=0)
 {
  if(!node || depth>16)return null;
  BaseCompartmentManagerComponent manager=BaseCompartmentManagerComponent.Cast(node.FindComponent(BaseCompartmentManagerComponent));
  if(manager){array<BaseCompartmentSlot> seats={};manager.GetCompartments(seats);foreach(BaseCompartmentSlot seat:seats){if(TurretCompartmentSlot.Cast(seat))return seat;}}
  for(IEntity child=node.GetChildren();child;child=child.GetSibling()){BaseCompartmentSlot found=FindProbeSeat(child,depth+1);if(found)return found;}
  return null;
 }
 GEP_GunStatus FindProbeHud(IEntity node,int depth=0)
 {
  if(!node || depth>16)return null;
  GEP_GunStatus found=GEP_GunStatus.Cast(node.FindComponent(GEP_GunStatus));if(found)return found;
  for(IEntity child=node.GetChildren();child;child=child.GetSibling()){found=FindProbeHud(child,depth+1);if(found)return found;}
  return null;
 }
 void StopProbe(string reason)
 {
  if(ProbeControl)ProbeControl.SetWeaponADS(false);
  GetGame().GetInputManager().SetActionValue("TurretADSHold",0);
  ProbeDone=true;PrintFormat("GEP_APPROVED_RENDER705 terminal=NOT_TESTED case=%1 reason=%2 physicalInput=0",ProbeCase,reason);
 }
 override void EOnFrame(IEntity owner,float timeSlice)
 {
  HintShown=true;super.EOnFrame(owner,timeSlice);
  if(!GetGame().InPlayMode() || !PlayerReady || ProbeDone || Age<ReadyAt+4)return;
  PlayerController player=GetGame().GetPlayerController();if(!player){StopProbe("player_missing");return;}
  if(ProbePhase==0)
  {
   IEntity vehicle=Baseline;
   ProbeSeat=FindProbeSeat(vehicle);ProbeHud=FindProbeHud(vehicle);
   if(!ProbeSeat || !ProbeHud){StopProbe("seat_or_style_mismatch");return;}
   ProbeControl=TurretControllerComponent.Cast(ProbeSeat.GetController());if(!ProbeControl)ProbeControl=ProbeSeat.GetAttachedTurret();
   Actor=Spawn("{26A9756790131354}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Rifleman.et",vehicle.GetOrigin()+Vector(4,0,-6),0.1);
   if(!Actor || !ProbeControl){StopProbe("actor_or_controller_missing");return;}
   player.SetControlledEntity(Actor);
   CompartmentAccessComponent access=CompartmentAccessComponent.Cast(Actor.FindComponent(CompartmentAccessComponent));
   if(!access || !access.GetInVehicle(ProbeSeat.GetOwner(),ProbeSeat,true,-1,ECloseDoorAfterActions.INVALID,true)){StopProbe("native_boarding_rejected");return;}
   ProbePhase=1;ProbeAt=Age;PrintFormat("GEP_APPROVED_RENDER705 boarding=1 case=%1 style=%2 physicalInput=0",ProbeCase,705);return;
  }
  if(ProbePhase==1)
  {
   CompartmentAccessComponent access=CompartmentAccessComponent.Cast(Actor.FindComponent(CompartmentAccessComponent));
   bool seated=access && access.GetCompartment()==ProbeSeat && ProbeSeat.GetOccupant()==Actor && !access.IsGettingIn() && !access.IsGettingOut();
   if(!seated){if(Age-ProbeAt>10)StopProbe("native_boarding_timeout");return;}
   player.SetCharacterCameraRenderActive(true);if(player.GetPlayerCamera())GetGame().GetCameraManager().SetCamera(player.GetPlayerCamera());
   ProbeControl.SetWeaponADS(true);GetGame().GetInputManager().SetActionValue("TurretADSHold",1);
   bool ads=ProbeControl.GetCurrentSightsADS();bool visible=ProbeHud.InstrumentsVisible702 && ProbeHud.Root && ProbeHud.Root.IsVisible();
   if(!ads || !visible){if(Age-ProbeAt>12)StopProbe("native_ADS_or_widget_timeout");return;}
   ProbePhase=2;ProbeAt=Age;PrintFormat("GEP_APPROVED_RENDER705 native_ready=1 case=%1 style=%2 seated=%3 ADS=%4 visible=%5 physicalInput=0",ProbeCase,705,seated,ads,visible);return;
  }
  if(ProbePhase==2)
  {
   ProbeControl.SetWeaponADS(true);GetGame().GetInputManager().SetActionValue("TurretADSHold",1);
   if(Age-ProbeAt<2)return;
   bool current=player.GetControlledEntity()==Actor && ProbeSeat.GetOccupant()==Actor && ProbeControl.GetCurrentSightsADS() && ProbeHud.InstrumentsVisible702 && ProbeHud.Root && ProbeHud.Root.IsVisible();
   if(!current){StopProbe("scope_lost_before_capture");return;}
   PrintFormat("GEP_APPROVED_RENDER705 data group=%1 left=%2/%3 right=%4/%5 rangeState=%6 rangeMetres=%7 rangeAge=%8 magnification=%9",ProbeHud.Snapshot.Group,ProbeHud.Snapshot.Left.Count,ProbeHud.Snapshot.Left.Capacity,ProbeHud.Snapshot.Right.Count,ProbeHud.Snapshot.Right.Capacity,ProbeHud.Snapshot.RangeState676,ProbeHud.Snapshot.RangeMetres676,ProbeHud.Snapshot.RangeAge676,ProbeHud.Snapshot.Magnification);
   PrintFormat("GEP_APPROVED_RENDER705 widgets left=%1 right=%2 range=%3 zoom=%4",ProbeHud.LeftCount.GetText(),ProbeHud.RightCount.GetText(),ProbeHud.Range676.GetText(),ProbeHud.Zoom.GetText());
   bool shot=System.MakeScreenshot("$profile:approved705-render-case-"+ProbeCase.ToString());
   PrintFormat("GEP_APPROVED_RENDER705 capture=%1 case=%2 style=%3 range=%4 zoom=%5 physicalInput=0",shot,ProbeCase,705,ProbeHud.Range676.GetText(),ProbeHud.Zoom.GetText());
   ProbeControl.SetWeaponADS(false);GetGame().GetInputManager().SetActionValue("TurretADSHold",0);
   if(!shot){StopProbe("screenshot_failed");return;}
   ProbeCase++;ProbePhase=0;
   if(ProbeCase==1){ProbeDone=true;Print("GEP_APPROVED_RENDER705 terminal=CAPTURED cases=1 renderedInspection=PENDING physicalInput=0 accuracy=NOT_TESTED");}
  }
 }
}

// PRIVATE source-only adaptation of the compiled GEP_WheelDriveProbe.
// Existing actor/subject from DamageScenario683; no scene/world/controller fork.
// These are ordinary native actions and public engine callbacks, not physical R.
class GEP_CoreWheeledCourse20261022
{
 IEntity Subject,Actor;
 BaseCompartmentSlot Seat;
 CarControllerComponent Controller;
 VehicleWheeledSimulation Sim;
 CompartmentAccessComponent Access;
 string CaseId,Nonce,Phase,LastPhase,Reason;
 float Start,SampleAt,Thrust,Brake,Steer,Hand,EngineStart,EngineStop,Persistent;
 int Sample;
 protected bool m_bCourseFinishedQA;
 bool Completed,PublicStartIssued,PublicRestartIssued,PublicDefaultStopIssued,PublicStopIssued;
 EVehicleDrivingAssistanceMode OriginalAssistance;

 bool Begin(IEntity subject,IEntity actor,BaseCompartmentSlot seat,string caseId,string nonce)
 {
  Subject=subject;Actor=actor;Seat=seat;CaseId=caseId;Nonce=nonce;
  if(!subject || !actor || !seat || nonce=="")return false;
  Controller=CarControllerComponent.Cast(subject.FindComponent(CarControllerComponent));
  Sim=VehicleWheeledSimulation.Cast(subject.FindComponent(VehicleWheeledSimulation));
  Access=CompartmentAccessComponent.Cast(actor.FindComponent(CompartmentAccessComponent));
  if(!Controller || !Sim || !Access || !PilotCompartmentSlot.Cast(Seat))return false;
  if(Controller.GetPilotCompartmentSlot()!=Seat)return false;
  PlayerController pc=GetGame().GetPlayerController();
  if(!pc || pc.GetControlledEntity()!=Actor || !Ready())return false;
  pc.SetCharacterCameraRenderActive(true);
  if(pc.GetPlayerCamera())GetGame().GetCameraManager().SetCamera(pc.GetPlayerCamera());
  OriginalAssistance=BaseVehicleControllerComponent.GetDrivingAssistanceMode();
  Start=GetGame().GetWorld().GetWorldTime()/1000.0;
  PrintFormat("GEP_CORE_WHEEL_READY case=%1 nonce=%2 subject=%3 actor=%4 assistance=%5 carContext=%6 physical=0 settingsChanged=0",CaseId,Nonce,subject,actor,OriginalAssistance,GetGame().GetInputManager().IsContextActive("CarContext"));
  return true;
 }
 bool Ready()
 {
  if(!Subject || Subject.IsDeleted() || !Actor || Actor.IsDeleted() || !Access || !Seat)return false;
  CharacterControllerComponent life=CharacterControllerComponent.Cast(Actor.FindComponent(CharacterControllerComponent));
  PlayerController pc=GetGame().GetPlayerController();
  return pc && pc.GetControlledEntity()==Actor && life && life.GetLifeState()==ECharacterLifeState.ALIVE && !life.IsUnconscious() && Seat.GetOccupant()==Actor && Access.GetCompartment()==Seat && !Access.IsGettingIn() && !Access.IsGettingOut() && !Access.IsSwitchingSeatsAnim();
 }
 void Actions(float thrust,float brake,float steering,float hand,float start,float stop,float persistent)
 {
  InputManager input=GetGame().GetInputManager();if(!input)return;
  input.SetActionValue("CarThrust",thrust);input.SetActionValue("CarBrake",brake);
  input.SetActionValue("CarSteering",steering);input.SetActionValue("CarHandBrake",hand);
  input.SetActionValue("CarHandBrakePersistent",persistent);input.SetActionValue("CarTurbo",0);
  input.SetActionValue("VehicleEngineStart",start);input.SetActionValue("VehicleEngineStop",stop);
 }
 void Release(){Actions(0,0,0,0,0,0,0);}
 bool IsCourseFinishedQA(){return m_bCourseFinishedQA;}
 void Done(string reason,bool completed=false)
 {
  if(m_bCourseFinishedQA)return;
  m_bCourseFinishedQA=true;
  Completed=completed;
  Reason=reason;
  Release();
  PrintFormat("GEP_CORE_WHEEL_DONE case=%1 nonce=%2 samples=%3 completed=%4 reason=%5 physical=0 thresholdAcceptance=NOT_TESTED",CaseId,Nonce,Sample,Completed,Reason);
 }
 void BeforePhysics()
 {
  if(m_bCourseFinishedQA)return;
  if(!Ready()){Done("wheeled_real_driver_or_life_lost");return;}
  if(BaseVehicleControllerComponent.GetDrivingAssistanceMode()!=OriginalAssistance){Done("driving_assistance_changed_INVALID_COURSE");return;}
  vector position=Subject.GetOrigin(),basis[4];Subject.GetWorldTransform(basis);
  if(position[0]<575 || position[0]>655 || position[2]<2450 || position[2]>2780 || Math.AbsFloat(basis[2][1])>0.2)
  {Done("left_previously_qualified_flat_corridor_INVALID_COURSE");return;}
  float t=GetGame().GetWorld().GetWorldTime()/1000.0-Start;
  Thrust=0;Brake=0;Steer=0;Hand=0;EngineStart=0;EngineStop=0;Persistent=0;Phase="start";
  if(t<2)EngineStart=1;
  else if(t<4)Phase="native_start_observe";
  else if(t<4.1)
  {
   Phase="ordinary_public_start";
   if(!PublicStartIssued)
   {
    PublicStartIssued=true;bool onBeforeFallback=Sim.EngineIsOn();bool accepted=Controller.StartEngine();
    PrintFormat("GEP_CORE_WHEEL_PUBLIC_START case=%1 nonce=%2 accepted=%3 nativeActionSucceededBeforeFallback=%4 physicalR=NOT_TESTED",CaseId,Nonce,accepted,onBeforeFallback);
   }
  }
  else if(t<8)Phase="public_start_observe";
  else if(t<8.2){Phase="parking_release";if(Controller.GetPersistentHandBrake())Persistent=1;}
  else if(t<20.2){Phase="forward_default";Thrust=1;}
  else if(t<24.2){Phase="service_brake";if(Subject.GetPhysics().GetVelocity().Length()>0.15)Brake=1;}
  else if(t<26.2)Phase="coast";
  else if(t<34.2){Phase="reverse";Brake=1;}
  else if(t<36.2)Phase="coast_after_reverse";
  else if(t<41.2){Phase="left";Thrust=0.5;Steer=-0.5;}
  else if(t<46.2){Phase="right";Thrust=0.5;Steer=0.5;}
  else if(t<50.2){Phase="final_service_brake";if(Subject.GetPhysics().GetVelocity().Length()>0.15)Brake=1;}
  else if(t<50.4){Phase="native_stop_click";EngineStop=1;}
  else if(t<53.2)Phase="native_stop_observe";
  else if(t<55.2){Phase="native_restart";EngineStart=1;}
  else if(t<58.2)Phase="native_restart_observe";
  else if(t<58.3)
  {
   Phase="ordinary_public_restart";
   if(!PublicRestartIssued){PublicRestartIssued=true;bool restartAccepted=Controller.StartEngine();PrintFormat("GEP_CORE_WHEEL_PUBLIC_RESTART case=%1 nonce=%2 accepted=%3 physicalR=NOT_TESTED",CaseId,Nonce,restartAccepted);}
  }
  else if(t<62.2)Phase="public_restart_observe";
  else if(t<62.4)
  {
   Phase="ordinary_public_stop_animated";
   if(!PublicDefaultStopIssued){PublicDefaultStopIssued=true;Controller.StopEngine();PrintFormat("GEP_CORE_WHEEL_PUBLIC_STOP case=%1 nonce=%2 animated=1 force=0 physicalR=NOT_TESTED",CaseId,Nonce);}
  }
  else if(t<65.2)Phase="native_stop_second_observe";
  else if(t<65.3)
  {
   Phase="ordinary_unanimated_stop";
   if(!PublicStopIssued && Controller.IsEngineOn()){PublicStopIssued=true;Controller.StopEngine(false);PrintFormat("GEP_CORE_WHEEL_PUBLIC_STOP case=%1 nonce=%2 animated=0 force=0 physicalR=NOT_TESTED",CaseId,Nonce);}
  }
  else if(t<68.2)Phase="public_stop_observe";
  else {Done("wheeled_course_samples_collected_review_required",true);return;}
  Actions(Thrust,Brake,Steer,Hand,EngineStart,EngineStop,Persistent);
  if(Phase!=LastPhase){LastPhase=Phase;PrintFormat("GEP_CORE_WHEEL_PHASE case=%1 nonce=%2 phase=%3 seconds=%4",CaseId,Nonce,Phase,t);}
 }
 void AfterPhysics()
 {
  if(m_bCourseFinishedQA || !Ready())return;
  float now=GetGame().GetWorld().GetWorldTime()/1000.0;if(now-SampleAt<0.25)return;
  SampleAt=now;Sample++;
  vector basis[4];Subject.GetWorldTransform(basis);Physics body=Subject.GetPhysics();if(!body){Done("wheeled_physics_unavailable");return;}
  PrintFormat("GEP_CORE_WHEEL_STATE case=%1 nonce=%2 sample=%3 phase=%4 worldMs=%5 timeScale=%6 controllerOn=%7 simOn=%8 rpm=%9",CaseId,Nonce,Sample,Phase,GetGame().GetWorld().GetWorldTime(),GetGame().GetWorld().GetTimeScale(),Controller.IsEngineOn(),Sim.EngineIsOn(),Sim.EngineGetRPM());
  PrintFormat("GEP_CORE_WHEEL_POWER case=%1 nonce=%2 sample=%3 load=%4 gear=%5 future=%6 assistance=%7",CaseId,Nonce,Sample,Sim.EngineGetLoad(),Sim.GetGear(),Controller.GetFutureGear(),OriginalAssistance);
  PrintFormat("GEP_CORE_WHEEL_MOTION case=%1 nonce=%2 sample=%3 position=%4 forward=%5 velocity=%6 yawRate=%7 controlledSeatedAlive=%8",CaseId,Nonce,Sample,basis[3],basis[2],body.GetVelocity(),body.GetAngularVelocity()[1],Ready());
  PrintFormat("GEP_CORE_WHEEL_CONTROL case=%1 nonce=%2 sample=%3 actionThrust=%4 actionBrake=%5 actionSteer=%6 actionStart=%7 actionStop=%8",CaseId,Nonce,Sample,GetGame().GetInputManager().GetActionValue("CarThrust"),GetGame().GetInputManager().GetActionValue("CarBrake"),GetGame().GetInputManager().GetActionValue("CarSteering"),GetGame().GetInputManager().GetActionValue("VehicleEngineStart"),GetGame().GetInputManager().GetActionValue("VehicleEngineStop"));
  PrintFormat("GEP_CORE_WHEEL_READBACK case=%1 nonce=%2 sample=%3 throttle=%4 brake=%5 handbrake=%6 persistent=%7",CaseId,Nonce,Sample,Sim.GetThrottle(),Sim.GetBrake(),Sim.IsHandbrakeOn(),Controller.GetPersistentHandBrake());
  for(int wheel=0;wheel<Sim.WheelCount();wheel++)PrintFormat("GEP_CORE_WHEEL_CONTACT case=%1 nonce=%2 sample=%3 wheel=%4 contact=%5 rpm=%6",CaseId,Nonce,Sample,wheel,Sim.WheelHasContact(wheel),Sim.WheelGetRPM(wheel));
 }
}

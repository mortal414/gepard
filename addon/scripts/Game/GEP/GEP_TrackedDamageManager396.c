// Original adapter: installed native vehicle damage efficiency setters target
// VehicleWheeledSimulation only. Apply the same native hit-zone states to tracked simulation.
class GEP_TrackedDamageManager396Class : SCR_WheeledDamageManagerComponentClass {}
class GEP_TrackedDamageManager396 : SCR_WheeledDamageManagerComponent
{
 override void UpdateVehicleState()
 {
  super.UpdateVehicleState();
  VehicleTrackedSimulation tracked=VehicleTrackedSimulation.Cast(GetOwner().FindComponent(VehicleTrackedSimulation));
  if(!tracked)return;
  float engine=GetEngineEfficiency();float gearbox=GetGearboxEfficiency();
  if(!GetEngineFunctional())engine=0;
  if(!GetGearboxFunctional())gearbox=0;
  tracked.EngineSetPeakTorqueState(tracked.EngineGetPeakTorque()*engine);
  tracked.EngineSetPeakPowerState(tracked.EngineGetPeakPower()*engine);
  tracked.GearboxSetEfficiencyState(tracked.GearboxGetEfficiency()*gearbox);
  VehicleControllerComponent controller=VehicleControllerComponent.Cast(m_Controller);
  if(controller)controller.SetCanMove(engine>0&&gearbox>0);
 }
}

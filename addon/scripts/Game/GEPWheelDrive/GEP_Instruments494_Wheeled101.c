// Original cabin gauges; wheeled read-only data source.
class GEP_Instruments494_Wheeled101Class : GEP_Instruments494Class {}
class GEP_Instruments494_Wheeled101 : GEP_Instruments494
{
 override void EOnPostFrame(IEntity owner,float timeSlice)
 {

  if(!GetGame().InPlayMode()||System.IsConsoleApp())return;

  Clock+=timeSlice;
if(Clock<0.1)return;
Clock=0;

  if(!Ready)Init(owner);

  VehicleWheeledSimulation sim=VehicleWheeledSimulation.Cast(owner.FindComponent(VehicleWheeledSimulation));

  FuelManagerComponent fuel=FuelManagerComponent.Cast(owner.FindComponent(FuelManagerComponent));

  SCR_VehicleDamageManagerComponent damage=SCR_VehicleDamageManagerComponent.Cast(owner.FindComponent(SCR_VehicleDamageManagerComponent));

  bool alive=damage&&damage.GetState()!=EDamageState.DESTROYED;

  float maximum;
if(fuel)maximum=fuel.GetTotalMaxFuel();

  if(sim){
RPM=sim.EngineGetRPM();
Load=sim.EngineGetLoad();
}

  if(owner.GetPhysics()){
vector velocity=owner.GetPhysics().GetVelocity();
velocity[1]=0;
Speed=velocity.Length()*3.6;
}

  if(maximum>0)Fuel=fuel.GetTotalFuel()/maximum;

  float values[4];
values[0]=RPM/3000;
values[1]=Speed/80;
values[2]=Fuel;
values[3]=Load;

  vector positions[4]={
"0.29 1.175 2.529","0.435 1.27 2.529","0.65 1.27 2.529","0.765 1.175 2.529"}
;

  for(int i=0; i<4; i++)
  {

   bool valid=alive&&sim;
if(i==1)valid=alive&&owner.GetPhysics()!=null;
if(i==2)valid=alive&&maximum>0;

   Visible(Needles[i],valid);
if(valid)Pose(Needles[i],positions[i],135+270*Math.Clamp(values[i],0,1));

  }

  LitMask=0;
  bool engineOn=false;
  bool parking=false;
  bool engineFault=false;
  bool gearboxFault=false;
  bool turretFault=false;
  if(alive && sim)
  {
   engineOn=sim.EngineIsOn();
   parking=sim.IsHandbrakeOn();
  }
  if(alive)
  {
   engineFault=!damage.GetEngineFunctional();
   gearboxFault=!damage.GetGearboxFunctional();
  }
  GEP_RadarSystem radar=GEP_RadarSystem.Cast(owner.FindComponent(GEP_RadarSystem));
  if(alive && radar && radar.Rig)
  {
   GEP_TurretController410 turret=GEP_TurretController410.Cast(radar.Rig.FindComponent(TurretControllerComponent));
   if(turret) turretFault=turret.IsDriveFailed();
  }
  Lamp(0,1,engineOn);
  Lamp(1,2,parking);
  Lamp(2,4,alive && maximum>0 && Fuel<0.15);
  Lamp(3,8,engineFault);
  Lamp(4,16,gearboxFault);
  Lamp(5,32,turretFault);

 }
}

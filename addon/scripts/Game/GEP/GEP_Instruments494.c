// Original local presentation only. Reads native replicated simulation/damage;
// never writes input, engine, fuel, damage or network state.
class GEP_Instruments494Class : ScriptComponentClass {
}

class GEP_Instruments494 : ScriptComponent
{

 ref array<IEntity> Needles={
}
;

 ref array<IEntity> Lamps={
}
;

 ref array<ref Resource> Meshes={
}
;

 IEntity Labels;

 float Clock;


 bool Ready;

 float RPM;

 float Speed;

 float Fuel;

 float Load;

 int LitMask;

 override void OnPostInit(IEntity owner){
SetEventMask(owner,EntityEvent.POSTFRAME);
}

 IEntity CreateVisual(IEntity owner,string material,vector vertices[],float uv[],int indices[],int nv,int ni)
 {

  int vertexCounts[1];
vertexCounts[0]=nv;
int indexCounts[1];
indexCounts[0]=ni;
string materials[1];
materials[0]=material;

  Resource res=MeshObject.Create(1,vertexCounts,indexCounts,materials,0);

  if(!res)return null;

  MeshObject mesh=res.GetResource().ToMeshObject();
if(!mesh)return null;

  mesh.UpdateVerts(0,vertices,uv);
mesh.UpdateIndices(0,indices);
Meshes.Insert(res);

  IEntity entity=GetGame().SpawnEntity(GenericEntity,GetGame().GetWorld());
if(!entity)return null;

  entity.SetObject(mesh,"");
owner.AddChild(entity,-1);
entity.SetFlags(EntityFlags.VISIBLE,true);
return entity;

 }

 IEntity Quad(IEntity owner,string material,float left,float right,float bottom,float top)
 {

  vector vertices[4]={
Vector(left,bottom,0),Vector(right,bottom,0),Vector(right,top,0),Vector(left,top,0)}
;

  float uv[8]={
0,1,1,1,1,0,0,0}
;
int indices[12]={
0,2,1,0,3,2,0,1,2,0,2,3}
;

  return CreateVisual(owner,material,vertices,uv,indices,4,12);

 }

 void Pose(IEntity entity,vector position,float angle)
 {

  if(!entity)return;
float a=angle*Math.DEG2RAD;
vector matrix[4];

  matrix[0]=Vector(Math.Cos(a),-Math.Sin(a),0);
matrix[1]=Vector(Math.Sin(a),Math.Cos(a),0);
matrix[2]="0 0 1";
matrix[3]=position;

  entity.SetLocalTransform(matrix);
entity.Update();

 }

 void Visible(IEntity entity,bool visible)
 {

  if(!entity)return;
if(visible)entity.SetFlags(EntityFlags.VISIBLE,true);
else entity.ClearFlags(EntityFlags.VISIBLE,true);

 }

 void Init(IEntity owner)
 {

  float radius[4]={
0.06,0.068,0.052,0.04255}
;

  for(int i=0; i<4; i++)Needles.Insert(Quad(owner,"{D561AD08932E9011}Assets/Gepard/Finish/InstrumentNeedle.emat",-0.007,radius[i]*0.74,-0.0013,0.0013));

  for(int j=0; j<6; j++)
  {

   string material="{D561AD08932E9014}Assets/Gepard/Finish/IndicatorRed.emat";

   if(j==0)material="{D561AD08932E9012}Assets/Gepard/Finish/IndicatorGreen.emat";

   if(j==1||j==2)material="{D561AD08932E9013}Assets/Gepard/Finish/IndicatorAmber.emat";

   vector vertices[9];
float uv[18];
int indices[48];
vertices[0]=vector.Zero;
uv[0]=0.5;
uv[1]=0.5;

   for(int k=0; k<8; k++)
   {

    float a=k*45*Math.DEG2RAD;
vertices[k+1]=Vector(Math.Cos(a)*0.008,Math.Sin(a)*0.008,0);
uv[(k+1)*2]=0.5+Math.Cos(a)*0.5;
uv[(k+1)*2+1]=0.5-Math.Sin(a)*0.5;

    int next=(k+1)%8+1;
indices[k*6]=0;
indices[k*6+1]=next;
indices[k*6+2]=k+1;
indices[k*6+3]=0;
indices[k*6+4]=k+1;
indices[k*6+5]=next;

   }

   IEntity lamp=CreateVisual(owner,material,vertices,uv,indices,9,48);
Lamps.Insert(lamp);
Pose(lamp,Vector(0.22+j*0.11,1.115,2.534),0);
Visible(lamp,false);

  }

  Labels=Quad(owner,"{D561AD08932E9010}Assets/Gepard/Finish/DriverStatus.emat",-0.33,0.33,-0.018,0.018);
Pose(Labels,"0.495 1.08 2.529",0);

  Ready=true;

 }

 void Lamp(int index,int mask,bool on)
 {
  Visible(Lamps[index],on);
  if(on)
  {
   LitMask=LitMask+mask;
  }
 }
 override void EOnPostFrame(IEntity owner,float timeSlice)
 {

  if(!GetGame().InPlayMode()||System.IsConsoleApp())return;

  Clock+=timeSlice;
if(Clock<0.1)return;
Clock=0;

  if(!Ready)Init(owner);

  VehicleTrackedSimulation sim=VehicleTrackedSimulation.Cast(owner.FindComponent(VehicleTrackedSimulation));

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

 // Child lifetime follows native parent lifetime; no physical entities are deleted.
}


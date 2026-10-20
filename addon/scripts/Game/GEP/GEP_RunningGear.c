/*
Third-party attribution retained in the native runtime package.
The following notice applies only to the Kiichi tracked-guide contribution;
it does not license the other Gepard vehicle code, models, textures or assets.

MIT License

Copyright (c) 2026 Kiichi

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/
// Original no-slip visual animation; uniform shoe pitch on a closed measured path.
// Path samples and moving shoe spacing are independent. No physics overrides.
class GEP_GearBone
{
 TNodeId Bone;
 int Track;
 int Wheel;
 float Angle;
 float Distance;
 float PathDistance;
 vector Rest[4];
}
class GEP_RunningGearClass : ScriptComponentClass {}
class GEP_RunningGear : ScriptComponent
{
 ref array<ref GEP_GearBone> Wheels={};
 ref array<ref GEP_GearBone> Left={};
 ref array<ref GEP_GearBone> Right={};
 VehicleTrackedSimulation Sim;
 bool Ready;
 float Warmup;
 float LeftLength;
 float RightLength;
 float LeftPhase;
 float RightPhase;
 float PhaseSign=1;
 float LeftSpeed;
 float RightSpeed;
 float Frames;
 GEP_GearBone Bone(string name,int track,int wheel)
 {
  Animation Anim=GetOwner().GetAnimation();
  TNodeId id=Anim.GetBoneIndex(name);
  if(id==-1) { Print("GEP_GEAR_MISSING "+name); return null; }
  GEP_GearBone item=new GEP_GearBone();item.Bone=id;item.Track=track;item.Wheel=wheel;
  if(!Anim.GetBoneMatrix(id,item.Rest)) return null;
  return item;
 }
 float Prepare(array<ref GEP_GearBone> links)
 {
  float length;
  for(int i=0;i<links.Count();i++)
  {
   links[i].PathDistance=length;
   int next=(i+1)%links.Count();int prev=(i+links.Count()-1)%links.Count();
   vector tangent=links[next].Rest[3]-links[prev].Rest[3];
   links[i].Angle=Math.Atan2(tangent[1],tangent[2])*Math.RAD2DEG;
   length+=vector.Distance(links[i].Rest[3],links[next].Rest[3]);
  }
  for(int shoe=0;shoe<links.Count();shoe++)links[shoe].Distance=length*shoe/links.Count();
  return length;
 }
 void Init(IEntity owner)
 {
  Sim=VehicleTrackedSimulation.Cast(owner.FindComponent(VehicleTrackedSimulation));Animation Anim=owner.GetAnimation();
  if(!Sim || !Anim) return;
  for(int side=0;side<2;side++)
  {
   string letter="L";if(side==1) {letter="R";}
   for(int w=0;w<9;w++)
   {
    GEP_GearBone wheel=Bone("GEP_Track_"+letter+"_0"+w.ToString(),side,w);
    if(wheel) Wheels.Insert(wheel);
   }
   for(int roller=0;roller<4;roller++)
   {
    GEP_GearBone returnWheel=Bone("GEP_VisualReturn_"+letter+"_0"+roller.ToString(),side,9);
    if(returnWheel) Wheels.Insert(returnWheel);
   }
   for(int link=0;link<100;link++)
   {
    string digits=link.ToString();if(link<10) {digits="00"+digits;} else {digits="0"+digits;}
    GEP_GearBone shoe=Bone("GEP_Link_"+letter+"_"+digits,side,0);
    if(!shoe) continue;
    if(side==0) {Left.Insert(shoe);} else {Right.Insert(shoe);}
   }
  }
  if(Left.Count()==100 && Right.Count()==100)
  {
   LeftLength=Prepare(Left);RightLength=Prepare(Right);
   if(Left[1].Rest[3][2]>Left[0].Rest[3][2]) PhaseSign=-1;
   PrintFormat("GEP_RUNNING_GEAR_REST first=%1 next=%2 phaseSign=%3",Left[0].Rest[3],Left[1].Rest[3],PhaseSign);
  }
  Ready=true;
  PrintFormat("GEP_RUNNING_GEAR_READY wheels=%1 leftLinks=%2 rightLinks=%3 leftLength=%4",Wheels.Count(),Left.Count(),Right.Count(),LeftLength);
 }
 void LocalDelta(GEP_GearBone item,vector rotation[4],vector target,out vector pose[4])
 {
  for(int axis=0;axis<3;axis++)
  {
   vector basis=item.Rest[axis];
   vector rotated=rotation[0]*basis[0]+rotation[1]*basis[1]+rotation[2]*basis[2];
   pose[axis]=Vector(vector.Dot(item.Rest[0],rotated),vector.Dot(item.Rest[1],rotated),vector.Dot(item.Rest[2],rotated));
  }
  vector offset=target-item.Rest[3];
  pose[3]=Vector(vector.Dot(item.Rest[0],offset),vector.Dot(item.Rest[1],offset),vector.Dot(item.Rest[2],offset));
 }
 void Belt(IEntity owner,array<ref GEP_GearBone> links,float length,float phase)
 {
  Animation Anim=owner.GetAnimation();
  if(links.Count()!=100 || length<1) return;
  foreach(GEP_GearBone link:links)
  {
   float distance=link.Distance+phase;while(distance<0) {distance+=length;}
   while(distance>=length) {distance-=length;}
   int segment=0;
   for(int s=1;s<links.Count();s++)
   {
    if(links[s].PathDistance>distance) {break;}
    segment=s;
   }
   int next=(segment+1)%links.Count();float end=length;if(next!=0) {end=links[next].PathDistance;}
   float blend=(distance-links[segment].PathDistance)/(end-links[segment].PathDistance);
   vector target=links[segment].Rest[3]+(links[next].Rest[3]-links[segment].Rest[3])*blend;
   float delta=links[next].Angle-links[segment].Angle;while(delta>180) {delta-=360;}
   while(delta<-180) {delta+=360;}
   float angle=links[segment].Angle+delta*blend-link.Angle;
   vector rotation[4],pose[4];Math3D.AnglesToMatrix(Vector(0,angle,0),rotation);
   LocalDelta(link,rotation,target,pose);
   Anim.SetBoneMatrix(owner,link.Bone,pose);
  }
 }
 override void OnPostInit(IEntity owner){SetEventMask(owner,EntityEvent.POSTFRAME);}
 override void EOnPostFrame(IEntity owner,float timeSlice)
 {
  float dt=timeSlice;
  Animation Anim=owner.GetAnimation();
   // Native destruction can remove the animated mesh before this component.
   // Wrecks no longer animate their running gear.
   if(!GetGame().InPlayMode() || !Anim){return;}
   SCR_DamageManagerComponent damage=SCR_DamageManagerComponent.Cast(owner.FindComponent(SCR_DamageManagerComponent));
   if(damage && damage.GetState()==EDamageState.DESTROYED){return;}
  Warmup+=dt;if(Warmup<1) return;
  if(!Ready){Init(owner);return;}
  if(!Sim){return;}
  Physics body=owner.GetPhysics();
  // Native tracked RPM can rotate at rest and diverge greatly in reverse.
  // Visual no-slip approximation follows measured rigid-body motion at each track.
  // Does not apply forces, fix reverse propulsion, or model wheelspin.
  if(!body)return;
  vector basis[4];owner.GetWorldTransform(basis);
  vector velocity=body.GetVelocity();vector angular=body.GetAngularVelocity();
  float leftX=-1.46;float rightX=1.46;
  if(Left.Count()>0)leftX=Left[0].Rest[3][0];
  if(Right.Count()>0)rightX=Right[0].Rest[3][0];
  LeftSpeed=vector.Dot(velocity+Vector(angular[1]*basis[0][2]-angular[2]*basis[0][1],angular[2]*basis[0][0]-angular[0]*basis[0][2],angular[0]*basis[0][1]-angular[1]*basis[0][0])*leftX,basis[2]);
  RightSpeed=vector.Dot(velocity+Vector(angular[1]*basis[0][2]-angular[2]*basis[0][1],angular[2]*basis[0][0]-angular[0]*basis[0][2],angular[0]*basis[0][1]-angular[1]*basis[0][0])*rightX,basis[2]);
  if(Math.AbsFloat(LeftSpeed)<0.002)LeftSpeed=0;
  if(Math.AbsFloat(RightSpeed)<0.002)RightSpeed=0;
  bool moving=LeftSpeed!=0 || RightSpeed!=0;
  foreach(GEP_GearBone wheel:Wheels)
  {
   float speed=LeftSpeed;if(wheel.Track==1)speed=RightSpeed;
   float radius=Sim.WheelGetRadius(wheel.Track,wheel.Wheel);
   float rpm;if(radius>0.01)rpm=speed/(radius*0.104719755);
   wheel.Angle-=rpm*6*dt;while(wheel.Angle>180) {wheel.Angle-=360;}
   while(wheel.Angle<-180) {wheel.Angle+=360;}
   vector rotation[4],pose[4];Math3D.AnglesToMatrix(Vector(0,wheel.Angle,0),rotation);
   LocalDelta(wheel,rotation,wheel.Rest[3],pose);
   Anim.SetBoneMatrix(owner,wheel.Bone,pose);
  }
  if(LeftLength>1 && RightLength>1)
  {
   LeftPhase+=PhaseSign*LeftSpeed*dt;
   RightPhase+=PhaseSign*RightSpeed*dt;
   while(LeftPhase<0) {LeftPhase+=LeftLength;}
   while(LeftPhase>=LeftLength) {LeftPhase-=LeftLength;}
   while(RightPhase<0) {RightPhase+=RightLength;}
   while(RightPhase>=RightLength) {RightPhase-=RightLength;}
   Belt(owner,Left,LeftLength,LeftPhase);Belt(owner,Right,RightLength,RightPhase);
  }
 }
}



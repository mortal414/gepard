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
// Derivative of the original body-motion visual path; no physics writes.
// Fixed shoe path, no wheelspin/suspension conformation. Native ordering untested.
class GEP_RunningGear_Wheeled101Class : GEP_RunningGearClass {}
class GEP_RunningGear_Wheeled101 : GEP_RunningGear
{
 VehicleWheeledSimulation Wheeled;
 override void Init(IEntity owner)
 {
  Wheeled=VehicleWheeledSimulation.Cast(owner.FindComponent(VehicleWheeledSimulation));Animation Anim=owner.GetAnimation();
  if(!Wheeled || !Anim) return;
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
  if(!Wheeled){return;}
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
   float radius=0.35;if(wheel.Wheel==0 || wheel.Wheel==8)radius=0.43;else if(wheel.Wheel>=9)radius=0.15;
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

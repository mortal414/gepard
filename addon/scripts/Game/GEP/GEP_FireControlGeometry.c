// Original geometry only. No entity, input, weapon or networking writes.
class GEP_FireControlGeometry
{
 static bool Finite(float value)
 {
  return value>=-10000000 && value<=10000000;
 }
 static bool Direction(vector value)
 {
  float length=value.Length();return length>0.000001 && length<10000000;
 }
 static float ShortestDegrees(float difference)
 {
  while(difference>180)difference-=360;
  while(difference<-180)difference+=360;
  return difference;
 }
 static float PitchDegrees(vector forward)
 {
  return Math.Atan2(forward[1],Math.Sqrt(forward[0]*forward[0]+forward[2]*forward[2]))*Math.RAD2DEG;
 }
 static bool Errors(vector worldDirection,vector worldPose[4],out float rightDegrees,out float upDegrees)
 {
  rightDegrees=0;upDegrees=0;
  if(!Direction(worldDirection) || !Direction(worldPose[0]) || !Direction(worldPose[1]) || !Direction(worldPose[2]))return false;
  vector target=worldDirection.Normalized();
  float forward=vector.Dot(target,worldPose[2]);
  rightDegrees=Math.Atan2(vector.Dot(target,worldPose[0]),forward)*Math.RAD2DEG;
  upDegrees=Math.Atan2(vector.Dot(target,worldPose[1]),forward)*Math.RAD2DEG;
  return Finite(rightDegrees) && Finite(upDegrees);
 }
 // The actuator axes belong to the unrotated owner/mount, not an ADS camera
 // that may have horizon/roll correction. Hull yaw/pitch/roll all enter here.
 static bool MountErrors(vector worldLOS,vector mount[4],vector barrelLocal[4],float actualZero,out vector requiredWorld,out float rightDegrees,out float upDegrees)
 {
  requiredWorld=vector.Zero;rightDegrees=0;upDegrees=0;
  if(!Direction(worldLOS) || !Direction(barrelLocal[2]) || !Finite(actualZero))return false;
  vector target=worldLOS.Normalized();
  vector local=Vector(vector.Dot(target,mount[0]),vector.Dot(target,mount[1]),vector.Dot(target,mount[2]));
  float horizontal=Math.Sqrt(local[0]*local[0]+local[2]*local[2]);
  if(horizontal<0.0001)return false; // Undefined yaw at the local zenith.
  float yaw=Math.Atan2(local[0],local[2]);
  float pitch=Math.Atan2(local[1],horizontal)+actualZero*Math.DEG2RAD;
  vector desired=Vector(Math.Sin(yaw)*Math.Cos(pitch),Math.Sin(pitch),Math.Cos(yaw)*Math.Cos(pitch));
  requiredWorld=mount[0]*desired[0]+mount[1]*desired[1]+mount[2]*desired[2];
  vector barrel=barrelLocal[2];
  rightDegrees=ShortestDegrees(yaw*Math.RAD2DEG-Math.Atan2(barrel[0],barrel[2])*Math.RAD2DEG);
  upDegrees=pitch*Math.RAD2DEG-PitchDegrees(barrel);
  return Direction(requiredWorld) && Finite(rightDegrees) && Finite(upDegrees);
 }
 // Positive actualUpDegrees is measured physical upward barrel movement.
 // Counter-rotate by that observed amount, never by the requested correction.
 static void CounterPitch(vector pose[4],float actualUpDegrees)
 {
  float angle=actualUpDegrees*Math.DEG2RAD;
  vector oldUp=pose[1],oldForward=pose[2];
  pose[1]=oldUp*Math.Cos(angle)+oldForward*Math.Sin(angle);
  pose[2]=oldForward*Math.Cos(angle)-oldUp*Math.Sin(angle);
 }
 // Height from the native direct next-projectile API is added in WORLD Y,
 // following its native direct-fire callsite. This is not an altitude solver.
 static bool DirectAimPoint(vector muzzle,vector hit,float height,out vector aimPoint,out float slopeDegrees)
 {
  aimPoint=vector.Zero;slopeDegrees=0;
  vector delta=hit-muzzle;
  float horizontal=Math.Sqrt(delta[0]*delta[0]+delta[2]*delta[2]);
  if(!(horizontal>0.1 && horizontal<10000000) || !(height>=0 && height<10000000))return false;
  slopeDegrees=Math.Atan2(delta[1],horizontal)*Math.RAD2DEG;
  aimPoint=hit+Vector(0,height,0);
  return Finite(slopeDegrees) && Direction(aimPoint-muzzle);
 }
}

// Original press state. Losing scope cancels; release after a hold is consumed.
class GEP_FireControlPress
{
 protected bool m_Down;
 protected bool m_HoldFired;
 protected float m_Seconds;
 void Cancel(){m_Down=false;m_HoldFired=false;m_Seconds=0;}
 void Down(){if(m_Down)return;m_Down=true;m_HoldFired=false;m_Seconds=0;}
 bool Advance(float dt)
 {
  if(!m_Down || m_HoldFired)return false;
  m_Seconds+=Math.Max(0,dt);
  if(m_Seconds<0.4)return false;
  m_HoldFired=true;return true;
 }
 bool Up()
 {
  bool tap=m_Down && !m_HoldFired && m_Seconds<0.4;
  Cancel();return tap;
 }
}

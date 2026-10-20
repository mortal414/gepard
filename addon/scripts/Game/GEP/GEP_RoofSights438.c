// Original simplified optical head: gun orientation, fixed turret-roof origin.
// Native ADS, zoom, user controls and weapon transforms remain authoritative.
class GEP_RoofSights438Class : SCR_2DSightsComponentClass {}
class GEP_RoofSights438 : SCR_2DSightsComponent
{
 void UpdateRoofPoint()
 {
  IEntity owner=GetOwner();
  if(!owner||!owner.GetAnimation()) {return;}
  Animation animation=owner.GetAnimation();
  TNodeId elevation=animation.GetBoneIndex("GEP_GunElevation");
  TNodeId yaw=animation.GetBoneIndex("GEP_TurretYaw");
  vector aim[4],turretPose[4],offset[4],roof[4];
  if(!animation.GetBoneMatrix(elevation,aim)||!animation.GetBoneMatrix(yaw,turretPose)) {return;}
  Math3D.MatrixIdentity4(offset);offset[3]="0.43 1.44 0.78";
  Math3D.MatrixMultiply4(turretPose,offset,roof);aim[3]=roof[3];
  GEP_FireControl fireControl=GEP_FireControl.Cast(owner.FindComponent(GEP_FireControl));
  if(fireControl)fireControl.CorrectSight(aim);
  PointInfo point=GetPositionPointInfo();
  if(point)point.Set(owner,"",aim);
 }
 override void OnSightADSActivated()
 {
  UpdateRoofPoint();
  super.OnSightADSActivated();
 }
 override void OnSightADSPostFrame(IEntity owner,float timeSlice)
 {
  UpdateRoofPoint();
  super.OnSightADSPostFrame(owner,timeSlice);
 }
}

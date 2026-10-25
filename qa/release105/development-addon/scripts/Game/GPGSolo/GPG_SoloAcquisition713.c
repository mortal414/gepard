// Original QA-only observation and route-admission gate. No input/action dispatch,
// target/aim/servo/physics/damage setters or dependency implementation copied.
class GPG_SoloAcquisition713
{
 protected string m_LastReason713;
 protected float m_ReadySince713=-1;
 protected float m_LastGateMs713=-1;
 protected bool m_Started713;
 protected vector m_FrozenPoint713;
 protected int m_Sequence713;
 protected ref array<IEntity> m_Exclude713={};

 void Reset713()
 {
  m_LastReason713="";
  m_ReadySince713=-1;
  m_LastGateMs713=-1;
  m_Started713=false;
  m_FrozenPoint713=vector.Zero;
 }
 protected void ExcludeTree713(IEntity entity713)
 {
  if(!entity713)return;
  m_Exclude713.Insert(entity713);
  for(IEntity child713=entity713.GetChildren();child713;child713=child713.GetSibling())ExcludeTree713(child713);
 }
 protected bool Camera713(IEntity turret713,out vector camera713[4],out float fov713)
 {
  if(!turret713)return false;
  PlayerController player713=GetGame().GetPlayerController();
  CameraManager cameras713=GetGame().GetCameraManager();
  TurretControllerComponent control713=TurretControllerComponent.Cast(turret713.FindComponent(TurretControllerComponent));
  if(!player713 || !cameras713 || !player713.GetPlayerCamera() || cameras713.CurrentCamera()!=player713.GetPlayerCamera() || !control713)return false;
  BaseSightsComponent sights713=control713.GetCurrentSights();
  if(!control713.GetCurrentSightsADS() || !sights713 || !sights713.IsSightADSActive() || GetGame().GetMenuManager().IsAnyMenuOpen())return false;
  return control713.GetCurrentSightsCameraTransform(camera713,fov713) && camera713[2].Length()>0.9;
 }
 protected TraceParam Trace713(IEntity turret713,vector from713,vector to713)
 {
  TraceParam trace713=new TraceParam();
  trace713.Start=from713;
  trace713.End=to713;
  trace713.Flags=TraceFlags.WORLD|TraceFlags.ENTS;
  m_Exclude713.Clear();
  ExcludeTree713(turret713.GetRootParent());
  PlayerController player713=GetGame().GetPlayerController();
  if(player713 && player713.GetControlledEntity())m_Exclude713.Insert(player713.GetControlledEntity());
  trace713.ExcludeArray=m_Exclude713;
  GetGame().GetWorld().TraceMove(trace713,null);
  return trace713;
 }
 protected static void Bounds713(IEntity entity713,out vector minimum713,out vector maximum713)
 {
  entity713.GetWorldBounds(minimum713,maximum713);
  for(IEntity child713=entity713.GetChildren();child713;child713=child713.GetSibling())
  {
   vector childMin713,childMax713;
   Bounds713(child713,childMin713,childMax713);
   for(int axis713=0;axis713<3;axis713++)
   {
    minimum713[axis713]=Math.Min(minimum713[axis713],childMin713[axis713]);
    maximum713[axis713]=Math.Max(maximum713[axis713],childMax713[axis713]);
   }
  }
 }
 protected static bool Inside713(vector point713,vector minimum713,vector maximum713)
 {
  for(int axis713=0;axis713<3;axis713++)
  {
   if(point713[axis713]<minimum713[axis713]-0.3 || point713[axis713]>maximum713[axis713]+0.3)return false;
  }
  return true;
 }
 protected bool Check713(string station713,IEntity turret713,IEntity expected713,out vector point713,out string reason713)
 {
  reason713="waiting_for_actual_sight";
  point713=vector.Zero;
  vector camera713[4];float fov713;
  if(!expected713 || expected713.IsDeleted() || !Camera713(turret713,camera713,fov713))return false;
  GEP_NextStabilizationConsumer625 stab713=GEP_NextStabilizationConsumer625.Cast(turret713.FindComponent(GEP_NextStabilizationConsumer625));
  reason713="waiting_for_native_designation";
  if(!stab713 || !stab713.GetStabilizationEnabled() || !stab713.GetTrackingPoint(point713))return false;
  IEntity selected713=stab713.GetStabilizationTarget();
  if(station713=="target")
  {
   reason713="waiting_for_exact_truck_entity_lock";
   if(stab713.GetMode()!=WCS_EStabilizationMode.TRACK_ENTITY || !selected713 || selected713.GetRootParent()!=expected713)return false;
  }
  else if(station713=="motion")
  {
   reason713="waiting_for_designation_on_elevated_panel";
   if(stab713.GetMode()!=WCS_EStabilizationMode.TRACK_POSITION || selected713)return false;
   vector minimum713,maximum713;
   Bounds713(expected713,minimum713,maximum713);
   if(!Inside713(point713,minimum713,maximum713))return false;
  }
  else
  {
   reason713="unknown_station";
   return false;
  }
  IEntity hull713=turret713.GetRootParent();
  reason713="waiting_for_stationary_acquisition_subjects";
  if(!hull713 || !hull713.GetPhysics() || hull713.GetPhysics().GetVelocity().Length()>0.25 || !expected713.GetPhysics() || expected713.GetPhysics().GetVelocity().Length()>0.25)return false;
  vector direction713=point713-camera713[3];
  reason713="designation_too_close";
  if(direction713.Length()<5)return false;
  // Require the actual native first collision to belong to the designated subject.
  // A box/centre estimate or terrain hit never admits the route.
  TraceParam trace713=Trace713(turret713,camera713[3],point713+direction713.Normalized()*0.5);
  reason713="native_line_of_sight_does_not_hit_required_subject";
  IEntity traceRoot713;
  if(trace713.TraceEnt)traceRoot713=trace713.TraceEnt.GetRootParent();
  if(!traceRoot713 || traceRoot713!=expected713)
  {
   if(m_LastReason713!=reason713)PrintFormat("GPG_ACQUIRE713_COLLISION expected=%1 firstEntity=%2 firstRoot=%3 point=%4 source=%5 matched=0",GPG_SoloRead708.NativeID624(expected713),GPG_SoloRead708.NativeID624(trace713.TraceEnt),GPG_SoloRead708.NativeID624(traceRoot713),point713,camera713[3]);
   return false;
  }
  reason713="designation_verified";
  if(m_LastReason713!=reason713)PrintFormat("GPG_ACQUIRE713_COLLISION expected=%1 firstEntity=%2 firstRoot=%3 point=%4 source=%5 matched=1",GPG_SoloRead708.NativeID624(expected713),GPG_SoloRead708.NativeID624(trace713.TraceEnt),GPG_SoloRead708.NativeID624(traceRoot713),point713,camera713[3]);
  return true;
 }
 bool Ready713(string station713,IEntity turret713,IEntity expected713)
 {
  vector point713;string reason713;
  bool valid713=Check713(station713,turret713,expected713,point713,reason713);
  float now713=GetGame().GetWorld().GetWorldTime();
  // A missing/paused gate observation cannot count toward stable acquisition.
  if(m_LastGateMs713>=0 && now713-m_LastGateMs713>250)m_ReadySince713=-1;
  m_LastGateMs713=now713;
  if(!valid713)m_ReadySince713=-1;
  else if(m_ReadySince713<0)m_ReadySince713=now713;
  if(reason713!=m_LastReason713)
  {
   m_LastReason713=reason713;
   PrintFormat("GPG_ACQUIRE713_GATE station=%1 reason=%2 expected=%3 point=%4 AI_not_started_by_this_gate=1",station713,reason713,GPG_SoloRead708.NativeID624(expected713),point713);
  }
  return valid713 && now713-m_ReadySince713>=500;
 }
 void Started713(string station713,IEntity turret713,IEntity expected713)
 {
  GEP_NextStabilizationConsumer625 stab713=GEP_NextStabilizationConsumer625.Cast(turret713.FindComponent(GEP_NextStabilizationConsumer625));
  vector frozen713;
  if(!stab713 || !stab713.GetTrackingPoint(frozen713))return;
  m_FrozenPoint713=frozen713;
  m_Started713=true;
  PrintFormat("GPG_ACQUIRE713_NATIVE_AI_ADMITTED station=%1 expected=%2 frozenPoint=%3 designationStableSeconds=0.5 actualDriving=NOT_TESTED",station713,GPG_SoloRead708.NativeID624(expected713),m_FrozenPoint713);
 }
 void Observe713(string station713,IEntity turret713,IEntity expected713)
 {
  if(!turret713 || !expected713 || expected713.IsDeleted())return;
  m_Sequence713++;
  GEP_NextStabilizationConsumer625 stab713=GEP_NextStabilizationConsumer625.Cast(turret713.FindComponent(GEP_NextStabilizationConsumer625));
  if(!stab713)return;
  vector point713;
  bool pointValid713=stab713.GetTrackingPoint(point713);
  IEntity selected713=stab713.GetStabilizationTarget();
  vector camera713[4];float fov713;
  bool cameraValid713=Camera713(turret713,camera713,fov713);
  float error713=-1,horizontal713=-1,vertical713=-1;
  if(pointValid713 && cameraValid713)
  {
   vector direction713=point713-camera713[3];
   float length713=direction713.Length();
   if(length713>0.01)
   {
    error713=Math.Acos(Math.Clamp(vector.Dot(camera713[2].Normalized(),direction713.Normalized()),-1,1))*Math.RAD2DEG;
    float forward713=vector.Dot(camera713[2].Normalized(),direction713);
    horizontal713=Math.Atan2(vector.Dot(camera713[0].Normalized(),direction713),forward713)*Math.RAD2DEG;
    vertical713=Math.Atan2(vector.Dot(camera713[1].Normalized(),direction713),forward713)*Math.RAD2DEG;
   }
  }
  PrintFormat("GPG_ACQUIRE713_POINT|seq=%1|worldMs=%2|station=%3|mode=%4|target=%5|expected=%6|pointValid=%7|point=%8|routeAdmitted=%9",m_Sequence713,GetGame().GetWorld().GetWorldTime(),station713,stab713.GetMode(),GPG_SoloRead708.NativeID624(selected713),GPG_SoloRead708.NativeID624(expected713),pointValid713,point713,m_Started713);
  PrintFormat("GPG_ACQUIRE713_OPTIC|seq=%1|actualADS=%2|cameraOrigin=%3|cameraForward=%4|fov=%5|angularErrorDeg=%6|horizontalErrorDeg=%7|verticalErrorDeg=%8|measurement=native_current_sights_matrix_NOT_requested_aim",m_Sequence713,cameraValid713,camera713[3],camera713[2],fov713,error713,horizontal713,vertical713);
  IEntity hull713=turret713.GetRootParent();
  vector minimum713,maximum713;
  Bounds713(expected713,minimum713,maximum713);
  float frozenDelta713=-1;
  if(m_Started713 && pointValid713 && station713=="motion")frozenDelta713=vector.Distance(point713,m_FrozenPoint713);
  PrintFormat("GPG_ACQUIRE713_WORLD|seq=%1|hullPosition=%2|hullYPR=%3|expectedPosition=%4|expectedBoundsMin=%5|expectedBoundsMax=%6|fixedPointDeltaMetres=%7",m_Sequence713,hull713.GetOrigin(),hull713.GetYawPitchRoll(),expected713.GetOrigin(),minimum713,maximum713,frozenDelta713);
 }
}

// Original portable state model, mounted through GEP_RadarSystem.
// Its caller must resolve actual crew/ownership, LOS, target identity and sensor
// observations on the authority. No turret movement or firing lives here.
enum GEP_AcquisitionSource { NONE, RADAR_CONTACT, OPTICAL_DESIGNATION }
enum GEP_TrackState { IDLE, ACQUIRING, FRESH, COAST, LOST }
enum GEP_LossReason { NONE, STALE, RADAR_OFF, TARGET_DESTROYED, OUT_OF_LIMITS }

class GEP_EngagementModel
{
 bool SearchPowered;
 bool TrackingPowered;
 int SelectedContact=-1;
 int ActiveTarget=-1;
 GEP_AcquisitionSource AcquisitionSource;
 GEP_TrackState State;
 GEP_LossReason LossReason;
 vector MeasuredPosition;
 vector EstimatedVelocity;
 float LastMeasurement;
 float AcquisitionStarted;
 int MeasurementCount;
 bool Invalidated;
 bool TrackingObstructed;
 bool FollowEnabled; // default off; future authority adapter must own activation
 const float FRESH_SECONDS=2.6;
 const float LOSS_SECONDS=7;

 void SetSearchPower(bool powered)
 {
  SearchPowered=powered;
  if(!powered) SelectedContact=-1;
  // Tracking channel and active acquisition deliberately remain independent.
 }
 void SelectContact(int contact)
 {
  if(SearchPowered) SelectedContact=contact;
  // Selection never replaces ActiveTarget or starts follow.
 }
 void SetTrackingPower(bool powered,float now)
 {
  if(TrackingPowered==powered) return;
  TrackingPowered=powered;MeasurementCount=0;FollowEnabled=false;
  AcquisitionStarted=now;
  if(ActiveTarget<0 || Invalidated) return;
  State=GEP_TrackState.ACQUIRING;
  LossReason=GEP_LossReason.NONE;
  if(!powered) {State=GEP_TrackState.LOST;LossReason=GEP_LossReason.RADAR_OFF;}
 }
 void BeginAcquisition(int target,GEP_AcquisitionSource source,float now)
 {
  ActiveTarget=target;AcquisitionSource=source;State=GEP_TrackState.ACQUIRING;
  AcquisitionStarted=now;MeasurementCount=0;Invalidated=false;TrackingObstructed=false;
  LossReason=GEP_LossReason.NONE;FollowEnabled=false;
  MeasuredPosition=vector.Zero;EstimatedVelocity=vector.Zero;
 }
 bool AcquireSelected(bool confirmed,bool withinLimits,float contactAge,float now)
 {
  if(!SearchPowered || !TrackingPowered || SelectedContact<0 || !confirmed || !withinLimits || contactAge<0 || contactAge>FRESH_SECONDS) return false;
  BeginAcquisition(SelectedContact,GEP_AcquisitionSource.RADAR_CONTACT,now);
  return true;
 }
 bool AcquireOptical(int target,bool visible,bool eligible,bool withinLimits,bool requestTrackingRadar,float now)
 {
  if(target<0 || !visible || !eligible || !withinLimits) return false;
  // This explicit acquisition action may request tracking radar. It cannot
  // power search. Passive EO ranging/autofollow is not implemented here.
  if(requestTrackingRadar) SetTrackingPower(true,now);
  BeginAcquisition(target,GEP_AcquisitionSource.OPTICAL_DESIGNATION,now);
  if(!TrackingPowered) {State=GEP_TrackState.LOST;LossReason=GEP_LossReason.RADAR_OFF;}
  return true;
 }
 bool AcceptTrackingMeasurement(int target,vector position,vector velocity,float now,bool qualified)
 {
  if(!TrackingPowered || Invalidated || target!=ActiveTarget || target<0 || now<AcquisitionStarted || (MeasurementCount>0 && now<=LastMeasurement)) return false;
  if(!qualified)
  {
   TrackingObstructed=true;FollowEnabled=false;
   if(MeasurementCount>0) State=GEP_TrackState.COAST;
   return false;
  }
  TrackingObstructed=false;
  MeasuredPosition=position;EstimatedVelocity=velocity;LastMeasurement=now;
  MeasurementCount++;LossReason=GEP_LossReason.NONE;
  State=GEP_TrackState.ACQUIRING;
  if(MeasurementCount>=2) State=GEP_TrackState.FRESH;
  return true;
 }
 void Tick(float now)
 {
  if(ActiveTarget<0 || Invalidated || !TrackingPowered) return;
  float age=now-AcquisitionStarted;
  if(MeasurementCount>0) age=now-LastMeasurement;
  if(age>LOSS_SECONDS) {State=GEP_TrackState.LOST;LossReason=GEP_LossReason.STALE;FollowEnabled=false;return;}
  if(MeasurementCount>0 && (TrackingObstructed || age>FRESH_SECONDS)) {State=GEP_TrackState.COAST;FollowEnabled=false;return;}
  if(MeasurementCount>=2) State=GEP_TrackState.FRESH;
  else State=GEP_TrackState.ACQUIRING;
 }
 bool CanShowLead(float now)
 {
  return !Invalidated && !TrackingObstructed && TrackingPowered && ActiveTarget>=0 && State==GEP_TrackState.FRESH && MeasurementCount>=2 && now>=LastMeasurement && now-LastMeasurement<=FRESH_SECONDS;
 }
 vector Predict(float now)
 {
  return MeasuredPosition+EstimatedVelocity*Math.Clamp(now-LastMeasurement,0,LOSS_SECONDS);
 }
 void Invalidate(int target,GEP_LossReason reason)
 {
  if(target!=ActiveTarget) return;
  Invalidated=true;LossReason=reason;State=GEP_TrackState.LOST;FollowEnabled=false;
 }
 void ManualBreakout() {FollowEnabled=false;}
 void ClearActive()
 {
  ActiveTarget=-1;AcquisitionSource=GEP_AcquisitionSource.NONE;State=GEP_TrackState.IDLE;
  MeasurementCount=0;Invalidated=false;TrackingObstructed=false;FollowEnabled=false;LossReason=GEP_LossReason.NONE;
 }
}

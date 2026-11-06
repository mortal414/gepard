// Reuse the installed WCS held-point presenter under the selected custom HUD.
// Camera, ballistics, target selection and WCS state remain authoritative.
modded class GEP_GunStatus
{
 ref WCS_Armament_SightStabilizationMarker HeldMarker801;
 Widget HeldRoot801;
 Widget HeldPanel801;
 WCS_Armament_TurretStabilizationComponent HeldStabilization801;

 void ReleaseHeldPoint801()
 {
  if(HeldMarker801)HeldMarker801.Release();
  HeldMarker801=null;HeldRoot801=null;HeldPanel801=null;HeldStabilization801=null;
 }
 bool HasHeldPointGate801()
 {
  return Root && Root.IsVisible() && HeldRoot801 && HeldRoot801.IsVisible();
 }
 void UpdateHeldPoint801(IEntity owner)
 {
  if(HeldPanel801 && HeldPanel801!=Root)ReleaseHeldPoint801();
  if(HudStyle702!=5 || !Root || !Root.IsVisible() || !Snapshot.Crew || !Snapshot.ADS)
  {
   if(HeldRoot801)HeldRoot801.SetVisible(false);
   return;
  }
  WCS_Armament_TurretStabilizationComponent stabilization=WCS_Armament_TurretStabilizationComponent.Cast(owner.FindComponent(WCS_Armament_TurretStabilizationComponent));
  if(!stabilization){ReleaseHeldPoint801();return;}
  if(!HeldMarker801)
  {
   HeldMarker801=new WCS_Armament_SightStabilizationMarker();
   // 12% inset keeps the existing native gate inside this HUD's footer/bezel.
   HeldRoot801=HeldMarker801.Bind(Root,80,0.12);HeldPanel801=Root;
  }
  if(!HeldRoot801)return;
  HeldStabilization801=stabilization;
  HeldRoot801.SetColor(Color.FromInt(STUDY_ACTIVE740));
  HeldMarker801.Update(stabilization,GetGame().GetWorld());
  // Preserve Gepard's existing confirmed-entity cue gate; the native marker is
  // a designated point cue and supplies no bounding box or confidence claim.
  GEP_TrackingStabilization707 tracking=GEP_TrackingStabilization707.Cast(stabilization);
  if(stabilization.GetMode()==WCS_EStabilizationMode.TRACK_ENTITY && tracking && !tracking.HasConfirmedEntityLockCue715())HeldRoot801.SetVisible(false);
 }
 override void EnsureVisual680()
 {
  if(!Ready680())ReleaseHeldPoint801();
  super.EnsureVisual680();
 }
 override void EOnFrame(IEntity owner,float timeSlice)
 {
  super.EOnFrame(owner,timeSlice);
  UpdateHeldPoint801(owner);
 }
 override void OnDelete(IEntity owner)
 {
  ReleaseHeldPoint801();
  super.OnDelete(owner);
 }
}

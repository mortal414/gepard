// REPLACES GEP_OpticEvidence679.c; do not mount both overrides.
// Native optic state/callbacks remain owned by the normal native lifecycle.
// Only its stock visual root is masked for an exact legitimate Gepard ADS sight.
modded class SCR_2DOpticsDisplay
{

 protected void MaskGepardVisual680(SCR_2DOpticsComponent comp)
 {
  if(!m_wRoot || !GEP_Presentation680.ExactSight(comp))return;
  GEP_GunStatus hud=GEP_GunStatus.Cast(comp.GetOwner().FindComponent(GEP_GunStatus));
  if(!hud)return;
  hud.EnsureVisual680();
  if(!hud.HasVisual680())return;
  if(!hud.Ready680() || !hud.Snapshot.Presentation680)hud.Minimal680();
  // Allocation alone is insufficient: retain the native sight if neither our
  // complete instruments nor the independent fault reticle is actually visible.
  if(!hud.HasVisibleSight702())return;
  m_wRoot.SetVisible(false);
  // No forced SetVisible(true). The native Activate/Deactivate lifecycle owns
  // visibility for other sights and the real ADS-off path.
 }


 override protected void OnADSChange(notnull SCR_2DOpticsComponent comp,bool inADS)
 {
  super.OnADSChange(comp,inADS);
  // Parent true initializes data, handlers and animations normally; parent
  // false removes callbacks/deactivates normally. No replay/synthetic ADS.
  MaskGepardVisual680(comp);
 }

 override protected void DisplayUpdate(IEntity owner,float timeSlice)
 {
  super.DisplayUpdate(owner,timeSlice);
  MaskGepardVisual680(m_Optic);
 }
}

// Temporary WIP policy for the mounted turret's separate default hit zone only.
// Native chassis/crew/component damage and damage-pass rules remain unchanged.
// Re-enable the retained native implementation with the prefab attribute below.
class GEP_WIPTurretHitZone692 : SCR_DestructibleHitzone
{
 [Attribute("0",UIWidgets.CheckBox,"Enable independent turret destruction; disabled for the early WIP release")]
 protected bool m_bIndependentTurretDamageEnabled692;

 bool IndependentTurretDamageEnabled692(){return m_bIndependentTurretDamageEnabled692;}

 override float ComputeEffectiveDamage(notnull BaseDamageContext damageContext,bool isDOT)
 {
  if(m_bIndependentTurretDamageEnabled692 || damageContext.damageValue<=0 || damageContext.damageType==EDamageType.HEALING || damageContext.damageType==EDamageType.REGENERATION)
   return super.ComputeEffectiveDamage(damageContext,isDOT);

  IEntity owner=GetOwner();
  if(owner)
  {
   IEntity root=owner.GetRootParent();
   if(root && root!=owner)
   {
    SCR_DamageManagerComponent hull=SCR_DamageManagerComponent.Cast(root.FindComponent(SCR_DamageManagerComponent));
    // Shield this independent state only while attached to a living hull.
    // Whole-vehicle destruction continues through the ordinary native path.
    if(hull && hull.GetState()!=EDamageState.DESTROYED)return 0;
   }
  }
  return super.ComputeEffectiveDamage(damageContext,isDOT);
 }
}

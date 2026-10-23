// Original instance-scoped functional policy. Native registration, damage,
// repair callbacks and simulation efficiency application remain stock authority.
class GEP_NextDestroyedGearbox2301 : SCR_GearboxHitZone
{
 override float GetEfficiency()
 {
  if(GetDamageState()==EDamageState.DESTROYED)return 0;
  return super.GetEfficiency();
 }
}

// Native inventory description of actual mounted magazines. Read-only;
// unavailable feeds are distinct from a known empty feed or zero rounds.
[BaseContainerProps()]
class GEP_VehicleInventoryInfo533 : SCR_InventoryUIInfo
{
 string FeedDescription582(MuzzleComponent muzzle)
 {
  if(!muzzle)return "unavailable";
  BaseMagazineComponent magazine=muzzle.GetMagazine();
  if(!magazine)return "empty";
  string type="unknown belt";
  IEntity owner=magazine.GetOwner();
  if(owner && owner.GetPrefabData() && owner.GetPrefabData().GetPrefabName().Contains("GEP_35_HEIT"))type="HEI-T";
  return string.Format("%1, %2 rounds",type,muzzle.GetAmmoCount());
 }
 override string GetInventoryItemDescription(InventoryItemComponent item)
 {
  if(!item)return "";
  GEP_RadarSystem system=GEP_RadarSystem.Cast(item.GetOwner().FindComponent(GEP_RadarSystem));
  if(!system || !system.Rig)return "Gepard crew service equipment. Mounted feeds unavailable.";
  array<Managed> slots={};system.Rig.FindComponents(WeaponSlotComponent,slots);
  MuzzleComponent left;MuzzleComponent right;
  foreach(Managed value:slots)
  {
   WeaponSlotComponent slot=WeaponSlotComponent.Cast(value);
   if(slot.GetWeaponSlotIndex()==0)left=MuzzleComponent.Cast(slot.GetCurrentMuzzle());
   if(slot.GetWeaponSlotIndex()==1)right=MuzzleComponent.Cast(slot.GetCurrentMuzzle());
  }
  return string.Format("Mounted 35 mm feeds: left %1; right %2. Use the feed rearm actions near an ammunition supply vehicle. This storage holds crew service equipment.",FeedDescription582(left),FeedDescription582(right));
 }
}

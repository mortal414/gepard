// PRIVATE optional readback only. Does not register input, an entity or a timer.
// Coordinator invokes Snapshot before/after an existing native service action.
class GEP_CoreServiceReadback20261022
{
 static void Snapshot(string caseId,string nonce,string phase,IEntity subject,IEntity provider)
 {
  if(caseId=="" || nonce=="" || !subject || !provider || subject.IsDeleted() || provider.IsDeleted())return;
  SCR_ResourceComponent resources=SCR_ResourceComponent.FindResourceComponent(provider);
  SCR_ResourceContainer supplies;if(resources)supplies=resources.GetContainer(EResourceType.SUPPLIES);
  float value=-1,maximum=-1;
  if(supplies){value=supplies.GetResourceValue();maximum=supplies.GetMaxResourceValue();}
  PrintFormat("GEP_CORE_SERVICE_SOURCE case=%1 nonce=%2 phase=%3 subject=%4 provider=%5 container=%6 supplies=%7 maximum=%8 accountingAcceptance=NOT_TESTED",caseId,nonce,phase,subject,provider,supplies!=null,value,maximum);
  FuelManagerComponent fuel=FuelManagerComponent.Cast(subject.FindComponent(FuelManagerComponent));
  if(fuel)PrintFormat("GEP_CORE_SERVICE_FUEL case=%1 nonce=%2 phase=%3 total=%4 maximum=%5",caseId,nonce,phase,fuel.GetTotalFuel(),fuel.GetTotalMaxFuel());
  ReadServiceTree(caseId,nonce,phase,subject,0);
 }
 static void ReadServiceTree(string caseId,string nonce,string phase,IEntity owner,int depth)
 {
  if(!owner || owner.IsDeleted() || depth>16)return;
  SCR_DamageManagerComponent damage=SCR_DamageManagerComponent.Cast(owner.FindComponent(SCR_DamageManagerComponent));
  if(damage)
  {
   array<HitZone> zones={};damage.GetAllHitZonesInHierarchy(zones);
   foreach(HitZone zone:zones)if(zone)PrintFormat("GEP_CORE_SERVICE_ZONE case=%1 nonce=%2 phase=%3 owner=%4 zone=%5 health=%6 maximum=%7 state=%8",caseId,nonce,phase,owner,zone.GetName(),zone.GetHealth(),zone.GetMaxHealth(),zone.GetDamageState());
  }
  TurretControllerComponent control=TurretControllerComponent.Cast(owner.FindComponent(TurretControllerComponent));
  if(control && control.GetWeaponManager())
  {
   array<WeaponSlotComponent> slots={};control.GetWeaponManager().GetWeaponsSlots(slots);
   foreach(WeaponSlotComponent slot:slots)
   {
    MuzzleComponent muzzle=MuzzleComponent.Cast(slot.GetCurrentMuzzle());
    if(muzzle)PrintFormat("GEP_CORE_SERVICE_AMMO case=%1 nonce=%2 phase=%3 owner=%4 slot=%5 ammo=%6 magazine=%7",caseId,nonce,phase,owner,slot.GetWeaponSlotIndex(),muzzle.GetAmmoCount(),muzzle.GetMagazine());
   }
  }
  for(IEntity child=owner.GetChildren();child;child=child.GetSibling())ReadServiceTree(caseId,nonce,phase,child,depth+1);
 }
}

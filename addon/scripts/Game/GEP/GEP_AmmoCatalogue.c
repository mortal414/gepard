// Original catalog extension through the native initialization/merge path.
// Supply price is provisional game tuning; no rank or extra crew requirement.
modded class SCR_Faction
{
 override void Init(IEntity owner)
 {
  if(!SCR_Global.IsEditMode() && GetFactionKey()=="US" && m_aEntityCatalogs)
  {
   Resource resource=Resource.Load("{D561AD08932E5600}Configs/EntityCatalog/GEP_35_Ammo.conf");
   if(resource && resource.IsValid())
   {
    SCR_EntityCatalog catalog=SCR_EntityCatalog.Cast(BaseContainerTools.CreateInstanceFromContainer(resource.GetResource().ToBaseContainer()));
    if(catalog) {m_aEntityCatalogs.Insert(catalog);Print("GEP_AMMO_CATALOG_QUEUED nativeMerge=1 faction=US rank=PRIVATE supplyPrice=3");}
   }
  }
  if(!SCR_Global.IsEditMode() && GetFactionKey()=="US" && m_aEntityCatalogs)
  {
   Resource vehicleResource=Resource.Load("{D561AD08932E8510}Configs/EntityCatalog/GEP_Vehicle.conf");
   if(vehicleResource && vehicleResource.IsValid())
   {
    SCR_EntityCatalog vehicleCatalog=SCR_EntityCatalog.Cast(BaseContainerTools.CreateInstanceFromContainer(vehicleResource.GetResource().ToBaseContainer()));
    if(vehicleCatalog)
    {
     m_aEntityCatalogs.Insert(vehicleCatalog);
     Print("GEP_VEHICLE_CATALOG_QUEUED nativeMerge=1 faction=US provisionalCost=1800");
    }
   }
  }
  super.Init(owner);
 }
}

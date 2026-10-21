// Registers the separate appearance variant through the same native merge path
// as GEP_AmmoCatalogue. No vehicle behavior or livery menu is implemented here.
modded class SCR_Faction
{
 override void Init(IEntity owner)
 {
  if (!SCR_Global.IsEditMode() && GetFactionKey() == "US" && m_aEntityCatalogs)
  {
   Resource resource = Resource.Load("{FE15D6471463FE8C}Configs/EntityCatalog/GEP_CharcoalVehicle018.conf");
   if (resource && resource.IsValid())
   {
    SCR_EntityCatalog catalog = SCR_EntityCatalog.Cast(BaseContainerTools.CreateInstanceFromContainer(resource.GetResource().ToBaseContainer()));
    if (catalog)
     m_aEntityCatalogs.Insert(catalog);
   }
  }
  super.Init(owner);
 }
}

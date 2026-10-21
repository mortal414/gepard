# Game Master discoverability candidate 1.0.2

Status: source candidate, not a tested release. No 1.0.2 package or upload exists.

The 1.0.1 addon relies on US faction catalog injection and uses literal editable display names. Native EDIT mode builds its browser from the mode's registries; catalog availability varies by scenario. Its browser searches names through `WidgetManager.SearchLocalized`. The candidate follows Bohemia's NewCar sample: an independent exposed two-prefab registry referenced by a narrow `EditorModeEdit.et` override, plus localized editable names and the modded-content label. Existing catalog editor exposure is disabled to avoid duplicate cards when US catalogs are present; vehicle-spawner data remains intact.

Only five existing metadata files and eight new registry/localization resources change. All driving, input, optics, weapons, damage scripts, models, materials and textures remain identical to the accepted 1.0.1 source. The Workshop title, actual published description, driving-only changelog, license, visibility, preview and screenshots must remain unchanged; only the version becomes 1.0.2 after validation.

Run `python -B qa/gm-discoverability/validate_static.py` from the repository. It verifies source preservation and registration/localization references, not native inheritance, search behavior or placement. `native-plan.json` requires actual normal Game Master browsing and placement, stock positive control, exact-once cards with/without US catalog, and a packaged-only repeat before release.

Evidence sources:

- [Bohemia NewCar editor override](https://github.com/BohemiaInteractive/Arma-Reforger-Samples/blob/main/SampleMod_NewCar/Prefabs/Editor/Modes/EditorModeEdit.et)
- [Bohemia asset-browser integration](https://community.bistudio.com/wiki/Arma_Reforger:Asset_Browser_Mod_Integration)
- Installed stable 1.8.0.13 `SCR_PlacingEditorComponentClass.GetPrefabs`, `SCR_PlaceableEntitiesRegistryFromCatalog.Init`, `SCR_ContentBrowserEditorComponent.FilterEntries` and `SCR_EntityCatalogEditorData.IsValidInEditorMode`.

Do not merge this candidate into main or create a release snapshot until native/browser/package gates pass. Keep release/1.0.1 frozen. Deferred feature branches remain separate.

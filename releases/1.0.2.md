# Game Master discoverability 1.0.2

The unchanged 1.0.1 package returned zero cards for a normal Game Master browser search for Gepard. This patch localizes the editable names and adds an independent exposed registry using Bohemia's normal `EditorModeEdit.et` override pattern. Both root prefabs remain searchable and placeable without depending on faction-catalog editor registration. Existing catalog editor exposure is disabled to prevent duplicate cards; vehicle spawner data is preserved.

Native stable1.8.0.13 validation passed in stock Game Master Arland: actual browser UI, one default and one charcoal result, relevant filters, real card callbacks, server and owner placement acknowledgements and visible vehicles. The unchanged1.0.1 package reproduced zero search matches. The fresh package repeated the checks with production source unmounted. A private catalog-disabled fixture also found and placed both vehicles. Its code is excluded from the production PAK. Physical input, dedicated-server, JIP and console testing are not newly claimed.

Five authored metadata files and eight new registry/localization resources change; the native resource database is regenerated. All gameplay scripts and artwork are preserved from1.0.1, including accepted driving/input, optics, charcoal and deliberate damage behavior.

Run `python -B qa/gm-discoverability/validate_static.py` for source preservation/reference checks. This is not a substitute for native tests. See `validation-1.0.2.json` for tested scope and package hashes and `native-plan.json` for the recipes.

The existing Workshop item/title/description/changelog/license/visibility/media are retained. Version is1.0.2. The native publisher confirmed upload and processing on2026-10-21; latest public-site visibility remains separately unverified.

Source pattern: [Bohemia NewCar editor override](https://github.com/BohemiaInteractive/Arma-Reforger-Samples/blob/main/SampleMod_NewCar/Prefabs/Editor/Modes/EditorModeEdit.et), [Asset Browser Mod Integration](https://community.bistudio.com/wiki/Arma_Reforger:Asset_Browser_Mod_Integration), installed1.8.0.13 native browser/registry APIs. This addon has no harness dependency.

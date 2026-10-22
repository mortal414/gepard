# HUD control-01 GUI diagnostic comparison

**Proposal: the exact `GUI(E) Unknown class SCR_WidgetExportRuleRoot` offset 282 is a legacy stock-intro-layout match.** The seven WORLD(E) lines also match the previously documented baseline signatures. The recorded run and `native.errors` remain **FAIL**. No whitelist, collector, source, native session or prior report was changed.

Current console: `D:\ArmaProjects\Gepard-Consolidated-20261022\native-runs\hud-render-control-01\hud-render-startup\native-run\logs\console.log`; SHA256 `b4a8da3be11d703b96d8f0cafcddd343295569b308cb79595da976003313b73b`.

## Direct resource and ordering evidence

Current lines 110–111 load `{1C71B463B3B66BAB}UI/layouts/Menus/MainMenu/IntroSplashScreen.layout`; line 112 reports the unknown class at decimal 282 / hex 0x11a. Game-script compilation begins at line 113. This is a stock main-menu splash resource parsed before the later QA scene/player/ADS/data/capture stages, not an error nested in a new QA widget/component resource.

No `SCR_WidgetExportRuleRoot`, `IntroSplashScreen.layout` or its GUID occurs in the scanned Gepard production source, staged Gepard source or staged HUD QA `.c/.layout/.conf/.et/.gproj` files. The source-file hashes and counts are retained in `COMPARISON-HUD.json`. This negative source scan supports the direct stock-layout load evidence; it does not replace a native resource trace.

## Exact historical comparisons

Every listed match has the same GUI error class, decimal/hex offset and immediately enclosing stock layout GUID/name.

| Historical native console | Error line | SHA256 |
| --- | --- | --- |
| `D:\ArmaProjects\Gepard-GM-Hotfix-1.0.2-20261021\native-runs\gm-baseline-01\logs\console.log` | 108 | `cc480056a3113b58f4a27e69aa3239f777557f196d9f3fc4090573c361261561` |
| `D:\ArmaProjects\Gepard-GM-Hotfix-1.0.2-20261021\native-runs\gm-catalogless-01\logs\console.log` | 108 | `27f98b01a1e3dee35d77a41be3a6864d0cdf48792127558c420541487dcabe18` |
| `D:\ArmaProjects\Gepard-GM-Hotfix-1.0.2-20261021\native-runs\gm-package-01\logs\console.log` | 108 | `bf6f7c7b5b3375d128996c0e27bf6c887377ed153f89dbed611651a3fb1b1558` |
| `D:\ArmaProjects\Gepard-GM-Hotfix-1.0.2-20261021\native-runs\gm-source-01\logs\console.log` | 108 | `9e52ec221ffe82be915211f8bcd90a0cc51c0bef778d480da3623b654545094f` |
| `D:\ArmaProjects\Gepard-GM-Hotfix-1.0.2-20261021\native-runs\gm-source-02\logs\console.log` | 108 | `3cafebf54278108b8fb774e5323098d82d3d5c3d8c84c948ffddad80630d0038` |
| `D:\ArmaProjects\Gepard-GM-Hotfix-1.0.2-20261021\native-runs\gm-uploaded-01\logs\console.log` | 111 | `aae7d33e8124ac3173e6cd64f9a52252fa11d4b61d7fed3610c2dcb2bc01ad3c` |
| `D:\ArmaProjects\Gepard-EveningSprint-20261021\native-runs\new-optic-user-01\logs\console.log` | 105 | `7e2d2b34fa54aa3a5f5f0a94f03a5260d3def8a9fbf7aba95465ee77fb219508` |
| `D:\ArmaProjects\Gepard-EveningSprint-20261021\native-runs\optic-render-01\logs\console.log` | 105 | `13bc34341a217c7669fa8aef24a0c3038d7348cd1efc3707e85a1d155a8af332` |
| `D:\ArmaProjects\Gepard-DriveHud-Update-20261020\native-runs\human-drive-20261020T231546Z-9bc1e8\user-failure-console.log` | 105 | `8d28d58584d661aac5e644102d13033f23f37b54c7c620699f3bc3f9a2208da4` |
| `D:\ArmaProjects\Gepard-DriveHud-Update-20261020\native-runs\human-drive-20261020T231546Z-9bc1e8\logs\console.log` | 105 | `1445cb89dc6c4ffdd3fd090962a8cd7bab38633dfe8d4213f37ed2d3158ebc69` |
| `D:\ArmaProjects\Gepard-DriveHud-Update-20261020\native-runs\human-drive-20261020T232728Z-b4ddd0\logs\console.log` | 105 | `f406f9c34b6c8c1a1a685cfdec2583653b5480a4c9e428883baa52dbeba0ecdd` |
| `D:\ArmaProjects\Gepard-DriveHud-Update-20261020\native-runs\human-release101-20261021T005124Z-eb6dae\logs\console.log` | 105 | `50a4c295cba446a11bfd75d6d197bdaa21e81205c51857e12d94f87e475bad51` |
| `D:\ArmaProjects\Gepard-DriveHud-Update-20261020\native-runs\human-wheel-e-20261021T001123Z-641c47\logs\console.log` | 105 | `b535867d130a8ebe6c683a3dc6805da3376e43b89138c727da85cf97e3e85043` |
| `D:\ArmaProjects\Gepard-DriveHud-Update-20261020\native-runs\human-wheel-e-usability-01\logs\console.log` | 105 | `d69bb3cefc04437d0883722453ef15d161d6b6b6afa96f1fe148c9004369856a` |

Bounded scan: 25 logs, 14 exact matches. Compile-only logs without splash-layout loads are retained as non-applicable comparisons, not evidence that the issue disappeared. `HISTORY-SCAN.json` preserves every scanned path/hash and matching context.

Released source/package consoles show the layout at 106–107 and the exact error at 108. The exact uploaded 1.0.2 console shows layout 109–110 and error 111. Earlier evening optic-render/new-optic-user runs and historical human HUD/driving runs show layout 103–104 and error 105. Their contexts and hashes are retained independently.

## WORLD and other diagnostic boundaries

The current seven WORLD(E) lines are 166, 173–174 and 181–184. They match the prior seven signatures by keyword, offset and enclosing stock M151A2/BRDM2/BTR70 prefab GUID/name. The separate original report and its seals remain untouched.

Shutdown resource-leak diagnostics are preserved separately and not classified or waived by this report. Other classes, offsets, resources or channels must be treated as new/unexplained until individually supported. No general Unknown-class exception is justified.

## Acceptance boundary

The control run records fixture/world/ADS/initial-data/capture checks as PASS. Those remain the original limited claims; this source-only comparison adds no visual readability, physical interaction, transition or gameplay acceptance. Both strict native.errors and aggregate run FAIL remain unmodified. Legacy matching shows occurrence predates the new HUD QA component; it does not prove harmlessness.

Missing evidence: decoded stock-layout field payload and historical full stock PAK hashes. They are not fabricated or required to call this an exact retained native-signature match, but no full stock-binary identity claim is made.

Reproduce by running `scan_history.py`, then `compare_hud.py`, with Python 3.11. Both scripts write only inside this new C: subfolder.

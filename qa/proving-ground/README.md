# Private Gepard proving ground — source staging

Current status: **SOURCE CHECKED / NATIVE NOT TESTED**. No native launch or compile
is authorized during the user's runtime hold. This is a private local addon, not a
public release, acceptance claim or user demo.

One inherited Arland world combines the comparison apron, marked drive loop,
finite stock fuel/ammunition/repair trucks, target fan, expendable damage lane,
and a default-off hostile cell. The addon depends on one authoritative Gepard
addon plus native game data. It contains no copied art or stock asset payloads.

Project: `addon/GepardProvingGround.gproj`  
World: `{A76A420022000002}Worlds/GPG_Arland.ent`  
Addon GUID: `A76A420022000001`

Start at600/2600. Bay pads OLD-CAMO/OLD-CHARCOAL and NEW-CAMO/NEW-CHARCOAL are
20m apart. Two OLD slots are source-bound; NEW slots stay visibly unbound until
the art/functional coordinator supplies actual additive resource IDs. Exact
prefab hashes and the shared runtime closure are in checks/variant-lock.json.
All four cases share the approved optic/functional classes; OLD is an exterior
comparison, not a byte-identical released-code runtime.

Follow numbered drive gates north, clockwise and back via the service bay.
Station02 also owns one resettable ROUTE-SUBJECT at600/2660/yaw0, selected by
registry/variants.json routeSubjectSlot. Return it near the pad before resetting.
The audio interface reuses its50–150m straight comparison window and provides
5/30/100m horizontal side markers; actual moving emitter distances and the
surface must be measured natively before an audio comparison is admitted.
The target fan fires east at nominal horizontal100/300/500/800/1000m, spread by
angle to avoid nearer silhouettes covering farther ones. Native terrain/sightline
survey must establish useful actual visibility. The damage lane is800m south;
weapons fire farther south with the bay behind the firing line. The optional cell
is roughly1.7km from the bay, behind a stock blast-cover enclosure candidate.
It cannot arm without a qualification flag and terrain-ray backstop checks.

Read a station board or approach it for its label/instructions. Boards provide
explicit station-local reset actions. Subjects receive no automatic health,
fuel or ammunition updates. Resets replace only owned, empty, stationary fixtures
remaining near their pads and refuse when the player is too close. Taken/moved
weapons can therefore require a full scenario restart rather than silently taking
items away. Corpses are retained for observation. For death-only on-foot respawn,
the coordinator can use tools/command.ps1 with the actual isolated profile storage;
it cannot replace a live actor. The same helper provides read/hostile-stop commands.

To regenerate from source without opening any GUI:

```powershell
& 'C:\Users\marco\AppData\Local\Programs\Python\Python311\python.exe' tools/generate.py --check
& 'C:\Users\marco\AppData\Local\Programs\Python\Python311\python.exe' tools/generate.py --verify-lock
```

Update registry/variants.json sharedRuntime sourceRoot/commit only to the actual
final integration. Bind NEW slots with tools/bind_variant.py --slot --prefab --label,
using the actual coordinator-created GUID-prefixed ResourceName. Then regenerate.
This tool never copies assets, renames script classes or starts a renderer.

See RESEARCH-AND-REUSE.md, NATIVE-VALIDATION-PLAN.md and the shared
WORLD-TO-FUNCTIONAL-HANDOFF.json for exact integration gates. No packed trio or
native resource database is fabricated. The coordinator must build/load/survey,
drive, service, fire, damage and reset the final frozen source after user resume.

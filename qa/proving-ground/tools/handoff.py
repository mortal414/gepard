"""Write the readable shared manifest locally for staged coordinator handoff."""
import json
from pathlib import Path
from generate import ROOT, digest

target=Path('D:/ArmaProjects/Gepard-ProvingGround-20261022')
lock=json.loads((ROOT/'checks/variant-lock.json').read_text())
manifest=json.loads((ROOT/'checks/world-source-manifest.json').read_text())
data={
 'schema':1,
 'status':'SOURCE_STAGED_NATIVE_NOT_TESTED',
 'owner':'Proving-ground source worker via parent01a15259-930d-7752-b93e-02d417fdf0fc',
 'functionalCoordinator':'01a15f4d-277b-76d9-b72f-4dde42bec0f4',
 'runtimeHold':{'active':True,'requestUtc':'2026-10-22T00:47:00Z','reason':'User using PC; no game/Workbench/native launches or GUI/focus operations','resumeGate':'Explicit user resume plus coordinator runtime slot; released lease alone insufficient'},
 'sourceRoot':str(target/'source'),
 'branch':'private/proving-ground-20261022',
 'addonId':'A76A420022000001',
 'project':str(target/'source/addon/GepardProvingGround.gproj'),
 'world':'{A76A420022000002}Worlds/GPG_Arland.ent',
 'resourceGuidMap':{'world':'A76A420022000002','stationBoard':'A76A420022000003'},
 'worldSourceSha256':manifest['sha256'],
 'registrySha256':lock['registrySha256'],
 'variantLockSha256':digest(ROOT/'checks/variant-lock.json'),
 'dependencyIds':['58D0FB3206B6F859','D561AD08932E7B40'],
 'sharedRuntime':{k:v for k,v in lock['sharedRuntime'].items() if k!='files'},
 'slots':lock['slots'],
 'comparisonScope':lock['comparisonScope'],
 'injection':[
  'Keep the world as a separate private child addon. Do not copy it or unpublished art into public release exports.',
  'Mount exactly one Gepard GUID. Update source/registry/variants.json sharedRuntime.sourceRoot and commit to the final frozen coordinator addon.',
  'Keep OLD base and Charcoal018 resource graphs intact. Create NEW camouflage and charcoal as additive functional prefabs with unique actual native GUIDs/paths; preserve crew/turret/gun/track hierarchy.',
  'Run tools/bind_variant.py --slot NEW-CAMO --prefab <actual GUID-prefixed ResourceName> --label <exact features/commit> and repeat NEW-CHARCOAL. This checks metadata/existence; it does not copy assets.',
  'Run tools/generate.py --check. It pins every file in the shared addon and exact per-slot prefab hashes, generates deterministic layout/resources from the small registry, and preserves unbound labels.',
  'Run tools/generate.py --verify-lock before packaging and again after native validation; any changed source requires a new reviewed freeze.',
  'Do not mount two same-GUID releases or claim two code baselines in one process: all four exterior cases share the one loaded script implementation. Compare untouched released behavior in a separate run.'
 ],
 'entrypointCommands':{
  'cwd':str(target/'source'),
  'python':'C:/Users/marco/AppData/Local/Programs/Python/Python311/python.exe',
  'sourceCheck':'tools/generate.py --check',
  'freezeCheck':'tools/generate.py --verify-lock',
  'nativeLaunch':'NOT_AUTHORIZED while user hold active; use existing harness owned launcher and adapter after resume, explicit project/world and narrow addon roots'
 },
 'stations':[
  {'id':'bay','board':[584,0,2582],'expected':'2 OLD source-bound vehicles; 2 visibly unbound NEW pads until coordinator binding; per-pad resource/hash labels'},
  {'id':'drive','board':[586,0,2650],'expected':'10 numbered gates; 200m straight,50m reverse marks,turns/terrain spur/return; grade/surface remain survey candidates'},
  {'id':'service','board':[664,0,2574],'expected':'stock ammo/repair/tanker;1000 finite ammo/repair supplies at initialization/reset; two loose wrenches; normal vehicle timed support actions'},
  {'id':'range','board':[884,0,2432],'expected':'east-facing fan100/300/500/800/1000m horizontal; slant/terrain trace log; native pop-up stays down until explicit reset; silhouettes non-explosive'},
  {'id':'damage','board':[580,0,1865],'expected':'D1/D2 fresh expendable subject, M16/M249/M2 tripod/M72, real ammo and stock arsenal/ammo source; fire SOUTH away from bay800m north'},
  {'id':'hostile','board':[1770,0,1370],'expected':'OFF and qualification-locked; native two-sentry defend group; owned-agent35m boundary/player-leave/180s timeout and STOP; enclosure/backstop native gate pending'}
 ],
 'resetInputs':{
  'local':'Read/reset/arm/stop contextual actions on physical station boards; no automatic subject healing. Only SP RplMode.None.',
  'guard':'Reset refused if any owned station object occupied,moving,>18m from its pad or player within9m;15s cooldown. Only owned refs are deleted/replaced with incremented epoch.',
  'deathRespawn':'tools/command.ps1 -ProfileStorage <actual isolated $profile directory> -Operation respawn -Station bay|drive|service|range|damage|hostile; only after dead actor; no live/seat replacement',
  'remoteStop':'tools/command.ps1 -ProfileStorage <actual isolated $profile directory> -Operation stop -Station hostile',
  'fullReset':'End owned session normally and restart scenario with clean isolated profile through coordinator; no global/user process close.'
 },
 'audioRouteInterface':{
  'id':'GEP033_SAME_ROUTE',
  'project':str(target/'source/addon/GepardProvingGround.gproj'),
  'world':'{A76A420022000002}Worlds/GPG_Arland.ent',
  'worldAddonGuid':'A76A420022000001',
  'routeResourceOrMarkerIds':['GPG_Board_G01 START','GPG_Board_G02 50m REVERSE LIMIT','GPG_Board_G03 150m BRAKE','GPG_Board_G04 200m STOP / RIGHT','GPG_Board_G09 RETURN'],
  'spawnWorldPosition':{'x':600,'z':2660,'y':'native GetSurfaceY(600,2660)+0.7; record actual rested pose'},
  'spawnHeadingDegrees':0,
  'routePointPositions':[[600,0,2660],[600,0,2710],[600,0,2810],[600,0,2860]],
  'comparisonWindow':{'start':[600,0,2710],'end':[600,0,2810],'coastStop':[600,0,2810,600,0,2860]},
  'surfaceResourceIdentities':'NOT_TESTED; native moving wheel contact/material survey required before audio pair admission',
  'bumpOrTransitionMarkerId':'G07 TERRAIN SPUR / G09 RETURN candidates; NOT_TESTED actual transition',
  'observerMarkerIds':{'cabin':'ordinary player camera','exterior':'ordinary third-person','GPG_Board_AUDIO-5m':[605,0,2750],'GPG_Board_AUDIO-30m':[630,0,2750],'GPG_Board_AUDIO-100m':[700,0,2750]},
  'resetCaseInput':'Station02 drive board Reset this station replaces owned ROUTE-SUBJECT at600/2660/yaw0 if empty/stopped/returned near pad. registry/variants.json routeSubjectSlot selects the qualified same-art variant. Regenerate/freeze before each changed source. Keep all bay/support engines off.',
  'worldSourceManifest':str(target/'source/checks/world-source-manifest.json'),
  'acceptance':'SOURCE_INTERFACE_ONLY_NOT_TESTED; old loops gone, surface, powered ordinary driving, audio and actual distances need native qualification'
 },
 'optionalWCS':{'coreIncluded':False,'status':'SOURCE_CONTRACT_ONLY_NOT_ADMITTED','stationResource':'{ECB8E313DFBD3415}Prefabs/Compositions/Slotted/SlotFlatSmall/VehicleMaintenance_S_US_01.et','finiteFallbackSource':'{D07C2D4844753F76}Prefabs/Props/Military/SupplyBox/SupplyCrate/SupplyCrate_01/SupplyCrate_01_storage.et','reference':str(target/'FUNCTIONAL-WCS-REARM-CONTRACT.md'),'candidatePlacement':{'station':[720,0,2590],'source':[724,0,2590],'subject':[717,0,2590],'y':'survey actual native surface; no authored terrain changes'},'gates':'Separate optional dependency addon only after native smoke qualification. Require actual WCS_REARM consumer, enabled SUPPLIES,finite aggregate and ammo+1/supplies-1 per real eligible action. No WCS symbols or unconditional dependency in core.'},
 'staticEvidence':{'checks':'checks/source-checks.json','resourceProvenance':'checks/native-resource-provenance.json','resourcesReadVerified':21,'worldManifest':'checks/world-source-manifest.json','variantLock':'checks/variant-lock.json','coverage':'checks/coverage.json','result':'PASS_SOURCE_CHECKS_ONLY; no native behavior inference'},
 'gates':['NEW-CAMO and NEW-CHARCOAL actual resource IDs','Final shared runtime closure','User explicit native resume and coordinator slot','Native all-configuration compile/RDB/load/package trio','Existing addon adapter extended/qualified for GPG world and station protocol; old Damage683 binding is not drop-in','Terrain and muzzle sightline survey; actual grade and wheel material','Visible ordinary drive/reverse/brake/turn/slope','Native refuel/rearm/repair and provider supply cost','Damage and post-hit usability on D1/D2 without autoheal','Reset rejection, replacement, no orphan loops and death respawn','Optional hostile enclosure/behavior/projectile containment before unlock','Human acceptance pending'],
 'nativeStatus':{'compile':'NOT_TESTED','load':'NOT_TESTED','drive':'NOT_TESTED','service':'NOT_TESTED','damage':'NOT_TESTED','reset':'NOT_TESTED','hostiles':'LOCKED_NOT_TESTED','acceptance':'PENDING_USER'},
 'noPublicPublish':True,
 'blocker':{'action':'Native compile/load/survey/drive/service/damage/reset validation','target':'Private proving-ground world and final consolidated build','reason':'Explicit user runtime hold; worker source-only and no runtime slot','state':'PENDING_NOT_ATTEMPTED','alreadyCompleted':'Research,deterministic source world,station registry,support/weapon references,hash identity and offline checks','nextOwner':'Functional coordinator after explicit user resume'},
 'documents':['README.md','RESEARCH-AND-REUSE.md','NATIVE-VALIDATION-PLAN.md']
}
(ROOT/'WORLD-TO-FUNCTIONAL-HANDOFF.json').write_text(json.dumps(data,indent=2)+'\n')
print(json.dumps({k:data[k] for k in ('status','project','world','worldSourceSha256','registrySha256')}))

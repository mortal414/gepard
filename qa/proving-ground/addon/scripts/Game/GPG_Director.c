// Minimal original orchestration around stock native resources and existing
// Gepard manual-range spawn/support APIs. This file never changes health, ammo,
// input, velocity or drivetrain values on a test subject.
[BaseContainerProps(category:"Respawn")]
class GPG_ManualSpawnLogic : SCR_AutoSpawnLogic
{
 override protected void DoSpawn_S(int playerId)
 {
  if(RplSession.Mode()==RplMode.None)return;
  super.DoSpawn_S(playerId);
 }
}
class GPG_Owned
{
 string Id; string Station; ResourceName Prefab; vector Pad; float Yaw;
 IEntity Entity; int Epoch;
}
class GPG_Station
{
 string Id; string Title; string Instructions; vector Position; vector Board;
}
class GPG_DirectorClass : GenericEntityClass {}
class GPG_Director : GenericEntity
{
 static GPG_Director Instance;
 ref array<ref GPG_Owned> Objects={};
 ref array<ref GPG_Station> Stations={};
 IEntity Actor;
 SCR_AIGroup Hostiles;
 AIWaypoint Defend;
 ref array<IEntity> HostileActors={};
 float Clock; float Poll; float HostileStart; float LastReset=-100;
 bool Staged; bool CameraReady; bool Ready; bool Failed;
 string LastCommand; string LastStation;

 void GPG_Director(IEntitySource src,IEntity parent)
 {
  Instance=this;SetEventMask(EntityEvent.FRAME);
 }
 vector Ground(vector p,float clearance=0.1)
 {
  p[1]=GetGame().GetWorld().GetSurfaceY(p[0],p[2])+clearance;return p;
 }
 IEntity Spawn(ResourceName prefab,vector pad,float yaw=0,float clearance=0.15)
 {
  Resource resource=Resource.Load(prefab);
  if(!resource || !resource.IsValid()){Failed=true;PrintFormat("GPG_RESOURCE_FAIL prefab=%1",prefab);return null;}
  EntitySpawnParams params=new EntitySpawnParams();params.TransformMode=ETransformMode.WORLD;
  params.Transform[3]=Ground(pad,clearance);
  IEntity entity=GetGame().SpawnEntityPrefab(resource,GetGame().GetWorld(),params);
  if(!entity){Failed=true;PrintFormat("GPG_SPAWN_FAIL prefab=%1",prefab);return null;}
  entity.SetYawPitchRoll(Vector(yaw,0,0));entity.Update();return entity;
 }
 GPG_Owned Add(string id,string station,ResourceName prefab,vector pad,float yaw=0)
 {
  GPG_Owned item=new GPG_Owned();item.Id=id;item.Station=station;item.Prefab=prefab;item.Pad=pad;item.Yaw=yaw;
  float clearance=0.15;
  if(station=="bay" || station=="drive" || station=="damage")clearance=0.7;
  item.Entity=Spawn(prefab,pad,yaw,clearance);Objects.Insert(item);
  if(item.Entity)item.Entity.SetName("GPG_"+id);
  PrintFormat("GPG_OBJECT id=%1 station=%2 epoch=%3 prefab=%4 origin=%5",id,station,item.Epoch,prefab,Ground(pad));
  return item;
 }
 void Station(string id,string title,string instructions,vector position,vector board)
 {
  GPG_Station station=new GPG_Station();station.Id=id;station.Title=title;station.Instructions=instructions;station.Position=position;station.Board=board;Stations.Insert(station);
  IEntity marker=Spawn("{A76A420022000003}Prefabs/GPG_StationBoard.et",board,180);
  if(marker)
  {
   marker.SetName("GPG_Board_"+id);
   GPG_BoardComponent component=GPG_BoardComponent.Cast(marker.FindComponent(GPG_BoardComponent));if(component)component.Station=id;
  }
  PrintFormat("GPG_STATION id=%1 board=%2 title=%3",id,Ground(board),title);
 }
 void Gate(string id,vector pad)
 {
  Spawn(GPG_Resources.Cone(),pad+Vector(-5,0,0));Spawn(GPG_Resources.Cone(),pad+Vector(5,0,0));
  PrintFormat("GPG_GATE id=%1 origin=%2 gradeDegrees=%3 surface=NOT_TESTED",id,Ground(pad),Grade(pad));
 }
 float Grade(vector pad)
 {
  float y1=GetGame().GetWorld().GetSurfaceY(pad[0],pad[2]-2);
  float y2=GetGame().GetWorld().GetSurfaceY(pad[0],pad[2]+2);
  return Math.Atan2(y2-y1,4)*Math.RAD2DEG;
 }
 void Target(string id,vector pad,ResourceName prefab,float nominal)
 {
  GPG_Owned object=Add(id,"range",prefab,pad,90);
  SCR_FiringRangeTarget pop;if(object.Entity)pop=SCR_FiringRangeTarget.Cast(object.Entity);
  if(pop){pop.SetAutoResetTarget(false);pop.SetState(ETargetState.TARGET_UP);}
  vector line=Ground("900 0 2450",2);vector aim=Ground(pad,1.2);
  TraceParam trace=new TraceParam();trace.Start=line;trace.End=aim;trace.Flags=TraceFlags.WORLD;
  float fraction=GetGame().GetWorld().TraceMove(trace,null);
  PrintFormat("GPG_SURVEY target=%1 nominalHorizontal=%2 slant=%3 terrainTraceFraction=%4 line=%5 aim=%6 nativeVisibilityReview=NOT_TESTED",id,nominal,vector.Distance(line,aim),fraction,line,aim);
 }
 void Supplies(IEntity provider)
 {
  if(!provider)return;
  SCR_ResourceComponent resource=SCR_ResourceComponent.FindResourceComponent(provider);SCR_ResourceContainer supplies;
  if(resource)supplies=resource.GetContainer(EResourceType.SUPPLIES);
  if(supplies){supplies.SetMaxResourceValue(1000,true);supplies.SetResourceValue(1000,true);PrintFormat("GPG_STOCK source=%1 supplies=%2 initializationOnly=1",provider.GetName(),supplies.GetResourceValue());}
 }
 void Service()
 {
  Supplies(Add("Ammo","service",GPG_Resources.AmmoTruck(),"687 0 2590").Entity);
  Supplies(Add("Repair","service",GPG_Resources.RepairTruck(),"680 0 2580").Entity);
  Add("Fuel","service",GPG_Resources.FuelTruck(),"673 0 2590");
  Add("Wrench1","service",GPG_Resources.Wrench(),"678 0 2576");
  Add("Wrench2","service",GPG_Resources.Wrench(),"680 0 2576");
 }
 void Damage()
 {
  ResourceName subject=GPG_Layout.DamagePrefab();
  if(subject!="")
  {
   Add("D1","damage",subject,"600 0 1800",180);
   Add("D2","damage",subject,"620 0 1800",180);
  }
  Add("M16","weapons",GPG_Resources.Rifle(),"586 0 1840");
  Add("M249","weapons",GPG_Resources.Machinegun(),"588 0 1840");
  Add("M2","weapons",GPG_Resources.HeavyMachinegun(),"610 0 1840",180);
  Add("Arsenal","weapons",GPG_Resources.Arsenal(),"580 0 1840");
  Supplies(Add("DamageAmmo","weapons",GPG_Resources.AmmoTruck(),"570 0 1840",180).Entity);
  for(int n=0;n<4;n++)
  {
   Add("M72_"+n.ToString(),"weapons",GPG_Resources.Launcher(),Vector(585+n,0,1843));
   Add("RifleAmmo_"+n.ToString(),"weapons",GPG_Resources.RifleAmmo(),Vector(585+n,0,1845));
   Add("MGAmmo_"+n.ToString(),"weapons",GPG_Resources.MachinegunAmmo(),Vector(585+n,0,1847));
  }
  Gate("DAMAGE FIRE SOUTH", "610 0 1840");
 }
 bool Occupied(IEntity entity)
 {
  if(!entity)return false;
  BaseCompartmentManagerComponent manager=BaseCompartmentManagerComponent.Cast(entity.FindComponent(BaseCompartmentManagerComponent));
  if(manager)
  {
   array<BaseCompartmentSlot> seats={};manager.GetCompartments(seats);
   foreach(BaseCompartmentSlot seat:seats)if(seat && seat.GetOccupant())return true;
  }
  for(IEntity child=entity.GetChildren();child;child=child.GetSibling())if(Occupied(child))return true;
  return false;
 }
 bool CanReset(GPG_Owned item)
 {
  if(!item.Entity)return true;
  if(Occupied(item.Entity))return false;
  if(vector.Distance(item.Entity.GetOrigin(),Ground(item.Pad))>18)return false;
  if(item.Entity.GetPhysics() && item.Entity.GetPhysics().GetVelocity().Length()>0.25)return false;
  // Keep the user outside the actual deletion/spawn footprint.
  if(Actor && vector.Distance(Actor.GetOrigin(),item.Entity.GetOrigin())<9)return false;
  return true;
 }
 void Reset(string station)
 {
  if(Clock-LastReset<15){Hint("Wait 15 seconds between explicit resets.","RESET PAUSED");return;}
  if(station=="hostile"){StopHostiles("explicit-reset");return;}
  bool weapons=station=="damage";
  foreach(GPG_Owned check:Objects)
  {
   if(check.Station!=station && !(weapons && check.Station=="weapons"))continue;
   if(!CanReset(check)){Hint("Reset refused: an owned object is occupied, moving, moved off its pad, or too close to you. Park it, exit and stand back. A normal scenario restart is the full reset.","RESET REFUSED");return;}
  }
  int count;
  foreach(GPG_Owned item:Objects)
  {
   if(item.Station!=station && !(weapons && item.Station=="weapons"))continue;
   if(item.Entity)SCR_EntityHelper.DeleteEntityAndChildren(item.Entity);
   float clearance=0.15;if(item.Station=="bay" || item.Station=="drive" || item.Station=="damage")clearance=0.7;
   item.Entity=Spawn(item.Prefab,item.Pad,item.Yaw,clearance);item.Epoch++;
   if(item.Entity)item.Entity.SetName("GPG_"+item.Id);
   SCR_FiringRangeTarget pop;if(item.Entity)pop=SCR_FiringRangeTarget.Cast(item.Entity);
   if(pop){pop.SetAutoResetTarget(false);pop.SetState(ETargetState.TARGET_UP);}
   if(station=="service" || item.Id=="DamageAmmo")Supplies(item.Entity);
   PrintFormat("GPG_RESET id=%1 epoch=%2 prefab=%3 fullReplacement=1",item.Id,item.Epoch,item.Prefab);count++;
  }
  LastReset=Clock;Hint("Fresh owned objects: "+count.ToString()+". Record the new epoch before another trial.","EXPLICIT RESET");
 }
 void Hint(string text,string title){SCR_HintManagerComponent.ShowCustomHint(text,title,16,true);}
 void Execute(string station,string operation,IEntity user)
 {
  if(RplSession.Mode()!=RplMode.None)return;
  if(operation=="reset"){Reset(station);return;}
  if(operation=="stop"){StopHostiles("explicit-stop");return;}
  if(operation=="arm"){ArmHostiles(user);return;}
  if(operation=="respawn"){Respawn(station);return;}
  foreach(GPG_Station entry:Stations)if(entry.Id==station)Hint(entry.Instructions+GPG_Layout.Identities(station),entry.Title);
 }
 void Respawn(string station)
 {
  // Never replace a live player or force a seat exit.
  CharacterControllerComponent life;if(Actor)life=CharacterControllerComponent.Cast(Actor.FindComponent(CharacterControllerComponent));
  if(Actor && (!life || life.GetLifeState()==ECharacterLifeState.ALIVE)){Hint("Respawn is only available after death. Live actors and vehicles are preserved.","RESPAWN REFUSED");return;}
  vector pad="584 0 2582";
  foreach(GPG_Station entry:Stations)if(entry.Id==station)pad=entry.Board;
  PlayerController pc=GetGame().GetPlayerController();if(!pc)return;
  IEntity fresh=Spawn(GPG_Resources.Actor(),pad+Vector(3,0,0));if(!fresh)return;
  Actor=fresh;pc.SetControlledEntity(Actor);CameraReady=false;
  PrintFormat("GPG_RESPAWN station=%1 replacementAfterDeath=1",station);
 }
 bool Backstop()
 {
  // Terrain must interrupt each chest-height ray to all comparison pads.
  vector source=Ground("1800 0 1400",2);
  array<vector> pads={"600 0 2600","620 0 2600","600 0 2622","620 0 2622"};
  foreach(vector pad:pads)
  {
   TraceParam trace=new TraceParam();trace.Start=source;trace.End=Ground(pad,2);trace.Flags=TraceFlags.WORLD;
   float fraction=GetGame().GetWorld().TraceMove(trace,null);
   if(!(fraction>0 && fraction<0.9))return false;
  }
  return true;
 }
 void ArmHostiles(IEntity user)
 {
  if(Hostiles)return;
  if(!user || vector.Distance(user.GetOrigin(),Ground("1800 0 1400"))>60)return;
  if(!FileIO.FileExists("$profile:GPG-hostiles-qualified.flag") || !Backstop())
  {Hint("Encounter remains OFF. Coordinator must qualify enclosure/backstop/native AI first. Terrain block to every bay pad is also required.","HOSTILES LOCKED");return;}
  Hostiles=SCR_AIGroup.Cast(Spawn(GPG_Resources.EnemyGroup(),"1800 0 1400"));
  Defend=AIWaypoint.Cast(Spawn(GPG_Resources.DefendWaypoint(),"1800 0 1400"));
  if(!Hostiles || !Defend){StopHostiles("spawn-failure");return;}
  Hostiles.AddWaypoint(Defend);HostileStart=Clock;
  Print("GPG_HOSTILE start=1 durationSeconds=180 radiusMetres=35 projectileContainment=NOT_TESTED");
 }
 void StopHostiles(string reason)
 {
  if(Hostiles)
  {
   array<AIAgent> agents={};Hostiles.GetAgents(agents);
   foreach(AIAgent agent:agents)
   {
    IEntity entity=agent.GetControlledEntity();if(entity && HostileActors.Find(entity)<0)HostileActors.Insert(entity);
   }
  }
  foreach(IEntity owned:HostileActors)if(owned)SCR_EntityHelper.DeleteEntityAndChildren(owned);
  HostileActors.Clear();
  if(Hostiles)SCR_EntityHelper.DeleteEntityAndChildren(Hostiles);Hostiles=null;
  if(Defend)SCR_EntityHelper.DeleteEntityAndChildren(Defend);Defend=null;
  PrintFormat("GPG_HOSTILE stop=1 reason=%1",reason);
 }
 void GuardHostiles()
 {
  if(!Hostiles)return;
  if(Clock-HostileStart>180 || !Actor || vector.Distance(Actor.GetOrigin(),Ground("1800 0 1400"))>100){StopHostiles("expiry-or-player-left");return;}
  array<AIAgent> agents={};Hostiles.GetAgents(agents);
  foreach(AIAgent agent:agents)
  {
   IEntity entity=agent.GetControlledEntity();if(!entity)continue;
   if(HostileActors.Find(entity)<0)HostileActors.Insert(entity);
   if(vector.Distance(entity.GetOrigin(),Ground("1800 0 1400"))>35){StopHostiles("boundary");return;}
  }
 }
 void PollCommands()
 {
  FileHandle command=FileIO.OpenFile("$profile:GPG-command.txt",FileMode.READ);if(!command)return;
  string line;command.ReadLine(line);command.Close();if(line=="" || line==LastCommand)return;
  LastCommand=line;array<string> parts={};line.Split("|",parts,true);
  if(parts.Count()!=3)return;
  // Sequence|operation|station, local tooling alternative to physical boards.
  // Only dead-player respawn is remotely callable; live resets stay local.
  if(parts[1]=="respawn")Respawn(parts[2]);
  else if(parts[1]=="stop")StopHostiles("profile-stop");
  else if(parts[1]=="read")Execute(parts[2],"read",Actor);
  PrintFormat("GPG_COMMAND seq=%1 operation=%2 station=%3",parts[0],parts[1],parts[2]);
 }
 override void EOnFrame(IEntity owner,float timeSlice)
 {
  if(!GetGame().InPlayMode() || RplSession.Mode()!=RplMode.None)return;
  Clock+=timeSlice;
  if(!Staged && Clock>1)
  {
   Staged=true;
   SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());if(mode)mode.SetResourceTypeEnabled(true,EResourceType.SUPPLIES);
   GPG_Layout.Build(this);Service();Damage();
   Actor=Spawn(GPG_Resources.Actor(),"584 0 2586");
   PlayerController controller=GetGame().GetPlayerController();if(controller && Actor)controller.SetControlledEntity(Actor);
   PrintFormat("GPG_STAGED registry=%1 source=%2 nativeCompiled=NOT_TESTED",GPG_Layout.RegistryHash(),GPG_Layout.RuntimeHash());
  }
  if(!CameraReady && Clock>5 && Actor)
  {
   PlayerController pc=GetGame().GetPlayerController();if(pc && pc.GetControlledEntity()==Actor && pc.GetPlayerCamera())
   {
    SCR_RespawnSystemComponent respawn=SCR_RespawnSystemComponent.GetInstance();if(respawn)respawn.DestroyLoadingPlaceholder();
    pc.SetCharacterCameraRenderActive(true);GetGame().GetCameraManager().SetCamera(pc.GetPlayerCamera());CameraReady=true;
    Hint("Read the numbered station boards. Comparison bay -> driving loop -> service -> east-facing target lane. Damage lane is 800 m SOUTH, firing farther SOUTH. Optional hostile cell is OFF. This source build still requires native survey and validation.","PRIVATE GEPARD PROVING GROUND");
   }
  }
  if(!Ready && CameraReady){Ready=true;PrintFormat("GPG_READY staged=%1 resourceFailure=%2 objects=%3 nativeBehaviorAcceptance=NOT_TESTED",Staged,Failed,Objects.Count());}
  Poll+=timeSlice;if(Poll<0.25)return;Poll=0;GuardHostiles();PollCommands();
  if(!Actor)return;
  GPG_Station nearest;float nearestDistance=14;
  foreach(GPG_Station station:Stations)
  {
   float distance=vector.Distance(Actor.GetOrigin(),Ground(station.Board));
   if(distance<nearestDistance){nearest=station;nearestDistance=distance;}
  }
  if(nearest && LastStation!=nearest.Id){LastStation=nearest.Id;Execute(nearest.Id,"read",Actor);}
  if(!nearest)LastStation="";
 }
 void ~GPG_Director(){if(Instance==this)Instance=null;}
}

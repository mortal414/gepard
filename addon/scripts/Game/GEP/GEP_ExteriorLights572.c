// Original tracked lamp bridge584. No propulsion/input/simulation writes.
// Reliable edges plus sequenced unreliable heartbeats; native light replication owns output.
class GEP_ExteriorLights572Class : ScriptComponentClass {}
class GEP_ExteriorLights572 : ScriptComponent
{
 protected VehicleTrackedSimulation m_Simulation;
 protected BaseLightManagerComponent m_Lights;
 protected float m_Timer;
 protected float m_SendTimer;
 protected bool m_SentPedal;
 protected bool m_HadLocalDriver584;
 protected bool m_PedalActive;
 protected float m_PedalAge=100;
 protected IEntity m_ReportedDriver;
 void DriverPedal584(IEntity user,bool pressed)
 {
  GEP_DriverStation station=GEP_DriverStation.Cast(GetOwner().FindComponent(GEP_DriverStation));
  if(!station || !station.IsDriver(user))return;
  m_ReportedDriver=user;m_PedalActive=pressed;m_PedalAge=0;
 }
 void ReadLocalPedal584(float dt)
 {
  if(System.IsConsoleApp())return;
  SCR_PlayerController player=SCR_PlayerController.Cast(GetGame().GetPlayerController());if(!player)return;
  GEP_DriverStation station=GEP_DriverStation.Cast(GetOwner().FindComponent(GEP_DriverStation));
  bool driver=station && station.IsDriver(player.GetControlledEntity());
  if(!driver){m_HadLocalDriver584=false;m_SentPedal=false;m_SendTimer=0;return;}
  RplComponent replication=RplComponent.Cast(GetOwner().FindComponent(RplComponent));if(!replication)return;
  bool pressed=!GetGame().GetMenuManager().IsAnyMenuOpen() && GetGame().GetInputManager().GetActionValue("TrackedBrake")>0.05;
  bool edge=!m_HadLocalDriver584 || pressed!=m_SentPedal;
  m_HadLocalDriver584=true;m_SendTimer+=dt;
  if(edge || m_SendTimer>=0.2)
  {
   m_SendTimer=0;m_SentPedal=pressed;player.GEP_RequestBrake584(pressed,replication.Id(),edge);
  }
 }
 override void OnPostInit(IEntity owner){SetEventMask(owner,EntityEvent.POSTFRAME);}
 override void EOnPostFrame(IEntity owner,float timeSlice)
 {
  float dt=timeSlice;
  if(!GetGame().InPlayMode())return;
  ReadLocalPedal584(dt);
  RplComponent replication=RplComponent.Cast(owner.FindComponent(RplComponent));
  if(RplSession.Mode()==RplMode.Client || (replication && replication.IsProxy()))return;
  m_PedalAge+=dt;m_Timer+=dt;if(m_Timer<0.05)return;m_Timer=0;
  if(!m_Simulation)m_Simulation=VehicleTrackedSimulation.Cast(owner.FindComponent(VehicleTrackedSimulation));
  if(!m_Lights)m_Lights=BaseLightManagerComponent.Cast(owner.FindComponent(BaseLightManagerComponent));
  if(!m_Simulation || !m_Lights)return;
  GEP_DriverStation station=GEP_DriverStation.Cast(owner.FindComponent(GEP_DriverStation));
  SCR_DamageManagerComponent damage=SCR_DamageManagerComponent.Cast(owner.FindComponent(SCR_DamageManagerComponent));
  bool alive=!damage || damage.GetState()!=EDamageState.DESTROYED;
  // Road presence lamps follow the existing headlight request; per-side lamp
  // functionality remains native and independent of the request.
  bool rear=alive && m_Lights.GetLightsState(ELightType.Head);
  if(m_Lights.GetLightsState(ELightType.Rear)!=rear)m_Lights.SetLightsState(ELightType.Rear,rear);
  // Native tracked automatic hold can report6 with no pedal. Keep actual pedal
  // intent distinct, validate current pilot, and expire lost reports in0.6s.
  bool brake=alive && m_PedalActive && m_PedalAge<0.6 && station && station.IsDriver(m_ReportedDriver) && m_Simulation.GetBrake()>0.05;
  bool reverse=alive && m_Simulation.GetGear()==0 && m_Simulation.GetClutch()>0.2 && m_Simulation.EngineGetRPM()>300;
  if(m_Lights.GetLightsState(ELightType.Brake)!=brake)m_Lights.SetLightsState(ELightType.Brake,brake);
  if(m_Lights.GetLightsState(ELightType.Reverse)!=reverse)m_Lights.SetLightsState(ELightType.Reverse,reverse);
 }
}

modded class SCR_PlayerController
{
 protected int m_GepBrakeSequence584;
 protected int m_GepAcceptedBrakeSequence584=-1;
 void GEP_RequestBrake584(bool pressed,RplId expectedVehicle,bool edge)
 {
  m_GepBrakeSequence584++;
  if(RplSession.Mode()==RplMode.None){GEP_AcceptBrake584(pressed,expectedVehicle,m_GepBrakeSequence584);return;}
  if(edge)Rpc(GEP_ServerBrakeEdge584,pressed,expectedVehicle,m_GepBrakeSequence584);
  else Rpc(GEP_ServerBrakeHeartbeat584,pressed,expectedVehicle,m_GepBrakeSequence584);
 }
 void GEP_AcceptBrake584(bool pressed,RplId expectedVehicle,int sequence)
 {
  if(sequence<=m_GepAcceptedBrakeSequence584)return;
  // Consume sequence even when the lease is no longer valid: an older true
  // report must not become valid after leaving/re-entering or changing vehicles.
  m_GepAcceptedBrakeSequence584=sequence;
  IEntity user=GetControlledEntity();if(!user)return;
  IEntity vehicle=CompartmentAccessComponent.GetVehicleIn(user);if(!vehicle)return;
  RplComponent replication=RplComponent.Cast(vehicle.FindComponent(RplComponent));
  if(!replication || replication.Id()!=expectedVehicle || replication.IsProxy())return;
  GEP_ExteriorLights572 lights=GEP_ExteriorLights572.Cast(vehicle.FindComponent(GEP_ExteriorLights572));
  if(lights)lights.DriverPedal584(user,pressed);
 }
 [RplRpc(RplChannel.Reliable,RplRcver.Server)]
 void GEP_ServerBrakeEdge584(bool pressed,RplId expectedVehicle,int sequence){GEP_AcceptBrake584(pressed,expectedVehicle,sequence);}
 [RplRpc(RplChannel.Unreliable,RplRcver.Server)]
 void GEP_ServerBrakeHeartbeat584(bool pressed,RplId expectedVehicle,int sequence){GEP_AcceptBrake584(pressed,expectedVehicle,sequence);}
}

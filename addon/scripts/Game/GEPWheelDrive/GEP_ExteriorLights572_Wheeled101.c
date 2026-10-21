// Original light intent bridge adapted to native car inputs/simulation.
// Existing inherited RPC authority/driver sequencing remains in the dependency.
class GEP_ExteriorLights572_Wheeled101Class : GEP_ExteriorLights572Class {}
class GEP_ExteriorLights572_Wheeled101 : GEP_ExteriorLights572
{
 VehicleWheeledSimulation Wheeled;
 override void ReadLocalPedal584(float dt)
 {
  if(System.IsConsoleApp())return;
  SCR_PlayerController player=SCR_PlayerController.Cast(GetGame().GetPlayerController());if(!player)return;
  GEP_DriverStation station=GEP_DriverStation.Cast(GetOwner().FindComponent(GEP_DriverStation));
  bool driver=station && station.IsDriver(player.GetControlledEntity());
  if(!driver){m_HadLocalDriver584=false;m_SentPedal=false;m_SendTimer=0;return;}
  RplComponent replication=RplComponent.Cast(GetOwner().FindComponent(RplComponent));if(!replication)return;
  bool pressed=!GetGame().GetMenuManager().IsAnyMenuOpen() && GetGame().GetInputManager().GetActionValue("CarBrake")>0.05;
  bool edge=!m_HadLocalDriver584 || pressed!=m_SentPedal;
  m_HadLocalDriver584=true;m_SendTimer+=dt;
  if(edge || m_SendTimer>=0.2)
  {
   m_SendTimer=0;m_SentPedal=pressed;player.GEP_RequestBrake584(pressed,replication.Id(),edge);
  }
 }
 override void EOnPostFrame(IEntity owner,float timeSlice)
 {
  float dt=timeSlice;
  if(!GetGame().InPlayMode())return;
  ReadLocalPedal584(dt);
  RplComponent replication=RplComponent.Cast(owner.FindComponent(RplComponent));
  if(RplSession.Mode()==RplMode.Client || (replication && replication.IsProxy()))return;
  m_PedalAge+=dt;m_Timer+=dt;if(m_Timer<0.05)return;m_Timer=0;
  if(!Wheeled)Wheeled=VehicleWheeledSimulation.Cast(owner.FindComponent(VehicleWheeledSimulation));
  if(!m_Lights)m_Lights=BaseLightManagerComponent.Cast(owner.FindComponent(BaseLightManagerComponent));
  if(!Wheeled || !m_Lights)return;
  GEP_DriverStation station=GEP_DriverStation.Cast(owner.FindComponent(GEP_DriverStation));
  SCR_DamageManagerComponent damage=SCR_DamageManagerComponent.Cast(owner.FindComponent(SCR_DamageManagerComponent));
  bool alive=!damage || damage.GetState()!=EDamageState.DESTROYED;
  // Road presence lamps follow the existing headlight request; per-side lamp
  // functionality remains native and independent of the request.
  bool rear=alive && m_Lights.GetLightsState(ELightType.Head);
  if(m_Lights.GetLightsState(ELightType.Rear)!=rear)m_Lights.SetLightsState(ELightType.Rear,rear);
  // Native tracked automatic hold can report6 with no pedal. Keep actual pedal
  // intent distinct, validate current pilot, and expire lost reports in0.6s.
  bool brake=alive && m_PedalActive && m_PedalAge<0.6 && station && station.IsDriver(m_ReportedDriver) && Wheeled.GetBrake()>0.05;
  bool reverse=alive && Wheeled.GetGear()==0 && Wheeled.GetClutch()>0.2 && Wheeled.EngineGetRPM()>300;
  if(m_Lights.GetLightsState(ELightType.Brake)!=brake)m_Lights.SetLightsState(ELightType.Brake,brake);
  if(m_Lights.GetLightsState(ELightType.Reverse)!=reverse)m_Lights.SetLightsState(ELightType.Reverse,reverse);
 }
}

// Original private German control hints. Accepted optic widgets are unchanged.
class GEP_NextAimingHints625Class : ScriptComponentClass {}
class GEP_NextAimingHints625 : ScriptComponent
{
 protected Widget m_Root625,m_ModeRoot625,m_RangeRoot625;
 protected SCR_InputButtonComponent m_Mode625,m_Range625;
 bool Visible625()
 {
  return m_Root625 && m_Mode625 && m_Range625 && m_Root625.IsVisible();
 }
 override void OnPostInit(IEntity owner)
 {
  SetEventMask(owner,EntityEvent.FRAME);
 }
 protected bool Build625()
 {
  if(m_Root625)
  {
   return m_Mode625 && m_Range625;
  }
  WorkspaceWidget workspace625=GetGame().GetWorkspace();
  if(!workspace625)
  {
   return false;
  }
  m_Root625=workspace625.CreateWidget(WidgetType.FrameWidgetTypeID,WidgetFlags.VISIBLE,Color.White,45,workspace625);
  if(!m_Root625)
  {
   return false;
  }
  m_Root625.SetName("GepardGermanConsumerControls625");
  m_Root625.SetVisible(false);
  m_ModeRoot625=workspace625.CreateWidgets("{C42847838185932E}UI/layouts/WidgetLibrary/Buttons/WLib_InputButton.layout",m_Root625);
  m_RangeRoot625=workspace625.CreateWidgets("{C42847838185932E}UI/layouts/WidgetLibrary/Buttons/WLib_InputButton.layout",m_Root625);
  if(m_ModeRoot625)
  {
   m_Mode625=SCR_InputButtonComponent.Cast(m_ModeRoot625.FindHandler(SCR_InputButtonComponent));
  }
  if(m_RangeRoot625)
  {
   m_Range625=SCR_InputButtonComponent.Cast(m_RangeRoot625.FindHandler(SCR_InputButtonComponent));
  }
  if(!m_Mode625 || !m_Range625)
  {
   return false;
  }
  m_Mode625.SetClickSoundDisabled(true);
  m_Mode625.SetAction("WCS_Armament_TurretStabilizationHold",-1,true);
  m_Range625.SetClickSoundDisabled(true);
  m_Range625.SetAction("GEPMeasureRange676",-1,true);
  m_Range625.SetLabel("Entfernung messen");
  return true;
 }
 override void EOnFrame(IEntity owner,float timeSlice)
 {
  GEP_NextStabilizationConsumer625 stab625=GEP_NextStabilizationConsumer625.Cast(owner.FindComponent(GEP_NextStabilizationConsumer625));
  bool show625=GetGame().InPlayMode() && stab625 && stab625.ControlEligible625();
  if(!show625)
  {
   if(m_Root625)
   {
    m_Root625.SetVisible(false);
   }
   return;
  }
  if(!Build625())
  {
   return;
  }
  WorkspaceWidget workspace625=GetGame().GetWorkspace();
  float height625=workspace625.DPIUnscale(workspace625.GetHeight());
  FrameSlot.SetPos(m_Root625,12,height625-58);
  FrameSlot.SetSize(m_Root625,430,54);
  FrameSlot.SetPos(m_RangeRoot625,0,0);
  FrameSlot.SetSize(m_RangeRoot625,430,26);
  FrameSlot.SetPos(m_ModeRoot625,0,27);
  FrameSlot.SetSize(m_ModeRoot625,430,26);
  m_Mode625.SetSize(20);
  m_Range625.SetSize(20);
  if(stab625.GetStabilizationEnabled() && stab625.GetMode()==WCS_EStabilizationMode.STABILIZED)
  {
   m_Mode625.SetLabel("Stabilisierung AUS (halten)");
  }
  else
  {
   m_Mode625.SetLabel("Stabilisierung EIN (halten)");
  }
  m_Root625.SetVisible(true);
 }
 override void OnDelete(IEntity owner)
 {
  if(m_Root625)
  {
   m_Root625.RemoveFromHierarchy();
  }
  m_Root625=null;
  m_ModeRoot625=null;
  m_RangeRoot625=null;
  m_Mode625=null;
  m_Range625=null;
 }
}

modded class SCR_AvailableActionsDisplay
{
 override protected int GetAvailableActions(notnull SCR_AvailableActionsConditionData data,array<ref SCR_AvailableActionContext> inActions,out array<SCR_AvailableActionContext> outActions,float timeSlice)
 {
  int count625=super.GetAvailableActions(data,inActions,outActions,timeSlice);
  GEP_TurretController410 control625=GEP_TurretController410.Cast(data.GetCurrentTurretController());
  if(!control625 || !outActions)
  {
   return count625;
  }
  IEntity owner625=control625.GetOwner();
  if(!owner625 || owner625.IsDeleted())
  {
   return count625;
  }
  GEP_NextAimingHints625 hints625=GEP_NextAimingHints625.Cast(owner625.FindComponent(GEP_NextAimingHints625));
  if(!hints625 || !hints625.Visible625())
  {
   return count625;
  }
  IEntity user625;
  BaseCompartmentSlot seat625;
  BaseSightsComponent sight625;
  if(!GEP_NextAimingBinding625.Read625(owner625,user625,seat625,sight625))
  {
   return count625;
  }
  for(int i625=outActions.Count()-1;i625>=0;i625--)
  {
   SCR_AvailableActionContext context625=outActions[i625];
   if(!context625)
   {
    continue;
   }
   string configured625=context625.WCS_Armament_GetConfiguredActionName();
   if(configured625=="WCS_Armament_TurretStabilizationHold" || configured625=="WCS_Armament_TurretStabilizationToggle")
   {
    outActions.Remove(i625);
   }
  }
  return outActions.Count();
 }
}

// Original independent plain visual fallback. No inputs or gameplay state.
class GEP_PlainSight680
{
 protected Widget m_Root;
 protected CanvasWidget m_Canvas;
 protected TextWidget m_Fault;
 protected int m_LastAttempt=-1000;
 protected ref array<ref CanvasWidgetCommand> m_Draw={};
 bool Ready(){return m_Root && m_Canvas && m_Fault;}
 bool Visible702(){return Ready() && m_Root.IsVisible();}
 bool Ensure()
 {
  if(Ready())return true;
  if(!GetGame() || System.IsConsoleApp())return false;
  WorkspaceWidget workspace=GetGame().GetWorkspace();if(!workspace)return false;
  int tick=System.GetTickCount();if(tick-m_LastAttempt<500)return false;m_LastAttempt=tick;
  Close();m_Root=workspace.CreateWidget(WidgetType.FrameWidgetTypeID,WidgetFlags.VISIBLE,Color.White,41,workspace);
  if(!m_Root)return false;m_Root.SetName("GepardPlainSight680");m_Root.SetVisible(false);
  m_Canvas=CanvasWidget.Cast(workspace.CreateWidget(WidgetType.CanvasWidgetTypeID,WidgetFlags.VISIBLE,Color.White,0,m_Root));
  if(!m_Canvas)return false;
  m_Fault=TextWidget.Cast(workspace.CreateWidget(WidgetType.TextWidgetTypeID,WidgetFlags.VISIBLE,Color.White,1,m_Root));
  return true;
 }
 protected void Line(float x1,float y1,float x2,float y2,int color,float width)
 {
  float dpi=GetGame().GetWorkspace().DPIScale(1);
  LineDrawCommand command=new LineDrawCommand();command.m_iColor=color;command.m_fWidth=width*dpi;command.m_Vertices={x1*dpi,y1*dpi,x2*dpi,y2*dpi};m_Draw.Insert(command);
 }
 void Show(string fault)
 {
  if(!Ensure())return;
  WorkspaceWidget workspace=GetGame().GetWorkspace();
  float w=workspace.DPIUnscale(workspace.GetWidth());float h=workspace.DPIUnscale(workspace.GetHeight());
  FrameSlot.SetPos(m_Root,0,0);FrameSlot.SetSize(m_Root,w,h);FrameSlot.SetSize(m_Canvas,w,h);
  m_Draw.Clear();float x=w*0.5;float y=h*0.5;float s=Math.Min(w/1920,h/1080);
  for(int layer=0;layer<2;layer++)
  {
   int color=0xff09100e;float stroke=6.8*s;if(layer==1){color=0xffeee9cd;stroke=3*s;}
   Line(x-144*s*1.4,y+18*s*1.4,x-96*s*1.4,y,color,stroke);
   Line(x-96*s*1.4,y,x-72*s*1.4,y,color,stroke);
   Line(x+72*s*1.4,y,x+96*s*1.4,y,color,stroke);
   Line(x+96*s*1.4,y,x+144*s*1.4,y+18*s*1.4,color,stroke);
   Line(x-22*s*1.4,y+13*s*1.4,x-7*s*1.4,y,color,stroke);
   Line(x+7*s*1.4,y,x+22*s*1.4,y+13*s*1.4,color,stroke);
   Line(x,y-62*s*1.4,x,y-18*s*1.4,color,stroke);
  }
  m_Canvas.SetDrawCommands(m_Draw);
  if(m_Fault){FrameSlot.SetPos(m_Fault,w*0.5-330*s,h*0.5+110*s);FrameSlot.SetSize(m_Fault,660*s,48*s);m_Fault.SetExactFontSize(Math.Max(12,Math.Round(23*s)));m_Fault.SetBold(true);m_Fault.SetColor(Color.FromInt(0xffffc37b));m_Fault.SetText(fault);}
  m_Root.SetVisible(true);
 }
 void Hide(){if(m_Root)m_Root.SetVisible(false);}
 void Close()
 {
  if(m_Root)m_Root.RemoveFromHierarchy();m_Root=null;m_Canvas=null;m_Fault=null;m_Draw.Clear();
 }
}

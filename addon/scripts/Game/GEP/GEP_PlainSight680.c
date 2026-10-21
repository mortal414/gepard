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
  m_Draw.Clear();float x=w*0.5;float y=h*0.5;
  for(int layer=0;layer<2;layer++)
  {
   int color=0xff111a14;float stroke=5;if(layer==1){color=0xffeff6ee;stroke=2;}
   Line(x-40,y,x-12,y,color,stroke);Line(x+12,y,x+40,y,color,stroke);Line(x,y-40,x,y-12,color,stroke);Line(x,y+12,x,y+40,color,stroke);
  }
  m_Canvas.SetDrawCommands(m_Draw);
  if(m_Fault){FrameSlot.SetPos(m_Fault,w*0.5-240,h*0.5+70);FrameSlot.SetSize(m_Fault,480,40);m_Fault.SetExactFontSize(19);m_Fault.SetColor(Color.FromInt(0xffffc37b));m_Fault.SetText(fault);}
  m_Root.SetVisible(true);
 }
 void Hide(){if(m_Root)m_Root.SetVisible(false);}
 void Close()
 {
  if(m_Root)m_Root.RemoveFromHierarchy();m_Root=null;m_Canvas=null;m_Fault=null;m_Draw.Clear();
 }
}

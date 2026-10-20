// Original sights-compatible sensor panel. Displays measured data, never live
// target entity transforms or omniscient faction identity. Lead is assistance.
class GEP_RadarDisplay
{
 Widget Root;

 CanvasWidget Scope;

 CanvasWidget Lead;

 TextWidget Header;

 TextWidget SearchStatus;

 TextWidget TrackStatus;

 TextWidget ContactStatus;

 TextWidget Message;

 TextWidget Help;

 bool IsVisible;

 bool LeadVisible;

 float Timer;

 float Scale;

 ref array<ref CanvasWidgetCommand> Draw={};

 ref array<ref CanvasWidgetCommand> LeadDraw={};

 const int GREEN=0xff8ed2aa;

 const int AMBER=0xffffcb67;

 const int CYAN=0xff92d9ed;

 const int GRID=0xff34574a;

 TextWidget Text(string name,float y,int size,float height)
 {
  TextWidget label=TextWidget.Cast(GetGame().GetWorkspace().CreateWidget(WidgetType.TextWidgetTypeID,WidgetFlags.VISIBLE,Color.FromInt(GREEN),1,Root));

  label.SetName(name);
  label.SetExactFontSize(size);
  label.SetTextWrapping(true);
  FrameSlot.SetPos(label,14,y);
  FrameSlot.SetSize(label,312,height);
  return label;

 }
 void GEP_RadarDisplay()
 {
  WorkspaceWidget workspace=GetGame().GetWorkspace();
  Scale=workspace.DPIScale(1);

  Root=workspace.CreateWidget(WidgetType.FrameWidgetTypeID,WidgetFlags.VISIBLE,Color.White,40,workspace);
  FrameSlot.SetPos(Root,24,224);
  FrameSlot.SetSize(Root,342,612);

  Scope=CanvasWidget.Cast(workspace.CreateWidget(WidgetType.CanvasWidgetTypeID,WidgetFlags.VISIBLE,Color.White,0,Root));
  FrameSlot.SetSize(Scope,342,612);

  Header=Text("GepardRadarHeader",12,20,28);
  SearchStatus=Text("GepardSearchStatus",43,16,24);

  TrackStatus=Text("GepardTrackingStatus",305,16,66);
  ContactStatus=Text("GepardSelectedContact",378,15,64);

  Message=Text("GepardRadarFeedback",450,14,50);
  Help=Text("GepardRadarHelp",510,14,92);

  Lead=CanvasWidget.Cast(workspace.CreateWidget(WidgetType.CanvasWidgetTypeID,WidgetFlags.VISIBLE,Color.White,50,workspace));
  FrameSlot.SetSize(Lead,30,30);

 }
 void Visible(bool visible) {IsVisible=visible;
  Root.SetVisible(visible);
  if(!visible) {Lead.SetVisible(false);
  LeadVisible=false;
  }}
 void Destroy() {if(Root) Root.RemoveFromHierarchy();
  if(Lead) Lead.RemoveFromHierarchy();
  }
 void Line(float a,float b,float c,float d,int color,float width=1)
 {
  LineDrawCommand line=new LineDrawCommand();
  line.m_iColor=color;
  line.m_fWidth=width*Scale;
  line.m_Vertices={a*Scale,b*Scale,c*Scale,d*Scale};
  Draw.Insert(line);

 }
 void Circle(float x,float y,float radius,int color)
 {
  LineDrawCommand line=new LineDrawCommand();
  line.m_iColor=color;
  line.m_fWidth=Scale;
  line.m_bShouldEnclose=true;
  line.m_Vertices={};

  for(int i=0;i<64;i++){float angle=i*Math.PI2/64;
  line.m_Vertices.Insert((x+Math.Sin(angle)*radius)*Scale);
  line.m_Vertices.Insert((y+Math.Cos(angle)*radius)*Scale);
  }Draw.Insert(line);

 }
 string TrackName(GEP_RadarSystem radar)
 {
  if(!radar.Model.TrackingPowered) return "OFF";

  if(radar.Model.ActiveTarget<0) return "READY";

  if(radar.Model.State==GEP_TrackState.ACQUIRING) return "ACQUIRING";

  if(radar.Model.State==GEP_TrackState.FRESH) return "TRACK";

  if(radar.Model.State==GEP_TrackState.COAST) return "COAST";

  if(radar.Model.State==GEP_TrackState.LOST) return "LOST";

  return "READY";

 }
 void Update(GEP_RadarSystem radar,IEntity user,float dt)
 {
  Timer+=dt;
  if(Timer<0.05) return;
  Timer=0;
  Scale=GetGame().GetWorkspace().DPIScale(1);

  bool gunner=radar.Gunner(user);
  string seat="DRIVER";
  if(gunner) seat="GUNNER";

  Header.SetText("GEPARD / "+seat);
  string search="OFF";
  if(radar.Model.SearchPowered) search="SWEEP";

  SearchStatus.SetTextFormat("SEARCH %1 / %2 m / %3",search,Math.Round(radar.Range),radar.Contacts.Count());

  string acquisition="SEARCH";
  if(radar.Model.AcquisitionSource==GEP_AcquisitionSource.OPTICAL_DESIGNATION) acquisition="SIGHTED";

  TrackStatus.SetText("TRACKER "+TrackName(radar));

  if(radar.Model.ActiveTarget>=0)
   TrackStatus.SetTextFormat("TRACKER %1 / ID %2\n%3 / AGE %4 s",TrackName(radar),radar.Model.ActiveTarget,acquisition,Math.Round((radar.Clock-radar.Model.LastMeasurement)*10)*0.1);

  string leadState="NO LEAD";
  if(radar.Model.CanShowLead(radar.Clock)) leadState="LEAD ASSIST <1800 m";
  TrackStatus.SetText(TrackStatus.GetText()+"\n"+leadState);
  ContactStatus.SetVisible(gunner);
  Message.SetVisible(gunner);
  Help.SetVisible(gunner);
  float panelHeight=612;
  if(!gunner) panelHeight=162;
  FrameSlot.SetSize(Root,342,panelHeight);
  FrameSlot.SetSize(Scope,342,panelHeight);
  float trackY=305;
  if(!gunner) trackY=76;
  FrameSlot.SetPos(TrackStatus,14,trackY);
  Draw.Clear();
  PolygonDrawCommand panel=new PolygonDrawCommand();
  panel.m_iColor=0xe80c1814;
  panel.m_Vertices={0,0,342*Scale,0,342*Scale,panelHeight*Scale,0,panelHeight*Scale};
  Draw.Insert(panel);

  if(!gunner)
  {
   Scope.SetDrawCommands(Draw);
   Lead.SetVisible(false);
   LeadVisible=false;
   return;
  }
  float cx=171;
  float cy=183;
  float radius=105;
  Circle(cx,cy,radius,GRID);
  Circle(cx,cy,radius*0.667,GRID);
  Circle(cx,cy,radius*0.333,GRID);
  Line(cx-radius,cy,cx+radius,cy,GRID);
  Line(cx,cy-radius,cx,cy+radius,GRID);

  // Scope is hull-up. The actual antenna sweep rotates with its turret parent.
  vector hull[4];
  radar.GetOwner().GetWorldTransform(hull);
  vector parentPose[4];
  radar.SearchHead.GetParent().GetWorldTransform(parentPose);

  float angle=radar.Sweep*Math.DEG2RAD;
  vector beam=parentPose[2]*Math.Cos(angle)+parentPose[0]*Math.Sin(angle);

  if(radar.Model.SearchPowered) Line(cx,cy,cx+vector.Dot(beam,hull[0])*radius,cy-vector.Dot(beam,hull[2])*radius,GREEN,2);

  string selected="NO SEARCH SELECTION\nChoose a return, train turret,\nthen acquire deliberately.";

  foreach(GEP_AirContact contact:radar.Contacts)
  {
   vector delta=radar.Predict(contact)-radar.AntennaOrigin();
  float right=vector.Dot(delta,hull[0]);
  float forward=vector.Dot(delta,hull[2]);

   float x=cx+right/radar.Range*radius;
  float y=cy-forward/radar.Range*radius;
  float age=radar.Clock-contact.Seen;

   int color=GREEN;
  if(contact.Hits<2 || age>2.6) color=AMBER;
  Line(x-3,y,x+3,y,color,2);
  Line(x,y-3,x,y+3,color,2);

   if(contact.ID==radar.Model.SelectedContact)
   {
    Line(x-7,y-7,x+7,y-7,CYAN,2);
  Line(x+7,y-7,x+7,y+7,CYAN,2);
  Line(x+7,y+7,x-7,y+7,CYAN,2);
  Line(x-7,y+7,x-7,y-7,CYAN,2);

    string quality="CONFIRMED";
  if(contact.Hits<2) quality="TENTATIVE";
  if(age>2.6) quality="COAST";

    selected=string.Format("SELECTED %1 / %2\n%3 m / BRG %4 / AGE %5 s\nIDENTITY UNKNOWN",contact.ID,quality,Math.Round(delta.Length()),Math.Round(Math.Atan2(right,forward)*Math.RAD2DEG),Math.Round(age*10)*0.1);

   }
  }
  ContactStatus.SetText(selected);
  Message.SetText(radar.Feedback);

  Help.SetText("Home search / End range\nPgUp-Dn select / Num1 acquire\nNum2 sighted / Del untrack\nNum0 tracker / Ins panel\nControls rebindable in Settings");

  if(!gunner) Help.SetText("Home search / End range / Ins display\nMove to gunner for acquisition and weapons.");

  Scope.SetDrawCommands(Draw);
  Lead.SetVisible(false);
  LeadVisible=false;

  vector solution;
  float flight;

  if(!gunner || !radar.Gun.IsWeaponADS() || !radar.AssistedAim(solution,flight)) return;

  vector screen=GetGame().GetWorkspace().ProjWorldToScreen(solution,GetGame().GetWorld());
  if(screen[2]<=0 || screen[0]<30 || screen[1]<30) return;

  FrameSlot.SetPos(Lead,screen[0]-15,screen[1]-15);
  LeadDraw.Clear();
  LineDrawCommand mark=new LineDrawCommand();
  mark.m_iColor=AMBER;
  mark.m_fWidth=2;
  mark.m_bShouldEnclose=true;
  mark.m_Vertices={15*Scale,Scale,29*Scale,15*Scale,15*Scale,29*Scale,Scale,15*Scale};
  LeadDraw.Insert(mark);
  Lead.SetDrawCommands(LeadDraw);
  Lead.SetVisible(true);
  LeadVisible=true;

 }
}

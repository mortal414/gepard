// Original sight instruments. Native SCR_2DOpticsDisplay owns the optic lifecycle.
modded class SCR_WeaponInfoVehicle
{
 protected bool m_bGepardHidden583;
 protected void RestoreGepardInfo702()
 {
  if(!m_bGepardHidden583)return;
  if(m_wRoot)m_wRoot.SetVisible(true);
  m_bGepardHidden583=false;
 }
 override protected void DisplayStartDraw(IEntity owner)
 {
  RestoreGepardInfo702();super.DisplayStartDraw(owner);
 }
 override protected void DisplayStopDraw(IEntity owner)
 {
  RestoreGepardInfo702();super.DisplayStopDraw(owner);
 }
 override protected void DisplayControlledEntityChanged(IEntity from,IEntity to)
 {
  RestoreGepardInfo702();super.DisplayControlledEntityChanged(from,to);
 }
 override protected void DisplayUpdate(IEntity owner,float timeSlice)
 {
  IEntity turretOwner702;if(m_TurretController)turretOwner702=m_TurretController.GetOwner();
  if(turretOwner702 && !turretOwner702.IsDeleted() && turretOwner702.FindComponent(GEP_GunStatus) && GEP_Presentation680.MaskWeaponInfo(turretOwner702))
  {
   if(m_wRoot)m_wRoot.SetVisible(false);
   m_bGepardHidden583=true;return;
  }
  RestoreGepardInfo702();
  super.DisplayUpdate(owner,timeSlice);
 }
}
class GEP_GunStatusClass : ScriptComponentClass {}
class GEP_GunStatus : ScriptComponent
{
 [Attribute("0",UIWidgets.EditBox,"0 current; 1-3 rejected archives; 4 integrated sight703. Visuals only.")]
 int HudStyle702;
 float SightHelpAge703;
 ref GEP_PlainSight680 Plain680=new GEP_PlainSight680();
 const int INK583=0xffeff6ee;
 const int MUTED583=0xffb2c5c0;
 const int ACTIVE583=0xff9be5be;
 const int WARN583=0xffffc37b;
 Widget Root;
 CanvasWidget Canvas;
 TextWidget Header;
 TextWidget OpticalMode;
 TextWidget Zoom;
 TextWidget Range676;
 TextWidget LeftTitle;
 TextWidget LeftCount;
 TextWidget LeftType;
 TextWidget RightTitle;
 TextWidget RightCount;
 TextWidget RightType;
 TextWidget Group;
 TextWidget Feedback;
 TextWidget Drive;
 TextWidget SightAngles;
 TextWidget TurretAngles;
 TextWidget Reference;
 ref GEP_GunnerSnapshot583 Snapshot=new GEP_GunnerSnapshot583();
 ref array<ref CanvasWidgetCommand> Draw={};
 ref array<Widget> HintRoots={};
 ref array<SCR_InputButtonComponent> Hints={};
 ref array<string> HintActions={"GEPNextGunGroup579","GEPOpticalNight","GEPOpticalTrack","GEPOpticalCancel","TurretADSHold","GEPMeasureRange676"};
 ref array<string> HintLabels={"Waffenauswahl","Restlicht","Optische Verfolgung","Verfolgung beenden","Visier / Innenraum","Entfernung messen"};
 float Timer;
 float HintTimer=2;
 float W;
 float H;
 float S;
 float Pad;
 float FooterY;
 float FooterW;
 float HeaderHeight676;
 float HeaderCorners676;
 int LastBuildAttempt680;
 bool InstrumentsVisible702;
 IEntity PresentedUser702;
 BaseCompartmentSlot PresentedSeat702;
 SCR_2DSightsComponent PresentedSight702;
 bool ReplacesWeaponInfo702(IEntity user,BaseCompartmentSlot seat,SCR_2DSightsComponent sight)
 {
  return InstrumentsVisible702 && Ready680() && Root.IsVisible() && user==PresentedUser702 && seat==PresentedSeat702 && sight==PresentedSight702;
 }
 void ClearPresentation702()
 {
  InstrumentsVisible702=false;PresentedUser702=null;PresentedSeat702=null;PresentedSight702=null;SightHelpAge703=0;
 }
 bool ActiveCrew(IEntity owner){return GEP_GunnerSnapshot583.IsLocalGunner(owner);}
 bool Present680(IEntity owner)
 {
  IEntity user;BaseCompartmentSlot seat;SCR_2DSightsComponent sight;string reason;
  return GEP_Presentation680.Evaluate(owner,user,seat,sight,reason);
 }
 bool Ready680()
 {
  if(!Root || !Canvas || !Header || !OpticalMode || !Zoom || !Range676)return false;
  if(!LeftTitle || !LeftCount || !LeftType || !RightTitle || !RightCount || !RightType)return false;
  if(!Group || !Feedback || !Drive || !SightAngles || !TurretAngles || !Reference)return false;
  return true;
 }
 string BindingAction584(int index)
 {
  // Installed keyboard RMB click is TurretADS. TurretADSHold is a separate
  // controller/joystick hold action, so it legitimately has no keyboard binding.
  if(index==4 && GetGame().GetInputManager().IsUsingMouseAndKeyboard())return "TurretADS";
  return HintActions[index];
 }
 TextWidget Label(string name)
 {
  TextWidget label=TextWidget.Cast(GetGame().GetWorkspace().CreateWidget(WidgetType.TextWidgetTypeID,WidgetFlags.VISIBLE,Color.FromInt(INK583),1,Root));
  if(!label)return null;
  label.SetName(name);label.SetTextWrapping(false);label.SetShadow(2,0xff08100e,1,0,1);return label;
 }
 void Put(TextWidget field,string value)
 {
  if(field.GetText()!=value)field.SetText(value);
 }
 void Place(TextWidget field,float x,float y,float width,float height,int font,int color=INK583)
 {
  FrameSlot.SetPos(field,x,y);FrameSlot.SetSize(field,width,height);field.SetExactFontSize(Math.Max(12,Math.Round(font*S)));field.SetColor(Color.FromInt(color));
 }
 void Line(float x1,float y1,float x2,float y2,int color,float width)
 {
  float dpi=GetGame().GetWorkspace().DPIScale(1);
  LineDrawCommand command=new LineDrawCommand();command.m_iColor=color;command.m_fWidth=width*dpi;command.m_Vertices={x1*dpi,y1*dpi,x2*dpi,y2*dpi};Draw.Insert(command);
 }
 void Bar(float x,float y,float width,float height,int color)
 {
  Line(x,y+height*0.5,x+width,y+height*0.5,color,height);
 }
 void Build()
 {
  WorkspaceWidget workspace=GetGame().GetWorkspace();if(!workspace)return;
  Root=workspace.CreateWidget(WidgetType.FrameWidgetTypeID,WidgetFlags.VISIBLE,Color.White,40,workspace);if(!Root)return;Root.SetName("GepardOriginalScreen583");Root.SetVisible(false);
  Canvas=CanvasWidget.Cast(workspace.CreateWidget(WidgetType.CanvasWidgetTypeID,WidgetFlags.VISIBLE,Color.White,0,Root));if(!Canvas)return;Canvas.SetName("GepardBezelAndReticle583");
  Header=Label("GepardHeader583");OpticalMode=Label("GepardOpticalMode583");Zoom=Label("GepardZoom583");Range676=Label("GepardMeasuredRange676");
  LeftTitle=Label("GepardLeftTitle583");LeftCount=Label("GepardLeftCount583");LeftType=Label("GepardLeftAmmo583");
  RightTitle=Label("GepardRightTitle583");RightCount=Label("GepardRightCount583");RightType=Label("GepardRightAmmo583");
  Group=Label("GepardSelectedGroup583");Feedback=Label("GepardOpticalFeedback583");Drive=Label("GepardDrive583");
  SightAngles=Label("GepardSightAngles583");TurretAngles=Label("GepardTurretAngles583");Reference=Label("GepardAngleReference583");
  if(LeftCount)LeftCount.SetBold(true);if(RightCount)RightCount.SetBold(true);if(Header)Header.SetBold(true);if(Group)Group.SetBold(true);
  foreach(string action:HintActions)
  {
   // Reference the installed native binding-display prefab; no copied native/WCS UI source.
   Widget hint=workspace.CreateWidgets("{C42847838185932E}UI/layouts/WidgetLibrary/Buttons/WLib_InputButton.layout",Root);
   HintRoots.Insert(hint);
   SCR_InputButtonComponent binding;
   if(hint)binding=SCR_InputButtonComponent.Cast(hint.FindHandler(SCR_InputButtonComponent));
   Hints.Insert(binding);
   if(binding){binding.SetClickSoundDisabled(true);binding.SetAction(BindingAction584(Hints.Count()-1),-1,true);}
  }
 }
 bool HasVisual680(){return Ready680() || (Plain680 && Plain680.Ready());}
 bool HasVisibleSight702(){return (Ready680() && Root.IsVisible()) || (Plain680 && Plain680.Visible702());}
 void ShowInstrumentWidgets680()
 {
  array<Widget> fields={Header,OpticalMode,Zoom,Range676,LeftTitle,LeftCount,LeftType,RightTitle,RightCount,RightType,Group,Feedback,Drive,SightAngles,TurretAngles,Reference};
  foreach(Widget field:fields){if(field)field.SetVisible(true);}
 }
 void EnsureVisual680()
 {
  if(Plain680)Plain680.Ensure();
  if(Ready680())return;
  int tick=System.GetTickCount();if(tick-LastBuildAttempt680<1000)return;LastBuildAttempt680=tick;
  ClearPresentation702();
  Header=null;OpticalMode=null;Zoom=null;Range676=null;
  LeftTitle=null;LeftCount=null;LeftType=null;RightTitle=null;RightCount=null;RightType=null;
  Group=null;Feedback=null;Drive=null;SightAngles=null;TurretAngles=null;Reference=null;
  if(Root)Root.RemoveFromHierarchy();Root=null;Canvas=null;HintRoots.Clear();Hints.Clear();Draw.Clear();Build();
 }
 // Original plain sight remains usable if an optional instrument widget failed.
 // It grants no weapon/range/assist eligibility and shows no invented values.
 void Minimal680()
 {
  ClearPresentation702();
  if(Root)Root.SetVisible(false);
  string fault="ANZEIGE NICHT VERFÜGBAR";string reason;
  if(!GEP_GunnerSnapshot583.Evaluate679(GetOwner(),reason))fault=GEP_Presentation680.DisabledLabel(GetOwner(),reason);
  if(Plain680)Plain680.Show(fault);
 }
 void Layout()
 {
  WorkspaceWidget workspace=GetGame().GetWorkspace();W=workspace.DPIUnscale(workspace.GetWidth());H=workspace.DPIUnscale(workspace.GetHeight());
  S=Math.Clamp(Math.Min(W/1920,H/1080),0.66,1.5);Pad=24*S;
  HeaderHeight676=44*S;HeaderCorners676=Pad+60*S;
  if(W<800){HeaderHeight676=76*S;HeaderCorners676=Pad+92*S;}
  FrameSlot.SetSize(Root,W,H);FrameSlot.SetPos(Root,0,0);FrameSlot.SetSize(Canvas,W,H);
  FooterY=H-Pad-96*S;FooterW=W-2*Pad;
  if(!Snapshot.ADS){FooterY=72*S;FooterW=760*S;}
  float x=Pad+16*S;
  Place(LeftTitle,x,FooterY+7*S,220*S,22*S,14,MUTED583);
  Place(LeftCount,x,FooterY+27*S,220*S,40*S,32);
  Place(LeftType,x,FooterY+70*S,220*S,21*S,14,MUTED583);
  x=Pad+254*S;
  Place(RightTitle,x,FooterY+7*S,220*S,22*S,14,MUTED583);
  Place(RightCount,x,FooterY+27*S,220*S,40*S,32);
  Place(RightType,x,FooterY+70*S,220*S,21*S,14,MUTED583);
  x=Pad+510*S;
  Place(Group,x,FooterY+8*S,440*S,25*S,20);
  Place(Feedback,x,FooterY+38*S,440*S,27*S,19);
  Place(Drive,x,FooterY+71*S,440*S,21*S,14,MUTED583);
  Header.SetVisible(Snapshot.ADS);OpticalMode.SetVisible(Snapshot.ADS);Zoom.SetVisible(Snapshot.ADS);Range676.SetVisible(Snapshot.ADS);
  SightAngles.SetVisible(Snapshot.ADS);TurretAngles.SetVisible(Snapshot.ADS);Reference.SetVisible(Snapshot.ADS);
  if(Snapshot.ADS)
  {
   float rangeX676=W*0.5-150*S;float rangeY676=Pad+10*S;
   float headerWidth676=Math.Min(680*S,rangeX676-Pad-28*S);
   if(W<800){rangeY676=Pad+42*S;headerWidth676=Math.Min(680*S,W-2*Pad-432*S);}
   Place(Header,Pad+16*S,Pad+9*S,Math.Max(40*S,headerWidth676),32*S,21);
   Place(Range676,rangeX676,rangeY676,300*S,30*S,19);
   Place(OpticalMode,W-Pad-400*S,Pad+10*S,240*S,30*S,19);
   Place(Zoom,W-Pad-142*S,Pad+10*S,140*S,30*S,19);
   float angleX=W-Pad-560*S;
   Place(SightAngles,angleX,FooterY+9*S,550*S,25*S,18);
   Place(TurretAngles,angleX,FooterY+40*S,550*S,25*S,18);
   Place(Reference,angleX,FooterY+73*S,550*S,20*S,13,MUTED583);
  }
  else
  {
   Place(Group,Pad+510*S,FooterY+22*S,235*S,28*S,19);
   Place(Feedback,Pad+510*S,FooterY+56*S,235*S,24*S,14,MUTED583);
   Drive.SetVisible(false);
  }
  if(Snapshot.ADS)Drive.SetVisible(true);
  int hintColumns676=Math.Max(1,HintRoots.Count());int hintRows676=1;
  if(W<1280){hintColumns676=3;hintRows676=2;}
  float hintWidth676=Math.Min(300*S,(W-2*Pad-(hintColumns676-1)*8*S)/hintColumns676);
  float hintLeft676=(W-hintColumns676*hintWidth676-(hintColumns676-1)*8*S)*0.5;
  int hintIndex;
  foreach(Widget hint:HintRoots)
  {
   if(!hint){hintIndex++;continue;}
   bool visible=Snapshot.ADS || hintIndex==0 || hintIndex==4;
   if(!Snapshot.Crew)visible=hintIndex==4;
   hint.SetVisible(visible);
   int column676=hintIndex%hintColumns676;int row676=hintIndex/hintColumns676;
   float hintX=hintLeft676+column676*(hintWidth676+8*S);
   float hintY=FooterY-(57+(hintRows676-1)*54)*S+row676*54*S;
   if(!Snapshot.ADS){hintY=FooterY+103*S;if(hintIndex==4)hintX=Pad+340*S;}
   FrameSlot.SetPos(hint,hintX,hintY);FrameSlot.SetSize(hint,hintWidth676,50*S);
   SCR_InputButtonComponent component=Hints[hintIndex];if(component)component.SetSize(Math.Max(32,Math.Round(54*S)));
   hintIndex++;
  }
  if(HudStyle702>=1 && HudStyle702<=3)LayoutCandidate702();
  if(HudStyle702==4)LayoutSight703();
 }
 void Text()
 {
  if(HudStyle702==4){TextSight703();return;}
  Put(Header,"GEPARD / ZIELVISIER");
  string mode="--";if(Snapshot.OpticsValid){mode="TAG";if(Snapshot.Night)mode="RESTLICHT";}Put(OpticalMode,mode);
  string zoom="--×";if(Snapshot.ZoomValid)zoom=NumberDE704(Snapshot.Magnification)+"×";Put(Zoom,zoom);
  Put(LeftTitle,"LINKS");Put(RightTitle,"RECHTS");Put(LeftCount,FeedCountDE704(Snapshot.Left));Put(RightCount,FeedCountDE704(Snapshot.Right));
  Put(LeftType,FeedTypeDE704(Snapshot.Left));Put(RightType,FeedTypeDE704(Snapshot.Right));
  string range="ENTFERNUNG --";
  if(Snapshot.RangeState676==EGEP_Range676State.VALID && Snapshot.RangeMetres676>0 && Snapshot.RangeAge676>=0)range="ENTFERNUNG "+Math.Round(Snapshot.RangeMetres676).ToString()+" m";
  else if(Snapshot.RangeState676==EGEP_Range676State.NO_RETURN)range="KEIN MESSWERT";
  else if(Snapshot.RangeState676==EGEP_Range676State.UNAVAILABLE)range="MESSUNG NICHT VERFÜGBAR";
  Put(Range676,range);string group="WAHL --";if(Snapshot.Group==0)group="BEIDE";if(Snapshot.Group==1)group="LINKS";if(Snapshot.Group==2)group="RECHTS";Put(Group,group);
  Put(Feedback,OperationalFeedbackDE704());Put(Drive,"");Put(SightAngles,"");Put(TurretAngles,"");Put(Reference,"");
 }
 void Graphics()
 {
  if(HudStyle702==4){GraphicsSight703();return;}
  if(HudStyle702>=1 && HudStyle702<=3){GraphicsCandidate702();return;}
  Draw.Clear();Bar(Pad,FooterY,FooterW,96*S,0xc0111d1d);
  Line(Pad,FooterY,Pad+FooterW,FooterY,0xff6c9287,1*S);
  Line(Pad+237*S,FooterY+12*S,Pad+237*S,FooterY+84*S,0xff46655c,1*S);
  Line(Pad+490*S,FooterY+12*S,Pad+490*S,FooterY+84*S,0xff46655c,1*S);
  if(Snapshot.Crew && (Snapshot.Group==0 || Snapshot.Group==1))Line(Pad+16*S,FooterY+94*S,Pad+218*S,FooterY+94*S,ACTIVE583,3*S);
  if(Snapshot.Crew && (Snapshot.Group==0 || Snapshot.Group==2))Line(Pad+254*S,FooterY+94*S,Pad+456*S,FooterY+94*S,ACTIVE583,3*S);
  if(Snapshot.ADS)
  {
   Bar(Pad,Pad,W-2*Pad,HeaderHeight676,0xb0111d1d);Line(Pad,Pad+HeaderHeight676,W-Pad,Pad+HeaderHeight676,0xff6c9287,1*S);
   // Thin open corners frame the unchanged native picture; no camera crop or side masks.
   float top=HeaderCorners676;float bottom=FooterY-68*S;if(W<1280)bottom=FooterY-122*S;
   Line(Pad,top,Pad+28*S,top,MUTED583,1*S);Line(Pad,top,Pad,top+44*S,MUTED583,1*S);
   Line(W-Pad-28*S,top,W-Pad,top,MUTED583,1*S);Line(W-Pad,top,W-Pad,top+44*S,MUTED583,1*S);
   Line(Pad,bottom-44*S,Pad,bottom,MUTED583,1*S);Line(Pad,bottom,Pad+28*S,bottom,MUTED583,1*S);
   Line(W-Pad,bottom-44*S,W-Pad,bottom,MUTED583,1*S);Line(W-Pad-28*S,bottom,W-Pad,bottom,MUTED583,1*S);
   // Read-only hull reference: fixed hull outline, moving native relative turret azimuth.
   float cx=W-Pad-635*S;float cy=FooterY+48*S;
   Line(cx-17*S,cy-25*S,cx+17*S,cy-25*S,MUTED583,1*S);Line(cx-17*S,cy-25*S,cx-17*S,cy+25*S,MUTED583,1*S);
   Line(cx+17*S,cy-25*S,cx+17*S,cy+25*S,MUTED583,1*S);Line(cx-17*S,cy+25*S,cx+17*S,cy+25*S,MUTED583,1*S);
   Line(cx-5*S,cy-31*S,cx,cy-36*S,MUTED583,1*S);Line(cx,cy-36*S,cx+5*S,cy-31*S,MUTED583,1*S);
   if(Snapshot.AimValid)
   {
    float radians=Snapshot.HullAzimuth*Math.DEG2RAD;
    Line(cx,cy,cx+Math.Sin(radians)*31*S,cy-Math.Cos(radians)*31*S,ACTIVE583,3*S);
   }
   // Original plain sight mark; no stock LAV scales or ballistic calibration.
   float x=W*0.5;float y=H*0.5;
   for(int layer=0;layer<2;layer++)
   {
    int color=0xff111a14;float stroke=5;if(layer==1){color=INK583;stroke=2;}
    Line(x-40,y,x-12,y,color,stroke);Line(x+12,y,x+40,y,color,stroke);Line(x,y-40,x,y-12,color,stroke);Line(x,y+12,x,y+40,color,stroke);
   }
  }
  Canvas.SetDrawCommands(Draw);
 }
 // BEGIN GENERATED HUD OPTIONS702
 // Original visual geometry only; no calibrated scales or camera changes.
 int Accent702()
 {
  if(HudStyle702==4)return 0xffeee9cd;
  if(HudStyle702==1)return 0xff9be5be;
  if(HudStyle702==2)return 0xffffd18a;
  if(HudStyle702==3)return 0xffe6f0f6;
  return ACTIVE583;
 }
 void LayoutCandidate702()
 {
  S=Math.Min(W/1920,H/1080);
  float ox=(W-1920*S)*0.5;float oy=(H-1080*S)*0.5;
  if(HudStyle702==1)
  {
   Place(Header,ox+48*S,oy+30*S,430*S,32*S,22,Accent702());
   Place(OpticalMode,ox+1500*S,oy+32*S,220*S,30*S,19,INK583);
   Place(Zoom,ox+1760*S,oy+32*S,120*S,30*S,19,INK583);
   Place(Range676,ox+790*S,oy+30*S,400*S,40*S,26,Accent702());
   Place(LeftTitle,ox+48*S,oy+882*S,190*S,23*S,14,MUTED583);
   Place(LeftCount,ox+48*S,oy+908*S,200*S,42*S,34,INK583);
   Place(LeftType,ox+48*S,oy+953*S,200*S,24*S,14,MUTED583);
   Place(RightTitle,ox+282*S,oy+882*S,190*S,23*S,14,MUTED583);
   Place(RightCount,ox+282*S,oy+908*S,200*S,42*S,34,INK583);
   Place(RightType,ox+282*S,oy+953*S,200*S,24*S,14,MUTED583);
   Place(Group,ox+582*S,oy+886*S,680*S,28*S,20,INK583);
   Place(Feedback,ox+582*S,oy+921*S,680*S,28*S,19,INK583);
   Place(Drive,ox+582*S,oy+956*S,680*S,22*S,14,MUTED583);
   Place(SightAngles,ox+1320*S,oy+888*S,550*S,25*S,17,INK583);
   Place(TurretAngles,ox+1320*S,oy+922*S,550*S,25*S,17,INK583);
   Place(Reference,ox+582*S,oy+988*S,720*S,23*S,13,MUTED583);
  }
  if(HudStyle702==2)
  {
   Place(Header,ox+48*S,oy+30*S,470*S,32*S,21,Accent702());
   Place(OpticalMode,ox+1510*S,oy+30*S,220*S,30*S,19,INK583);
   Place(Zoom,ox+1760*S,oy+30*S,120*S,30*S,19,INK583);
   Place(Range676,ox+798*S,oy+632*S,430*S,42*S,26,Accent702());
   Place(LeftTitle,ox+48*S,oy+918*S,210*S,23*S,14,MUTED583);
   Place(LeftCount,ox+48*S,oy+943*S,210*S,42*S,33,INK583);
   Place(LeftType,ox+48*S,oy+986*S,210*S,21*S,14,MUTED583);
   Place(RightTitle,ox+1668*S,oy+918*S,210*S,23*S,14,MUTED583);
   Place(RightCount,ox+1668*S,oy+943*S,210*S,42*S,33,INK583);
   Place(RightType,ox+1668*S,oy+986*S,210*S,21*S,14,MUTED583);
   Place(Group,ox+698*S,oy+918*S,580*S,28*S,20,INK583);
   Place(Feedback,ox+698*S,oy+951*S,580*S,28*S,19,INK583);
   Place(Drive,ox+1460*S,oy+70*S,420*S,24*S,14,MUTED583);
   Place(SightAngles,ox+48*S,oy+866*S,550*S,26*S,17,INK583);
   Place(TurretAngles,ox+1320*S,oy+866*S,550*S,26*S,17,INK583);
   Place(Reference,ox+698*S,oy+990*S,680*S,22*S,13,MUTED583);
  }
  if(HudStyle702==3)
  {
   Place(Header,ox+48*S,oy+30*S,550*S,32*S,22,Accent702());
   Place(OpticalMode,ox+1510*S,oy+30*S,220*S,30*S,19,INK583);
   Place(Zoom,ox+1760*S,oy+30*S,120*S,30*S,19,INK583);
   Place(Range676,ox+48*S,oy+891*S,460*S,44*S,28,Accent702());
   Place(LeftTitle,ox+540*S,oy+884*S,210*S,23*S,14,MUTED583);
   Place(LeftCount,ox+540*S,oy+911*S,210*S,42*S,33,INK583);
   Place(LeftType,ox+540*S,oy+955*S,210*S,23*S,14,MUTED583);
   Place(RightTitle,ox+792*S,oy+884*S,210*S,23*S,14,MUTED583);
   Place(RightCount,ox+792*S,oy+911*S,210*S,42*S,33,INK583);
   Place(RightType,ox+792*S,oy+955*S,210*S,23*S,14,MUTED583);
   Place(Group,ox+1048*S,oy+886*S,322*S,28*S,19,INK583);
   Place(Feedback,ox+1048*S,oy+917*S,322*S,49*S,16,INK583);
   Place(Drive,ox+1048*S,oy+970*S,322*S,22*S,12,MUTED583);
   Place(SightAngles,ox+1410*S,oy+886*S,460*S,25*S,16,INK583);
   Place(TurretAngles,ox+1410*S,oy+920*S,460*S,25*S,16,INK583);
   Place(Reference,ox+48*S,oy+953*S,460*S,23*S,12,MUTED583);
  }
  Feedback.SetTextWrapping(HudStyle702==3);
  for(int i=0;i<HintRoots.Count();i++)
  {
   Widget hint=HintRoots[i];if(!hint)continue;
   FrameSlot.SetPos(hint,ox+(32+i*310)*S,oy+1020*S);FrameSlot.SetSize(hint,302*S,50*S);
   hint.SetVisible(true);
   if(Hints[i])Hints[i].SetSize(Math.Max(28,Math.Round(46*S)));
  }
 }
 void GraphicsCandidate702()
 {
  Draw.Clear();
  float ox=(W-1920*S)*0.5;float oy=(H-1080*S)*0.5;
  int accent=Accent702();
  if(HudStyle702==1)
  {
   Bar(ox+32*S,oy+20*S,500*S,60*S,0xc4111b1b);
   Bar(ox+760*S,oy+20*S,460*S,60*S,0xc4111b1b);
   Bar(ox+1468*S,oy+20*S,420*S,60*S,0xc4111b1b);
   Bar(ox+32*S,oy+870*S,484*S,116*S,0xc4111b1b);
   Bar(ox+562*S,oy+870*S,720*S,116*S,0xc4111b1b);
   Bar(ox+1300*S,oy+870*S,588*S,116*S,0xc4111b1b);
   Line(ox+32*S,oy+80*S,ox+532*S,oy+80*S,accent,1*S);
   Line(ox+760*S,oy+80*S,ox+1220*S,oy+80*S,accent,1*S);
   Line(ox+1468*S,oy+80*S,ox+1888*S,oy+80*S,accent,1*S);
   Line(ox+264*S,oy+886*S,ox+264*S,oy+970*S,accent,1*S);
   Line(ox+32*S,oy+870*S,ox+516*S,oy+870*S,accent,1*S);
   Line(ox+562*S,oy+870*S,ox+1282*S,oy+870*S,accent,1*S);
   Line(ox+1300*S,oy+870*S,ox+1888*S,oy+870*S,accent,1*S);
   Line(ox+48*S,oy+982*S,ox+240*S,oy+982*S,accent,1*S);
   Line(ox+282*S,oy+982*S,ox+474*S,oy+982*S,accent,1*S);
  }
  if(HudStyle702==2)
  {
   Bar(ox+32*S,oy+20*S,530*S,60*S,0xc4111b1b);
   Bar(ox+1480*S,oy+20*S,408*S,82*S,0xc4111b1b);
   Bar(ox+770*S,oy+620*S,480*S,66*S,0xc4111b1b);
   Bar(ox+32*S,oy+906*S,244*S,104*S,0xc4111b1b);
   Bar(ox+1648*S,oy+906*S,240*S,104*S,0xc4111b1b);
   Bar(ox+678*S,oy+906*S,624*S,80*S,0xc4111b1b);
   Line(ox+770*S,oy+684*S,ox+1250*S,oy+684*S,accent,1*S);
   Line(ox+32*S,oy+906*S,ox+276*S,oy+906*S,accent,1*S);
   Line(ox+1648*S,oy+906*S,ox+1888*S,oy+906*S,accent,1*S);
   Line(ox+678*S,oy+906*S,ox+1302*S,oy+906*S,accent,1*S);
  }
  if(HudStyle702==3)
  {
   Bar(ox+32*S,oy+20*S,1856*S,62*S,0xc4111b1b);
   Bar(ox+32*S,oy+868*S,1856*S,126*S,0xc4111b1b);
   Line(ox+32*S,oy+82*S,ox+1888*S,oy+82*S,accent,1*S);
   Line(ox+32*S,oy+868*S,ox+1888*S,oy+868*S,accent,1*S);
   Line(ox+516*S,oy+884*S,ox+516*S,oy+978*S,accent,1*S);
   Line(ox+768*S,oy+884*S,ox+768*S,oy+978*S,accent,1*S);
   Line(ox+1020*S,oy+884*S,ox+1020*S,oy+978*S,accent,1*S);
   Line(ox+1386*S,oy+884*S,ox+1386*S,oy+978*S,accent,1*S);
   Line(ox+540*S,oy+988*S,ox+742*S,oy+988*S,accent,1*S);
   Line(ox+792*S,oy+988*S,ox+994*S,oy+988*S,accent,1*S);
  }
  // Decorative open corners have no angular or range calibration.
  if(HudStyle702!=2)
  {
   Line(ox+32*S,oy+114*S,ox+62*S,oy+114*S,accent,1*S);Line(ox+32*S,oy+114*S,ox+32*S,oy+160*S,accent,1*S);
   Line(ox+1858*S,oy+114*S,ox+1888*S,oy+114*S,accent,1*S);Line(ox+1888*S,oy+114*S,ox+1888*S,oy+160*S,accent,1*S);
   Line(ox+32*S,oy+770*S,ox+32*S,oy+816*S,accent,1*S);Line(ox+32*S,oy+816*S,ox+62*S,oy+816*S,accent,1*S);
   Line(ox+1888*S,oy+770*S,ox+1888*S,oy+816*S,accent,1*S);Line(ox+1858*S,oy+816*S,ox+1888*S,oy+816*S,accent,1*S);
  }
  float x=W*0.5;float y=H*0.5;
  for(int layer=0;layer<2;layer++)
  {
   int color=0xff08100e;float width=5*S;if(layer==1){color=accent;width=2*S;}
   Line(x-40*S,y,x-12*S,y,color,width);Line(x+12*S,y,x+40*S,y,color,width);
   if(HudStyle702==2){Line(x-12*S,y+24*S,x,y+12*S,color,width);Line(x,y+12*S,x+12*S,y+24*S,color,width);}
   else{Line(x,y-40*S,x,y-12*S,color,width);Line(x,y+12*S,x,y+40*S,color,width);}
  }
  Canvas.SetDrawCommands(Draw);
 }
 // END GENERATED HUD OPTIONS702
 // Original approved-concept vector instrument geometry. Presentation only.
 void DLine704(float x1,float y1,float x2,float y2,int ink,float weight)
 {
  float ox=(W-1920*S)*0.5;float oy=(H-1080*S)*0.5;
  Line(ox+x1*S,oy+y1*S,ox+x2*S,oy+y2*S,ink,weight*S);
 }
 void Contour704(array<float> points,int ink,float weight)
 {
  int n=points.Count();if(n<6 || n%2!=0)return;
  for(int i=0;i<n;i+=2){int next=(i+2)%n;DLine704(points[i],points[i+1],points[next],points[next+1],ink,weight);}
 }
 void Panel704(array<float> points,array<int> triangles,int ink)
 {
  float dpi=GetGame().GetWorkspace().DPIScale(1);float ox=(W-1920*S)*0.5;float oy=(H-1080*S)*0.5;
  TriMeshDrawCommand command=new TriMeshDrawCommand();command.m_iColor=ink;command.m_Vertices={};command.m_Indices=triangles;
  for(int i=0;i<points.Count();i+=2){command.m_Vertices.Insert((ox+points[i]*S)*dpi);command.m_Vertices.Insert((oy+points[i+1]*S)*dpi);}
  Draw.Insert(command);
 }
 void Ring704(float cx,float cy,float radius,int ink,float weight,int segments=48)
 {
  for(int i=0;i<segments;i++)
  {
   float a=i*Math.PI2/segments;float b=(i+1)*Math.PI2/segments;
   DLine704(cx+Math.Sin(a)*radius,cy-Math.Cos(a)*radius,cx+Math.Sin(b)*radius,cy-Math.Cos(b)*radius,ink,weight);
  }
 }
 void FeedBars704(GEP_GunnerFeed583 feed,float x,float y,int ink)
 {
  bool valid=feed && feed.State==2 && feed.Count>=0 && feed.Capacity>0;
  float ratio=0;if(valid){ratio=feed.Count;ratio=Math.Clamp(ratio/feed.Capacity,0,1);}
  for(int i=0;i<16;i++)
  {
   float left=x+i*11;array<float> cell={left,y,left+7,y,left+7,y+29,left,y+29};
   Contour704(cell,0xff8d9188,0.8);
   float filled=Math.Clamp(ratio*16-i,0,1);
   if(valid && filled>0){DLine704(left+0.75+2.75*filled,y+1,left+0.75+2.75*filled,y+28,ink,5.5*filled);}
  }
 }
 void FeedGlyph704(float cx,float cy,int ink)
 {
  array<float> housing={cx-18,cy+18,cx-14,cy-9,cx+14,cy-9,cx+18,cy+18,cx+18,cy+33,cx-18,cy+33};
  Contour704(housing,ink,1.2);
  DLine704(cx-8,cy-9,cx-8,cy-61,ink,1.6);DLine704(cx-3,cy-9,cx-3,cy-61,ink,1.6);
  DLine704(cx+3,cy-9,cx+3,cy-61,ink,1.6);DLine704(cx+8,cy-9,cx+8,cy-61,ink,1.6);
  DLine704(cx-10,cy-60,cx-1,cy-60,ink,1.1);DLine704(cx+1,cy-60,cx+10,cy-60,ink,1.1);
  DLine704(cx-10,cy-54,cx-1,cy-54,ink,1.1);DLine704(cx+1,cy-54,cx+10,cy-54,ink,1.1);
 }
 void HullGlyph704(float cx,float cy)
 {
  int ink=0xffeee9cd;int detail=0xffadb2a5;
  // Fixed bow-up hull and two separately described track runs.
  array<float> hull={cx-22,cy-62,cx+22,cy-62,cx+31,cy-44,cx+31,cy+53,cx+22,cy+63,cx-22,cy+63,cx-31,cy+53,cx-31,cy-44};
  Contour704(hull,ink,1.8);
  for(int side=-1;side<=1;side+=2)
  {
   float sx=cx+side*36;
   array<float> track={sx-7,cy-45,sx+7,cy-45,sx+7,cy+57,sx-7,cy+57};Contour704(track,ink,1.4);
   for(int t=0;t<11;t++){float ty=cy-41+t*9;DLine704(sx-7,ty,sx+7,ty,detail,0.9);}
   for(int wheel=0;wheel<6;wheel++){Ring704(sx,cy-34+wheel*16,4.3,detail,0.8,12);}
  }
  DLine704(cx-22,cy-40,cx+22,cy-40,detail,1);DLine704(cx-22,cy+43,cx+22,cy+43,detail,1);
  DLine704(cx-17,cy+48,cx+17,cy+48,detail,1);DLine704(cx-17,cy+53,cx+17,cy+53,detail,1);
  array<float> hatch={cx-15,cy+21,cx-3,cy+21,cx-3,cy+34,cx-15,cy+34};Contour704(hatch,detail,1);
  Ring704(cx+11,cy+27,6,detail,1,16);Ring704(cx,cy-4,22,ink,1.5,40);
  // The unlabelled orientation ring has no world heading or ballistic scale.
  for(int tick=0;tick<60;tick++)
  {
   float angle=tick*Math.PI2/60;float end=96;
   DLine704(cx+Math.Sin(angle)*94,cy-Math.Cos(angle)*94,cx+Math.Sin(angle)*end,cy-Math.Cos(angle)*end,detail,1);
  }
  // Numeric and graphic direction are both actual hull-relative native aim.
  if(Snapshot.AimValid)
  {
   float angle=Snapshot.HullAzimuth*Math.DEG2RAD;float dx=Math.Sin(angle);float dy=-Math.Cos(angle);float px=Math.Cos(angle);float py=Math.Sin(angle);
   for(int barrel=-1;barrel<=1;barrel+=2)
   {
    float offset=barrel*9;
    DLine704(cx+px*offset+dx*14,cy+py*offset+dy*14,cx+px*offset+dx*73,cy+py*offset+dy*73,ink,2);
    DLine704(cx+px*(offset+3)+dx*14,cy+py*(offset+3)+dy*14,cx+px*(offset+3)+dx*73,cy+py*(offset+3)+dy*73,ink,0.9);
   }
   DLine704(cx,cy,cx+dx*102,cy+dy*102,0xffffcc80,1.8);
   DLine704(cx+dx*108,cy+dy*108,cx+dx*96+px*7,cy+dy*96+py*7,0xffffcc80,2.5);
   DLine704(cx+dx*108,cy+dy*108,cx+dx*96-px*7,cy+dy*96-py*7,0xffffcc80,2.5);
  }
 }
 string NumberDE704(float value,int decimals=1)
 {
  string text=value.ToString(-1,decimals);text.Replace(".",",");return text;
 }
 string FeedCountDE704(GEP_GunnerFeed583 feed)
 {
  if(!feed || feed.State!=2 || feed.Count<0)return "--";
  return feed.Count.ToString();
 }
 string FeedTypeDE704(GEP_GunnerFeed583 feed)
 {
  if(!feed || feed.State==0)return "NICHT VERFÜGBAR";
  if(feed.State==1)return "KEIN MAGAZIN";
  if(feed.Ammunition=="HEI-T")return "HEI-T";
  return "UNBEKANNT";
 }
 string OperationalFeedbackDE704()
 {
  if(!Snapshot.Crew)return GEP_Presentation680.DisabledLabel(GetOwner(),Snapshot.OperationalReason680);
  if(Snapshot.DriveValid && Snapshot.DriveFault)return "TURMANTRIEB GESTÖRT";
  // Do not forward raw FireControl development status or claim unavailable assists.
  return "";
 }

 // BEGIN INTEGRATED SIGHT703
 // Creative Gepard-inspired graphic. No copied/ballistically calibrated scales.
 void LayoutSight703()
 {
  S=Math.Min(W/1920,H/1080);float ox=(W-1920*S)*0.5;float oy=(H-1080*S)*0.5;
  Place(Header,ox+78*S,oy+46*S,380*S,42*S,25,0xffeee9cd);
  Header.SetBold(true);
  Place(OpticalMode,ox+1510*S,oy+46*S,180*S,42*S,26,0xffeee9cd);
  OpticalMode.SetBold(true);
  Place(Zoom,ox+1750*S,oy+46*S,130*S,42*S,27,0xffeee9cd);
  Zoom.SetBold(true);
  Place(LeftTitle,ox+140*S,oy+870*S,285*S,35*S,25,0xffeee9cd);
  LeftTitle.SetBold(true);
  Place(LeftCount,ox+140*S,oy+911*S,130*S,76*S,60,0xffeee9cd);
  LeftCount.SetBold(true);
  Place(LeftType,ox+140*S,oy+987*S,300*S,32*S,23,0xffeee9cd);
  LeftType.SetBold(true);
  Place(RightTitle,ox+585*S,oy+870*S,285*S,35*S,25,0xffeee9cd);
  RightTitle.SetBold(true);
  Place(RightCount,ox+585*S,oy+911*S,130*S,76*S,60,0xffeee9cd);
  RightCount.SetBold(true);
  Place(RightType,ox+585*S,oy+987*S,300*S,32*S,23,0xffeee9cd);
  RightType.SetBold(true);
  Place(Reference,ox+1054*S,oy+891*S,390*S,37*S,23,0xffeee9cd);
  Reference.SetBold(true);
  int rangeFont704=50;if(Snapshot.RangeState676==EGEP_Range676State.NO_RETURN || Snapshot.RangeState676==EGEP_Range676State.UNAVAILABLE)rangeFont704=23;
  Place(Range676,ox+1010*S,oy+936*S,430*S,69*S,rangeFont704,0xffeee9cd);
  Range676.SetBold(true);
  Place(Feedback,ox+963*S,oy+1005*S,475*S,30*S,18,0xffeee9cd);
  Feedback.SetBold(true);
  Place(Group,ox+1763*S,oy+850*S,130*S,34*S,23,0xffeee9cd);
  Group.SetBold(true);
  Place(Drive,ox+1763*S,oy+957*S,130*S,34*S,23,0xffeee9cd);
  Drive.SetBold(true);
  Place(SightAngles,ox+1763*S,oy+886*S,130*S,52*S,37,0xffeee9cd);
  SightAngles.SetBold(true);
  Place(TurretAngles,ox+1763*S,oy+991*S,130*S,52*S,37,0xffeee9cd);
  TurretAngles.SetBold(true);
  Drive.SetVisible(true);Reference.SetVisible(true);Group.SetVisible(true);Feedback.SetTextWrapping(false);
  foreach(Widget hint:HintRoots){if(hint)hint.SetVisible(false);}
 }
 void TextSight703()
 {
  Put(Header,"GEPARD  |  ZIELVISIER");
  string mode="--";if(Snapshot.OpticsValid){mode="TAG";if(Snapshot.Night)mode="RESTLICHT";}Put(OpticalMode,mode);
  string zoom="--×";if(Snapshot.ZoomValid)zoom=NumberDE704(Snapshot.Magnification)+"×";Put(Zoom,zoom);
  Put(LeftTitle,"LINKS");Put(RightTitle,"RECHTS");
  Put(LeftCount,FeedCountDE704(Snapshot.Left));Put(RightCount,FeedCountDE704(Snapshot.Right));
  Put(LeftType,FeedTypeDE704(Snapshot.Left));Put(RightType,FeedTypeDE704(Snapshot.Right));
  int leftInk=0xffeee9cd;int rightInk=0xffeee9cd;
  if(Snapshot.Left.State!=2)leftInk=MUTED583;if(Snapshot.Right.State!=2)rightInk=MUTED583;
  if(Snapshot.Left.State==2 && Snapshot.Left.Count==0)leftInk=WARN583;
  if(Snapshot.Right.State==2 && Snapshot.Right.Count==0)rightInk=WARN583;
  LeftCount.SetColor(Color.FromInt(leftInk));RightCount.SetColor(Color.FromInt(rightInk));
  string distance="-- m";string measured="ENTFERNUNG";
  if(Snapshot.RangeState676==EGEP_Range676State.VALID && Snapshot.RangeMetres676>0 && Snapshot.RangeAge676>=0)distance=Math.Round(Snapshot.RangeMetres676).ToString()+" m";
  else if(Snapshot.RangeState676==EGEP_Range676State.NO_RETURN)distance="KEIN MESSWERT";
  else if(Snapshot.RangeState676==EGEP_Range676State.UNAVAILABLE)distance="MESSUNG NICHT VERFÜGBAR";
  if(Snapshot.RangeState676==EGEP_Range676State.VALID && Snapshot.RangeAge676>10)measured="ENTFERNUNG / VERALTET";
  Put(Range676,distance);Put(Reference,measured);
  int rangeInk=0xffeee9cd;if(Snapshot.RangeState676==EGEP_Range676State.NO_RETURN || Snapshot.RangeState676==EGEP_Range676State.UNAVAILABLE || Snapshot.RangeAge676>10)rangeInk=WARN583;
  Range676.SetColor(Color.FromInt(rangeInk));Reference.SetColor(Color.FromInt(rangeInk));
  Put(Group,"TURM");Put(Drive,"HÖHE");
  string turret="--°";string elevation="--°";
  if(Snapshot.AimValid){turret=NumberDE704(Snapshot.HullAzimuth,0)+"°";elevation=NumberDE704(Snapshot.GunElevation,0)+"°";}
  Put(SightAngles,turret);Put(TurretAngles,elevation);
  string operational=OperationalFeedbackDE704();Put(Feedback,operational);Feedback.SetVisible(operational!="");Feedback.SetColor(Color.FromInt(WARN583));
 }
 void GraphicsSight703()
 {
  Draw.Clear();float ox=(W-1920*S)*0.5;float oy=(H-1080*S)*0.5;float x=W*0.5;float y=H*0.5;
  // The accepted seven-segment open-centre reticle remains the exact aiming datum.
  for(int layer=0;layer<2;layer++)
  {
   int color=0xff09100e;float stroke=6.8*S;if(layer==1){color=0xffeee9cd;stroke=3*S;}
   Line(x-144*S*1.4,y+18*S*1.4,x-96*S*1.4,y,color,stroke);
   Line(x-96*S*1.4,y,x-72*S*1.4,y,color,stroke);
   Line(x+72*S*1.4,y,x+96*S*1.4,y,color,stroke);
   Line(x+96*S*1.4,y,x+144*S*1.4,y+18*S*1.4,color,stroke);
   Line(x-22*S*1.4,y+13*S*1.4,x-7*S*1.4,y,color,stroke);
   Line(x+7*S*1.4,y,x+22*S*1.4,y+13*S*1.4,color,stroke);
   Line(x,y-62*S*1.4,x,y-18*S*1.4,color,stroke);
  }
  array<float> panel0704={28,862,48,846,495,846,501,851,835,851,871,881,901,881,922,867,1430,867,1453,881,1472,881,1518,830,1544,830,1576,799,1686,799,1718,830,1860,830,1886,848,1892,868,1892,1024,1874,1040,1520,1040,1504,1028,901,1028,883,1016,263,1016,243,1028,47,1028,28,1012};
  array<int> triangles0704={28,0,1,28,1,2,28,2,3,28,3,4,28,4,5,28,5,6,6,7,8,28,6,8,28,8,9,28,9,10,10,11,12,12,13,14,10,12,14,10,14,15,10,15,16,10,16,17,28,10,17,28,17,18,18,19,20,18,20,21,18,21,22,18,22,23,18,23,24,28,18,24,28,24,25,28,25,26,26,27,28};
  Panel704(panel0704,triangles0704,0xc511181a);
  Contour704(panel0704,0xcce6e3cd,1.6);
  array<float> panel1704={36,38,512,38,448,96,36,96};
  array<int> triangles1704={3,0,1,1,2,3};
  Panel704(panel1704,triangles1704,0x9811181a);
  Contour704(panel1704,0xcce6e3cd,1.1);
  array<float> panel2704={1410,38,1885,38,1885,96,1474,96};
  array<int> triangles2704={3,0,1,1,2,3};
  Panel704(panel2704,triangles2704,0x9811181a);
  Contour704(panel2704,0xcce6e3cd,1.1);
  // Short inset edge details follow the approved tapered instrument panel.
  DLine704(47,853,492,853,0xff9b9f91,0.9);DLine704(930,874,1421,874,0xff9b9f91,0.9);DLine704(1581,806,1681,806,0xff9b9f91,0.9);
  DLine704(465,864,465,1009,0xffd8d7c3,1.1);DLine704(929,890,929,1008,0xffd8d7c3,1.1);DLine704(1416,889,1416,1008,0xffd8d7c3,1.1);DLine704(1760,930,1867,930,0xffd8d7c3,1);
  DLine704(244,884,425,884,0xff818b80,1);DLine704(691,884,871,884,0xff818b80,1);DLine704(1692,51,1692,79,0xffd8d7c3,1);
  // Open picture frame does not crop or mask the world.
  DLine704(36,115,81,115,0xcce6e3cd,1.4);DLine704(36,115,36,274,0xcce6e3cd,1.4);DLine704(36,488,36,795,0xcce6e3cd,1.4);DLine704(36,795,81,795,0xcce6e3cd,1.4);
  DLine704(1839,115,1884,115,0xcce6e3cd,1.4);DLine704(1884,115,1884,274,0xcce6e3cd,1.4);DLine704(1884,488,1884,795,0xcce6e3cd,1.4);DLine704(1839,795,1884,795,0xcce6e3cd,1.4);
  int leftInk=0xffa3aa9c;int rightInk=0xffa3aa9c;
  if(Snapshot.Crew && (Snapshot.Group==0 || Snapshot.Group==1))leftInk=0xffeee9cd;if(Snapshot.Crew && (Snapshot.Group==0 || Snapshot.Group==2))rightInk=0xffeee9cd;
  FeedGlyph704(92,946,leftInk);FeedGlyph704(537,946,rightInk);FeedBars704(Snapshot.Left,275,936,leftInk);FeedBars704(Snapshot.Right,720,936,rightInk);
  // Range brackets frame information, not a tracking box.
  DLine704(1017,895,1032,895,0xffd8d7c3,1.2);DLine704(1017,895,1017,910,0xffd8d7c3,1.2);DLine704(1395,895,1410,895,0xffd8d7c3,1.2);DLine704(1410,895,1410,910,0xffd8d7c3,1.2);
  DLine704(1017,987,1017,1002,0xffd8d7c3,1.2);DLine704(1017,1002,1032,1002,0xffd8d7c3,1.2);DLine704(1395,1002,1410,1002,0xffd8d7c3,1.2);DLine704(1410,987,1410,1002,0xffd8d7c3,1.2);
  HullGlyph704(1620,937);Canvas.SetDrawCommands(Draw);
 }
 // END INTEGRATED SIGHT703
 void RefreshHints()
 {
  for(int i=0;i<Hints.Count();i++)
  {
   if(!Hints[i])continue;
   Hints[i].SetLabel(HintLabels[i]);Hints[i].SetAction(BindingAction584(i),-1,true);
  }
 }
 override void OnPostInit(IEntity owner){SetEventMask(owner,EntityEvent.FRAME);}
 override void EOnFrame(IEntity owner,float timeSlice)
 {
  IEntity currentUser702;BaseCompartmentSlot currentSeat702;SCR_2DSightsComponent currentSight702;string currentReason702;
  bool currentPresentation702=GEP_Presentation680.Evaluate(owner,currentUser702,currentSeat702,currentSight702,currentReason702);
  // Hide on the first frame of a menu, death, transition, lost owner or seat change.
  if(!currentPresentation702)
  {
   ClearPresentation702();
   if(Root)Root.SetVisible(false);if(Plain680)Plain680.Hide();
   Snapshot.Reset();Timer=0;HintTimer=2;return;
  }
  if(InstrumentsVisible702 && (currentUser702!=PresentedUser702 || currentSeat702!=PresentedSeat702 || currentSight702!=PresentedSight702))
  {
   ClearPresentation702();if(Root)Root.SetVisible(false);if(Plain680)Plain680.Hide();
   Snapshot.Reset();Timer=0;HintTimer=2;
  }
  SightHelpAge703+=Math.Max(0,timeSlice);
  Timer+=timeSlice;HintTimer+=timeSlice;
  if(Timer<0.05)return;Timer=0;
  Snapshot.Sample(owner);
  if(!Snapshot.Presentation680 || !Snapshot.ADS){ClearPresentation702();if(Root)Root.SetVisible(false);if(Plain680)Plain680.Hide();return;}
  EnsureVisual680();
  if(!Ready680()){Minimal680();return;}
  if(Plain680)Plain680.Hide();Root.SetVisible(true);ShowInstrumentWidgets680();Layout();Text();Graphics();
  string presentedReason702;
  InstrumentsVisible702=GEP_Presentation680.Evaluate(owner,PresentedUser702,PresentedSeat702,PresentedSight702,presentedReason702);
  if(HintTimer>=1){HintTimer=0;RefreshHints();}
 }
 override void OnDelete(IEntity owner)
 {
  ClearPresentation702();
  if(Plain680)Plain680.Close();
  if(Root)Root.RemoveFromHierarchy();Root=null;HintRoots.Clear();Hints.Clear();Draw.Clear();Snapshot.Reset();
 }
}

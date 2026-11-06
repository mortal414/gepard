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
 [Attribute("0",UIWidgets.EditBox,"0 current; 1-3 rejected archives; 4 integrated sight703; 5 isolated open instrument study740. Visuals only.")]
 int HudStyle702;
 [Attribute("2",UIWidgets.EditBox,"0 fallback teal; 1 amber; 2 ice blue; 3 mineral. Presentation only.")]
 int HudPalette740;
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
 TextWidget TopSubtitle740;
 TextWidget TopBrand740;
 TextWidget TopMode740;
 TextWidget TopZoom740;
 TextWidget TopBearing740;
 TextWidget TopBearingTitle740;
 TextWidget RangeDetail740;
 TextWidget OrientationHeader740;
 TextWidget LeftCapacity740;
 TextWidget RightCapacity740;
 ref array<TextWidget> ElevationLabels740={};
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
 bool EntityLockCue715;
 IEntity PresentedUser702;
 BaseCompartmentSlot PresentedSeat702;
 SCR_2DSightsComponent PresentedSight702;
 bool ReplacesWeaponInfo702(IEntity user,BaseCompartmentSlot seat,SCR_2DSightsComponent sight)
 {
  return InstrumentsVisible702 && Ready680() && Root.IsVisible() && user==PresentedUser702 && seat==PresentedSeat702 && sight==PresentedSight702;
 }
 void ClearPresentation702()
 {
  EntityLockCue715=false;InstrumentsVisible702=false;PresentedUser702=null;PresentedSeat702=null;PresentedSight702=null;SightHelpAge703=0;
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
  if(HudStyle702==5)
  {
   if(!TopSubtitle740 || !TopBrand740 || !TopMode740 || !TopZoom740 || !TopBearing740 || !TopBearingTitle740 || !RangeDetail740 || !OrientationHeader740 || !LeftCapacity740 || !RightCapacity740)return false;
   if(ElevationLabels740.Count()!=7)return false;
   foreach(TextWidget elevation740:ElevationLabels740){if(!elevation740)return false;}
  }
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
  if(HudStyle702==5)
  {
   TopSubtitle740=Label("GepardTopSubtitle740");
   TopBrand740=Label("GepardTopBrand740");TopMode740=Label("GepardTopMode740");TopZoom740=Label("GepardTopZoom740");TopBearing740=Label("GepardTopBearing740");TopBearingTitle740=Label("GepardTopBearingTitle740");
   array<TextWidget> topFields740={TopBrand740,TopSubtitle740,TopMode740,TopZoom740,TopBearing740,TopBearingTitle740};foreach(TextWidget topField740:topFields740){if(topField740){topField740.SetVisible(false);topField740.SetShadow(0,0xff08100e,0,0,0);}}
   RangeDetail740=Label("GepardRangeDetail740");OrientationHeader740=Label("GepardOrientationHeader740");LeftCapacity740=Label("GepardLeftCapacity740");RightCapacity740=Label("GepardRightCapacity740");
   if(RangeDetail740)RangeDetail740.SetVisible(false);if(OrientationHeader740)OrientationHeader740.SetVisible(false);if(LeftCapacity740)LeftCapacity740.SetVisible(false);if(RightCapacity740)RightCapacity740.SetVisible(false);
   for(int elevationIndex740=0;elevationIndex740<7;elevationIndex740++){TextWidget elevation740=Label("GepardElevation740_"+elevationIndex740.ToString());ElevationLabels740.Insert(elevation740);if(elevation740)elevation740.SetVisible(false);}
  }
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
  Header=null;OpticalMode=null;Zoom=null;Range676=null;TopSubtitle740=null;TopBrand740=null;TopMode740=null;TopZoom740=null;TopBearing740=null;TopBearingTitle740=null;RangeDetail740=null;OrientationHeader740=null;LeftCapacity740=null;RightCapacity740=null;ElevationLabels740.Clear();
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
  string fault="ANZEIGE NICHT VERFUEGBAR";string reason;
  if(!GEP_GunnerSnapshot583.Evaluate679(GetOwner(),reason))fault=GEP_Presentation680.DisabledLabel(GetOwner(),reason);
  if(Plain680)Plain680.Show(fault);
 }
 void Layout()
 {
  if(TopSubtitle740)TopSubtitle740.SetVisible(false);if(TopBrand740)TopBrand740.SetVisible(false);if(TopMode740)TopMode740.SetVisible(false);if(TopZoom740)TopZoom740.SetVisible(false);if(TopBearing740)TopBearing740.SetVisible(false);if(TopBearingTitle740)TopBearingTitle740.SetVisible(false);
  array<TextWidget> resetShadow740={LeftTitle,RightTitle,LeftType,RightType,LeftCount,RightCount,OpticalMode,Group,Reference,Range676,SightAngles,Feedback};foreach(TextWidget shadow740:resetShadow740){if(shadow740)shadow740.SetShadow(2,0xff08100e,1,0,1);}
  if(Feedback)Feedback.ClearFlags(WidgetFlags.CENTER);if(Reference)Reference.ClearFlags(WidgetFlags.CENTER);if(Range676)Range676.ClearFlags(WidgetFlags.CENTER);
  if(LeftType)LeftType.ClearFlags(WidgetFlags.RALIGN);if(RightType)RightType.ClearFlags(WidgetFlags.RALIGN);
  if(RangeDetail740)RangeDetail740.SetVisible(false);if(OrientationHeader740)OrientationHeader740.SetVisible(false);if(LeftCapacity740)LeftCapacity740.SetVisible(false);if(RightCapacity740)RightCapacity740.SetVisible(false);
  foreach(TextWidget elevation740:ElevationLabels740){if(elevation740)elevation740.SetVisible(false);}
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
  if(HudStyle702==5)LayoutStudy740();
 }
 void Text()
 {
  if(HudStyle702==4){TextSight703();return;}
  if(HudStyle702==5){TextStudy740();return;}
  Put(Header,"GEPARD / ZIELVISIER");
  string mode="--";if(Snapshot.OpticsValid){mode="TAG";if(Snapshot.Night)mode="RESTLICHT";}Put(OpticalMode,mode);
  string zoom="--Ã—";if(Snapshot.ZoomValid)zoom=NumberDE704(Snapshot.Magnification)+"Ã—";Put(Zoom,zoom);
  Put(LeftTitle,"LINKS");Put(RightTitle,"RECHTS");Put(LeftCount,FeedCountDE704(Snapshot.Left));Put(RightCount,FeedCountDE704(Snapshot.Right));
  Put(LeftType,FeedTypeDE704(Snapshot.Left));Put(RightType,FeedTypeDE704(Snapshot.Right));
  string range="ENTFERNUNG --";
  if(Snapshot.RangeState676==EGEP_Range676State.VALID && Snapshot.RangeMetres676>0 && Snapshot.RangeAge676>=0)range="ENTFERNUNG "+Math.Round(Snapshot.RangeMetres676).ToString()+" m";
  else if(Snapshot.RangeState676==EGEP_Range676State.NO_RETURN)range="KEIN MESSWERT";
  else if(Snapshot.RangeState676==EGEP_Range676State.UNAVAILABLE)range="MESSUNG NICHT VERFÃœGBAR";
  Put(Range676,range);string group="WAHL --";if(Snapshot.Group==0)group="BEIDE";if(Snapshot.Group==1)group="LINKS";if(Snapshot.Group==2)group="RECHTS";Put(Group,group);
  Put(Feedback,OperationalFeedbackDE704());Put(Drive,"");Put(SightAngles,"");Put(TurretAngles,"");Put(Reference,"");
 }
 void Graphics()
 {
  if(HudStyle702==4){GraphicsSight703();return;}
  if(HudStyle702==5){GraphicsStudy740();return;}
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
  if(!feed || feed.State==0)return "NICHT VERFUEGBAR";
  if(feed.State==1)return "KEIN MAGAZIN";
  if(feed.Ammunition=="HEI-T")return "HEI-T";
  return "UNBEKANNT";
 }
 string OperationalFeedbackDE704()
 {
  if(!Snapshot.Crew)return GEP_Presentation680.DisabledLabel(GetOwner(),Snapshot.OperationalReason680);
  if(Snapshot.DriveValid && Snapshot.DriveFault)return "TURMANTRIEB GESTOERT";
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
  string zoom="--x";if(Snapshot.ZoomValid)zoom=NumberDE704(Snapshot.Magnification)+"x";Put(Zoom,zoom);
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
  else if(Snapshot.RangeState676==EGEP_Range676State.UNAVAILABLE)distance="MESSUNG NICHT VERFUEGBAR";
  if(Snapshot.RangeState676==EGEP_Range676State.VALID && Snapshot.RangeAge676>10)measured="ENTFERNUNG / VERALTET";
  Put(Range676,distance);Put(Reference,measured);
  int rangeInk=0xffeee9cd;if(Snapshot.RangeState676==EGEP_Range676State.NO_RETURN || Snapshot.RangeState676==EGEP_Range676State.UNAVAILABLE || Snapshot.RangeAge676>10)rangeInk=WARN583;
  Range676.SetColor(Color.FromInt(rangeInk));Reference.SetColor(Color.FromInt(rangeInk));
  Put(Group,"TURM GR.");Put(Drive,"HOEHE GR.");
  string turret="--";string elevation="--";
  if(Snapshot.AimValid){turret=NumberDE704(Snapshot.HullAzimuth,0)+"";elevation=NumberDE704(Snapshot.GunElevation,0)+"";}
  Put(SightAngles,turret);Put(TurretAngles,elevation);
  string operational=OperationalFeedbackDE704();Put(Feedback,operational);Feedback.SetVisible(operational!="");Feedback.SetColor(Color.FromInt(WARN583));
 }
 void GraphicsSight703()
 {
  Draw.Clear();float ox=(W-1920*S)*0.5;float oy=(H-1080*S)*0.5;float x=W*0.5;float y=H*0.5;
  // Central aiming geometry is unchanged; confirmed entity tracking adds only
  // mint end caps to the existing outer horizontal arms.
  for(int layer=0;layer<2;layer++)
  {
   int color=0xff09100e;float stroke=6.8*S;if(layer==1){color=0xffeee9cd;stroke=3*S;}
   Line(x-144*S*1.4,y+18*S*1.4,x-96*S*1.4,y,color,stroke);
   int armInk715=color;if(layer==1 && EntityLockCue715)armInk715=ACTIVE583;
   Line(x-96*S*1.4,y,x-72*S*1.4,y,armInk715,stroke);
   Line(x+72*S*1.4,y,x+96*S*1.4,y,armInk715,stroke);
   if(EntityLockCue715)
   {
    Line(x-72*S*1.4,y-5*S,x-72*S*1.4,y+5*S,armInk715,stroke);
    Line(x+72*S*1.4,y-5*S,x+72*S*1.4,y+5*S,armInk715,stroke);
   }
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
 // BEGIN ISOLATED HUD STUDY740
 // Open optical instrument study. Read-only, opt-in and not native-qualified.
 const int STUDY_INK740=0xffe8e1c8;
 const int STUDY_MUTED740=0xffa6ad9c;
 const int STUDY_ACTIVE740=0xffb9e6a7;
 int LOWER_INK740=0xffe0e8e2;
 int LOWER_MUTED740=0xff9aaea4;
 int LOWER_ACTIVE740=0xff89b6a6;
 int RAIL740=0xff18211f;
 int RAIL_LINE740=0xff445650;
 int BRAND740=0xff799188;
 int MODE_INK740=0xffb7c7be;
 int LOWER_WARN740=0xffffc36b;
 const int STUDY_WARN740=0xffffc36b;
 void ConfigurePaletteStudy740()
 {
  LOWER_INK740=0xffe0e8e2;  LOWER_MUTED740=0xff9aaea4;  LOWER_ACTIVE740=0xff89b6a6;  RAIL740=0xff18211f;  RAIL_LINE740=0xff445650;  BRAND740=0xff799188;  MODE_INK740=0xffb7c7be;  LOWER_WARN740=0xffffc36b;
  if(HudPalette740==1)
  {
   LOWER_INK740=0xfff1bd74;   LOWER_MUTED740=0xffbda47c;   LOWER_ACTIVE740=0xffd99b49;   RAIL740=0xff15130f;   RAIL_LINE740=0xff55452d;   BRAND740=0xff9e865f;   MODE_INK740=0xffe0bd8b;   LOWER_WARN740=0xffff7e70;
  }
  if(HudPalette740==2)
  {
   LOWER_INK740=0xffe2f0ff;   LOWER_MUTED740=0xff9bb4cf;   LOWER_ACTIVE740=0xff6cbce8;   RAIL740=0xff101a2a;   RAIL_LINE740=0xff334a63;   BRAND740=0xff7d99b8;   MODE_INK740=0xffbfd9ed;   LOWER_WARN740=0xffffc36b;
  }
  if(HudPalette740==3)
  {
   LOWER_INK740=0xff20262b;   LOWER_MUTED740=0xff3e4a53;   LOWER_ACTIVE740=0xff235273;   RAIL740=0xffc8c5bd;   RAIL_LINE740=0xff959d9f;   BRAND740=0xff475866;   MODE_INK740=0xff334855;   LOWER_WARN740=0xff8a2f1e;
  }
 }
 bool ValidRange740(){return Snapshot.RangeState676==EGEP_Range676State.VALID && Snapshot.RangeMetres676>0 && Snapshot.RangeAge676>=0;}
 bool FeedSelected740(int side){return Snapshot.Crew && (Snapshot.Group==0 || Snapshot.Group==side);}
 int FeedInk740(GEP_GunnerFeed583 feed,int side)
 {
  if(feed && feed.State==2 && feed.Count==0){return LOWER_WARN740;}
  else{return LOWER_INK740;}
 }
 int FeedBarInk740(GEP_GunnerFeed583 feed,int side)
 {
  if(feed && feed.State==2 && feed.Count==0){return LOWER_WARN740;}
  else if(FeedSelected740(side)){return LOWER_ACTIVE740;}
  else{return LOWER_MUTED740;}
 }
 float ElevationTop740(){return Math.Floor(Snapshot.GunElevation/10)*10+40;}
 string FeedCapacityText740(GEP_GunnerFeed583 feed)
 {
  if(feed && feed.State==2 && feed.Capacity>0){return "/"+feed.Capacity.ToString();}
  else{return "/--";}
 }
 string FeedTypeStudy740(GEP_GunnerFeed583 feed)
 {
  if(!feed || feed.State==0){return "KEINE DATEN";}
  else if(feed.State==1){return "KEIN MAG.";}
  else{return FeedTypeDE704(feed);}
 }
 void PlaceLowerStudy740(TextWidget field,float x,float y,float width,float height,int font,int ink)
 {
  float ox=(W-1920*S)*0.5;Place(field,ox+x*S,H-(1080-y)*S,width*S,height*S,Math.Round(font*0.9),ink);field.SetShadow(0,0xff08100e,0,0,0);
 }
 void LowerLineStudy740(float x1,float y1,float x2,float y2,int ink,float width)
 {
  float ox=(W-1920*S)*0.5;Line(ox+x1*S,H-(1080-y1)*S,ox+x2*S,H-(1080-y2)*S,ink,width*S);
 }
 void LowerPanelStudy740(array<float> points,array<int> triangles,int ink)
 {
  float dpi=GetGame().GetWorkspace().DPIScale(1);float ox=(W-1920*S)*0.5;TriMeshDrawCommand command=new TriMeshDrawCommand();command.m_iColor=ink;command.m_Vertices={};command.m_Indices=triangles;
  for(int i=0;i<points.Count();i+=2){command.m_Vertices.Insert((ox+points[i]*S)*dpi);command.m_Vertices.Insert((H-(1080-points[i+1])*S)*dpi);}Draw.Insert(command);
 }
 void LowerContourStudy740(array<float> points,int ink,float width)
 {
  for(int i=0;i<points.Count();i+=2){int next=(i+2)%points.Count();LowerLineStudy740(points[i],points[i+1],points[next],points[next+1],ink,width);}
 }
 void LayoutStudy740()
 {
  ConfigurePaletteStudy740();
  S=Math.Min(W/1920,H/1080);float ox=(W-1920*S)*0.5;float oy=(H-1080*S)*0.5;
  Place(Header,ox+1746*S,oy+145*S,174*S,38*S,23,STUDY_INK740);
  Place(TurretAngles,ox+1746*S,oy+699*S,174*S,45*S,29,STUDY_INK740);
  PlaceLowerStudy740(LeftTitle,52,955,130,35,25,LOWER_INK740);
  PlaceLowerStudy740(RightTitle,1634,955,130,35,25,LOWER_INK740);
  int leftTypeFont740=16;int rightTypeFont740=16;if(Snapshot.Left && Snapshot.Left.Ammunition=="HEI-T")leftTypeFont740=20;if(Snapshot.Right && Snapshot.Right.Ammunition=="HEI-T")rightTypeFont740=20;
  PlaceLowerStudy740(LeftType,186,962,100,29,leftTypeFont740,LOWER_MUTED740);LeftType.SetFlags(WidgetFlags.RALIGN);
  PlaceLowerStudy740(RightType,1768,962,100,29,rightTypeFont740,LOWER_MUTED740);RightType.SetFlags(WidgetFlags.RALIGN);
  PlaceLowerStudy740(LeftCount,169,985,72,68,58,LOWER_INK740);
  PlaceLowerStudy740(RightCount,1751,985,72,68,58,LOWER_INK740);
  PlaceLowerStudy740(LeftCapacity740,246,1015,40,32,23,LOWER_MUTED740);LeftCapacity740.SetFlags(WidgetFlags.RALIGN);
  PlaceLowerStudy740(RightCapacity740,1828,1015,40,32,23,LOWER_MUTED740);RightCapacity740.SetFlags(WidgetFlags.RALIGN);
  LeftTitle.SetMinFontSize(Math.Max(12,Math.Round(12*S)));RightTitle.SetMinFontSize(Math.Max(12,Math.Round(12*S)));
  LeftCount.SetMinFontSize(Math.Max(12,Math.Round(30*S)));RightCount.SetMinFontSize(Math.Max(12,Math.Round(30*S)));
  LeftCapacity740.SetMinFontSize(Math.Max(12,Math.Round(14*S)));RightCapacity740.SetMinFontSize(Math.Max(12,Math.Round(14*S)));
  LeftType.SetMinFontSize(Math.Max(12,Math.Round(12*S)));RightType.SetMinFontSize(Math.Max(12,Math.Round(12*S)));
  PlaceLowerStudy740(OpticalMode,438,976,250,31,20,LOWER_MUTED740);
  PlaceLowerStudy740(Group,438,1000,280,41,31,LOWER_ACTIVE740);
  PlaceLowerStudy740(Reference,729,961,342,32,22,LOWER_INK740);Reference.SetFlags(WidgetFlags.CENTER);
  int rangeFont740=26;if(ValidRange740() || Snapshot.RangeState676==EGEP_Range676State.NONE)rangeFont740=48;
  PlaceLowerStudy740(Range676,711,982,378,59,rangeFont740,LOWER_INK740);Range676.SetFlags(WidgetFlags.CENTER);
  PlaceLowerStudy740(RangeDetail740,666,1036,468,32,20,LOWER_MUTED740);RangeDetail740.SetFlags(WidgetFlags.CENTER);
  PlaceLowerStudy740(OrientationHeader740,1180,977,234,31,20,LOWER_MUTED740);
  PlaceLowerStudy740(SightAngles,1180,997,198,48,39,LOWER_INK740);
  PlaceLowerStudy740(Feedback,438,956,306,31,20,LOWER_ACTIVE740);
  if(OperationalFeedbackDE704()!=""){Place(Feedback,ox+650*S,oy+173*S,620*S,42*S,25,STUDY_WARN740);Feedback.SetFlags(WidgetFlags.CENTER);Feedback.SetShadow(2,0xff08100e,1,0,1);}
  RangeDetail740.SetVisible(true);OrientationHeader740.SetVisible(true);LeftCapacity740.SetVisible(true);RightCapacity740.SetVisible(true);
  Zoom.SetVisible(false);Drive.SetVisible(false);
  Place(TopBrand740,34*S,3*S,100*S,30*S,21,LOWER_MUTED740);
  Place(TopSubtitle740,160*S,4*S,700*S,28*S,17,BRAND740);TopSubtitle740.SetVisible(true);
  float modeX740=W-640*S;float modeW740=52*S;if(Snapshot.OpticsValid && Snapshot.Night){modeX740=W-710*S;modeW740=122*S;}
  Place(TopMode740,modeX740,4*S,modeW740,28*S,17,MODE_INK740);
  Place(TopZoom740,W-566*S,3*S,68*S,29*S,18,LOWER_INK740);
  Place(TopBearing740,W-150*S,2*S,116*S,31*S,20,LOWER_INK740);TopBearing740.SetFlags(WidgetFlags.RALIGN);
  Place(TopBearingTitle740,W-470*S,4*S,230*S,28*S,16,LOWER_MUTED740);
  TopBrand740.SetVisible(true);TopMode740.SetVisible(true);TopZoom740.SetVisible(true);TopBearing740.SetVisible(true);TopBearingTitle740.SetVisible(true);
  Header.SetBold(true);LeftTitle.SetBold(true);RightTitle.SetBold(true);OpticalMode.SetBold(true);Group.SetBold(true);Reference.SetBold(true);Feedback.SetBold(true);
  LeftCount.SetBold(false);RightCount.SetBold(false);Range676.SetBold(false);SightAngles.SetBold(false);TurretAngles.SetBold(false);
  for(int e=0;e<7;e++)
  {
   TextWidget label740=ElevationLabels740[e];label740.SetVisible(false);if(!Snapshot.AimValid)continue;
   float degree740=ElevationTop740()-e*10;float y740=206+e*80;
   Place(label740,ox+1778*S,oy+(y740-24)*S,142*S,39*S,25,STUDY_INK740);
   string text740=Math.Round(degree740).ToString()+"°";if(degree740>0)text740="+"+text740;Put(label740,text740);label740.SetVisible(true);
  }
  foreach(Widget hint:HintRoots){if(hint)hint.SetVisible(false);}
 }
 void TextStudy740()
 {
  Put(Header,"ROHRHOEHE");Put(LeftTitle,"LINKS / 35 mm");Put(RightTitle,"RECHTS / 35 mm");
  Put(TopBrand740,"GEPARD");Put(TopSubtitle740,"RICHTSCHUETZENVISIER");string mode740="--";if(Snapshot.OpticsValid){mode740="TAG";if(Snapshot.Night)mode740="RESTLICHT";}Put(TopMode740,mode740);
  string zoom740="--×";if(Snapshot.ZoomValid)zoom740=NumberDE704(Snapshot.Magnification)+"×";Put(TopZoom740,zoom740);
  string bearing740="--";if(Snapshot.ViewValid)bearing740=NumberDE704(Snapshot.Bearing)+"";Put(TopBearing740,bearing740);Put(TopBearingTitle740,"BLICKRICHTUNG");
  Put(LeftType,FeedTypeStudy740(Snapshot.Left));Put(RightType,FeedTypeStudy740(Snapshot.Right));
  Put(LeftCount,FeedCountDE704(Snapshot.Left));Put(RightCount,FeedCountDE704(Snapshot.Right));
  Put(LeftCapacity740,FeedCapacityText740(Snapshot.Left));Put(RightCapacity740,FeedCapacityText740(Snapshot.Right));
  LeftCount.SetColor(Color.FromInt(FeedInk740(Snapshot.Left,1)));RightCount.SetColor(Color.FromInt(FeedInk740(Snapshot.Right,2)));
  Put(OpticalMode,"WAFFENWAHL");string group="--";if(Snapshot.Group==0)group="BEIDE";if(Snapshot.Group==1)group="LINKS";if(Snapshot.Group==2)group="RECHTS";Put(Group,group);
  int groupInk740=LOWER_MUTED740;if(Snapshot.Crew && Snapshot.Group>=0 && Snapshot.Group<=2)groupInk740=LOWER_ACTIVE740;Group.SetColor(Color.FromInt(groupInk740));
  string distance="-- m";string detail="NOCH KEINE MESSUNG";int rangeInk740=LOWER_INK740;int noteInk740=LOWER_MUTED740;
  if(ValidRange740())
  {
   distance=Math.Round(Snapshot.RangeMetres676).ToString()+" m";detail="MESSUNG "+NumberDE704(Snapshot.RangeAge676)+" s";
   if(Snapshot.RangeAge676>10){detail+=" / VERALTET";rangeInk740=LOWER_WARN740;noteInk740=LOWER_WARN740;}
  }
  else if(Snapshot.RangeState676==EGEP_Range676State.NO_RETURN){distance="KEIN MESSWERT";detail="KEINE GUELTIGE MESSUNG";rangeInk740=LOWER_WARN740;noteInk740=LOWER_WARN740;}
  else if(Snapshot.RangeState676==EGEP_Range676State.UNAVAILABLE || Snapshot.RangeState676==EGEP_Range676State.VALID){distance="NICHT VERFUEGBAR";detail="KEINE GUELTIGE MESSUNG";rangeInk740=LOWER_WARN740;noteInk740=LOWER_WARN740;}
  Put(Reference,"ENTFERNUNG");Put(Range676,distance);Put(RangeDetail740,detail);Range676.SetColor(Color.FromInt(rangeInk740));RangeDetail740.SetColor(Color.FromInt(noteInk740));
  Put(OrientationHeader740,"TURM / WANNE");string azimuth="--";string elevation="--";
  if(Snapshot.AimValid){azimuth=NumberDE704(Snapshot.HullAzimuth)+"";elevation=NumberDE704(Snapshot.GunElevation)+"";if(Snapshot.GunElevation>0)elevation="+"+elevation;}
  Put(SightAngles,azimuth);Put(TurretAngles,elevation);
  string feedback=OperationalFeedbackDE704();int feedbackInk740=STUDY_WARN740;
  if(feedback=="" && EntityLockCue715 && Snapshot.Crew){feedback="ZIEL ERFASST";feedbackInk740=LOWER_ACTIVE740;}
  Put(Feedback,feedback);Feedback.SetVisible(feedback!="");Feedback.SetColor(Color.FromInt(feedbackInk740));
 }
 // Purposeful contrast shading, not a calibrated field-of-view or sensor boundary.
 // Separate from native WCS display reuse, which this study does not alter.
 void ShadeStudy740()
 {
  array<float> radii740={676,735,776,816,856,896,935,4000};array<int> alpha740={7,20,45,84,150,222,255};
  for(int band740=0;band740<7;band740++)
  {
   array<float> points740={};array<int> triangles740={};
   for(int i740=0;i740<64;i740++)
   {
    float a740=i740*Math.PI2/64;float sn740=Math.Sin(a740);float cs740=Math.Cos(a740);
    points740.Insert(960+sn740*radii740[band740]);points740.Insert(540+cs740*radii740[band740]);
    points740.Insert(960+sn740*radii740[band740+1]);points740.Insert(540+cs740*radii740[band740+1]);
    int ia740=i740*2;int ib740=((i740+1)%64)*2;
    triangles740.Insert(ia740);triangles740.Insert(ib740+1);triangles740.Insert(ia740+1);triangles740.Insert(ia740);triangles740.Insert(ib740);triangles740.Insert(ib740+1);
   }
   int ink740=(alpha740[band740]<<24)|0x00060806;Panel704(points740,triangles740,ink740);
  }
 }
 void FeedBarsStudy740(GEP_GunnerFeed583 feed,float x,int ink)
 {
  bool valid=feed && feed.State==2 && feed.Count>=0 && feed.Capacity>0;float ratio=0;if(valid){ratio=feed.Count;ratio=Math.Clamp(ratio/feed.Capacity,0,1);}
  for(int i=0;i<32;i++){float bx=x+i*7.3125;LowerLineStudy740(bx+2.61,1062,bx+2.61,1072.8,((LOWER_ACTIVE740 & 0x00ffffff)|0x26000000),5.22);if(valid){float fill=Math.Clamp(ratio*32-i,0,1);if(fill>0)LowerLineStudy740(bx+2.61*fill,1062,bx+2.61*fill,1072.8,ink,5.22*fill);}}
 }
 void ShellStudy740(float x)
 {
  array<float> shell740={x,1010,x+40.5,1010,x+49.5,1011.8,x+63.9,1011.8,x+86.4,1017.2,x+63.9,1022.6,x+49.5,1022.6,x+40.5,1024.4,x,1024.4};LowerContourStudy740(shell740,LOWER_INK740,1.35);
  LowerLineStudy740(x+39.6,1010.9,x+39.6,1023.5,LOWER_INK740,0.99);LowerLineStudy740(x+49.5,1012.7,x+49.5,1021.7,LOWER_INK740,0.99);LowerLineStudy740(x+63,1012.7,x+63,1021.7,LOWER_INK740,0.99);
 }
 void VehicleLineStudy740(float ax,float ay,float bx,float by,bool rotating,float width=1.1)
 {
  float ca=1;
  float sa=0;
  float angle740=0;
  if(rotating)
  {
   if(!Snapshot.AimValid){return;}
   angle740=Snapshot.HullAzimuth*Math.DEG2RAD;
   ca=Math.Cos(angle740);
   sa=Math.Sin(angle740);
  }
  LowerLineStudy740(1406+0.66*(ax*ca-ay*sa),1020+0.66*(ax*sa+ay*ca),1406+0.66*(bx*ca-by*sa),1020+0.66*(bx*sa+by*ca),LOWER_INK740,width*0.66);
 }
 void VehiclePathStudy740(array<float> points,bool rotating,bool closed,float width=1.1)
 {
  for(int i=0;i<points.Count()-2;i+=2)VehicleLineStudy740(points[i],points[i+1],points[i+2],points[i+3],rotating,width);
  if(closed)VehicleLineStudy740(points[points.Count()-2],points[points.Count()-1],points[0],points[1],rotating,width);
 }
 void HullStudy740()
 {
  array<float> path0Study740={-63,-28,33,-28,56,-17,56,17,33,28,-63,28,-71,20,-71,-20};VehiclePathStudy740(path0Study740,false,true,1.1);
  array<float> path1Study740={-69,-33,33,-33,47,-27,47,-19,-69,-19};VehiclePathStudy740(path1Study740,false,true,1.1);
  array<float> path2Study740={-69,19,47,19,47,27,33,33,-69,33};VehiclePathStudy740(path2Study740,false,true,1.1);
  array<float> path3Study740={-54,-20,29,-20,45,-10,45,10,29,20,-54,20};VehiclePathStudy740(path3Study740,false,true,1.1);
  array<float> path4Study740={-49,-13,-25,-13,-25,13,-49,13};VehiclePathStudy740(path4Study740,false,true,1.1);
  array<float> path5Study740={-44,-9,-30,-9};VehiclePathStudy740(path5Study740,false,false,1.1);
  array<float> path6Study740={-44,-5,-30,-5};VehiclePathStudy740(path6Study740,false,false,1.1);
  array<float> path7Study740={-44,-1,-30,-1};VehiclePathStudy740(path7Study740,false,false,1.1);
  array<float> path8Study740={-44,3,-30,3};VehiclePathStudy740(path8Study740,false,false,1.1);
  array<float> path9Study740={-44,7,-30,7};VehiclePathStudy740(path9Study740,false,false,1.1);
  array<float> path10Study740={29,-13,37,-9,37,9,29,13};VehiclePathStudy740(path10Study740,false,false,1.1);
  array<float> path11Study740={-62,-17,-62,17};VehiclePathStudy740(path11Study740,false,false,1.1);
  array<float> path12Study740={-18,-17,-7,-25,18,-25,33,-15,33,15,18,25,-7,25,-18,17};VehiclePathStudy740(path12Study740,true,true,1.1);
  array<float> path13Study740={-12,-10,-2,-16,14,-16,23,-10,23,10,14,16,-2,16,-12,10};VehiclePathStudy740(path13Study740,true,true,1.1);
  array<float> path14Study740={25,-17,84,-17,84,-14,25,-14};VehiclePathStudy740(path14Study740,true,false,1.1);
  array<float> path15Study740={25,14,84,14,84,17,25,17};VehiclePathStudy740(path15Study740,true,false,1.1);
  array<float> path16Study740={33,-15.5,81,-15.5};VehiclePathStudy740(path16Study740,true,false,1.1);
  array<float> path17Study740={33,15.5,81,15.5};VehiclePathStudy740(path17Study740,true,false,1.1);
  array<float> path18Study740={-20,-10,-28,-10,-28,10,-20,10};VehiclePathStudy740(path18Study740,true,false,1.1);
  array<float> path19Study740={-27,-4,-36,-4,-36,4,-27,4};VehiclePathStudy740(path19Study740,true,false,1.1);
  for(int track740=-64;track740<43;track740+=9){VehicleLineStudy740(track740,-32,track740,-21,false,0.85);VehicleLineStudy740(track740,21,track740,32,false,0.85);}
  if(!Snapshot.AimValid)return;
  float angle740=Snapshot.HullAzimuth*Math.DEG2RAD;float ca740=Math.Cos(angle740);float sa740=Math.Sin(angle740);
  for(int circle740=0;circle740<16;circle740++){float aa740=circle740*Math.PI2/16;float ab740=(circle740+1)*Math.PI2/16;LowerLineStudy740(1406+0.66*(7*ca740+6*Math.Cos(aa740)),1020+0.66*(7*sa740+6*Math.Sin(aa740)),1406+0.66*(7*ca740+6*Math.Cos(ab740)),1020+0.66*(7*sa740+6*Math.Sin(ab740)),LOWER_INK740,0.66);}
  array<float> arrowLocal740={75,0,50,-6,55,0,50,6};array<float> arrow740={};
  for(int a740=0;a740<arrowLocal740.Count();a740+=2){float ax740=arrowLocal740[a740];float ay740=arrowLocal740[a740+1];arrow740.Insert(1406+0.66*(ax740*ca740-ay740*sa740));arrow740.Insert(1020+0.66*(ax740*sa740+ay740*ca740));}
  array<int> arrowTriangles740={0,2,1,0,3,2};LowerPanelStudy740(arrow740,arrowTriangles740,LOWER_ACTIVE740);
 }
 void ElevationStudy740()
 {
  if(!Snapshot.AimValid)return;
  DLine704(1746,206,1746,686,STUDY_INK740,1.5);
  for(int i740=0;i740<=60;i740++){float y740=206+i740*8;float width740=7;if(i740%5==0)width740=13;if(i740%10==0)width740=22;DLine704(1746,y740,1746+width740,y740,STUDY_INK740,1.1);}
  float marker740=206+(ElevationTop740()-Snapshot.GunElevation)*8;
  array<float> markerPoints740={1748,marker740,1770,marker740-10,1770,marker740+10};array<int> markerTriangles740={0,1,2};Panel704(markerPoints740,markerTriangles740,STUDY_ACTIVE740);
 }
 void GraphicsStudy740()
 {
  Draw.Clear();bool lockStudy740=EntityLockCue715 && Snapshot.Crew;
  // Uncalibrated broken-ring alignment aid, not a range/lead/acquisition scale.
  for(int layer=0;layer<2;layer++)
  {
   int ink=0xff080e09;float stroke=2.6;if(layer==1){ink=STUDY_INK740;stroke=1.25;}
   DLine704(960,28,960,427,ink,stroke);DLine704(960,653,960,893,ink,stroke);
   DLine704(474,540,859,540,ink,stroke);DLine704(1061,540,1446,540,ink,stroke);
   // Ten-pixel open center keeps the exact boresight unobscured. No ballistic calibration.
   DLine704(946,540,955,540,ink,stroke);DLine704(965,540,974,540,ink,stroke);DLine704(960,526,960,535,ink,stroke);DLine704(960,545,960,554,ink,stroke);
   for(int sector740=0;sector740<4;sector740++)for(int segment740=0;segment740<18;segment740++)
   {
    float arcA740=(sector740*90+12+segment740*66.0/18)*Math.DEG2RAD;float arcB740=(sector740*90+12+(segment740+1)*66.0/18)*Math.DEG2RAD;
    DLine704(960+Math.Sin(arcA740)*162,540-Math.Cos(arcA740)*162,960+Math.Sin(arcB740)*162,540-Math.Cos(arcB740)*162,ink,stroke);
   }
   if(lockStudy740){int cueInk740=ink;if(layer==1)cueInk740=STUDY_ACTIVE740;DLine704(835,540,859,540,cueInk740,stroke);DLine704(1061,540,1085,540,cueInk740,stroke);DLine704(859,535,859,545,cueInk740,stroke);DLine704(1061,535,1061,545,cueInk740,stroke);}
  }
  ElevationStudy740();
  Bar(0,H-120*S,W,120*S,RAIL740);Line(0,H-119.5*S,W,H-119.5*S,RAIL_LINE740,S);
  Bar(0,0,W,36*S,RAIL740);Line(0,35.5*S,W,35.5*S,RAIL_LINE740,S);
  Line(137*S,10*S,137*S,26*S,RAIL_LINE740,S);Line(W-588*S,10*S,W-588*S,26*S,RAIL_LINE740,S);Line(W-495*S,10*S,W-495*S,26*S,RAIL_LINE740,S);
  ShellStudy740(52);ShellStudy740(1634);
  FeedBarsStudy740(Snapshot.Left,52,FeedBarInk740(Snapshot.Left,1));FeedBarsStudy740(Snapshot.Right,1634,FeedBarInk740(Snapshot.Right,2));HullStudy740();
  // Native WCS held-point marker reuse belongs to a separate verified integration.
  Canvas.SetDrawCommands(Draw);
 }
 // END ISOLATED HUD STUDY740

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
  bool lockCue715=false;
  GEP_TrackingStabilization707 tracking715=GEP_TrackingStabilization707.Cast(owner.FindComponent(GEP_TrackingStabilization707));
  if(tracking715)lockCue715=tracking715.HasConfirmedEntityLockCue715();
  // Redraw on the first frame of a state change, without waiting for the HUD timer.
  if(lockCue715!=EntityLockCue715){EntityLockCue715=lockCue715;Timer=0.05;}
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

// Private acceptance probe. Never packaged in the production addon.
modded class SCR_ContentBrowserEditorUIComponent
{
 static SCR_ContentBrowserEditorUIComponent GEP_ActiveBrowser102;
 override protected void HandlerAttachedScripted(Widget w)
 {
  super.HandlerAttachedScripted(w);
  GEP_ActiveBrowser102=this;
 }
 bool GEP_ClickResource102(ResourceName prefab)
 {
  if(!m_ContentWidget)return false;
  Widget card=m_ContentWidget.GetChildren();
  while(card)
  {
   int index=GetEntryIndex(card);
   int id=m_ContentBrowserManager.GetFilteredPrefabID(index);
   if(id>=0 && m_ContentBrowserManager.GetResourceNamePrefabID(id)==prefab)
   {
    OnCardLMB(card);
    return true;
   }
   card=card.GetSibling();
  }
  return false;
 }
}
modded class SCR_BaseGameMode
{
 protected ref GEP_GMProbe102 m_GEPProbe102;
 protected override void OnGameStart()
 {
  super.OnGameStart();
  m_GEPProbe102 = new GEP_GMProbe102();
  GetGame().GetCallqueue().CallLater(m_GEPProbe102.Tick,1000,true);
 }
}

class GEP_GMProbe102
{
 protected int m_Tick;
 protected int m_Stage;
 protected int m_ServerAck;
 protected int m_OwnerAck;
 protected int m_ExpectedID = -1;
 protected int m_RequestIndex;
 protected CameraBase m_CaptureCamera;
 protected SCR_ContentBrowserEditorComponent m_Browser;
 protected SCR_PlacingEditorComponent m_Placing;
 protected ref array<ResourceName> m_Matches = {};
 protected ref array<IEntity> m_Placed = {};
 protected bool m_Done;

 void Stop(string reason)
 {
  m_Done=true;
  Print("GM102_RESULT "+reason);
  GetGame().GetCallqueue().Remove(Tick);
  System.MakeScreenshot("$profile:GM102-final");
 }
 void ServerPlaced(int prefabID,SCR_EditableEntityComponent entity,int playerID)
 {
  if(!entity || prefabID!=m_ExpectedID)return;
  m_ServerAck++;
  Print(string.Format("GM102_SERVER_ACK id=%1 player=%2 prefab=%3 pos=%4",prefabID,playerID,entity.GetPrefab(),entity.GetOwner().GetOrigin()));
 }
 void OwnerPlaced(int prefabID,SCR_EditableEntityComponent entity)
 {
  Print(string.Format("GM102_OWNER_CALLBACK id=%1 entity=%2",prefabID,entity));
  if(!entity || prefabID!=m_ExpectedID)return;
  m_OwnerAck++;
  m_Placed.Insert(entity.GetOwner());
  Print(string.Format("GM102_OWNER_ACK id=%1 prefab=%2 pos=%3",prefabID,entity.GetPrefab(),entity.GetOwner().GetOrigin()));
 }
 void RequestPlace(ResourceName prefab,int index)
 {
  SCR_EditorPreviewParams params=new SCR_EditorPreviewParams();
  vector pos=Vector(1680+index*14,0,2110);
  pos[1]=GetGame().GetWorld().GetSurfaceY(pos[0],pos[2])+0.3;
  Math3D.AnglesToMatrix("0 0 0",params.m_vTransform);
  params.m_vTransform[3]=pos;
  m_ExpectedID=m_Placing.GetPrefabID(prefab);
  bool requested=m_Placing.CreateEntity(prefab,params);
  Print(string.Format("GM102_PLACE_REQUEST id=%1 prefab=%2 accepted=%3",m_ExpectedID,prefab,requested));
  if(!requested)Stop("FAIL placement request rejected");
 }
 void Tick()
 {
  if(m_Done)return;
  m_Tick++;
  if(m_Tick>100){Stop("FAIL bounded readiness/ack timeout");return;}
  SCR_EditorManagerEntity manager=SCR_EditorManagerEntity.GetInstance();
  if(!manager){if(m_Tick%5==0)Print("GM102_WAIT manager");return;}
  if((!manager.GetCurrentModeEntity() || manager.GetCurrentModeEntity().GetModeType()!=EEditorMode.EDIT)){manager.SetCurrentMode(EEditorMode.EDIT);return;}
  if(!manager.IsOpened())
  {
   if(manager.CanOpen())manager.Open();
   if(m_Tick%5==0)Print(string.Format("GM102_WAIT open allowed=%1 modes=%2",manager.CanOpen(),manager.GetEditorModes()));
   return;
  }
  m_Browser=SCR_ContentBrowserEditorComponent.Cast(SCR_BaseEditorComponent.GetInstance(SCR_ContentBrowserEditorComponent));
  m_Placing=SCR_PlacingEditorComponent.Cast(SCR_BaseEditorComponent.GetInstance(SCR_PlacingEditorComponent));
  if(!m_Browser || !m_Placing || m_Browser.GetInfoCount()<10)return;
  if(m_Stage==0)
  {
   Print(string.Format("GM102_EDITOR_READY mode=%1 browserInfos=%2",manager.GetCurrentModeEntity().GetModeType(),m_Browser.GetInfoCount()));
   m_Placing.GetOnPlaceEntityServer().Insert(ServerPlaced);
   m_Placing.GetOnPlaceEntity().Insert(OwnerPlaced);
   m_Browser.ResetAllLabels(false);
   m_Browser.SetCurrentSearch("Gepard");
   m_Browser.FilterEntries();
   bool opened=m_Browser.OpenBrowser();
   Print(string.Format("GM102_BROWSER_OPEN requested=%1",opened));
   m_Stage=1;return;
  }
  if(m_Stage==1)
  {
   m_Browser.ResetAllLabels(false);
   m_Browser.SetCurrentSearch("Gepard");
   m_Browser.FilterEntries();
   int count=m_Browser.GetFilteredPrefabCount();
   Print(string.Format("GM102_SEARCH count=%1",count));
   for(int i=0;i<count;i++)
   {
    int id=m_Browser.GetFilteredPrefabID(i);
    ResourceName prefab=m_Browser.GetResourceNamePrefabID(id);
    Print(string.Format("GM102_CARD id=%1 prefab=%2 key=%3",id,prefab,m_Browser.GetInfo(id).GetName()));
    m_Matches.Insert(prefab);
   }
   System.MakeScreenshot("$profile:GM102-browser-search");
   if(count!=2){Stop("FAIL expected exactly two search cards");return;}
   m_Stage=2;return;
  }
  if(m_Stage==2)
  {
   m_Browser.SetLabel(EEditableEntityLabel.CONTENT_MODDED,true);
   m_Browser.FilterEntries();
   Print(string.Format("GM102_MODDED_FILTER count=%1",m_Browser.GetFilteredPrefabCount()));
   if(m_Browser.GetFilteredPrefabCount()!=2){Stop("FAIL modded filter");return;}
   System.MakeScreenshot("$profile:GM102-browser-modded");
   m_Browser.SetLabel(EEditableEntityLabel.ENTITYTYPE_VEHICLE,true);
   m_Browser.SetLabel(EEditableEntityLabel.TRAIT_ARMOR,true);
   m_Browser.SetLabel(EEditableEntityLabel.FACTION_US,true);
   m_Browser.FilterEntries();
   Print(string.Format("GM102_VEHICLE_ARMOR_US_FILTER count=%1",m_Browser.GetFilteredPrefabCount()));
   if(m_Browser.GetFilteredPrefabCount()!=2){Stop("FAIL combined vehicle armor faction filter");return;}

   m_Browser.ResetAllLabels(false);m_Browser.SetCurrentSearch("");m_Browser.FilterEntries();
   ResourceName stock;
   for(int j=0;j<m_Browser.GetFilteredPrefabCount();j++)
   {
    ResourceName test=m_Browser.GetResourceNamePrefabID(m_Browser.GetFilteredPrefabID(j));
    if(test.Contains("M998") && test.EndsWith(".et")){stock=test;break;}
   }
   if(stock.IsEmpty()){Stop("FAIL stock M998 browser control absent");return;}
   m_Matches.Insert(stock);
   Print("GM102_STOCK_CONTROL "+stock);
   m_Stage=3;return;
  }
  if(m_Stage==3)
  {
   m_Browser.ResetAllLabels(false);
   if(m_RequestIndex==2)m_Browser.SetCurrentSearch("M998");else m_Browser.SetCurrentSearch("Gepard");
   m_Browser.FilterEntries();m_Browser.OpenBrowser();m_Stage=30;return;
  }
  if(m_Stage==30)
  {
   SCR_ContentBrowserEditorUIComponent ui=SCR_ContentBrowserEditorUIComponent.GEP_ActiveBrowser102;
   if(!ui || !ui.GEP_ClickResource102(m_Matches[m_RequestIndex])){Stop("FAIL real browser card selection");return;}
   Print("GM102_REAL_CARD_SELECTED "+m_Matches[m_RequestIndex]);
   m_Stage=31;return;
  }
  if(m_Stage==31){RequestPlace(m_Matches[m_RequestIndex],m_RequestIndex);m_Stage=32;return;}
  if(m_Stage==32 && m_OwnerAck>=m_RequestIndex+1 && m_ServerAck>=m_RequestIndex+1)
  {
   m_RequestIndex++;
   if(m_RequestIndex<3)m_Stage=3;else m_Stage=6;
   return;
  }
  if(m_Stage==6 && m_OwnerAck>=3 && m_ServerAck>=3)
  {
   Print(string.Format("GM102_PLACED count=%1 serverAck=%2 ownerAck=%3",m_Placed.Count(),m_ServerAck,m_OwnerAck));
   foreach(IEntity placed:m_Placed){if(!placed){Stop("FAIL placed entity disappeared");return;}Print("GM102_VISIBLE_ENTITY "+placed.GetPrefabData().GetPrefabName());}
   m_CaptureCamera=SCR_CameraEditorComponent.GetCameraInstance();
   if(!m_CaptureCamera){Stop("FAIL capture camera unavailable");return;}
   vector aim=m_Placed[0].GetOrigin()+"14 2 0";vector eye=m_Placed[0].GetOrigin()+"15 16 32";vector pose[4];
   Math3D.DirectionAndUpMatrix((aim-eye).Normalized(),vector.Up,pose);pose[3]=eye;
   m_CaptureCamera.SetTransform(pose);m_CaptureCamera.SetFOVDegree(60);
   SCR_ManualCamera manual=SCR_ManualCamera.Cast(m_CaptureCamera);if(manual)manual.ResetComponents();
   SCR_HintManagerComponent.HideHint();
   GetGame().GetCameraManager().SetCamera(m_CaptureCamera);m_CaptureCamera.ApplyTransform(1);
   m_Stage=7;return;
  }
  if(m_Stage==7){Stop("PASS real browser card actions and native placement; programmatic actions, physical input untested");}
 }
}

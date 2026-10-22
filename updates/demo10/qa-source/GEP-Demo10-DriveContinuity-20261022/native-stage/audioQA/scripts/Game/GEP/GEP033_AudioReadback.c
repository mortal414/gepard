// Original read-only QA helper. No signal writes, sound triggers or driving changes.
// Call Init once after the existing subject is bound; Tick after course.AfterPhysics.
class GEP033_AudioReadback
{
 protected IEntity m_Subject;
 protected VehicleWheeledSimulation m_Sim;
 protected SCR_VehicleSoundComponent m_Sound;
 protected SignalsManagerComponent m_Signals;
 protected ref array<string> m_Names={};
 protected ref array<string> m_Pivots={};
 protected string m_Case,m_Nonce;
 protected int m_LastSample=-4;
 protected bool m_Initialized;

 void Init(IEntity subject,string caseId,string nonce)
 {
  if(m_Initialized)return;m_Initialized=true;m_Subject=subject;m_Case=caseId;m_Nonce=nonce;
  if(!subject)return;
  m_Sim=VehicleWheeledSimulation.Cast(subject.FindComponent(VehicleWheeledSimulation));
  m_Sound=SCR_VehicleSoundComponent.Cast(subject.FindComponent(SCR_VehicleSoundComponent));
  m_Signals=SignalsManagerComponent.Cast(subject.FindComponent(SignalsManagerComponent));
  PrintFormat("GEP033_AUDIO_BIND case=%1 nonce=%2 prefab=%3 sim=%4 sound=%5 signalManager=%6",m_Case,m_Nonce,subject.GetPrefabData().GetPrefabName(),m_Sim!=null,m_Sound!=null,m_Signals!=null);
  if(!m_Sound)return;
  BaseContainer source=m_Sound.GetComponentSource(subject);
  if(source)
  {
   BaseContainerList wheels=source.GetObjectArray("Wheels");
   int count=0;if(wheels)count=wheels.Count();
   PrintFormat("GEP033_AUDIO_BIND_WHEELS case=%1 nonce=%2 sourceClass=%3 count=%4",m_Case,m_Nonce,source.GetClassName(),count);
   if(count>16)count=16;
   for(int i=0;i<count;i++)
   {
    BaseContainer wheel=wheels.Get(i);if(!wheel)continue;
    string filename,pivot,pointClass;wheel.Get("Filename",filename);BaseContainer point=wheel.GetObject("SoundPoint");if(point){point.Get("PivotID",pivot);pointClass=point.GetClassName();}
    m_Pivots.Insert(pivot);
    PrintFormat("GEP033_AUDIO_BIND_SLOT case=%1 nonce=%2 index=%3 slot=%4 graph=%5 pointClass=%6 pivot=%7",m_Case,m_Nonce,i,wheel.GetName(),filename,pointClass,pivot);
   }
  }
  // Parent pool visibility is diagnostic only: private native wheel pools may differ.
  PrintFormat("GEP033_AUDIO_PARENT_POOL case=%1 nonce=%2 rollEventIndex=%3 skidEventIndex=%4 playing=%5 limit=parent-pool-not-wheel-handle",m_Case,m_Nonce,m_Sound.GetEventIndex("SOUND_TIRE_ROLL_LP"),m_Sound.GetEventIndex("SOUND_TIRE_SKID_LP"),m_Sound.IsPlaying());
  array<string> allNames={};m_Sound.GetSignalNames(allNames);
  PrintFormat("GEP033_AUDIO_SIGNAL_NAMES case=%1 nonce=%2 total=%3",m_Case,m_Nonce,allNames.Count());
  foreach(string name:allNames)
  {
   if(m_Names.Count()>=128){Print("GEP033_AUDIO_READBACK_TRUNCATED signal-name-cap=128");break;}
   string lower=name;lower.ToLower();
   if(lower.Contains("wheel") || lower.Contains("tire") || lower.Contains("tyre") || lower.Contains("roll") || lower.Contains("speed") || lower.Contains("surface") || lower.Contains("suspension") || lower.Contains("distance") || lower.Contains("inside") || lower.Contains("occlusion") || lower.Contains("water") || lower.Contains("engine") || lower.Contains("volume"))
   {
    m_Names.Insert(name);int index=-1;if(m_Signals)index=m_Signals.FindSignal(name);
    PrintFormat("GEP033_AUDIO_SIGNAL_NAME case=%1 nonce=%2 name=%3 soundIndex=%4 managerIndex=%5",m_Case,m_Nonce,name,m_Sound.GetSignalIndex(name),index);
   }
  }
 }

 void Tick(int sample,string phase)
 {
  if(!m_Subject || m_Subject.IsDeleted() || !m_Sim)
  {
   return;
  }
  if(sample<=0 || sample-m_LastSample<4)
  {
   return;
  }
  m_LastSample=sample;
  float now=GetGame().GetWorld().GetWorldTime();
  PrintFormat("GEP033_AUDIO_OBSERVE case=%1 nonce=%2 sample=%3 phase=%4 worldMs=%5 wheelCount=%6 parentPlaying=%7",m_Case,m_Nonce,sample,phase,now,m_Sim.WheelCount(),m_Sound && m_Sound.IsPlaying());
  Animation animation=m_Subject.GetAnimation();
  if(animation)foreach(string pivot:m_Pivots)
  {
   TNodeId bone=animation.GetBoneIndex(pivot);
   PrintFormat("GEP033_AUDIO_PIVOT case=%1 nonce=%2 sample=%3 pivot=%4 bone=%5",m_Case,m_Nonce,sample,pivot,bone);
  }
  if(m_Signals)foreach(string name:m_Names)
  {
   int index=m_Signals.FindSignal(name);float value=0;if(index>=0)value=m_Signals.GetSignalValue(index);
   PrintFormat("GEP033_AUDIO_SIGNAL_VALUE case=%1 nonce=%2 sample=%3 phase=%4 worldMs=%5 name=%6 managerIndex=%7 value=%8",m_Case,m_Nonce,sample,phase,now,name,index,value);
  }
  for(int wheel=0;wheel<m_Sim.WheelCount();wheel++)
  {
   GameMaterial material=m_Sim.WheelGetContactMaterial(wheel);string resource;int surface=-1;
   if(material){resource=material.GetResourceName();SoundInfo info=material.GetSoundInfo();if(info)surface=info.GetSignalValue();}
   PrintFormat("GEP033_AUDIO_WHEEL case=%1 nonce=%2 sample=%3 wheel=%4 name=%5 contact=%6 rpm=%7 material=%8 contactPosition=%9",m_Case,m_Nonce,sample,wheel,m_Sim.WheelGetName(wheel),m_Sim.WheelHasContact(wheel),m_Sim.WheelGetRPM(wheel),resource,m_Sim.WheelGetContactPosition(wheel));
   PrintFormat("GEP033_AUDIO_SURFACE case=%1 nonce=%2 sample=%3 wheel=%4 worldMs=%5 surfaceSignal=%6",m_Case,m_Nonce,sample,wheel,now,surface);
  }
 }
}

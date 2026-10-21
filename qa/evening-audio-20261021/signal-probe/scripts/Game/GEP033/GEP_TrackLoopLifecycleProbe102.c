// Original GEP033 QA lifecycle, excluded from production; deliberately opt-in and unqualified.
[ComponentEditorProps(category: "Gepard/Audio", description: "GEP033 one-attempt loop lifecycle probe (disabled by default)")]
class GEP_TrackLoopLifecycleProbe102Class : ScriptComponentClass {}

class GEP_TrackLoopLifecycleProbe102 : ScriptComponent
{
 [Attribute("0", UIWidgets.CheckBox, "QA opt-in only after original ACP registration, event validation and duplicate-layer inspection")]
 protected bool m_bEnableCandidate;
 protected AudioHandle m_Handle = AudioHandle.Invalid;
 protected int m_StartAttempts;
 protected float m_ReadyElapsed;
 protected bool m_Terminal;
 protected string m_TerminalReason;

 int GetStartAttemptCount() { return m_StartAttempts; }
 bool HasLiveHandle() { return m_Handle != AudioHandle.Invalid && AudioSystem.IsSoundPlayed(m_Handle); }
 string GetTerminalReason() { return m_TerminalReason; }

 override void OnPostInit(IEntity owner)
 {
  super.OnPostInit(owner);
  SetEventMask(owner, EntityEvent.FRAME);
 }

 protected void StopOwnedLoop(bool fade, string reason)
 {
  m_Terminal = true;
  m_TerminalReason = reason;
  if (m_Handle == AudioHandle.Invalid) return;
  // Clear ownership before the native call so repeated cleanup cannot terminate twice.
  AudioHandle owned = m_Handle;
  m_Handle = AudioHandle.Invalid;
  if (fade) AudioSystem.TerminateSoundFadeOut(owned, true, 0.22);
  else AudioSystem.TerminateSound(owned);
  Print("GEP033_LIFECYCLE STOP_ISSUED reason=" + reason);
 }

 override void EOnFrame(IEntity owner, float timeSlice)
 {
  if (!m_bEnableCandidate || m_Terminal || !GetGame().InPlayMode() || System.IsConsoleApp()) return;
  SCR_DamageManagerComponent damage = SCR_DamageManagerComponent.Cast(owner.FindComponent(SCR_DamageManagerComponent));
  if (damage && damage.GetState() == EDamageState.DESTROYED)
  {
   StopOwnedLoop(true, "destroyed");
   return;
  }
  if (m_StartAttempts > 0)
  {
   if (!HasLiveHandle()) StopOwnedLoop(false, "ended-or-not-observable");
   return; // Never restart per frame, on throttle changes, or after culling/failed playback.
  }
  m_ReadyElapsed += Math.Clamp(timeSlice, 0, 0.1);
  GEP_TrackMovementSignals102 signals = GEP_TrackMovementSignals102.Cast(owner.FindComponent(GEP_TrackMovementSignals102));
  if (!signals || !signals.HasBoundTrackSignals())
  {
   if (m_ReadyElapsed >= 5) StopOwnedLoop(false, "signals-not-ready");
   return;
  }
  SoundComponent sound = SoundComponent.Cast(owner.FindComponent(SoundComponent));
  if (!sound)
  {
   StopOwnedLoop(false, "sound-component-missing");
   return;
  }
  // One native sound-component event. Its ACP must already be attached to the QA child.
  // SoundEventOffset keeps native entity ownership rather than a detached world loop.
  m_StartAttempts = 1;
  m_Handle = sound.SoundEventOffset("GEP_TRACK_MOVEMENT_LP", "0 0.5 0");
  Print("GEP033_LIFECYCLE START_ISSUED attempts=1");
  if (m_Handle == AudioHandle.Invalid) StopOwnedLoop(false, "invalid-start-handle");
 }

 override void OnDelete(IEntity owner)
 {
  StopOwnedLoop(false, "deleted");
  super.OnDelete(owner);
 }
}

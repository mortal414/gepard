// Original GEP033 source candidate. No WCS code/assets and no playback or physics writes.
// This is a no-slip rolling estimate, matching E's body-motion belt approximation.
// Native compile and audible graph binding are deliberately separate acceptance gates.
[ComponentEditorProps(category: "Gepard/Audio", description: "GEP033 bounded rolling audio signals")]
class GEP_TrackMovementSignals102Class : ScriptComponentClass {}

class GEP_TrackMovementSignals102 : ScriptComponent
{
 protected SignalsManagerComponent m_Signals;
 protected int m_Level = -1;
 protected int m_Pitch = -1;
 protected int m_Speed = -1;
 protected float m_LevelValue;
 protected float m_SpeedValue;
 protected float m_Elapsed;

 // Diagnostic-only readback for a future addon-owned native fixture.
 float GetTrackLevel() { return m_LevelValue; }
 float GetRollingSpeed() { return m_SpeedValue; }
 float GetTrackPitch() { return -3 + 8 * Math.Clamp(m_SpeedValue / 14, 0, 1); }

 override void OnPostInit(IEntity owner)
 {
  super.OnPostInit(owner);
  SetEventMask(owner, EntityEvent.FRAME);
 }

 protected bool Bind(IEntity owner)
 {
  m_Signals = SignalsManagerComponent.Cast(owner.FindComponent(SignalsManagerComponent));
  if (!m_Signals) return false;
  m_Level = m_Signals.AddOrFindSignal("GEPTrackLevel", 0);
  m_Pitch = m_Signals.AddOrFindSignal("GEPTrackPitch", -3);
  m_Speed = m_Signals.AddOrFindSignal("GEPTrackRollingSpeed", 0);
  return m_Level >= 0 && m_Pitch >= 0 && m_Speed >= 0;
 }

 override void EOnFrame(IEntity owner, float timeSlice)
 {
  if (!GetGame().InPlayMode()) return;
  m_Elapsed += Math.Clamp(timeSlice, 0, 0.1);
  if (m_Elapsed < 0.033333) return;
  float dt = m_Elapsed;
  m_Elapsed = 0;
  if (m_Level < 0 && !Bind(owner)) return;

  float rolling = 0;
  Physics body = owner.GetPhysics();
  if (body)
  {
   vector basis[4];
   owner.GetWorldTransform(basis);
   float forward = vector.Dot(body.GetVelocity(), basis[2]);
   float yaw = vector.Dot(body.GetAngularVelocity(), basis[1]);
   // +/-1.46m is the existing E visual track-centre approximation.
   // Absolute side rates retain reverse and actual rolling turns without throttle.
   float left = forward + yaw * 1.46;
   float right = forward - yaw * 1.46;
   rolling = Math.Clamp((Math.AbsFloat(left) + Math.AbsFloat(right)) * 0.5, 0, 22);
  }
  SCR_DamageManagerComponent damage = SCR_DamageManagerComponent.Cast(owner.FindComponent(SCR_DamageManagerComponent));
  if (damage && damage.GetState() == EDamageState.DESTROYED) rolling = 0;

  // Quiet below 0.15m/s (0.54km/h), full entry by 0.65m/s.
  float gate = Math.Clamp((rolling - 0.15) / 0.5, 0, 1);
  gate = gate * gate * (3 - 2 * gate);
  float target = gate * (0.18 + 0.27 * Math.Clamp(rolling / 14, 0, 1));
  float tau = 0.22;
  if (target > m_LevelValue) tau = 0.12;
  float blend = dt / (tau + dt);
  m_LevelValue += (target - m_LevelValue) * blend;
  if (target == 0 && m_LevelValue < 0.001) m_LevelValue = 0;
  m_SpeedValue += (rolling - m_SpeedValue) * (dt / (0.18 + dt));
  float pitch = -3 + 8 * Math.Clamp(m_SpeedValue / 14, 0, 1);
  m_Signals.SetSignalValue(m_Level, Math.Clamp(m_LevelValue, 0, 0.45));
  m_Signals.SetSignalValue(m_Pitch, Math.Clamp(pitch, -3, 5));
  m_Signals.SetSignalValue(m_Speed, Math.Clamp(m_SpeedValue, 0, 22));
 }
}

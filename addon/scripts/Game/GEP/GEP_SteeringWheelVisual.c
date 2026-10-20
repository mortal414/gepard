// Original visual-only coupling to the native pilot animation. No input,
// simulation, actor pose, IK or network state is written by this component.
class GEP_SteeringWheelVisualClass : ScriptComponentClass {}
class GEP_SteeringWheelVisual : ScriptComponent
{
 protected IEntity m_Wheel;
 protected bool m_Ready;
 protected vector m_Rest[4];
 protected float m_Angle;
 float GetVisualAngle() {return m_Angle;}
 IEntity GetVisual() {return m_Wheel;}
 override void OnPostInit(IEntity owner) {SetEventMask(owner,EntityEvent.POSTFRAME);}
 override void EOnPostFrame(IEntity owner,float timeSlice)
 {
  if(!GetGame().InPlayMode()) return;
  if(!m_Wheel)
  {
   for(IEntity child=owner.GetChildren();child;child=child.GetSibling())
    if(child.GetPrefabData() && child.GetPrefabData().GetPrefabName().Contains("GepardSteeringWheel.et")){m_Wheel=child;break;}
  }
  if(!m_Wheel) return;
  SCR_DamageManagerComponent damage=SCR_DamageManagerComponent.Cast(owner.FindComponent(SCR_DamageManagerComponent));
  if(damage && damage.GetState()==EDamageState.DESTROYED){m_Wheel.ClearFlags(EntityFlags.VISIBLE,true);return;}
  if(!m_Ready){m_Wheel.GetLocalTransform(m_Rest);m_Ready=true;}
  TrackedControllerComponent controller=TrackedControllerComponent.Cast(owner.FindComponent(TrackedControllerComponent));
  PilotCompartmentSlot seat;if(controller) seat=controller.GetPilotCompartmentSlot();
  IEntity pilot;if(seat && seat.IsPiloting()) pilot=seat.GetOccupant();
  CharacterControllerComponent life;if(pilot) life=CharacterControllerComponent.Cast(pilot.FindComponent(CharacterControllerComponent));
  Animation animation;if(life && life.GetLifeState()==ECharacterLifeState.ALIVE) animation=pilot.GetAnimation();
  if(animation)
  {
   int bone=animation.GetBoneIndex("RightHandProp");vector hand[4];
   if(bone>=0 && animation.GetBoneMatrix(bone,hand))
   {
    vector relative=owner.CoordToLocal(pilot.CoordToParent(hand[3]))-m_Rest[3];
    float right=vector.Dot(relative,m_Rest[0]);float up=vector.Dot(relative,m_Rest[1]);
    if(right*right+up*up>0.01) m_Angle=Math.Atan2(up,right)*Math.RAD2DEG-45;
   }
  }
  else m_Angle=Math.Lerp(m_Angle,0,Math.Min(timeSlice*6,1));
  // Native roll's positive direction is opposite the grip-plane atan2 angle.
  // Verified from native wheel basis telemetry, not inferred from FBX axes.
  vector turn[4],pose[4];Math3D.AnglesToMatrix(Vector(0,0,-m_Angle),turn);
  Math3D.MatrixMultiply3(m_Rest,turn,pose);pose[3]=m_Rest[3];
  m_Wheel.SetLocalTransform(pose);m_Wheel.Update();
 }
}

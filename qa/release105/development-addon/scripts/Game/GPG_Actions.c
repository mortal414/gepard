// Private local fixture controls. No keybind injection, health mutation or MP claim.
class GPG_BoardComponentClass : ScriptComponentClass {}
class GPG_BoardComponent : ScriptComponent
{
 string Station;
}

class GPG_BoardAction : ScriptedUserAction
{
 [Attribute("read",UIWidgets.EditBox,"read/reset/arm/stop/respawn")]
 string Operation;

 override bool HasLocalEffectOnlyScript() {return true;}
 override bool CanBeShownScript(IEntity user)
 {
  GPG_BoardComponent board=GPG_BoardComponent.Cast(GetOwner().FindComponent(GPG_BoardComponent));
  if(!board || !GPG_Director.Instance)return false;
  if(Operation=="arm" || Operation=="stop")return board.Station=="hostile";
  if(Operation=="reset")return board.Station=="bay" || board.Station=="drive" || board.Station=="service" || board.Station=="range" || board.Station=="damage" || board.Station=="hostile" || board.Station=="smoke-service";
  // Death respawn is also available from the profile command for a dead player,
  // whose ordinary interaction camera cannot reach a board.
  if(Operation=="respawn")return false;
  return true;
 }
 override bool CanBePerformedScript(IEntity user)
 {
  if(RplSession.Mode()!=RplMode.None){SetCannotPerformReason("Private single-player fixture; multiplayer unqualified");return false;}
  return user && vector.Distance(user.GetOrigin(),GetOwner().GetOrigin())<12;
 }
 override void PerformAction(IEntity pOwnerEntity,IEntity pUserEntity)
 {
  if(!CanBePerformedScript(pUserEntity))return;
  GPG_BoardComponent board=GPG_BoardComponent.Cast(pOwnerEntity.FindComponent(GPG_BoardComponent));
  if(board && GPG_Director.Instance)GPG_Director.Instance.Execute(board.Station,Operation,pUserEntity);
 }
}

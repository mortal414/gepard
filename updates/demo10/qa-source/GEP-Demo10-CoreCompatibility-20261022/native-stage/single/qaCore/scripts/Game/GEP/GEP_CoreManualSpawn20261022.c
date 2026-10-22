// Private standalone fixture: preserve native respawn in multiplayer.
[BaseContainerProps(category:"Respawn")]
class GEP_ManualSpawnLogic : SCR_AutoSpawnLogic
{
 override protected void DoSpawn_S(int playerId)
 {if(RplSession.Mode()==RplMode.None)return;super.DoSpawn_S(playerId);}
}

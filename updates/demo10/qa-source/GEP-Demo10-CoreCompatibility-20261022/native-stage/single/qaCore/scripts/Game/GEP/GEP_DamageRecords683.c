// Source-only integration proposal. Passive snapshots, no damage/state changes.
class GEP_DamageRecord683
{
 int Sequence683;
 int Tick683;
 int Type683;
 float Value683;
 string Manager683;
 string HitEntity683;
 string Source683;
 string Zone683;
 string Effect683;
 int Collider683;
 int Bone683;
 vector Position683;
 vector ImpactVelocity683;
 float DefaultHealth683 = -1;
 int ManagerState683 = -1;

 // Explicit primitive serialization avoids serializing live native contexts or
 // relying on registration/reflection of this proposal's record class.
 bool Write683(SaveContext context)
 {
  if (!context.StartObject()) return false;
  if (!context.WriteValue("sequence", Sequence683)) return false;
  if (!context.WriteValue("tick", Tick683)) return false;
  if (!context.WriteValue("damageType", Type683)) return false;
  if (!context.WriteValue("damageValue", Value683)) return false;
  if (!context.WriteValue("manager", Manager683)) return false;
  if (!context.WriteValue("hitEntity", HitEntity683)) return false;
  if (!context.WriteValue("damageSource", Source683)) return false;
  if (!context.WriteValue("zone", Zone683)) return false;
  if (!context.WriteValue("effect", Effect683)) return false;
  if (!context.WriteValue("collider", Collider683)) return false;
  if (!context.WriteValue("bone", Bone683)) return false;
  if (!context.WriteValue("position", Position683)) return false;
  if (!context.WriteValue("impactVelocityRaw", ImpactVelocity683)) return false;
  if (!context.WriteValue("defaultHealthAtCallback", DefaultHealth683)) return false;
  if (!context.WriteValue("managerStateAtCallback", ManagerState683)) return false;
  return context.EndObject();
 }
}

class GEP_DamageRecordSubscription683
{
 protected SCR_DamageManagerComponent Manager683;
 ref array<ref GEP_DamageRecord683> Records683 = {};
 int Seen683;
 int Dropped683;
 int Limit683 = 512;

 void GEP_DamageRecordSubscription683(SCR_DamageManagerComponent manager)
 {
  Manager683 = manager;
  if (Manager683) Manager683.GetOnDamage().Insert(OnDamage683);
 }

 void OnDamage683(BaseDamageContext context)
 {
  if (!context || !Manager683 || !Manager683.GetOwner() || Manager683.GetOwner().IsDeleted()) return;
  Seen683++;
  if (Records683.Count() >= Limit683) { Dropped683++; return; }
  GEP_DamageRecord683 record = new GEP_DamageRecord683();
  record.Sequence683 = Seen683;
  record.Tick683 = System.GetTickCount();
  record.Type683 = context.damageType;
  record.Value683 = context.damageValue;
  record.Manager683 = string.Format("%1", Manager683.GetOwner());
  record.HitEntity683 = string.Format("%1", context.hitEntity);
  record.Source683 = string.Format("%1", context.damageSource);
  if (context.struckHitZone) record.Zone683 = context.struckHitZone.GetName();
  if (context.damageEffect) record.Effect683 = context.damageEffect.Type().ToString();
  record.Collider683 = context.colliderID;
  record.Bone683 = context.boneIndex;
  record.Position683 = context.hitPosition;
  record.ImpactVelocity683 = context.impactVelocity;
  record.ManagerState683 = Manager683.GetState();
  HitZone zone = Manager683.GetDefaultHitZone();
  if (zone) record.DefaultHealth683 = zone.GetHealth();
  Records683.Insert(record);
 }

 bool Write683(SaveContext context, string name)
 {
  int count = Records683.Count();
  if (!context.StartArray(name, count)) return false;
  foreach (GEP_DamageRecord683 record : Records683)
   if (!record.Write683(context)) return false;
  return context.EndArray();
 }

 void Close683()
 {
  if (Manager683 && Manager683.GetOwner() && !Manager683.GetOwner().IsDeleted())
   Manager683.GetOnDamage().Remove(OnDamage683);
  Manager683 = null;
 }
}

// Integration sketch, called once by root after final state snapshots:
// JsonSaveContext json = new JsonSaveContext();
// if (!json.WriteValue("schema", 1)) { /* fail result */ }
// if (!json.WriteValue("scriptedGameQA", true)) { /* fail result */ }
// if (!json.WriteValue("physicalInputAccepted", false)) { /* fail result */ }
// if (!subscription.Write683(json, "damageEvents")) { /* fail result */ }
// if (!json.WriteValue("eventsDropped", subscription.Dropped683)) { /* fail result */ }
// if (!json.SaveToFile("$profile:result683.json")) { /* fail result */ }
// subscription.Close683(); // Also call during owner teardown.

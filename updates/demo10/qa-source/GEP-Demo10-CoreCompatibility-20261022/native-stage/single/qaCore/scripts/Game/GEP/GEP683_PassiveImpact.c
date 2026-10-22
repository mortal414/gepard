// Original unmounted/uncompiled/unrun next-slice proposal.
// Native OnEffect parameters are READ ONLY here, including all outMat entries.
// No native class modification, effect state, damage, velocity, controls or flags.
class GEP688_ImpactReceipt
{
	string CaseNonce;
	int Ordinal;
	int MillisecondsFromOpen;
	string SourceID;
	string SourcePrefab;
	string HitID;
	string HitPrefab;
	string Collider;
	vector Position;
	vector Direction;
	vector Normal;
	float Speed;
	bool DirectionKnown;
	bool NormalKnown;

	GEP688_ImpactReceipt Copy()
	{
		GEP688_ImpactReceipt result = new GEP688_ImpactReceipt();
		result.CaseNonce = CaseNonce;
		result.Ordinal = Ordinal; result.MillisecondsFromOpen = MillisecondsFromOpen;
		result.SourceID = SourceID; result.SourcePrefab = SourcePrefab;
		result.HitID = HitID; result.HitPrefab = HitPrefab; result.Collider = Collider;
		result.Position = Position; result.Direction = Direction; result.Normal = Normal;
		result.Speed = Speed; result.DirectionKnown = DirectionKnown; result.NormalKnown = NormalKnown;
		return result;
	}
}

class GEP688_ImpactReport
{
	string CaseNonce;
	string SubjectRootID;
	string SubjectRootPrefab;
	bool Active;
	int RegisteredProjectiles;
	int SubjectCallbacks;
	int ValidSubjectCallbacks;
	int OtherCallbacks;
	int UnregisteredCallbacks;
	int InvalidSubjectCallbacks;
	int DroppedSubjectReceipts;
	int RejectedRegistrations;
	int ExpiredCallbacks;
	ref array<ref GEP688_ImpactReceipt> Receipts = {};
}

class GEP688_ImpactInbox
{
	protected const int MAX_PROJECTILES = 64;
	protected const int MAX_RECEIPTS = 128;
	protected const int MAX_CASE_MS = 30000;
	protected static bool ObservedPrefab(string value)
 {
  return value.IndexOf("{A7846B9D39014801}")==0 || value.IndexOf("{A7846B9D39014802}")==0 || value.IndexOf("{A7846B9D39014803}")==0;
 }
	protected static bool s_Active;
	protected static string s_Nonce;
	protected static IEntity s_SubjectRoot;
	protected static int s_OpenedAt;
	protected static ref map<IEntity, string> s_SourceNonces = new map<IEntity, string>();
	protected static ref GEP688_ImpactReport s_Report;

	// One isolated case at a time. Caller supplies jobNonce + unique case identity.
	static bool Open(string caseNonce, IEntity subject)
	{
		if (s_Active || caseNonce == "" || (s_Report && caseNonce == s_Nonce) || !subject || subject.IsDeleted()) return false;
		s_SubjectRoot = Root(subject);
		if (!s_SubjectRoot) return false;
		s_SourceNonces.Clear(); s_Nonce = caseNonce; s_OpenedAt = System.GetTickCount();
		s_Report = new GEP688_ImpactReport();
		s_Report.CaseNonce = caseNonce;
		s_Report.SubjectRootID = string.Format("%1", s_SubjectRoot.GetID());
		s_Report.SubjectRootPrefab = Prefab(s_SubjectRoot);
		s_Active = true;
		return true;
	}

	// Root calls this AFTER spawning its owned observed M33, BEFORE native Launch.
	// A projectile carries its original registration nonce via this exact identity.
	static bool Register(string caseNonce, IEntity projectile)
	{
		if (!s_Active || !s_Report || caseNonce != s_Nonce) return false;
		if (!projectile || projectile.IsDeleted() || !ObservedPrefab(Prefab(projectile))
			|| s_SourceNonces.Contains(projectile) || s_SourceNonces.Count() >= MAX_PROJECTILES)
		{
			s_Report.RejectedRegistrations++;
			return false;
		}
		s_SourceNonces.Insert(projectile, caseNonce);
		s_Report.RegisteredProjectiles = s_SourceNonces.Count();
		return true;
	}

	static void Close(string caseNonce)
	{
		if (!s_Active || caseNonce != s_Nonce) return;
		s_Active = false;
	}

	// Copies scalar/vector evidence. No output matrix or native effect is retained.
	static void Observe(IEntity hit, IEntity source, string collider, vector position, vector direction, vector normal, float speed)
	{
		if (!s_Active || !s_Report) return;
		int elapsed = System.GetTickCount() - s_OpenedAt;
		if (elapsed < 0 || elapsed > MAX_CASE_MS) { s_Report.ExpiredCallbacks++; return; }
		if (!source || !s_SourceNonces.Contains(source) || s_SourceNonces.Get(source) != s_Nonce)
		{ s_Report.UnregisteredCallbacks++; return; }
		if (!hit || Root(hit) != s_SubjectRoot) { s_Report.OtherCallbacks++; return; }
		s_Report.SubjectCallbacks++;
		if (!FiniteVector(position) || !FiniteVector(direction) || !FiniteVector(normal) || !(speed >= 0 && speed <= 1000000))
		{ s_Report.InvalidSubjectCallbacks++; return; }
		s_Report.ValidSubjectCallbacks++;
		if (s_Report.Receipts.Count() >= MAX_RECEIPTS) { s_Report.DroppedSubjectReceipts++; return; }
		GEP688_ImpactReceipt receipt = new GEP688_ImpactReceipt();
		receipt.CaseNonce = s_Nonce;
		receipt.Ordinal = s_Report.SubjectCallbacks; receipt.MillisecondsFromOpen = elapsed;
		receipt.SourceID = string.Format("%1", source.GetID()); receipt.SourcePrefab = Prefab(source);
		receipt.HitID = string.Format("%1", hit.GetID()); receipt.HitPrefab = Prefab(hit);
		receipt.Collider = collider; receipt.Position = position;
		receipt.Direction = direction; receipt.Normal = normal; receipt.Speed = speed;
		receipt.DirectionKnown = direction.Length() > 0.000001;
		receipt.NormalKnown = normal.Length() > 0.000001;
		s_Report.Receipts.Insert(receipt);
	}

	// Deep snapshot: report writer receives no mutable references to the inbox.
	static GEP688_ImpactReport Snapshot(string caseNonce)
	{
		if (!s_Report || caseNonce != s_Nonce) return null;
		GEP688_ImpactReport result = new GEP688_ImpactReport();
		result.CaseNonce = s_Report.CaseNonce; result.SubjectRootID = s_Report.SubjectRootID;
		result.SubjectRootPrefab = s_Report.SubjectRootPrefab; result.Active = s_Active;
		result.RegisteredProjectiles = s_Report.RegisteredProjectiles;
		result.SubjectCallbacks = s_Report.SubjectCallbacks; result.ValidSubjectCallbacks = s_Report.ValidSubjectCallbacks;
		result.OtherCallbacks = s_Report.OtherCallbacks; result.UnregisteredCallbacks = s_Report.UnregisteredCallbacks;
		result.InvalidSubjectCallbacks = s_Report.InvalidSubjectCallbacks;
		result.DroppedSubjectReceipts = s_Report.DroppedSubjectReceipts;
		result.RejectedRegistrations = s_Report.RejectedRegistrations; result.ExpiredCallbacks = s_Report.ExpiredCallbacks;
		foreach (GEP688_ImpactReceipt receipt : s_Report.Receipts) result.Receipts.Insert(receipt.Copy());
		return result;
	}

	protected static IEntity Root(IEntity entity)
	{
		if (!entity) return null;
		IEntity result = entity.GetRootParent();
		if (!result) result = entity;
		return result;
	}
	protected static string Prefab(IEntity entity)
	{
		if (!entity) return "";
		EntityPrefabData data = entity.GetPrefabData();
		if (!data) return "";
		return data.GetPrefabName();
	}
	protected static bool FiniteVector(vector value)
	{
		return value[0] >= -10000000 && value[0] <= 10000000
			&& value[1] >= -10000000 && value[1] <= 10000000
			&& value[2] >= -10000000 && value[2] <= 10000000;
	}
}

[BaseContainerProps()]
class GEP688_PassiveM33Impact : BaseProjectileEffect
{
	override void OnEffect(IEntity pHitEntity, inout vector outMat[3], IEntity damageSource, notnull Instigator instigator, string colliderName, float speed)
	{
		GEP688_ImpactInbox.Observe(pHitEntity, damageSource, colliderName, outMat[0], outMat[1], outMat[2], speed);
	}
}

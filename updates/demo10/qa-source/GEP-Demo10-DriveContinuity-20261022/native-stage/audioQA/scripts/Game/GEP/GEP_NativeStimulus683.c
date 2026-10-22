// Original integration helper proposal. Unmounted/uncompiled/unrun in this lane.
// One native stimulus per instance. Root's owned harness calls Begin then Tick.
// No health/damage-context writes, input injection, RNG changes or entity deletion.
class GEP_NativeStimulus683
{
	protected bool m_Used;
	protected bool m_Done;
	protected bool m_CommandIssued;
	protected bool m_Blast;
	protected float m_Age;
	protected float m_ReadyAfter;
	protected string m_Error;
	protected ResourceName m_Resource;
	protected IEntity m_Entity;
	protected IEntity m_FixtureOwner;
	protected ProjectileMoveComponent m_Move;
	protected BaseTriggerComponent m_Trigger;
	protected ref Instigator m_Instigator;
	protected vector m_Direction;

	static ResourceName ShellResource(string key)
	{
		if (key == "m855_688") return "{A7846B9D39014801}Prefabs/QA/GEP688_M855Observed.et";
		if (key == "m80_688") return "{A7846B9D39014802}Prefabs/QA/GEP688_M80Observed.et";
		if (key == "m33_688") return "{A7846B9D39014803}Prefabs/QA/GEP688_M33Observed.et";
		if (key == "m33") return "{A4E070505C87AD42}Prefabs/Weapons/Ammo/Ammo_127x99_Ball_M33.et";
		if (key == "m2_ap") return "{6DEC9F287271AB24}Prefabs/Weapons/Ammo/Ammo_127x99_AP_M2.et";
		if (key == "m33_observed") return "{D561AD08932E8001}Prefabs/QA/GEP_ObservedBall.et";
		if (key == "m2_ap_observed") return "{D561AD08932E8011}Prefabs/QA/GEP_ObservedAPBall.et";
		if (key == "m792_25mm") return "{A47C098284929418}Prefabs/Weapons/Ammo/Ammo_25x137/Ammo_25x137_HEIT_M792.et";
		if (key == "gep_35mm") return "{D561AD08932E4010}Prefabs/Weapons/GEP_35_HEIT_Projectile.et";
		return "";
	}
	static ResourceName BlastResource(string key)
	{
		if (key == "rgd5") return "{F8DFA475BC5A3B7C}Prefabs/Weapons/Warheads/Warhead_Grenade_RGD5.et";
		if (key == "m67") return "{9C7B7B7ECDC3A596}Prefabs/Weapons/Warheads/Warhead_Grenade_M67.et";
		if (key == "m792_25mm") return "{627F225E34E4B5F1}Prefabs/Weapons/Warheads/Warhead_HEIT_M792.et";
		if (key == "gep_35mm") return "{D561AD08932E4011}Prefabs/Weapons/GEP_35_HEIT_Warhead.et";
		return "";
	}

	bool Begin(bool isolatedOwnedFixture, bool blast, string key, IEntity fixtureOwner, vector source, vector aimPoint, string caseNonce688="")
	{
		if (m_Used) return false;
		m_Used = true;
		if (!isolatedOwnedFixture || !GetGame() || !GetGame().InPlayMode() || !fixtureOwner || fixtureOwner.IsDeleted()) return Fail("fixture_guard");
		if (Replication.IsRunning() && !Replication.IsServer()) return Fail("not_server");
		RplComponent authority = RplComponent.Cast(fixtureOwner.FindComponent(RplComponent));
		if (authority && authority.IsProxy()) return Fail("fixture_proxy");
		if (!Position(source) || !Position(aimPoint)) return Fail("invalid_coordinates");
		vector delta = aimPoint - source;
		float distance = delta.Length();
		if (!blast && !(distance > 1 && distance <= 1000)) return Fail("shell_range_outside_1_to_1000m");
		m_Blast = blast;
		m_Resource = ShellResource(key);
		if (blast) m_Resource = BlastResource(key);
		if (m_Resource == "") return Fail("unknown_payload_key");
		Resource resource = Resource.Load(m_Resource);
		if (!resource || !resource.IsValid()) return Fail("resource_missing");
		m_Instigator = Instigator.CreateInstigator(fixtureOwner);
		if (!m_Instigator) return Fail("instigator_missing");
		EntitySpawnParams spawn = new EntitySpawnParams();
		spawn.TransformMode = ETransformMode.WORLD;
		vector frame[4];
		Math3D.MatrixIdentity4(frame);
		if (!blast)
		{
			m_Direction = delta / distance;
			vector up = vector.Up;
			if (Math.AbsFloat(vector.Dot(up, m_Direction)) > 0.99) up = Vector(1, 0, 0);
			SCR_Math3D.LookAt(source, aimPoint, up, frame);
		}
		frame[3] = source;
		spawn.Transform = frame;
		m_Entity = GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), spawn);
		if (!m_Entity) return Fail("spawn_failed");

  if(key.Contains("_688") && !GEP688_ImpactInbox.Register(caseNonce688,m_Entity))return Fail("exact_projectile_registration_failed688");
		m_FixtureOwner = fixtureOwner;
		m_Trigger = BaseTriggerComponent.Cast(m_Entity.FindComponent(BaseTriggerComponent));
		m_Move = ProjectileMoveComponent.Cast(m_Entity.FindComponent(ProjectileMoveComponent));
		if (blast)
		{
			if (!m_Trigger || m_Move || m_Trigger.WasTriggered()) return Fail("warhead_contract");
			m_Trigger.SetInstigator(m_Instigator);
			float arming = m_Trigger.GetArmingTime();
			if (!(arming >= 0 && arming <= 2)) return Fail("warhead_arming_unqualified");
			m_Trigger.SetLive();
			m_ReadyAfter = 0.05 + arming;
		}
		else
		{
			if (!m_Move || !ShellMoveComponent.Cast(m_Move)) return Fail("shell_move_missing");
			m_Move.SetInstigator(m_Instigator);
			if (m_Trigger) m_Trigger.SetInstigator(m_Instigator);
			// Preserve authored collision/fuse safety. Native Launch owns simulation.
			m_ReadyAfter = 0.05;
		}
		PrintFormat("GEP683_STIMULUS_READY resource=%1 blast=%2 origin=%3 aimPoint=%4 armingWait=%5 seed=UNCONTROLLED physical=0", m_Resource, blast, source, aimPoint, m_ReadyAfter);
		return true;
	}

	void Tick(float dt)
	{
		if (!m_Used || m_Done) return;
		if (!(dt >= 0 && dt <= 1)) { Fail("invalid_tick"); return; }
		m_Age += dt;
		if (m_Age > 3) { Fail("command_timeout"); return; }
		if (m_Age < m_ReadyAfter) return;
		if (!m_Entity || m_Entity.IsDeleted() || !m_FixtureOwner || m_FixtureOwner.IsDeleted()) { Fail("fixture_lost"); return; }
		// Reserve command before the native call: never retry a stimulus.
		m_Done = true;
		m_CommandIssued = true;
		if (m_Blast)
		{
			if (!m_Trigger || m_Trigger.WasTriggered()) { m_CommandIssued = false; Fail("trigger_changed"); return; }
			PrintFormat("GEP683_NATIVE_BLAST resource=%1 entity=%2 origin=%3 nativeTrigger=1 physical=0 armorConclusion=0", m_Resource, m_Entity, m_Entity.GetOrigin());
			m_Trigger.OnUserTriggerOverrideInstigator(m_Entity, m_Instigator);
			return;
		}
		if (!m_Move) { m_CommandIssued = false; Fail("movement_lost"); return; }
		// Same native contract as the installed server-side effect-module caller.
		m_Move.Launch(m_Direction, vector.Zero, 1.0, m_Entity, null, m_FixtureOwner, null, null);
		// Keep explicit fixture attribution after native launch initialization.
		m_Move.SetInstigator(m_Instigator);
		if (m_Trigger) m_Trigger.SetInstigator(m_Instigator);
		PrintFormat("GEP683_NATIVE_LAUNCH resource=%1 entity=%2 direction=%3 immediateVelocity=%4 coefficient=1 nativeLaunch=1 physical=0 armorConclusion=0", m_Resource, m_Entity, m_Direction, m_Move.GetVelocity());
	}

	bool Done() { return m_Done; }
	bool CommandIssued() { return m_CommandIssued; }
	string Error() { return m_Error; }
	IEntity SpawnedEntity() { return m_Entity; }
	protected bool Fail(string reason)
	{
		m_Done = true;
		m_Error = reason;
		PrintFormat("GEP683_STIMULUS_FAILED reason=%1 resource=%2 commandIssued=%3 physical=0", reason, m_Resource, m_CommandIssued);
		return false;
	}
	protected bool Position(vector value)
	{
		return value[0] >= -1000000 && value[0] <= 1000000 && value[1] >= -1000000 && value[1] <= 1000000 && value[2] >= -1000000 && value[2] <= 1000000;
	}
}

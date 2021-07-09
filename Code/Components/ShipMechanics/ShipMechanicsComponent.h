#pragma once

#include <CryRenderer/IRenderer.h>
#include <CryRenderer/IShader.h>

#include <CrySchematyc/ResourceTypes.h>
#include <CrySchematyc/MathTypes.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CryPhysics/physinterface.h>

#include <DefaultComponents/Geometry/StaticMeshComponent.h>

#include "Common/NavigationTypes.h"

class CPlugin_CryDefaultEntities;
//! \namespace Navigation
//! \brief The ship navigation namespace.
namespace Navigation
{
//! \class CShipMechanicsComponent
//! \brief The main class of the ship
class CShipMechanicsComponent : public IEntityComponent
{
	enum class EChangeVelocityMode : int
	{
		SetAsTarget = 0, // velocity change with snapping to the ground, can be used for walking
		Jump = 1, // instant velocity change, can be used for jumping or flying
		Add = 2, // adding velocity to the current value of velocity
	};
protected:
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);

	// IEntityComponent
	virtual void Initialize() override;

	virtual void ProcessEvent(const SEntityEvent& event) override;
	virtual Cry::Entity::EventFlags GetEventMask() const override;

	virtual bool NetSerialize(TSerialize ser, EEntityAspects aspect, uint8 profile, int flags) override;
	virtual NetworkAspectType GetNetSerializeAspectMask() const override { return eEA_Physics; };
	// ~IEntityComponent

public:
	CShipMechanicsComponent() {}
	virtual ~CShipMechanicsComponent();
	//! \brief get ship type
	//! \return {string} type of the ship
	const string GetShipType() const
	{
		return m_shipType.c_str();
	}

	//! \brief set ship type
	//! \param shipType {string} new type of the ship
	void SetShipType(const string& shipType)
	{
		m_shipType = shipType;
	}

	//! \brief get ship name
	//! \return {string} name of the ship
	const string GetShipName() const
	{
		return m_shipName.c_str();
	}

	//! \brief set ship name
	//! \param shipName {string} new name of the ship
	void SetShipName(const string& shipName)
	{
		m_shipName = shipName;
	}

	//! \brief transfer control of the ship
	//! \param shipControlBy {EShipControl}
	//! \param controlId {int} controlling id
	void SetControllingBy(const EShipControl& shipControlBy, const int controllingId = -1)
	{
		m_shipControlBy = shipControlBy;
		m_shipControllingId = controllingId;
		string controlBy;
		switch (m_shipControlBy)
		{
		case Navigation::EShipControl::AutoPilot:
			controlBy = "Autopilot";
			break;
		case Navigation::EShipControl::Player:
			controlBy = "Player";
			break;
		default:
			controlBy = "Ufo";
			break;
		}
		CryLog("%s (%d) took control of the ship '%s'", controlBy, controllingId, GetShipName());
	}

	//! \brief get info about who control of the ship
	//! \return {EShipControl} who control the ship
	EShipControl GetControllingBy() const
	{
		return m_shipControlBy;
	}

	//! \brief get owner controlling id
	//! \return {int} controlling id
	int GetControllingId() const
	{
		return m_shipControllingId;
	}

	//! \brief update movement parameters from status ship
	//! \param frameTime The frame's difference time, in milliseconds.
	void UpdateMovementParameters(float frameTime);

	struct SBuoyancyParameters
	{
		inline bool operator==(const SBuoyancyParameters& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }

		static void ReflectType(Schematyc::CTypeDesc<SBuoyancyParameters>& desc)
		{
			desc.SetGUID("{B94D6ECD-B1F0-4710-BB40-61527F408939}"_cry_guid);
			desc.SetLabel("Buoyancy Parameters dev");
			desc.AddMember(&CShipMechanicsComponent::SBuoyancyParameters::damping, 'damp', "Damping", "Damping", "Uniform damping while submerged, will be scaled with submerged fraction", 0.f);
			desc.AddMember(&CShipMechanicsComponent::SBuoyancyParameters::density, 'dens', "Density", "Density", "Density of the fluid", 1.f);
			desc.AddMember(&CShipMechanicsComponent::SBuoyancyParameters::resistance, 'rest', "Resistance", "Resistance", "Resistance of the fluid", 1.f);
		}

		Schematyc::PositiveFloat damping = 1000.0f;
		Schematyc::PositiveFloat density = 1000.0f;
		Schematyc::PositiveFloat resistance = 10000.0f;
	};

	struct SSimulationParameters
	{
		inline bool operator==(const SSimulationParameters& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }

		static void ReflectType(Schematyc::CTypeDesc<SSimulationParameters>& desc)
		{
			desc.SetGUID("{1058335C-F546-4EB6-9D37-155D8177A03E}"_cry_guid);
			desc.SetLabel("Simulation Parameters dev");
			desc.AddMember(&CShipMechanicsComponent::SSimulationParameters::maxTimeStep, 'maxt', "MaxTimeStep", "Maximum Time Step", "The largest time step the entity can make before splitting. Smaller time steps increase stability (can be required for long and thin objects, for instance), but are more expensive.", 0.02f);
			desc.AddMember(&CShipMechanicsComponent::SSimulationParameters::sleepSpeed, 'slps', "SleepSpeed", "Sleep Speed", "If the object's kinetic energy falls below some limit over several frames, the object is considered sleeping. This limit is proportional to the square of the sleep speed value.", 0.04f);
			desc.AddMember(&CShipMechanicsComponent::SSimulationParameters::damping, 'damp', "Damping", "Damping", "This sets the strength of damping on an object's movement. Most objects can work with 0 damping; if an object has trouble coming to rest, try values like 0.2-0.3.", 0.0f);
		}

		Schematyc::PositiveFloat maxTimeStep = 0.02f;
		Schematyc::PositiveFloat sleepSpeed = 0.04f;
		float                    damping = 0.0f;
	};

	static void ReflectType(Schematyc::CTypeDesc<CShipMechanicsComponent>& desc)
	{
		desc.SetGUID("{207A2E52-3317-4638-B9B6-D1EAE6AB980B}"_cry_guid);
		desc.SetEditorCategory("Navigation");
		desc.SetLabel("Ship Mechanics Base");
		desc.SetDescription("Creates a physical representation of the entity, ");
		desc.SetIcon("icons:ObjectTypes/object.ico");
		desc.SetComponentFlags({ IEntityComponent::EFlags::Socket, IEntityComponent::EFlags::Attach, IEntityComponent::EFlags::Singleton });

		// Mark the Character Controller component as incompatible
		desc.AddComponentInteraction(SEntityComponentRequirements::EType::Incompatibility, "{62EA28A3-DAAC-4889-A6DC-70A59CCDA9FD}"_cry_guid);
		// Mark the Vehicle Physics component as incompatible
		desc.AddComponentInteraction(SEntityComponentRequirements::EType::Incompatibility, "{5D97D109-1F94-4DCA-9FD9-29BCF6EFB853}}"_cry_guid);
		// Mark the Area component as incompatible
		desc.AddComponentInteraction(SEntityComponentRequirements::EType::Incompatibility, "{82684621-C8B6-47C6-8DD8-494C231EA980}"_cry_guid);
		// Static meshes must be initialized since they need to have slots assigned during physicalization
		desc.AddComponentInteraction(SEntityComponentRequirements::EType::SoftDependency, "{CD4A3285-A2CF-4017-A21D-C3852A2E61DA}"_cry_guid);

		// TODO: Min: default value for CSharedString. Max: List of ship types dynamically from a xml file to select.
		desc.AddMember(&CShipMechanicsComponent::m_shipType, 'ship', "ShipType", "Ship Type", nullptr, Schematyc::CSharedString("VolgoDon"));
		desc.AddMember(&CShipMechanicsComponent::m_shipName, 'name', "ShipName", "Ship Name", nullptr, "");

		desc.AddMember(&CShipMechanicsComponent::m_isNetworked, 'netw', "Networked", "Network Synced", "Syncs the physical entity over the network, and keeps it in sync with the server", false);
		desc.AddMember(&CShipMechanicsComponent::m_isEnabledByDefault, 'enab', "EnabledByDefault", "Enabled by Default", "Whether the component is enabled by default", false);
		desc.AddMember(&CShipMechanicsComponent::m_isResting, 'res', "Resting", "Resting", "If resting is enabled the object will only start to be simulated if it was hit by something else.", false);
		desc.AddMember(&CShipMechanicsComponent::m_type, 'type', "Type", "Type", "Type of physicalized object to create", CShipMechanicsComponent::EPhysicalType::Rigid);
		desc.AddMember(&CShipMechanicsComponent::m_bSendCollisionSignal, 'send', "SendCollisionSignal", "Send Collision Signal", "Whether or not this component should listen for collisions and report them", false);
		desc.AddMember(&CShipMechanicsComponent::m_buoyancyParameters, 'buoy', "Buoyancy", "Buoyancy Parameters", "Fluid behavior related to this entity", SBuoyancyParameters());
		desc.AddMember(&CShipMechanicsComponent::m_simulationParameters, 'simp', "Simulation", "Simulation Parameters", "Parameters related to the simulation of this entity", SSimulationParameters());
		desc.AddMember(&CShipMechanicsComponent::m_physics, 'phys', "Physics", "Physics Parameters", "Parameters related to the physics of this entity", SPhysics());
		desc.AddMember(&CShipMechanicsComponent::m_movement, 'move', "Movement", "Movement Parameters", "Parameters related to the movement of this entity", SMovement());

		desc.AddMember(&CShipMechanicsComponent::m_engineParams, 'engn', "EngineParams", "Engine Parameters", nullptr, CShipMechanicsComponent::SEngineParams());
		desc.AddMember(&CShipMechanicsComponent::m_gearParams, 'gear', "GearParams", "Gear Parameters", nullptr, CShipMechanicsComponent::SGearParams());
	}

	struct SCollisionSignal
	{
		Schematyc::ExplicitEntityId otherEntity;
		Schematyc::SurfaceTypeName surfaceType;

		SCollisionSignal() {};
		SCollisionSignal(EntityId id, const Schematyc::SurfaceTypeName& srfType) : otherEntity(Schematyc::ExplicitEntityId(id)), surfaceType(srfType) {}
	};

	enum class EPhysicalType
	{
		Static = PE_STATIC,
		Rigid = PE_RIGID,
		Articulated = PE_ARTICULATED
	};

	virtual void AddVelocity(const Vec3& velocity)
	{
		if (IPhysicalEntity* pPhysicalEntity = m_pEntity->GetPhysics())
		{
			pe_action_move moveAction;

			// Apply movement request directly to velocity
			moveAction.iJump = 2;
			moveAction.dir = velocity;

			// Dispatch the movement request
			pPhysicalEntity->Action(&moveAction);
		}
	}

	virtual void Enable(bool bEnable)
	{
		m_pEntity->EnablePhysics(bEnable);
	}

	bool IsEnabled() const
	{
		return m_pEntity->IsPhysicsEnabled();
	}

	virtual void Physicalize();

	struct SPhysics
	{
		inline bool operator==(const SPhysics& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }

		static void ReflectType(Schematyc::CTypeDesc<SPhysics>& desc)
		{
			desc.SetGUID("{C6DFC4FE-E919-48C0-B37B-66E7FD8BA38F}"_cry_guid);
			desc.AddMember(&CShipMechanicsComponent::SPhysics::m_mass, 'mass', "Mass", "Mass", "Mass of the character in kg", 5000000.f);
			desc.AddMember(&CShipMechanicsComponent::SPhysics::m_radius, 'radi', "Radius", "Collider Radius", "Radius of the capsule or cylinder", 0.45f);
			desc.AddMember(&CShipMechanicsComponent::SPhysics::m_height, 'heig', "Height", "Collider Height", "Height of the capsule or cylinder", 0.935f);
			desc.AddMember(&CShipMechanicsComponent::SPhysics::m_bCapsule, 'caps', "Capsule", "Use Capsule", "Whether or not to use a capsule as the main collider, otherwise cylinder", true);
			desc.AddMember(&CShipMechanicsComponent::SPhysics::m_groundContactEps, 'gce', "GroundContactEps", "Ground Contact Epsilon", "The amount that the player needs to move upwards before ground contact is lost", 0.004f);
			desc.AddMember(&CShipMechanicsComponent::SPhysics::m_bSendCollisionSignal, 'send', "SendCollisionSignal", "Send Collision Signal", "Whether or not this component should listen for collisions and report them", false);
		}

		Schematyc::PositiveFloat m_mass = 5000000.f;
		float m_radius = 10.45f;
		float m_height = 110.935f;
		bool m_bCapsule = false;
		Schematyc::Range<-100, 100> m_groundContactEps = 0.4f;

		bool m_bSendCollisionSignal = false;
	};

	struct SMovement
	{
		inline bool operator==(const SMovement& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }

		static void ReflectType(Schematyc::CTypeDesc<SMovement>& desc)
		{
			desc.SetGUID("{C14ACD32-B736-44F1-85D3-CD710A5E8002}"_cry_guid);
			desc.AddMember(&CShipMechanicsComponent::SMovement::m_airControlRatio, 'airc', "AirControl", "Air Control Ratio", "Indicates how much the character can move in the air, 0 means no movement while 1 means full control.", 1.f);
			desc.AddMember(&CShipMechanicsComponent::SMovement::m_airResistance, 'airr', "AirResistance", "Air Resistance", nullptr, 0.2f);
			desc.AddMember(&CShipMechanicsComponent::SMovement::m_inertia, 'iner', "Inertia", "Inertia Coefficient", "More amount gives less inertia, 0 being none", 8.f);
			desc.AddMember(&CShipMechanicsComponent::SMovement::m_inertiaAcceleration, 'inea', "InertiaAcc", "Inertia Acceleration Coefficient", "More amount gives less inertia on acceleration, 0 being none", 8.f);
			desc.AddMember(&CShipMechanicsComponent::SMovement::m_maxGroundVelocity, 'maxg', "MaxGroundVelocity", "Maximum Surface Velocity", "Maximum velocity of the surface the character is on before they are considered airborne and slide off", DEG2RAD(50.f));
		}

		Schematyc::Range<0, 1> m_airControlRatio = 0.f;
		Schematyc::Range<0, 10000> m_airResistance = 0.2f;
		Schematyc::Range<0, 10000> m_inertia = 8.f;
		Schematyc::Range<0, 10000> m_inertiaAcceleration = 8.f;

		Schematyc::Range<0, 10000> m_maxGroundVelocity = 16.f;
	};


	static void ReflectType(Schematyc::CTypeDesc<CShipMechanicsComponent::EChangeVelocityMode>& desc)
	{
		desc.SetGUID("{9D3C812E-0136-10EB-8364-457DBABBE871}"_cry_guid);
		desc.SetLabel("Velocity change mode dev");

		desc.AddConstant(CShipMechanicsComponent::EChangeVelocityMode::SetAsTarget, "SetAsTarget", "Velocity change with snapping to the ground, can be used for walking");
		desc.AddConstant(CShipMechanicsComponent::EChangeVelocityMode::Jump, "Jump", "Instant velocity change, can be used for jumping or flying");
		desc.AddConstant(CShipMechanicsComponent::EChangeVelocityMode::Add, "Add", "Adding velocity to the current value of velocity");
	}

	virtual void ChangeVelocity(const Vec3& velocity, const EChangeVelocityMode mode)
	{
		if (IPhysicalEntity* pPhysicalEntity = m_pEntity->GetPhysicalEntity())
		{
			pe_action_move moveAction;

			// Override velocity
			moveAction.iJump = static_cast<int>(mode);
			moveAction.dir = velocity;

			// Dispatch the movement request
			pPhysicalEntity->Action(&moveAction);
		}
	}

	// const Vec3 &GetVelocity() const { return m_velocity; }
	Vec3 GetMoveDirection() const { return m_velocity.GetNormalized(); }

	bool IsWalking() const { return m_velocity.GetLength2D() > 0.2f && m_bOnGround; }

	struct SEngineParams
	{
		inline bool operator==(const SEngineParams& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }

		static void ReflectType(Schematyc::CTypeDesc<SEngineParams>& desc)
		{
			desc.SetGUID("{27293757-F925-4441-ABC7-57C06594F763}"_cry_guid);
			desc.SetLabel("Engine Parameters dev");
			desc.AddMember(&CShipMechanicsComponent::SEngineParams::m_power, 'powe', "Power", "Power", "Power of the engine", 10000.f);
			desc.AddMember(&CShipMechanicsComponent::SEngineParams::m_maxRPM, 'maxr', "MaxRPM", "Maximum RPM", "engine torque decreases to 0 after reaching this rotation speed", 1200.f);
			desc.AddMember(&CShipMechanicsComponent::SEngineParams::m_minRPM, 'minr', "MinRPM", "Minimum RPM", "disengages the clutch when falling behind this limit, if braking with the engine", 60.f);
			desc.AddMember(&CShipMechanicsComponent::SEngineParams::m_idleRPM, 'idle', "IdleRPM", "Idle RPM", "RPM for idle state", 120.f);
			desc.AddMember(&CShipMechanicsComponent::SEngineParams::m_startRPM, 'star', "StartRPM", "Start RPM", "RPM when the engine is started", 400.f);
		}

		Schematyc::Range<0, 10000000> m_power = 50000.f;
		float m_maxRPM = 1200.f;
		float m_minRPM = 60.f;
		float m_idleRPM = 120.f;
		float m_startRPM = 400.f;
	};

	struct SGearParams
	{
		inline bool operator==(const SGearParams& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }

		struct SGear
		{
			inline bool operator==(const SGear& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }
			inline bool operator!=(const SGear& rhs) const { return 0 != memcmp(this, &rhs, sizeof(rhs)); }

			void Serialize(Serialization::IArchive& archive)
			{
				archive(m_ratio, "Ratio", "Ratio");
			}

			static void ReflectType(Schematyc::CTypeDesc<SGear>& desc)
			{
				desc.SetGUID("{BE0183E0-5C2A-410F-A30E-43DE999C09B6}"_cry_guid);
				desc.SetLabel("Gear dev");
				desc.AddMember(&CShipMechanicsComponent::SGearParams::SGear::m_ratio, 'rati', "Ratio", "Ratio", "assumes 0-backward gear, 1-neutral, 2 and above - forward", 2.f);
			}

			Schematyc::Range<0, 2> m_ratio;
		};

		static void ReflectType(Schematyc::CTypeDesc<SGearParams>& desc)
		{
			desc.SetGUID("{847B210E-C87B-490D-AD3F-8D45762861C4}"_cry_guid);
			desc.SetLabel("Gear Parameters dev");
			desc.AddMember(&CShipMechanicsComponent::SGearParams::m_gears, 'gear', "Gears", "Gears", "Specifies number of gears, and their parameters", Schematyc::CArray<CShipMechanicsComponent::SGearParams::SGear>());
			desc.AddMember(&CShipMechanicsComponent::SGearParams::m_shiftUpRPM, 'shiu', "ShiftUpRPM", "Shift Up RPM", "RPM threshold for for automatic gear switching", 600.f);
			desc.AddMember(&CShipMechanicsComponent::SGearParams::m_shiftDownRPM, 'shid', "ShiftDownRPM", "Shift Down RPM", "RPM threshold for for automatic gear switching", 240.f);
			desc.AddMember(&CShipMechanicsComponent::SGearParams::m_directionSwitchRPM, 'dirs', "DirectionSwitchRPM", "Direction Switch RPM", "RPM threshold for switching back and forward gears", 10.f);
		}

		Schematyc::CArray<SGear> m_gears;

		float m_shiftUpRPM = 600.f;
		float m_shiftDownRPM = 240.f;

		float m_directionSwitchRPM = 10.f;
	};

	enum class EGear
	{
		Reverse = -1,
		Neutral,
		// First forward gear, note how there can be more specified by the user
		Forward,

		DefaultCount = Forward + 2
	};
	virtual void UseHandbrake(bool bSet)
	{
		pe_action_drive driveAction;
		driveAction.bHandBrake = bSet ? 1 : 0;
		m_pEntity->GetPhysicalEntity()->Action(&driveAction);
	}
	bool IsUsingHandbrake() const { return m_shipStatus.anchorBrake; }

	virtual void SetThrottle(Schematyc::Range<0, 1> ratio)
	{
		pe_action_drive driveAction;
		driveAction.pedal = ratio;
		m_pEntity->GetPhysicalEntity()->Action(&driveAction);
	}

	void SetBrake(Schematyc::Range<0, 1> ratio)
	{
		pe_action_drive driveAction;
		driveAction.pedal = -ratio;
		m_pEntity->GetPhysicalEntity()->Action(&driveAction);
	}
	// Schematyc::Range<0, 1> GetBrake() const { return m_shipStatus.footbrake; }

	virtual void SetCurrentGear(int gearId)
	{
		pe_action_drive driveAction;
		driveAction.iGear = gearId + 1;
		m_pEntity->GetPhysicalEntity()->Action(&driveAction);
	}
	int GetLeftGear() const { return m_shipStatus.leftGear - 1; }

	virtual void GearLeftUp() { SetCurrentGear(min(m_shipStatus.leftGear + 1, (int) m_gearParams.m_gears.Size())); }
	virtual void GearLeftDown() { SetCurrentGear(max(m_shipStatus.leftGear - 1, (int) EGear::Reverse + 1)); }

	virtual void SetSteeringAngle(CryTransform::CAngle angle)
	{
		pe_action_drive driveAction;
		driveAction.steer = angle.ToRadians();
		m_pEntity->GetPhysicalEntity()->Action(&driveAction);
	}
	CryTransform::CAngle GetSteeringAngle() const { return CryTransform::CAngle::FromRadians(m_shipStatus.steer); }

	float GetLeftEngineRPM() const { return m_shipStatus.leftEngineRPM; }
	float GetRightEngineRPM() const { return m_shipStatus.rightEngineRPM; }

	virtual void SetVelocity(const Vec3& velocity)
	{
		if (IPhysicalEntity* pPhysicalEntity = m_pEntity->GetPhysics())
		{
			pe_action_set_velocity action_set_velocity;
			action_set_velocity.v = velocity;
			pPhysicalEntity->Action(&action_set_velocity);
		}
	}

	Vec3 GetVelocity() const
	{
		if (IPhysicalEntity* pPhysicalEntity = m_pEntity->GetPhysics())
		{
			pe_status_dynamics dynStatus;
			if (pPhysicalEntity->GetStatus(&dynStatus))
			{
				return dynStatus.v;
			}
		}

		return ZERO;
	}

	virtual void SetAngularVelocity(const Vec3& angularVelocity)
	{
		if (IPhysicalEntity* pPhysicalEntity = m_pEntity->GetPhysics())
		{
			pe_action_set_velocity action_set_velocity;
			action_set_velocity.w = angularVelocity;
			pPhysicalEntity->Action(&action_set_velocity);
		}
	}

	Vec3 GetAngularVelocity() const
	{
		if (IPhysicalEntity* pPhysicalEntity = m_pEntity->GetPhysics())
		{
			pe_status_dynamics dynStatus;
			if (pPhysicalEntity->GetStatus(&dynStatus))
			{
				return dynStatus.w;
			}
		}

		return ZERO;
	}

	virtual void ApplyImpulse(const Vec3& force)
	{
		// Only dispatch the impulse to physics if one was provided
		if (!force.IsZero())
		{
			if (IPhysicalEntity* pPhysicalEntity = m_pEntity->GetPhysics())
			{
				pe_action_impulse impulseAction;
				impulseAction.impulse = force;

				pPhysicalEntity->Action(&impulseAction);
			}
		}
	}

	virtual void ApplyAngularImpulse(const Vec3& force)
	{
		// Only dispatch the impulse to physics if one was provided
		if (!force.IsZero())
		{
			if (IPhysicalEntity* pPhysicalEntity = m_pEntity->GetPhysics())
			{
				pe_action_impulse impulseAction;
				impulseAction.angImpulse = force;

				pPhysicalEntity->Action(&impulseAction);
			}
		}
	}

public:
	Cry::DefaultComponents::CStaticMeshComponent* m_pShipMesh = nullptr;

	SPhysics m_physics;
	SMovement m_movement;
	bool m_bOnGround = false;
	Schematyc::UnitLength<Vec3> m_groundNormal = Vec3(0, 0, 1);
	Vec3 m_velocity = ZERO;

public:
	bool m_isNetworked = false;

	bool m_isResting = false;

	bool m_isEnabledByDefault = true;
	EPhysicalType m_type = EPhysicalType::Rigid;
	bool m_bSendCollisionSignal = true;
	SBuoyancyParameters m_buoyancyParameters;
	SSimulationParameters m_simulationParameters;

	std::vector<float> m_gearRatios;

	SEngineParams m_engineParams;
	SGearParams m_gearParams;

	SStatusShip m_shipStatus;						//!< status of the ship
	SShipParameters m_shipParameters;				//!< parameters of the ship

protected:
	Schematyc::CSharedString m_shipType = "VolgoDon";			//!< type of the ship
	Schematyc::CSharedString m_shipName;						//!< name of the ship
	EShipControl m_shipControlBy = EShipControl::AutoPilot;		//!< who is responsible for steering the ship
	int m_shipControllingId = -1;								//!< controlling id
	EShipState m_shipState = EShipState::Normal;				//!< state of the ship (Normal, RanAground)
};

static void ReflectType(Schematyc::CTypeDesc<CShipMechanicsComponent::EPhysicalType>& desc)
{
	desc.SetGUID("{968E03B0-E0D4-4C91-A6DB-B7D44361B833}"_cry_guid);
	desc.SetLabel("Simple Physicalization Type");
	desc.SetDescription("Determines how to physicalize a simple (CGF) object");
	desc.SetDefaultValue(CShipMechanicsComponent::EPhysicalType::Rigid);
	desc.AddConstant(CShipMechanicsComponent::EPhysicalType::Static, "Static", "Static");
	desc.AddConstant(CShipMechanicsComponent::EPhysicalType::Rigid, "Ship", "Ship");
}

} // namespace Navigation


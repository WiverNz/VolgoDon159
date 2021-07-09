#include "StdAfx.h"
#include "ShipMechanicsComponent.h"

//#include "Physics/CharacterControllerComponent.h"
//#include "Physics/Vehicles/VehicleComponent.h"

#include <Cry3DEngine/IRenderNode.h>
#include <Cry3DEngine/ISurfaceType.h>
#include <CrySchematyc/Env/Elements/EnvFunction.h>
#include <CrySchematyc/Env/Elements/EnvSignal.h>
#include <CrySchematyc/IObject.h>
#include <CryFlowGraph/IFlowSystem.h>

#include <CryCore/StaticInstanceList.h>						// CRY_STATIC_AUTO_REGISTER_FUNCTION
#include <CrySchematyc/Env/Elements/EnvComponent.h>			// SCHEMATYC_MAKE_ENV_COMPONENT
#include <CryRenderer/IRenderAuxGeom.h>

#include "Common/ConfigLoader.h"

static void RegisterShipMechanicsComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Navigation::CShipMechanicsComponent));
		// Functions
		{
		}
	}
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterShipMechanicsComponent)

namespace Navigation
{
void CShipMechanicsComponent::Register(Schematyc::CEnvRegistrationScope& componentScope)
{
	// Functions
	{
		auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CShipMechanicsComponent::Enable, "{97D3FBEF-B4D2-468F-BE0D-67585EC83058}"_cry_guid, "Enable");
		pFunction->SetDescription("Enables the physical collider");
		pFunction->BindInput(1, 'enab', "enable");
		componentScope.Register(pFunction);
	}

	{
		auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CShipMechanicsComponent::IsEnabled, "{E79F2AB4-AAD5-4EFC-8B59-46BAD514AA8E}"_cry_guid, "IsEnabled");
		pFunction->SetDescription("Checks if the physical collider is enabled");
		pFunction->BindOutput(0, 'enab', "enabled");
		componentScope.Register(pFunction);
	}

	{
		auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CShipMechanicsComponent::SetVelocity, "{5F8C8EDB-7E7B-4573-9631-D58743317956}"_cry_guid, "SetVelocity");
		pFunction->SetDescription("Set Entity Velocity");
		pFunction->BindInput(1, 'vel', "velocity");
		pFunction->BindInput(2, 'part', "part id", "Physical part id; <0 to ignore and use default", -1);
		componentScope.Register(pFunction);
	}

	{
		auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CShipMechanicsComponent::GetVelocity, "{FB9937D9-ADB1-4D91-B5B8-F25BAE58E1A6}"_cry_guid, "GetVelocity");
		pFunction->SetDescription("Get Entity Velocity");
		pFunction->BindInput(1, 'part', "part id", "Physical part id; <0 to ignore and use default", -1);
		pFunction->BindOutput(0, 'vel', "velocity");
		componentScope.Register(pFunction);
	}

	{
		auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CShipMechanicsComponent::SetAngularVelocity, "{23D31227-93CA-4240-8F63-D87798352C9D}"_cry_guid, "SetAngularVelocity");
		pFunction->SetDescription("Set Entity Angular Velocity");
		pFunction->BindInput(1, 'vel', "angular velocity");
		pFunction->BindInput(2, 'part', "part id", "Physical part id; <0 to ignore and use default", -1);
		componentScope.Register(pFunction);
	}

	{
		auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CShipMechanicsComponent::GetAngularVelocity, "{3E7CB766-FC6C-4D6D-98EE-E15E8CD7055D}"_cry_guid, "GetAngularVelocity");
		pFunction->SetDescription("Get Entity Angular Velocity");
		pFunction->BindInput(1, 'part', "part id", "Physical part id; <0 to ignore and use default", -1);
		pFunction->BindOutput(0, 'vel', "angular velocity");
		componentScope.Register(pFunction);
	}

	{
		auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CShipMechanicsComponent::ApplyImpulse, "{2E785DA6-6B25-45EE-88F0-351618460E07}"_cry_guid, "ApplyImpulse");
		pFunction->SetDescription("Applies an impulse to the physics object");
		pFunction->BindInput(1, 'for', "Force", "Force of the impulse", Vec3(ZERO));
		pFunction->BindInput(2, 'pt', "Point", "Application point in the world space; (0,0,0) to ignore the point and apply the impulse to the center of mass", Vec3(ZERO));
		pFunction->BindInput(3, 'part', "Part Id", "Physical part id; <0 to ignore and use default", -1);
		componentScope.Register(pFunction);
	}
	{
		auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CShipMechanicsComponent::ApplyAngularImpulse, "{680DBE3B-6201-4992-9014-207600B66EB4}"_cry_guid, "ApplyAngularImpulse");
		pFunction->SetDescription("Applies an angular impulse to the physics object");
		pFunction->BindInput(1, 'for', "Force", "Force of the impulse", Vec3(ZERO));
		pFunction->BindInput(2, 'part', "Part Id", "Physical part id; <0 to ignore and use default", -1);
		componentScope.Register(pFunction);
	}
	{
		auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CShipMechanicsComponent::AddVelocity, "{F6852EE6-9472-42B4-3C43-8F32A8842A2B}"_cry_guid, "AddVelocity");
		pFunction->SetDescription("Adds velocity to that of the character");
		pFunction->BindInput(1, 'vel', "Velocity");
		componentScope.Register(pFunction);
	}
	/*
	{
		auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CShipMechanicsComponent::ChangeVelocity, "{6F95D84F-DA1A-43B8-D9EA-5EFB503F5B44}"_cry_guid, "ChangeVelocity");
		pFunction->SetDescription("Changes the character velocity using specified mode");
		pFunction->BindInput(1, 'vel', "Velocity");
		pFunction->BindInput(2, 'mode', "Mode");
		componentScope.Register(pFunction);
	}
	*/

	{
		auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CShipMechanicsComponent::GetMoveDirection, "{8E34EE28-48D1-5F79-9294-C326249665D7}"_cry_guid, "GetMovementDirection");
		pFunction->SetDescription("Gets the direction the character is moving in");
		pFunction->BindOutput(0, 'dir', "Direction");
		componentScope.Register(pFunction);
	}

	// Signals
	{
		auto pSignal = SCHEMATYC_MAKE_ENV_SIGNAL(CShipMechanicsComponent::SCollisionSignal);
		pSignal->SetDescription("Sent when the entity collided with another object");
		componentScope.Register(pSignal);
	}
}

static void ReflectType(Schematyc::CTypeDesc<CShipMechanicsComponent::SCollisionSignal>& desc)
{
	desc.SetGUID("{BAC0D055-7EE5-46BB-913A-03A0EC764ABE}"_cry_guid);
	desc.SetLabel("On Collision dev");
	desc.AddMember(&CShipMechanicsComponent::SCollisionSignal::otherEntity, 'ent', "OtherEntity", "OtherEntity", "Other Colliding Entity", Schematyc::ExplicitEntityId());
	desc.AddMember(&CShipMechanicsComponent::SCollisionSignal::surfaceType, 'srf', "SurfaceType", "SurfaceType", "Material Surface Type at the collision point", "");

	//desc.AddMember(&CShipMechanicsComponent::m_bNetworked, 'netw', "Networked", "Network Synced", "Syncs the physical entity over the network, and keeps it in sync with the server", false);
	//desc.AddMember(&CShipMechanicsComponent::m_physics, 'phys', "Physics", "Physics", "Physical properties for the character", CShipMechanicsComponent::SPhysics());
	//desc.AddMember(&CShipMechanicsComponent::m_movement, 'move', "Movement", "Movement", "Movement properties for the character", CShipMechanicsComponent::SMovement());
}

CShipMechanicsComponent::~CShipMechanicsComponent()
{
	SEntityPhysicalizeParams physParams;
	physParams.type = PE_NONE;
	m_pEntity->Physicalize(physParams);
}

void CShipMechanicsComponent::Initialize()
{
	// FIXME: default value for CSharedString
	if (GetShipType().empty())
	{
		SetShipType("VolgoDon");
	}
	if (GetShipName().empty())
	{
		SetShipName(GetGUID().ToString());
	}
	const bool isLoadedShipParameters = Common::GetIConfigLoaderPtr()->LoadShipParameters(m_shipParameters, GetShipType());
	CRY_ASSERT(
		isLoadedShipParameters,
		"Can't load ship parameters for type '%s' in file '%s'", GetShipType(), Common::GetIConfigLoaderPtr()->GetShipTypesXmlPath());
	const string shipResourcePath = m_shipParameters.resourcePath;
	const string shipMaterialPath = m_shipParameters.materialPath;
	CRY_ASSERT(!shipResourcePath.empty(), "Can't find resource path for type '%s' in file '%s'", GetShipType(), Common::GetPConfigLoader()->GetShipTypesXmlPath());
	if (shipResourcePath.empty())
	{
		CryFatalError("Can't find resource path for type '%s' in file '%s'", GetShipType(), Common::GetPConfigLoader()->GetShipTypesXmlPath());
		return;
	}

	//m_pShipMesh = m_pEntity->CreateComponent<Cry::DefaultComponents::CStaticMeshComponent>();
	//m_pShipMesh->GetEntity()->EnablePhysics(true);
	//int slot = m_pShipMesh->GetEntity()->LoadGeometry(GetOrMakeEntitySlotId(), "Assets/props/fisher_boat/fishing_boat.cgf");
	//m_pShipMesh->GetEntity()->LoadGeometry(m_pShipMesh->GetOrMakeEntitySlotId(), "Assets/props/fisher_boat/fishing_boat.cgf");
	//int slot = m_pEntity->LoadGeometry(GetOrMakeEntitySlotId(), "Assets/props/fisher_boat/fishing_boat.cgf");
	//int slot = m_pEntity->LoadGeometry(GetOrMakeEntitySlotId(), "Assets/props/big_box/big_box.cgf");

	int slot = -1;
	if (!shipResourcePath.empty())
	{
		slot = m_pEntity->LoadGeometry(GetOrMakeEntitySlotId(), shipResourcePath);
		if (slot != -1 && !shipMaterialPath.empty())
		{
			IMaterial* pMaterial = gEnv->p3DEngine->GetMaterialManager()->LoadMaterial(shipMaterialPath);
			if (pMaterial)
			{
				// m_pEntity->SetMaterial(pMaterial);
				// m_pShipMesh->GetEntity()->SetSlotMaterial(GetOrMakeEntitySlotId(), pMaterial);
			}
		}
	}
	//m_pEntity->GetPhysicalEntity()->AddGeometry()
	//m_buoyancyParameters.resistance = 100.f;
	//m_buoyancyParameters.damping = 50.f;
	//m_buoyancyParameters.density = 50.f;






	//
	//m_pShipMesh->GetEntity()->LoadGeometry(GetOrMakeEntitySlotId(), "Assets/props/volgo_don/volgo_don.cgf"/*"%ENGINE%/EngineAssets/Objects/primitive_sphere.cgf"*/);
	// 


	Physicalize();

	if (m_isNetworked)
	{
		m_pEntity->GetNetEntity()->EnableDelegatableAspect(eEA_Physics, false);
		m_pEntity->GetNetEntity()->BindToNetwork();
		m_pEntity->GetNetEntity()->SetAspectProfile(eEA_Physics, static_cast<int>(m_type));
	}
}

void CShipMechanicsComponent::Physicalize()
{
	CryLog("CShipMechanicsComponent::Physicalize");
	SEntityPhysicalizeParams physParams;
	physParams.type = (int) m_type;

	Enable(m_isEnabledByDefault);

	// Don't physicalize a slot by default
	//physParams.nSlot = std::numeric_limits<int>::max();
	physParams.nSlot = GetOrMakeEntitySlotId();

	//m_pShipMesh->GetEntity()->Physicalize(physParams);
	//m_pShipMesh->GetEntity()->Physicalize(physParams);



	physParams.mass = m_physics.m_mass;

	pe_player_dimensions playerDimensions;

	// Prefer usage of a cylinder
	playerDimensions.bUseCapsule = m_physics.m_bCapsule ? 1 : 0;

	// Specify the size of our capsule, physics treats the input as the half-size, so we multiply our value by 0.5.
	// This ensures that 1 unit = 1m for designers.
	playerDimensions.sizeCollider = Vec3(m_physics.m_radius * 0.5f, 1.f, m_physics.m_height * 0.5f);
	// Capsule height needs to be adjusted to match 1 unit ~= 1m.
	if (playerDimensions.bUseCapsule)
	{
		playerDimensions.sizeCollider.z *= 0.5f;
	}
	playerDimensions.groundContactEps = m_physics.m_groundContactEps;
	// Keep pivot at the player's feet (defined in player geometry) 
	playerDimensions.heightPivot = 0.f;
	// Offset collider upwards
	playerDimensions.heightCollider = m_pTransform != nullptr ? m_pTransform->GetTranslation().z : 0.f;

	physParams.pPlayerDimensions = &playerDimensions;


	pe_player_dynamics playerDynamics;
	playerDynamics.mass = physParams.mass;
	playerDynamics.kAirControl = m_movement.m_airControlRatio;
	playerDynamics.kAirResistance = m_movement.m_airResistance;
	playerDynamics.kInertia = m_movement.m_inertia;
	playerDynamics.kInertiaAccel = m_movement.m_inertiaAcceleration;

	playerDynamics.maxJumpAngle = 12;

	playerDynamics.maxVelGround = m_movement.m_maxGroundVelocity;

	physParams.pPlayerDynamics = &playerDynamics;
	m_pEntity->Physicalize(physParams);
	m_pEntity->UpdateComponentEventMask(this);


	pe_simulation_params simParams;
	simParams.maxTimeStep = m_simulationParameters.maxTimeStep;
	simParams.minEnergy = sqr(m_simulationParameters.sleepSpeed);
	simParams.damping = m_simulationParameters.damping;
	m_pEntity->GetPhysicalEntity()->SetParams(&simParams);
	//m_pShipMesh->GetEntity()->GetPhysicalEntity()->SetParams(&simParams);

	pe_params_buoyancy buoyancyParams;
	buoyancyParams.waterDensity = m_buoyancyParameters.density;
	buoyancyParams.waterResistance = m_buoyancyParameters.resistance;
	buoyancyParams.waterDamping = m_buoyancyParameters.damping;
	m_pEntity->GetPhysicalEntity()->SetParams(&buoyancyParams);
	//m_pShipMesh->GetEntity()->GetPhysicalEntity()->SetParams(&buoyancyParams);

	pe_action_awake aa;
	aa.bAwake = m_isResting ? 0 : 1;
	m_pEntity->GetPhysicalEntity()->Action(&aa);
	//m_pShipMesh->GetEntity()->GetPhysicalEntity()->Action(&aa);
}

void CShipMechanicsComponent::UpdateMovementParameters(float frameTime)
{
	return;
	Vec3 velocity = GetVelocity();
	//CryLog("", velocity);

	const float moveSpeed = 200.5f;
	velocity.y += moveSpeed * frameTime;
	SetVelocity(GetEntity()->GetWorldRotation() * velocity);
}

void CShipMechanicsComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
	case Cry::Entity::EEvent::Update:
		{
			const float frameTime = event.fParam[0];

			IPhysicalEntity* pPhysicalEntity = m_pEntity->GetPhysics();
			if (pPhysicalEntity)
			{
				// Update stats
				pe_status_living livingStatus;
				if (pPhysicalEntity->GetStatus(&livingStatus) != 0)
				{
					m_bOnGround = !livingStatus.bFlying;

					// Store the ground normal in case it is needed
					// Note that users have to check if we're on ground before using, is considered invalid in air.
					m_groundNormal = livingStatus.groundSlope;
				}

				// Get the player's velocity from physics
				pe_status_dynamics playerDynamics;
				if (pPhysicalEntity->GetStatus(&playerDynamics) != 0)
				{
					m_velocity = playerDynamics.v;
				}
			}
			UpdateMovementParameters(frameTime);
		}
	case Cry::Entity::EEvent::PhysicsCollision:
		{
			break;

			// Collision info can be retrieved using the event pointer
			EventPhysCollision* physCollision = reinterpret_cast<EventPhysCollision*>(event.nParam[0]);

			const char* surfaceTypeName = "";
			EntityId otherEntityId = INVALID_ENTITYID;

			ISurfaceTypeManager* pSurfaceTypeManager = gEnv->p3DEngine->GetMaterialManager()->GetSurfaceTypeManager();

			IEntity* pOtherEntity = gEnv->pEntitySystem->GetEntityFromPhysics(physCollision->pEntity[0]);
			ISurfaceType* pSurfaceType = pSurfaceTypeManager->GetSurfaceType(physCollision->idmat[0]);

			if (pOtherEntity == m_pEntity || pOtherEntity == nullptr)
			{
				pSurfaceType = pSurfaceTypeManager->GetSurfaceType(physCollision->idmat[1]);
				pOtherEntity = gEnv->pEntitySystem->GetEntityFromPhysics(physCollision->pEntity[1]);
			}

			if (pSurfaceType != nullptr)
			{
				surfaceTypeName = pSurfaceType->GetName();
			}

			if (pOtherEntity == nullptr)
			{
				return;
			}

			otherEntityId = pOtherEntity->GetId();

			// Send OnCollision signal
			if (Schematyc::IObject* pSchematycObject = m_pEntity->GetSchematycObject())
			{
				pSchematycObject->ProcessSignal(SCollisionSignal(otherEntityId, Schematyc::SurfaceTypeName(surfaceTypeName)), GetGUID());
			}
		}
		break;
	case Cry::Entity::EEvent::EditorPropertyChanged:
		{
			// Start validating inputs
#ifndef RELEASE
		// Slide value will have no effect if larger than the fall angle, since we'll fall instead
#endif
			m_pEntity->UpdateComponentEventMask(this);

			Physicalize();
		}
		break;
	case Cry::Entity::EEvent::GameplayStarted:
		{
			if (m_isEnabledByDefault)
			{
				pe_action_awake pa;
				pa.bAwake = !m_isResting;
				auto physicalEntity = m_pEntity->GetPhysicalEntity();
				CRY_ASSERT(physicalEntity, "Can't get physical entity");
				if (physicalEntity)
				{
					physicalEntity->Action(&pa);
				}
			}
		}
		break;
	default: break;
	}

}

Cry::Entity::EventFlags CShipMechanicsComponent::GetEventMask() const
{
	Cry::Entity::EventFlags bitFlags = ENTITY_EVENT_COMPONENT_PROPERTY_CHANGED | ENTITY_EVENT_START_GAME;

	// Only update when we have a physical entity
	if (m_pEntity->GetPhysicalEntity() != nullptr)
	{
		bitFlags |= ENTITY_EVENT_UPDATE;
	}

	if (m_bSendCollisionSignal)
	{
		bitFlags |= ENTITY_EVENT_COLLISION;
	}

	return bitFlags;
}

bool CShipMechanicsComponent::NetSerialize(TSerialize ser, EEntityAspects aspect, uint8 profile, int flags)
{
	if (aspect == eEA_Physics)
	{
		// Serialize physics state in order to keep the clients in sync
		m_pEntity->PhysicsNetSerializeTyped(ser, static_cast<int>(m_type), flags);
	}

	return true;
}

} // namespace Navigation

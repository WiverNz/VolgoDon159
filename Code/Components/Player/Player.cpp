#include "StdAfx.h"
#include "Player.h"

#include <CryRenderer/IRenderAuxGeom.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CryCore/StaticInstanceList.h>
#include <CryNetwork/Rmi.h>

#ifndef __cpp_lib_format
#include <ThirdParty/fmt/format.h>
using fmt::format;
using namespace fmt::literals;
#else
#include <format>
using std::format;
using namespace std::literals;
#endif

#include "SpawnPoint.h"
#include "GamePlugin.h"
#include "GameModules/GameState.h"
#include "GameModules/GameManager.h"
#include "UserInterface/MainMenu.h"
#include "UserInterface/ShipInterface.h"

namespace
{
static void RegisterPlayerComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Navigation::CPlayerComponent));
	}
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterPlayerComponent);
}

namespace Navigation
{

void CPlayerComponent::Initialize()
{
	const GameModules::CGameSettings &gameSettings = GameModules::GetIGameManagerPtr()->GetGameSettings();
	m_pShipMechanics = m_pEntity->GetOrCreateComponent<CShipComponent>();
	m_pShipMechanics->SetShipName(gameSettings.GetShipName());
	m_pShipMechanics->SetControllingBy(EShipControl::Player);

	// By default, the server delegates authority to a single Player-entity on the client.
	// However, we want the player physics to be simulated server-side, so we need
	// to prevent physics aspect delegation.
	// This should be done on both sides.
	m_pEntity->GetNetEntity()->EnableDelegatableAspect(eEA_Physics, false);

	// Mark the entity to be replicated over the network
	m_pEntity->GetNetEntity()->BindToNetwork();

	// Indicate the physics type, this will be synchronized across all clients
	// m_pEntity->GetNetEntity()->SetAspectProfile(eEA_Physics, physParams.type);

	// Register the RemoteReviveOnClient function as a Remote Method Invocation (RMI) that can be executed on clients from the server
	SRmi<RMI_WRAP(&CPlayerComponent::RemoteReviveOnClient)>::Register(this, eRAT_NoAttach, false, eNRT_ReliableOrdered);
	// Register the RemoteRequestJumpOnServer as an RMI that can be executed on the server from clients
	// SRmi<RMI_WRAP(&CPlayerComponent::RemoteRequestJumpOnServer)>::Register(this, eRAT_NoAttach, true, eNRT_ReliableOrdered);
}

void CPlayerComponent::InitializeLocalPlayer()
{
	// Create the camera component, will automatically update the viewport every frame
	m_pCameraComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CCameraComponent>();
	m_pCameraComponent->SetTransformMatrix(IDENTITY);

	// Create the audio listener component.
	m_pAudioListenerComponent = m_pEntity->GetOrCreateComponent<Cry::Audio::DefaultComponents::CListenerComponent>();

	// Get the input component, wraps access to action mapping so we can easily get callbacks when inputs are triggered
	m_pInputComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CInputComponent>();

	// Register an action, and the callback that will be sent when it's triggered
	m_pInputComponent->RegisterAction("player", "moveleft", [this](int activationMode, float value) { HandleInputFlagChange(EInputFlag::MoveLeft, (EActionActivationMode) activationMode); });
	// Bind the 'A' key the "moveleft" action
	m_pInputComponent->BindAction("player", "moveleft", eAID_KeyboardMouse, EKeyId::eKI_A);

	m_pInputComponent->RegisterAction("player", "moveright", [this](int activationMode, float value) { HandleInputFlagChange(EInputFlag::MoveRight, (EActionActivationMode) activationMode); });
	m_pInputComponent->BindAction("player", "moveright", eAID_KeyboardMouse, EKeyId::eKI_D);

	m_pInputComponent->RegisterAction("player", "moveforward", [this](int activationMode, float value) { HandleInputFlagChange(EInputFlag::MoveForward, (EActionActivationMode) activationMode); });
	m_pInputComponent->BindAction("player", "moveforward", eAID_KeyboardMouse, EKeyId::eKI_W);

	m_pInputComponent->RegisterAction("player", "moveback", [this](int activationMode, float value) { HandleInputFlagChange(EInputFlag::MoveBack, (EActionActivationMode) activationMode); });
	m_pInputComponent->BindAction("player", "moveback", eAID_KeyboardMouse, EKeyId::eKI_S);

	m_pInputComponent->RegisterAction("player", "mouse_rotateyaw", [this](int activationMode, float value) { m_mouseDeltaRotation.x -= value; });
	m_pInputComponent->BindAction("player", "mouse_rotateyaw", eAID_KeyboardMouse, EKeyId::eKI_MouseX);

	m_pInputComponent->RegisterAction("player", "mouse_rotatepitch", [this](int activationMode, float value) { m_mouseDeltaRotation.y -= value; });
	m_pInputComponent->BindAction("player", "mouse_rotatepitch", eAID_KeyboardMouse, EKeyId::eKI_MouseY);

	m_pInputComponent->RegisterAction("player", "rotateleft", [this](int activationMode, float value) { HandleInputFlagChange(EInputFlag::RotateLeft, (EActionActivationMode) activationMode); });
	m_pInputComponent->BindAction("player", "rotateleft", eAID_KeyboardMouse, EKeyId::eKI_Q);

	m_pInputComponent->RegisterAction("player", "exit", [this](int activationMode, float value) { HandleInputFlagChange(EInputFlag::ExitGame, (EActionActivationMode) activationMode); });
	m_pInputComponent->BindAction("player", "exit", eAID_KeyboardMouse, EKeyId::eKI_F4);

	m_pInputComponent->RegisterAction("player", "pause", [this](int activationMode, float value) { HandleInputFlagChange(EInputFlag::PauseGame, (EActionActivationMode) activationMode); });
	m_pInputComponent->BindAction("player", "pause", eAID_KeyboardMouse, EKeyId::eKI_Escape);
}

Cry::Entity::EventFlags CPlayerComponent::GetEventMask() const
{
	Cry::Entity::EventFlags eventFlags;

	eventFlags |= Cry::Entity::EEvent::BecomeLocalPlayer |
		Cry::Entity::EEvent::Update |
		Cry::Entity::EEvent::Reset;

	return eventFlags;
}

void CPlayerComponent::DrawDebugInfo(float frameTime)
{
	float color[4] = { 1, 0, 0, 1 };
	const auto& steer = m_pShipMechanics->GetSteeringAngle().ToDegrees();
	const auto& leftEngineRpm = m_pShipMechanics->GetLeftEngineRPM();
	const std::string degreeSymbol = "\xB0";
	const std::string infoText = format(R"INFO_TEXT(
	steer: {steer:.1f}{degree_sym:s}
	leftEngineRpm: {leftEngineRpm}
		)INFO_TEXT",
		"degree_sym"_a = degreeSymbol,
		"steer"_a = steer,
		"leftEngineRpm"_a = leftEngineRpm
	);
	gEnv->pAuxGeomRenderer->Draw2dLabel(0, 0, 2, color, false, infoText.c_str());
}

void CPlayerComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
	case EEntityEvent::LevelLoaded:
	case EEntityEvent::LevelStarted:
	case EEntityEvent::GameplayStarted:
		{
			UserInterface::GetMainManuPtr()->SetVisible(true);
		}
		break;
	case Cry::Entity::EEvent::BecomeLocalPlayer:
		{
			InitializeLocalPlayer();

		}
		break;
	case Cry::Entity::EEvent::Update:
		{
			if (GameModules::GetGameStateContextPtr()->GetEState() != GameModules::EGameState::GameRunning)
			{
				return;
			}
			// Don't update the player if we haven't spawned yet
			if (!m_isAlive)
			{
				return;
			}

			const float frameTime = event.fParam[0];

			// Start by updating the movement request we want to send to the character controller
			// This results in the physical representation of the character moving
			UpdateMovementRequest(frameTime);

			// Process mouse input to update look orientation.
			UpdateLookDirectionRequest(frameTime);

			// Update the animation state of the character
			UpdateAnimation(frameTime);

			if (IsLocalClient())
			{
				// Update the camera component offset
				UpdateCamera(frameTime);
			}

			DrawDebugInfo(frameTime);
		}
		break;
	case Cry::Entity::EEvent::Reset:
		{
			// Disable player when leaving game mode.
			m_isAlive = event.nParam[0] != 0;
		}
		break;
	}

	if (m_exitRequested)
	{
		// IConsole *pConsole = gEnv->pSystem->GetIConsole();
		// pConsole->Exit("Goodbye!");
		// gEnv->pSystem->GetI3DEngine()->ShutDown();
		gEnv->pSystem->Quit();

	}
}

bool CPlayerComponent::NetSerialize(TSerialize ser, EEntityAspects aspect, uint8 profile, int flags)
{
	if (aspect == InputAspect)
	{
		if (GameModules::GetGameStateContextPtr()->GetEState() != GameModules::EGameState::GameRunning)
		{
			return true;
		}

		ser.BeginGroup("PlayerInput");

		const CEnumFlags<EInputFlag> prevInputFlags = m_inputFlags;

		ser.Value("m_inputFlags", m_inputFlags.UnderlyingValue(), 'ui8');

		if (ser.IsReading())
		{
			const CEnumFlags<EInputFlag> changedKeys = prevInputFlags ^ m_inputFlags;

			const CEnumFlags<EInputFlag> pressedKeys = changedKeys & prevInputFlags;
			if (!pressedKeys.IsEmpty())
			{
				HandleInputFlagChange(pressedKeys, eAAM_OnPress);
			}

			const CEnumFlags<EInputFlag> releasedKeys = changedKeys & prevInputFlags;
			if (!releasedKeys.IsEmpty())
			{
				HandleInputFlagChange(pressedKeys, eAAM_OnRelease);
			}
		}

		// Serialize the player look orientation
		ser.Value("m_lookOrientation", m_lookOrientation, 'ori3');

		ser.EndGroup();
	}

	return true;
}

void CPlayerComponent::UpdateMovementRequest(float frameTime)
{
	if (!m_pShipMechanics)
	{
		return;
	}

	Vec3 velocity = ZERO;

	using CuFloat = decltype(m_pShipMechanics->m_shipStatus.steer);

	const CuFloat moveSpeed = 200.5f;
	const CuFloat moveChange = 0.5f;
	const CuFloat frameSTime = static_cast<CuFloat>(frameTime);

	if (m_inputFlags & EInputFlag::MoveLeft)
	{
		velocity.y += moveSpeed * frameTime;
		velocity.x -= moveSpeed * frameTime;
		m_pShipMechanics->m_shipStatus.steer = m_pShipMechanics->m_shipStatus.steer - frameSTime * moveChange;
		m_pEntity->SetRotation(Quat::CreateRotationZ(m_pShipMechanics->m_shipStatus.steer));
	}
	if (m_inputFlags & EInputFlag::MoveRight)
	{
		velocity.y += moveSpeed * frameTime;
		velocity.x += moveSpeed * frameTime;
		m_pShipMechanics->m_shipStatus.steer = m_pShipMechanics->m_shipStatus.steer + frameSTime * moveChange;
		m_pEntity->SetRotation(Quat::CreateRotationZ(m_pShipMechanics->m_shipStatus.steer));

	}
	if (m_inputFlags & EInputFlag::MoveForward)
	{
		velocity.y += moveSpeed * frameTime;
	}
	if (m_inputFlags & EInputFlag::MoveBack)
	{
		velocity.y -= moveSpeed * frameTime;
	}
	if (m_inputFlags & EInputFlag::RotateLeft)
	{
		static float rotVal = 0.01f;
		rotVal += 0.01f;
		m_pEntity->SetRotation(Quat::CreateRotationZ(rotVal));
		Physicalize();
	}
	if (m_inputFlags & EInputFlag::PauseGame)
	{
		GameModules::GetGameStateContextPtr()->Pause();
	}
	if (m_inputFlags & EInputFlag::ExitGame)
	{
		//m_isAlive = false;
		//m_pCameraComponent->GetCamera().SetPosition(Vec3(10, 10, 10));
		//m_pCameraComponent->Activate();

		m_exitRequested = true;
	}

	if (!velocity.IsZero())
	{
		m_pShipMechanics->SetVelocity(GetEntity()->GetWorldRotation() * velocity);
		//m_pShipMechanics->ApplyAngularImpulse(GetEntity()->GetWorldRotation() * velocity);
		//m_pShipMechanics->SetAngularVelocity(GetEntity()->GetWorldRotation() * velocity);
	}

	UserInterface::GetShipInterfacePtr()->SetValuesJson(m_pShipMechanics->m_shipStatus);
}

void CPlayerComponent::UpdateLookDirectionRequest(float frameTime)
{
	const float rotationSpeed = 0.002f;
	const float rotationLimitsMinPitch = -0.84f;
	const float rotationLimitsMaxPitch = 1.5f;

	// Apply smoothing filter to the mouse input
	m_mouseDeltaRotation = m_mouseDeltaSmoothingFilter.Push(m_mouseDeltaRotation).Get();

	if (m_mouseDeltaRotation.IsValid())
	{
		// Update angular velocity metrics
		m_horizontalAngularVelocity = (m_mouseDeltaRotation.x * rotationSpeed) / frameTime;
		m_averagedHorizontalAngularVelocity.Push(m_horizontalAngularVelocity);

		// Start with updating look orientation from the latest input
		Ang3 ypr = CCamera::CreateAnglesYPR(Matrix33(m_lookOrientation));

		// Yaw
		ypr.x += m_mouseDeltaRotation.x * rotationSpeed;

		// Pitch
		// TODO: Perform soft clamp here instead of hard wall, should reduce rot speed in this direction when close to limit.
		ypr.y = CLAMP(ypr.y + m_mouseDeltaRotation.y * rotationSpeed, rotationLimitsMinPitch, rotationLimitsMaxPitch);

		// Roll (skip)
		ypr.z = 0;

		m_lookOrientation = Quat(CCamera::CreateOrientationYPR(ypr));
	}

	// Reset the mouse delta accumulator every frame
	m_mouseDeltaRotation = ZERO;
}

void CPlayerComponent::UpdateAnimation(float frameTime)
{

}

void CPlayerComponent::UpdateCamera(float frameTime)
{
	Ang3 ypr = CCamera::CreateAnglesYPR(Matrix33(m_lookOrientation));

	// Ignore z-axis rotation, that's set by CPlayerAnimations
	ypr.x = 0;

	// Start with changing view rotation to the requested mouse look orientation
	Matrix34 localTransform = IDENTITY;
	localTransform.SetRotation33(CCamera::CreateOrientationYPR(ypr));

	// Offset the player along the forward axis (normally back)
	// Also offset upwards

	const float viewOffsetForward = -80.f;
	const float viewOffsetUp = 15.f;

	localTransform.SetTranslation(Vec3(0, viewOffsetForward, viewOffsetUp));

	m_pCameraComponent->SetTransformMatrix(localTransform);
	m_pAudioListenerComponent->SetOffset(localTransform.GetTranslation());
}

void CPlayerComponent::OnReadyForGameplayOnServer()
{
	CRY_ASSERT(gEnv->bServer, "This function should only be called on the server!");

	const Matrix34 newTransform = CSpawnPointComponent::GetFirstSpawnPointTransform();

	Revive(newTransform);


	// Invoke the RemoteReviveOnClient function on all remote clients, to ensure that Revive is called across the network
	SRmi<RMI_WRAP(&CPlayerComponent::RemoteReviveOnClient)>::InvokeOnOtherClients(this, RemoteReviveParams{ newTransform.GetTranslation(), Quat(newTransform) });

	// Go through all other players, and send the RemoteReviveOnClient on their instances to the new player that is ready for gameplay
	const int channelId = m_pEntity->GetNetEntity()->GetChannelId();
	CGamePlugin::GetInstance()->IterateOverPlayers([this, channelId](CPlayerComponent& player)
	{
		// Don't send the event for the player itself (handled in the RemoteReviveOnClient event above sent to all clients)
		if (player.GetEntityId() == GetEntityId())
		{
			return;
		}

		// Only send the Revive event to players that have already respawned on the server
		if (!player.m_isAlive)
		{
			return;
		}

		// Revive this player on the new player's machine, on the location the existing player was currently at
		const QuatT currentOrientation = QuatT(player.GetEntity()->GetWorldTM());
		SRmi<RMI_WRAP(&CPlayerComponent::RemoteReviveOnClient)>::InvokeOnClient(&player, RemoteReviveParams{ currentOrientation.t, currentOrientation.q }, channelId);
	});
}

bool CPlayerComponent::RemoteReviveOnClient(RemoteReviveParams&& params, INetChannel* pNetChannel)
{
	// Call the Revive function on this client
	Revive(Matrix34::Create(Vec3(1.f), params.rotation, params.position));

	return true;
}

void CPlayerComponent::Revive(const Matrix34& transform)
{
	m_isAlive = true;

	// Set the entity transformation, except if we are in the editor
	// In the editor case we always prefer to spawn where the viewport is
	if (!gEnv->IsEditor())
	{
		m_pEntity->SetWorldTM(transform);
	}

	// Apply character to the entity
	// m_pAnimationComponent->ResetCharacter();
	m_pShipMechanics->Physicalize();

	// Reset input now that the player respawned
	m_inputFlags.Clear();
	NetMarkAspectsDirty(InputAspect);
	ResetMouseMovement();

	m_lookOrientation = IDENTITY;
	m_horizontalAngularVelocity = 0.0f;
	m_averagedHorizontalAngularVelocity.Reset();
}

void CPlayerComponent::HandleInputFlagChange(const CEnumFlags<EInputFlag> flags, const CEnumFlags<EActionActivationMode> activationMode, const EInputFlagType type)
{
	switch (type)
	{
	case EInputFlagType::Hold:
		{
			if (activationMode == eAAM_OnRelease)
			{
				m_inputFlags &= ~flags;
			}
			else
			{
				m_inputFlags |= flags;
			}
		}
		break;
	case EInputFlagType::Toggle:
		{
			if (activationMode == eAAM_OnRelease)
			{
				// Toggle the bit(s)
				m_inputFlags ^= flags;
			}
		}
		break;
	}

	if (IsLocalClient())
	{
		NetMarkAspectsDirty(InputAspect);
	}
}

} // namespace Navigation

#include "StdAfx.h"
#include "AutoPilotShip.h"

#include <CrySchematyc/Reflection/TypeDesc.h>
#include <CrySchematyc/Utils/EnumFlags.h>
#include <CrySchematyc/Env/IEnvRegistry.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CrySchematyc/Env/Elements/EnvFunction.h>
#include <CrySchematyc/Env/Elements/EnvSignal.h>
#include <CrySchematyc/ResourceTypes.h>
#include <CrySchematyc/MathTypes.h>
#include <CrySchematyc/Utils/SharedString.h>
#include <CryCore/StaticInstanceList.h>

#include "GameModules/GameManager.h"
#include "AutoPilotShipSpawnPoint.h"
#include "GamePlugin.h"

static void RegisterAutoPilotShipComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Navigation::CAutoPilotShipComponent));
	}
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterAutoPilotShipComponent);

namespace Navigation
{

void CAutoPilotShipComponent::Initialize()
{
	CShipMechanicsComponent::Initialize();
	const int controllingId = GameModules::GetIGameManagerPtr()->GenNextControllingId();
	SetShipName(GetShipType() + " " + ToString(controllingId) + " (Autopilot)");
	SetControllingBy(EShipControl::AutoPilot, controllingId);
}

} // namespace Navigation

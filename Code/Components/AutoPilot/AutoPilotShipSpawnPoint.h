// Copyright 2001-2019 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

#include <CryEntitySystem/IEntitySystem.h>
#include <DefaultComponents/Input/InputComponent.h>

/**
 * @namespace Navigation
 * @brief The ship navigation namespace.
 */
namespace Navigation
{
////////////////////////////////////////////////////////
// Spawn point
////////////////////////////////////////////////////////
class CAutoPilotShipSpawnPointComponent final : public IEntityComponent
{
public:
	CAutoPilotShipSpawnPointComponent() = default;
	virtual ~CAutoPilotShipSpawnPointComponent() = default;

	// Reflect type to set a unique identifier for this component
	// and provide additional information to expose it in the sandbox
	static void ReflectType(Schematyc::CTypeDesc<CAutoPilotShipSpawnPointComponent>& desc)
	{
		desc.SetGUID("{FC6C9749-7C0C-437A-ABF4-E92BDC4CB0C1}"_cry_guid);
		desc.SetEditorCategory("Game");
		desc.SetLabel("AutoPilotShipSpawnPoint");
		desc.SetDescription("This spawn point can be used to spawn auto-pilot entities");
		desc.SetComponentFlags({ IEntityComponent::EFlags::Transform, IEntityComponent::EFlags::Socket, IEntityComponent::EFlags::Attach });
	}

	static Matrix34 GetFirstSpawnPointTransform()
	{
		// Spawn at first default spawner
		IEntityItPtr pEntityIterator = gEnv->pEntitySystem->GetEntityIterator();
		pEntityIterator->MoveFirst();

		while (!pEntityIterator->IsEnd())
		{
			IEntity* pEntity = pEntityIterator->Next();

			if (CAutoPilotShipSpawnPointComponent* pSpawner = pEntity->GetComponent<CAutoPilotShipSpawnPointComponent>())
			{
				return pSpawner->GetWorldTransformMatrix();
			}
		}

		return IDENTITY;
	}
};

} // namespace Navigation

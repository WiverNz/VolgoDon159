#pragma once

#include <CryEntitySystem/IEntitySystem.h>
#include <DefaultComponents/Input/InputComponent.h>

#include "Components/ShipMechanics/ShipMechanicsComponent.h"

/**
 * @namespace Navigation
 * @brief The ship navigation namespace.
 */
namespace Navigation
{
////////////////////////////////////////////////////////
// Represents a player participating in gameplay
////////////////////////////////////////////////////////
class CAutoPilotShipComponent final : public CShipMechanicsComponent
{
public:
	CAutoPilotShipComponent() = default;
	virtual ~CAutoPilotShipComponent() {}

	// Reflect type to set a unique identifier for this component
	static void ReflectType(Schematyc::CTypeDesc<CAutoPilotShipComponent>& desc)
	{
		desc.SetEditorCategory("Navigation");
		desc.SetLabel("Autopilot Ship");
		desc.SetGUID("{ECD93667-7A5B-4DDF-9E75-25F065047FDB}"_cry_guid);
	}

	virtual void Initialize() override;

	virtual void ProcessEvent(const SEntityEvent& event) override
	{

	}
protected:
	virtual void Physicalize()
	{

	}
};

} // namespace Navigation

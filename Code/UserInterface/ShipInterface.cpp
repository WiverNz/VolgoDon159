#include "StdAfx.h"

#include "ShipInterface.h"

#include "Common/NavigationTypes.h"

CRYREGISTER_SINGLETON_CLASS(UserInterface::CShipInterface)

// !iterToDelete.empty()
// The given ParameterDescription is not valid!
// FlashUIElement.cpp(2199)
// ReadEventNodes(xmlNode->findChild("functions"), pElement->m_functions, "funcname", pElement->m_pRoot); // m_functions size = 0
// -> Something wrong with xml file - functions can't be found.

// (Detail::Validate<InputEncoding, InputStringType>(source)) && "Convert was used with non-safe input"
// -> For wrong symbols. E.g. Ship.RudderText.text = String(rudderValue) + "°";

namespace UserInterface
{
const CryStringLocal CShipInterface::s_uiElementName = "ShipInterface";

CShipInterface::~CShipInterface()
{

}

bool CShipInterface::Initialize()
{
	if (m_isShipInterfaceInitialized)
	{
		CRY_ASSERT(false, "The ship interface has already been initialized");
		return m_isShipInterfaceInitialized;
	}

	if (GameModules::GetIGameManagerPtr()->IsSkipFlash())
	{
		m_isSkipShipInterface = true;
		return true;
	}

	IFlashUIPtr pFlashUI = GetIFlashUIPtr();
	m_isShipInterfaceInitialized = false;
	CRY_ASSERT(pFlashUI, "FlashUI extension is not supported");
	if (pFlashUI != nullptr)
	{
		m_pShipInterfaceElement = pFlashUI->GetUIElement(s_uiElementName);
		CRY_ASSERT(m_pShipInterfaceElement, "Can't find '%s' UIElement", s_uiElementName);
		if (m_pShipInterfaceElement)
		{
			m_isShipInterfaceInitialized = m_pShipInterfaceElement->Init();
		}
	}

	if (!m_isShipInterfaceInitialized)
	{
		// If CryEngine was compiled from source code, then you need Scaleform in this folder Cryengine/Code/SDKs/Scaleform
		CryLogAlways("Continue without a ship interface");
		m_isSkipShipInterface = true;
		GameModules::GetIGameManagerPtr()->SetSkipFlash(true);
	}
	else
	{
		m_pShipInterfaceEventListener.reset(new CShipInterfaceEventListener(this));
		m_pShipInterfaceElement->AddEventListener(m_pShipInterfaceEventListener.get(), "ShipInterfaceListener");
		m_pShipInterfaceElement->Init();
		GameModules::GetIGameManagerPtr()->RegisterListener(this);
		SetExtendDebug(GameModules::GetIGameManagerPtr()->IsExtendDebug());
	}

	return m_isShipInterfaceInitialized;
}

void CShipInterface::MainUpdate(float frameTime)
{

}

bool CShipInterface::IsSkipShipInterface() const
{
	return m_isSkipShipInterface;
}

void CShipInterface::SetVisible(const bool isVisible)
{
	CryLog("Set menu visible %d", isVisible);
	if (IsSkipShipInterface())
	{
		return;
	}

	m_isVisible = isVisible;
	CRY_ASSERT(m_pShipInterfaceElement, "Ship interface element was not initialized. Please call the Initialize method before.");
	if (!m_pShipInterfaceElement)
	{
		return;
	}

	m_pShipInterfaceElement->SetVisible(m_isVisible);
}

bool CShipInterface::IsVisible() const
{
	return m_isVisible;
}

bool CShipInterface::CallFlashFunction(const string& flashFunction, const float valueToSet) const
{

	if (!m_isShipInterfaceInitialized || !m_pShipInterfaceElement)
	{
		CRY_ASSERT(IsSkipShipInterface(), "Can't set '" + flashFunction + "' as interface was not initialized");
		return false;
	}
	SUIArguments args;
	args.AddArgument(float(valueToSet), eUIDT_Float);
	TUIData res;
	m_pShipInterfaceElement->CallFunction(flashFunction, args, &res);

	bool result = false;
	res.GetValueWithConversion(result);
	CRY_ASSERT(result, "Can't set '" + flashFunction + "' in the ship interface flash item");

	return result;
}

bool CShipInterface::SetCoordinates(const double& latCoord, const double& longCoord) const
{
	if (!m_isShipInterfaceInitialized || !m_pShipInterfaceElement)
	{
		CRY_ASSERT(IsSkipShipInterface(), "Can't set 'SetCoordinates' as interface was not initialized");
		return false;
	}
	SUIArguments args;
	args.AddArgument(float(latCoord), eUIDT_Float);
	args.AddArgument(float(longCoord), eUIDT_Float);
	TUIData res;
	m_pShipInterfaceElement->CallFunction("SetCoordinates", args, &res);

	bool result = false;
	res.GetValueWithConversion(result);
	CRY_ASSERT(result, "Can't set 'SetCoordinates' in the ship interface flash item");

	return result;
}

bool CShipInterface::SetRudderValue(const float rudderValue) const
{
	const bool result = CallFlashFunction("SetRudderValue", rudderValue);

	return result;
}

bool CShipInterface::SetSpeedValue(const float speedValue) const
{
	const bool result = CallFlashFunction("SetSpeedValue", speedValue);

	return result;
}

bool CShipInterface::SetRateOfTurn(const float rateOfTurn) const
{
	const bool result = CallFlashFunction("SetRateOfTurn", rateOfTurn);

	return result;
}

bool CShipInterface::SetCourseDegree(const float courseDegree) const
{
	const bool result = CallFlashFunction("SetCourseDegree", courseDegree);

	return result;
}

bool CShipInterface::SetAccelerationValue(const float accelerationValue) const
{
	const bool result = CallFlashFunction("SetAccelerationValue", accelerationValue);

	return result;
}


bool CShipInterface::SetEnginesMaxRpm(const float maxRpm) const
{
	const bool result = CallFlashFunction("SetEnginesMaxRpm", maxRpm);

	return result;
}

bool CShipInterface::SetEnginesCurrRpm(const float currRpm) const
{
	const bool result = CallFlashFunction("SetEnginesCurrRpm", currRpm);

	return result;
}

bool CShipInterface::SetDepthValue(const float currDepth) const
{
	const bool result = CallFlashFunction("SetDepthValue", currDepth);

	return result;
}

bool CShipInterface::SetValuesJson(const Navigation::SStatusShip& statusShip) const
{
	if (!m_isShipInterfaceInitialized || !m_pShipInterfaceElement)
	{
		CRY_ASSERT(IsSkipShipInterface(), "Can't set 'SetValuesJson' as interface was not initialized");
		return false;
	}
	const string jsonValuesString = statusShip.GetJsonString();

	SUIArguments args;
	args.AddArgument(jsonValuesString, eUIDT_String);
	TUIData res;
	m_pShipInterfaceElement->CallFunction("SetValuesJson", args, &res);

	bool result = false;
	res.GetValueWithConversion(result);
	CRY_ASSERT(result, "Can't set 'SetValuesJson' in the ship interface flash item");

	return result;
}

bool CShipInterface::SetExtendDebug(bool isExtendDebug) const
{
	if (!m_isShipInterfaceInitialized || !m_pShipInterfaceElement)
	{
		CRY_ASSERT(IsSkipShipInterface(), "Can't set 'SetExtendDebug' as interface was not initialized");
		return false;
	}

	SUIArguments args;
	args.AddArgument(isExtendDebug);
	TUIData res;
	m_pShipInterfaceElement->CallFunction("SetExtendDebug", args, &res);

	bool result = false;
	res.GetValueWithConversion(result);
	CRY_ASSERT(result, "Can't set extend debug flag in the ship interface flash item");

	return result;
}

void CShipInterface::OnGameManagerEvent(GameModules::EGameManagerEvent event)
{
	CryLog("CShipInterface::OnGameManagerEvent");
	switch (event)
	{
	case GameModules::EGameManagerEvent::GM_EVENT_EXTEND_DEBUG_UPDATE:
		{
			SetExtendDebug(GameModules::GetIGameManagerPtr()->IsExtendDebug());
		}
		break;
	default:
		break;
	}
}

CShipInterfaceEventListener::CShipInterfaceEventListener(CShipInterface* pShipInterface)
{

}

CShipInterfaceEventListener::~CShipInterfaceEventListener()
{

}

void CShipInterfaceEventListener::OnUIEvent(IUIElement* pSender, const SUIEventDesc& event, const SUIArguments& args)
{

}

void CShipInterfaceEventListener::OnUIEventEx(IUIElement* pSender, const char* fscommand, const SUIArguments& args, void* pUserData)
{
	CryLog("fscommand: %s", fscommand);
}

}
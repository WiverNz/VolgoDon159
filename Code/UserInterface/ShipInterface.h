#pragma once
#include <CrySystem/Scaleform/IFlashUI.h>
//#include <CryEntitySystem/IEntityBasicTypes.h>
//#include <CrySystem/IEngineModule.h>
#include <GameModules/GameManager.h>

// Forward declaration of Navigation types
namespace Navigation
{
struct SStatusShip;
}

//! \namespace UserInterface
//! \brief The user interface namespace. Only Common and GameModules dependencies are possible here.
namespace UserInterface
{
// Forward declaration of local classes
class CShipInterfaceEventListener;

//! \class IShipInterface
//! \brief Interface for {CShipInterface}
class IShipInterface : public ICryUnknown, public GameModules::IGameManagerListener
{
	CRYINTERFACE_DECLARE_GUID(IShipInterface, "C4D2AF26-657D-4000-9C61-F8A6F183210C"_cry_guid);
public:
	//! \brief Initialization of the ship interface
	//! \return {bool} true if ship interface initialized successfully
	virtual bool Initialize() = 0;

	//! \brief Additional update for each frame
	//! \param frameTime {float} frame time
	//! Called after ISystem has been updated by the {CGamePlugin} class
	virtual void MainUpdate(float frameTime) = 0;

	//! \brief Check if the ship interface needs to be skipped (disable, for example, if there is no Scaleform support)
	//! \return {bool} true if the ship interface should be skipped
	virtual bool IsSkipShipInterface() const = 0;

	//! \brief Sets the visibility of the ship interface
	//! \param isVisible {bool} true for visible
	virtual void SetVisible(const bool isVisible) = 0;

	//! \brief Gets the visibility of the ship interface
	//! \return {bool} true if visible
	virtual bool IsVisible() const = 0;

	//! \brief Calls a flash function passed as a parameter
	//! \param flashFunction {string} a flash function to call
	//! \param valueToSet {float} an argument to the function
	//! \return {bool} true if set successfully
	virtual bool CallFlashFunction(const string &flashFunction, const float valueToSet) const = 0;

	//! \brief Set coordinates in the ship interface
	//! \param latCoord {double} latitude coordinate
	//! \param longCoord {double} longitude coordinate
	//! \return {bool} true if set successfully
	virtual bool SetCoordinates(const double& latCoord, const double& longCoord) const = 0;

	//! \brief Setting the rudder value in the ship interface
	//! \param rudderValue {float} rudder value
	//! \return {bool} true if set successfully
	virtual bool SetRudderValue(const float rudderValue) const = 0;

	//! \brief Setting the speed value in the ship interface
	//! \param rudderValue {float} rudder value
	//! \return {bool} true if set successfully
	virtual bool SetSpeedValue(const float speedValue) const = 0;

	//! \brief Setting the rate of turn in the ship interface
	//! \param rateOfTurn {float} rate of turn
	//! \return {bool} true if set successfully
	virtual bool SetRateOfTurn(const float rateOfTurn) const = 0;

	//! \brief Setting course degree in the ship interface
	//! \param courseDegree {float} course degree
	//! \return {bool} true if set successfully
	virtual bool SetCourseDegree(const float courseDegree) const = 0;

	//! \brief Setting acceleration value in the ship interface
	//! \param accelerationValue {float} acceleration value
	//! \return {bool} true if set successfully
	virtual bool SetAccelerationValue(const float accelerationValue) const = 0;

	//! \brief Setting engines max RPM in the ship interface
	//! \param maxRpm {float} max RPM
	//! \return {bool} true if set successfully
	virtual bool SetEnginesMaxRpm(const float maxRpm) const = 0;

	//! \brief Setting engines current RPM in the ship interface
	//! \param currRpm {float} current RPM
	//! \return {bool} true if set successfully
	virtual bool SetEnginesCurrRpm(const float currRpm) const = 0;

	//! \brief Setting depth value in the ship interface
	//! \param currDepth {float} current depth
	//! \return {bool} true if set successfully
	virtual bool SetDepthValue(const float currDepth) const = 0;

	//! \brief Set values from the {SStatusShip} to the ship interface
	//! \param statusShip {SStatusShip} a structure with parameters to set
	//! \return {bool} true if set successfully
	virtual bool SetValuesJson(const Navigation::SStatusShip& statusShip) const = 0;

	//! \brief Establishes an extend debugging of the flash main menu
	//! \param isExtendDebug {bool} extended debug flag
	//! \return {bool} true if set successfully
	virtual bool SetExtendDebug(bool isExtendDebug) const = 0;

protected:
	// Inherited via IGameManagerListener
	virtual void OnGameManagerEvent(GameModules::EGameManagerEvent event) = 0;
};

class CShipInterface : public IShipInterface
{
	CRYINTERFACE_SIMPLE(IShipInterface)
	CRYGENERATE_SINGLETONCLASS_GUID(CShipInterface, "CShipInterface", "63398EE5-5322-47CF-ACAE-71666E1B2381"_cry_guid)
	CShipInterface() = default;
	virtual ~CShipInterface();

public:
	bool Initialize() override;
	void MainUpdate(float frameTime) override;
	inline bool IsSkipShipInterface() const override;
	void SetVisible(const bool isVisible) override;
	inline bool IsVisible() const override;
	bool CallFlashFunction(const string& flashFunction, const float valueToSet) const override;
	bool SetCoordinates(const double& latCoord, const double& longCoord) const override;
	bool SetRudderValue(const float rudderValue) const override;
	bool SetSpeedValue(const float speedValue) const override;
	bool SetRateOfTurn(const float rateOfTurn) const override;
	bool SetCourseDegree(const float courseDegree) const override;
	bool SetAccelerationValue(const float accelerationValue) const override;
	bool SetEnginesMaxRpm(const float maxRpm) const override;
	bool SetEnginesCurrRpm(const float currRpm) const override;
	bool SetDepthValue(const float currDepth) const override;
	bool SetValuesJson(const Navigation::SStatusShip& statusShip) const override;
	bool SetExtendDebug(bool isExtendDebug) const override;

protected:
	void OnGameManagerEvent(GameModules::EGameManagerEvent event) override;

protected:
	static const CryStringLocal s_uiElementName;												//!< an ui element name (in the xml)
	IUIElement* m_pShipInterfaceElement = nullptr;												//!< a pointer to the main element of the ship interface (removal from outside)
	bool m_isSkipShipInterface = false;															//!< ship interface skip flag
	bool m_isVisible = false;																	//!< ship interface visibility flag
	bool m_isShipInterfaceInitialized = false;													//!< ship interface initialization flag
	std::shared_ptr<CShipInterfaceEventListener> m_pShipInterfaceEventListener = nullptr;       //!< the main listener element of messages from the ship interface flash item
};

DECLARE_SHARED_POINTERS(IShipInterface);

//! \brief Get the ship interface pointer
static IShipInterfacePtr GetShipInterfacePtr()
{
	IShipInterfacePtr pShipInterface;
	CryCreateClassInstanceForInterface(cryiidof<IShipInterface>(), pShipInterface);
	CRY_ASSERT(pShipInterface, "Can't create ship interface");

	return pShipInterface;
}

//! \class CShipInterfaceEventListener
//! \brief Main listener class for messages from the ship interface flash item
class CShipInterfaceEventListener : public IUIElementEventListener
{
public:
	//! \brief Ship interface event listener class constructor
	//! \param pShipInterface {CShipInterface*} a pointer to the ship interface flash item
	CShipInterfaceEventListener(CShipInterface* pShipInterface);
	//! \brief Class destructor
	virtual ~CShipInterfaceEventListener();

	//! \brief This is called, if an event (fscommand) is triggered from flash. This function is only called if the event is found in the UI Element XML file.
	//! The OnUIEvent callback function will only handle events, which are defined in the UI Element XML file. 
	//! There is also a OnUIEventEx callback function, which will trigger for other fscommands coming from your UI element
	//! \param pSender {IUIElement*} the UI element that sent the event
	//! \param event {SUIEventDesc} the UI event that occurred
	//! \param args {SUIArguments} the arguments that are tied to the event
	virtual void OnUIEvent(IUIElement* pSender, const SUIEventDesc& event, const SUIArguments& args) override;

	//! \brief This is called, if an event is triggered from flash and it is not found in the UI Element XML file.
	//! \param pSender {IUIElement*} the UI element that sent the event
	//! \param fscommand {char*} the command that was sent
	//! \param args {SUIArguments} the arguments that are tied to the event
	//! \param pUserData {void*} user data (are set when the flash menu is initialized?)
	virtual void OnUIEventEx(IUIElement* pSender, const char* fscommand, const SUIArguments& args, void* pUserData) override;

protected:
	CShipInterface* m_pShipInterface = nullptr;				//!< a pointer to a ship interface flash item
};

} // namespace UserInterface

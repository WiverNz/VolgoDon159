#pragma once
#include <CrySystem/Scaleform/IFlashUI.h>
#include <CryEntitySystem/IEntityBasicTypes.h>
#include <CrySystem/IEngineModule.h>
#include <GameModules/GameManager.h>

//! \namespace UserInterface
//! \brief The user interface namespace. Only Common and GameModules dependencies are possible here.
namespace UserInterface
{
// Forward declaration of local classes
class CMainMenuEventListener;

//! \class IMainMenu
//! \brief Interface for {CMainMenu}
class IMainMenu : public ICryUnknown, public GameModules::IGameManagerListener
{
	CRYINTERFACE_DECLARE_GUID(IMainMenu, "1901565A-7C5C-4300-A9C8-49EF0E051C86"_cry_guid);
public:
	//! \brief Check if the menu needs to be skipped (disable, for example, if there is no Scaleform support)
	//! \return {bool} true if menu should be skipped
	virtual bool IsSkipMainMenu() const = 0;

	//! \brief Sets the visibility of the menu
	//! \param isVisible {bool} true for visible
	virtual void SetVisible(const bool isVisible) = 0;

	//! \brief Gets the visibility of the menu
	//! \return {bool} true if visible
	virtual bool IsVisible() const = 0;

	//! \brief Initialization of the main menu
	//! \return {bool} true if menu initialized successfully
	virtual bool Initialize() = 0;

	//! \brief Additional update for each frame
	//! \param frameTime {float} frame time
	//! Called after ISystem has been updated by the {CGamePlugin} class
	virtual void MainUpdate(float frameTime) = 0;

	//! \brief Loads the current game settings to the flash menu
	//! \return {bool} true if loaded successfully
	virtual bool LoadToFlashSettings() = 0;

	//! \brief Setting the "resume game state" flag in the flash menu
	//! \return {bool} true if set successfully
	virtual bool SetResumeGameState(bool resumeGameState) = 0;

	//! \brief Establishes an extend debugging of the flash main menu
	//! \param isExtendDebug {bool} extended debug flag
	//! \return {bool} true if set successfully
	virtual bool SetExtendDebug(bool isExtendDebug) const = 0;

protected:
	// Inherited via IGameManagerListener
	virtual void OnGameManagerEvent(GameModules::EGameManagerEvent event) = 0;
};

class CMainMenu : public IMainMenu
{
	CRYINTERFACE_SIMPLE(IMainMenu)
	CRYGENERATE_SINGLETONCLASS_GUID(CMainMenu, "CMainMenu", "BD3C2170-740C-41A3-9D4B-136E181C2E76"_cry_guid)
	CMainMenu() = default;
	virtual ~CMainMenu();

public:
	bool Initialize() override;
	bool LoadToFlashSettings() override;
	void SetVisible(const bool isVisible) override;
	inline bool IsSkipMainMenu() const override;
	inline bool IsVisible() const override;
	void MainUpdate(float frameTime) override;
	bool SetResumeGameState(const bool resumeGameState) override;
	bool SetExtendDebug(bool isExtendDebug) const override;
	void OnGameManagerEvent(GameModules::EGameManagerEvent event) override;

protected:
	static const CryStringLocal s_uiElementName;								//!< an ui element name (in the xml)
	IUIElement* m_pMainMenuElement = nullptr;									//!< main element of the flush menu (removal from outside)
	ITexture* m_pBackgroundTexture = nullptr;									//!< texture for background display (not used)
	bool m_isSkipMainMenu = false;												//!< menu skip flag
	bool m_isVisible = false;													//!< menu visibility flag
	bool m_isMenuInitialized = false;											//!< menu initialization flag
	bool m_resumeGameState = false;												//!< flag of the current state (pause/resume) of the menu
	std::shared_ptr<CMainMenuEventListener> m_pMenuEventListener = nullptr;		//!< the main listener element of messages from the flash menu
};

DECLARE_SHARED_POINTERS(IMainMenu);

//! \brief Get the main menu interface pointer
static IMainMenuPtr GetMainManuPtr()
{
	IMainMenuPtr pMainMenu;
	CryCreateClassInstanceForInterface(cryiidof<IMainMenu>(), pMainMenu);
	CRY_ASSERT(pMainMenu, "Can't create main menu");

	return pMainMenu;
}

//! \class CMainMenuEventListener
//! \brief Main listener class for messages from the flash main menu
class CMainMenuEventListener : public IUIElementEventListener
{
public:
	//! \brief Main menu event listener class constructor
	//! \param pMainMenu {CMainMenu*} a pointer to the main menu
	CMainMenuEventListener(CMainMenu* pMainMenu);
	//! \brief Class destructor
	virtual ~CMainMenuEventListener();

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
	IMainMenu* m_pMainMenu = nullptr;				//!< a pointer to a main menu item
	static const CryStringLocal s_startCmd;			//!< an start event from the flash menu
	static const CryStringLocal s_resumeCmd;		//!< a resume event from the flash menu
	static const CryStringLocal s_exitCmd;			//!< an exit event from the flash menu
	static const CryStringLocal s_playSound;		//!< a play sound event from the flash menu
	static const CryStringLocal s_applySettings;	//!< an apply settings event from the flash menu
};

} // namespace UserInterface

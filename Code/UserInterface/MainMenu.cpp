#include "StdAfx.h"
#include "MainMenu.h"

#include <CryCore/StaticInstanceList.h>
#include <CrySystem/ICryMiniGUI.h>
#include <CrySystem/Profilers/IPerfHud.h>
#include <CrySystem/IBudgetingSystem.h>
#include <CryGame/IGameFramework.h>
#include <CryRenderer/IRenderAuxGeom.h>
#include <CryInput/IHardwareMouse.h>
#include <CrySerialization/IArchiveHost.h>
#include <IActionMapManager.h>

#include <CryCore/Assert/CryAssert.h>

#include "Common/ConfigLoader.h"
#include "GameModules/GameManager.h"
#include "GameModules/GameState.h"
#include "GameModules/GameSound.h"

CRYREGISTER_SINGLETON_CLASS(UserInterface::CMainMenu)

namespace UserInterface
{
const CryStringLocal CMainMenu::s_uiElementName = "MainMenu";

const CryStringLocal CMainMenuEventListener::s_startCmd = "StartGame";
const CryStringLocal CMainMenuEventListener::s_resumeCmd = "ResumeGame";
const CryStringLocal CMainMenuEventListener::s_exitCmd = "ExitGame";
const CryStringLocal CMainMenuEventListener::s_playSound = "PlaySound";
const CryStringLocal CMainMenuEventListener::s_applySettings = "ApplySettings";

CMainMenu::~CMainMenu()
{
	if (m_pBackgroundTexture)
	{
		m_pBackgroundTexture->Release();
		m_pBackgroundTexture = nullptr;
	}
}

bool CMainMenu::IsSkipMainMenu() const
{
	return m_isSkipMainMenu;
}
bool CMainMenu::IsVisible() const
{
	return m_isVisible;
}

bool CMainMenu::Initialize()
{
	if (m_isMenuInitialized)
	{
		CRY_ASSERT(false, "The main menu has already been initialized");
		return m_isMenuInitialized;
	}

	if (GameModules::GetIGameManagerPtr()->IsSkipFlash())
	{
		m_isSkipMainMenu = true;
		return true;
	}

	// m_pBackgroundTexture = gEnv->pRenderer->EF_LoadTexture(Common::GetIConfigLoaderPtr()->GetMenuBackgroundPath());

	IFlashUIPtr pFlashUI = GetIFlashUIPtr();
	m_isMenuInitialized = false;
	CRY_ASSERT(pFlashUI, "FlashUI extension is not supported");
	if (pFlashUI != nullptr)
	{
		m_pMainMenuElement = pFlashUI->GetUIElement(s_uiElementName);
		CRY_ASSERT(m_pMainMenuElement, "Can't find MainMenu UIElement");
		if (m_pMainMenuElement)
		{
			m_isMenuInitialized = m_pMainMenuElement->Init();
		}
	}
	if (!m_isMenuInitialized)
	{
		if (Cry::Assert::ShowDialogOnAssert())
		{
			CRY_ASSERT(m_isMenuInitialized, "Can't initialize MainMenu UIElement. Probably something wrong with the Scaleform GFx. Continue without a menu?");
		}
		else
		{
			if (CryMessageBox("Probably something wrong with the Scaleform GFx. Continue without a menu?", "Can't initialize MainMenu UIElement", eMB_YesCancel) == eQR_No)
			{
				TerminateProcess(GetCurrentProcess(), 0);
			}
		}
		// If CryEngine was compiled from source code, then you need Scaleform in this folder Cryengine/Code/SDKs/Scaleform
		CryLogAlways("Ok, let's continue without a menu");
		m_isSkipMainMenu = true;
		GameModules::GetIGameManagerPtr()->SetSkipFlash(true);
	}
	else
	{
		m_pMenuEventListener.reset(new CMainMenuEventListener(this));
		m_pMainMenuElement->AddEventListener(m_pMenuEventListener.get(), "MenuEventListener");
		m_pMainMenuElement->Init();
		LoadToFlashSettings();
		GameModules::GetIGameManagerPtr()->RegisterListener(this);
		SetExtendDebug(GameModules::GetIGameManagerPtr()->IsExtendDebug());
	}

	return m_isMenuInitialized;
}

bool CMainMenu::LoadToFlashSettings()
{
	if (!m_isMenuInitialized || !m_pMainMenuElement || gEnv->IsEditor())
	{
		return false;
	}
	const string cryJsonSettings = GameModules::GetIGameManagerPtr()->GetGameSettings().GetJson();
	SUIArguments args;
	args.AddArgument(cryJsonSettings);
	TUIData res;
	m_pMainMenuElement->CallFunction("LoadCrySettings", args, &res);

	bool result = false;
	res.GetValueWithConversion(result);

	CRY_ASSERT(result, "Can't load cry settings to the flash menu '" + cryJsonSettings + "'");

	return result;
}

void CMainMenu::SetVisible(const bool isVisible)
{
	CryLog("Set menu visible %d", isVisible);
	if (IsSkipMainMenu())
	{
		return;
	}

	m_isVisible = isVisible;
	CRY_ASSERT(m_pMainMenuElement, "Main menu element was not initialized. Please call the Initialize method before.");
	if (!m_pMainMenuElement)
	{
		return;
	}

	m_pMainMenuElement->SetVisible(m_isVisible);
}

void CMainMenu::MainUpdate(float frameTime)
{
	if (!m_isVisible)
	{
		return;
	}

	if (m_pBackgroundTexture)
	{
		// TODO: how to print background image and flash on top of it?
		//float scale = 1.0;
		//IRenderAuxImage::Draw2dImage(0, 0, 1280 * scale, 720 * scale, -1, 0.0f, 0.0f, 1.0f, 1.0f, 0.f, 0.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0);
		//IRenderAuxImage::Draw2dImage(0.0f, 0.0f, 1280 * scale, 720 * scale, m_pBackgroundTexture->GetTextureID(), 0.0f, 1.0f, 1.0f, 0.0f, 0, 1, 1, 1, 0.5f, 1.f, 0);
		//gEnv->pSystem->GetIConsole()->SetImage(NULL, FT_NOMIPS);
	}
	if (m_pMainMenuElement && m_pMainMenuElement->IsVisible())
	{
		// TODO: find a way to use xml UIElement name="MainMenu" mouseevents="1" in flash. There are no mouse events. (possibly due to pause mode?)
		float pfX = -1;
		float pfY = -1;
		gEnv->pHardwareMouse->GetHardwareMouseClientPosition(&pfX, &pfY);
		m_pMainMenuElement->SendCursorEvent(SFlashCursorEvent::eCursorMoved, static_cast<int>(pfX), static_cast<int>(pfY));
	}
}

bool CMainMenu::SetResumeGameState(const bool resumeGameState)
{
	m_resumeGameState = resumeGameState;

	if (!m_isMenuInitialized || !m_pMainMenuElement)
	{
		return false;
	}

	SUIArguments args;
	args.AddArgument(m_resumeGameState);
	TUIData res;
	m_pMainMenuElement->CallFunction("SetResumeGameState", args, &res);

	bool result = false;
	res.GetValueWithConversion(result);
	CRY_ASSERT(result, "Can't set resume game state in the flash menu");

	return result;
}

bool CMainMenu::SetExtendDebug(bool isExtendDebug) const
{
	if (!m_isMenuInitialized || !m_pMainMenuElement)
	{
		return false;
	}

	SUIArguments args;
	args.AddArgument(isExtendDebug);
	TUIData res;
	m_pMainMenuElement->CallFunction("SetExtendDebug", args, &res);

	bool result = false;
	res.GetValueWithConversion(result);
	CRY_ASSERT(result, "Can't set extend debug flag in the flash menu");

	return result;
}

void CMainMenu::OnGameManagerEvent(GameModules::EGameManagerEvent event)
{
	CryLog("CMainMenu::OnGameManagerEvent");
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

CMainMenuEventListener::CMainMenuEventListener(CMainMenu* pMainMenu) : m_pMainMenu(pMainMenu)
{

}

CMainMenuEventListener::~CMainMenuEventListener()
{

}

void CMainMenuEventListener::OnUIEvent(IUIElement* pSender, const SUIEventDesc& event, const SUIArguments& args)
{
	CryLog("event.sDisplayName: %s", event.sDisplayName);

	CryStringLocal eventName = event.sDisplayName;
	if (eventName == s_startCmd || eventName == s_resumeCmd)
	{
		GameModules::GetGameStateContextPtr()->Resume();
	}
	else if (eventName == s_exitCmd)
	{
		GameModules::GetGameStateContextPtr()->EndGame();
		gEnv->pSystem->Quit();
	}
	else if (eventName == s_playSound)
	{
		string soundFile = args.GetAsString();
		GameModules::GetIGameSoundPtr()->PlaySoundTrigger(soundFile);
	}
	else if (eventName == s_applySettings)
	{
		CRY_ASSERT(args.GetArgCount(), "There is no settings argument from the flash side");
		const string &settingsJson = args.GetAsString();
		if (!settingsJson.empty())
		{
			CryLog(settingsJson);
			GameModules::CGameSettings settings;
			if (Serialization::LoadJsonBuffer(settings, settingsJson, settingsJson.size()))
			{
				GameModules::GetIGameManagerPtr()->GetGameSettings().ApplyNewSettings(settings);
			}
		}
	}
	else
	{
		CryLog("Unhandled event: %s", event.sDisplayName);
	}
}

void CMainMenuEventListener::OnUIEventEx(IUIElement* pSender, const char* fscommand, const SUIArguments& args, void* pUserData)
{
	CryLog("fscommand: %s", fscommand);
}

} // namespace UserInterface

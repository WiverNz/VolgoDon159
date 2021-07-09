#include "StdAfx.h"
#include "GameManager.h"

#include <CryCore/Assert/CryAssert.h>

// Register the factory
// Note that this has to be done in a source file that is not included anywhere else.
CRYREGISTER_SINGLETON_CLASS(GameModules::CGameManager)

namespace GameModules
{
inline int CGameManager::GenNextControllingId()
{
	return ++m_lastShipControllingId;
}

inline bool CGameManager::IsSkipFlash() const
{
	return m_isSkipFlash;
}

inline void CGameManager::SetSkipFlash(bool isSkipFlash)
{
	m_isSkipFlash = isSkipFlash;
}

inline CGameSettings& CGameManager::GetGameSettings()
{
	return m_gameSettings;
}

inline bool CGameManager::IsExtendDebug() const
{
	if (!m_pExtendDebugCVar)
	{
		return false;
	}
	if (m_pExtendDebugCVar->GetIVal() != 0)
	{
		return true;
	}

	return false;
}

const char* CGameManager::GetName() const
{
	return nullptr;
}

const char* CGameManager::GetCategory() const
{
	return nullptr;
}

bool CGameManager::Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams)
{
	m_pExtendDebugCVar = REGISTER_CVAR_CB(ga_extendDebug, 0, VF_CHEAT, "Extend debug. 0=disabled, 1=enabled", OnExtendDebugChanged);

	return true;
}

void CGameManager::RegisterListener(IGameManagerListener* pListener)
{
	stl::push_back_unique(m_listeners, pListener);
}

void CGameManager::UnregisterListner(IGameManagerListener* pListener)
{
	stl::find_and_erase(m_listeners, pListener);
}

void CGameManager::SetSupressEvents(bool isSupress)
{
	m_isSupressEvents = isSupress;
}

void CGameManager::SendEvent(EGameManagerEvent event)
{
	if (m_isSupressEvents)
	{
		return;
	}
	for (size_t i = 0; i < m_listeners.size(); i++)
	{
		m_listeners[i]->OnGameManagerEvent(event);
	}
}

void CGameManager::OnExtendDebugChanged(ICVar* pCVar)
{
	auto gameManager = GetIGameManagerPtr();
	if (gameManager)
	{
		CryLog("OnExtendDebugChanged %d", gameManager->IsExtendDebug());
		gameManager->SendEvent(EGameManagerEvent::GM_EVENT_EXTEND_DEBUG_UPDATE);
	}
}

} // namespace GameModules

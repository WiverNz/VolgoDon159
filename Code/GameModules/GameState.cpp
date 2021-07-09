#include "StdAfx.h"
#include "GameState.h"

#include <CrySchematyc/CoreAPI.h>
#include <CryGame/IGameFramework.h>
#include <CryCore/Assert/CryAssert.h>
#include <CryInput/IHardwareMouse.h>

namespace GameModules
{
// IGameState
EGameState IGameState::GetState() const
{
	return m_gameState;
}
// ~IGameState

// CGamePausedState
CGamePausedState::CGamePausedState() : IGameState(EGameState::GamePaused)
{
	CryLog("Creating a game paused state");
}

CGamePausedState::~CGamePausedState() 
{
	CryLog("Deleting a game paused State");
}

string CGamePausedState::GetName() const
{
	return "Paused";
}

void CGamePausedState::Pause(IGameStateContext* stateContext)
{
	IGameStatePtr thisKeeper = stateContext->GetState();
}

void CGamePausedState::Resume(IGameStateContext* stateContext)
{
	CryLog("CGamePausedState: Resume");
	IGameStatePtr thisKeeper = stateContext->GetState();
	gEnv->pHardwareMouse->DecrementCounter();
	stateContext->SetState(std::make_shared<CGameRunningState>());
	gEnv->pGameFramework->PauseGame(false, true);
	CrySleep(200); // TODO: find a better way to prevent 'Esc' after closing the main menu with Esc (remove key message event)
}

void CGamePausedState::EndGame(IGameStateContext* stateContext)
{
	IGameStatePtr thisKeeper = stateContext->GetState();
	stateContext->SetState(std::make_shared<CGameOverState>());
}
// ~CGamePausedState

// CGameRunningState
CGameRunningState::CGameRunningState() : IGameState(EGameState::GameRunning)
{
	CryLog("Creating a game running state");
}

CGameRunningState::~CGameRunningState()
{
	CryLog("Deleting a game running State");
}

string CGameRunningState::GetName() const
{
	return "Running";
}

void CGameRunningState::Pause(IGameStateContext* stateContext)
{
	CryLog("CGameRunningState: Pause");
	IGameStatePtr thisKeeper = stateContext->GetState();
	gEnv->pHardwareMouse->IncrementCounter();
	stateContext->SetState(std::make_shared<CGamePausedState>());
	gEnv->pGameFramework->PauseGame(true, true);
}

void CGameRunningState::Resume(IGameStateContext* stateContext)
{
	IGameStatePtr thisKeeper = stateContext->GetState();
}

void CGameRunningState::EndGame(IGameStateContext* stateContext)
{
	IGameStatePtr thisKeeper = stateContext->GetState();
	stateContext->SetState(std::make_shared<CGameOverState>());
}
// ~CGameRunningState

// CGameOverState
CGameOverState::CGameOverState() : IGameState(EGameState::GameOver)
{
	CryLog("Creating a game over state");
}

CGameOverState::~CGameOverState()
{
	CryLog("Deleting a game over State");
}

string CGameOverState::GetName() const
{
	return "Game over";
}

void CGameOverState::Pause(IGameStateContext* stateContext)
{
	IGameStatePtr thisKeeper = stateContext->GetState();
	stateContext->SetState(std::make_shared<CGamePausedState>());
	CryLog(this->GetName());
}

void CGameOverState::Resume(IGameStateContext* stateContext)
{
	IGameStatePtr thisKeeper = stateContext->GetState();
	stateContext->SetState(std::make_shared<CGameRunningState>());
}

void CGameOverState::EndGame(IGameStateContext* stateContext)
{
	IGameStatePtr thisKeeper = stateContext->GetState();
}
// ~CGameOverState

// CGameStateContext
CRYREGISTER_SINGLETON_CLASS(CGameStateContext)

CGameStateContext::CGameStateContext()
{
	CryLog("Creating a game state context");
}

void CGameStateContext::Pause()
{
	if (gEnv->IsEditor())
	{
		CryLog("Can't pause in editor mode");
		return;
	}
	CRY_ASSERT(m_gameContextState);
	m_gameContextState->Pause(this);
}

void CGameStateContext::Resume()
{
	if (gEnv->IsEditor())
	{
		CryLog("Can't resume in editor mode");
		return;
	}
	CRY_ASSERT(m_gameContextState);
	m_gameContextState->Resume(this);
}

void CGameStateContext::EndGame()
{
	if (gEnv->IsEditor())
	{
		CryLog("Can't end game in editor mode");
		return;
	}
	CRY_ASSERT(m_gameContextState);
	m_gameContextState->EndGame(this);
}

void CGameStateContext::SetState(IGameStatePtr state)
{
	CRY_ASSERT(m_gameContextState);
	CryLog("Game state changed from %s to %s", m_gameContextState->GetName(), state->GetName());
	m_gameContextState = state;
}

IGameStatePtr CGameStateContext::GetState() const
{
	CRY_ASSERT(m_gameContextState);
	return m_gameContextState;
}

EGameState CGameStateContext::GetEState() const
{
	auto state = GetState();
	return state->GetState();
}
// ~CGameStateContext

} // namespace GameModules

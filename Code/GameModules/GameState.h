#pragma once

#include <CryExtension/ICryFactory.h>
#include <CryExtension/ClassWeaver.h>
#include <CryExtension/CryCreateClassInstance.h>

//! \namespace GameModules
//! \brief The game modules namespace. Here only dependence with the Common is possible.
namespace GameModules
{

struct IGameStateContext;

//! \brief State of the game
enum class EGameState
{
	GamePaused,
	GameRunning,
	GameOver
};

//! \class IGameState
//! \brief Interface for {CGameState}
class IGameState
{
public:
	IGameState(const EGameState& gameState)
		: m_gameState(gameState)
	{
	}
	virtual ~IGameState() = default;
	inline EGameState GetState() const;
	virtual string GetName() const = 0;

	virtual void Pause(IGameStateContext*) = 0;
	virtual void Resume(IGameStateContext*) = 0;
	virtual void EndGame(IGameStateContext*) = 0;

protected:
	EGameState m_gameState = EGameState::GameOver;
};

DECLARE_SHARED_POINTERS(IGameState);

//! \struct IGameStateContext
//! \brief Interface for {CGameStateContext}
struct IGameStateContext : public Cry::IDefaultModule
{
	CRYINTERFACE_DECLARE_GUID(IGameStateContext, "813286FF-91DC-4FEB-AFD2-955A59F0793E"_cry_guid);
	friend class CGamePausedState;
	friend class CGameRunningState;
	friend class CGameOverState;
public:
	//! \brief Pause game
	virtual void Pause() = 0;
	//! \brief Resume game
	virtual void Resume() = 0;
	//! \brief End game
	virtual void EndGame() = 0;
	//! \brief Get current game state 
	//! \return {IGameStatePtr} game state pointer
	virtual IGameStatePtr GetState() const = 0;
	//! \brief Get current game state enum
	//! \return {EGameState} game state enum
	virtual EGameState GetEState() const = 0;
protected:
	//! \brief Set game state
	//! \param state {IGameStatePtr} game state
	virtual void SetState(IGameStatePtr state) = 0;
};

DECLARE_SHARED_POINTERS(IGameStateContext);

//! \class CGamePausedState
//! \brief Game paused state
class CGamePausedState : public IGameState
{
public:
	CGamePausedState();
	virtual ~CGamePausedState();
	string GetName() const override;
	void Pause(IGameStateContext* stateContext) override;
	void Resume(IGameStateContext* stateContext) override;
	void EndGame(IGameStateContext* stateContext) override;
};

//! \class CGameRunningState
//! \brief Game running state
class CGameRunningState : public IGameState
{
public:
	CGameRunningState();
	virtual ~CGameRunningState();
	string GetName() const override;
	void Pause(IGameStateContext* stateContext) override;
	void Resume(IGameStateContext* stateContext) override;
	void EndGame(IGameStateContext* stateContext) override;
};

//! \class CGameOverState
//! \brief Game over state
class CGameOverState : public IGameState
{
public:
	CGameOverState();
	virtual ~CGameOverState();
	string GetName() const override;
	void Pause(IGameStateContext* stateContext) override;
	void Resume(IGameStateContext* stateContext) override;
	void EndGame(IGameStateContext* stateContext) override;
};

//! \class CGameStateContext
//! \brief Main class for controlling the current state of the game
class CGameStateContext : public IGameStateContext
{
	CRYINTERFACE_BEGIN()
		CRYINTERFACE_ADD(Cry::IDefaultModule)
		CRYINTERFACE_ADD(IGameStateContext)
	CRYINTERFACE_END()

	CRYGENERATE_SINGLETONCLASS_GUID(CGameStateContext, "CGameStateContext", "DC373FF4-61CC-43D5-A572-A72431CF6C23"_cry_guid)
	CGameStateContext();
	virtual ~CGameStateContext() = default;
public:
	const char* GetName() const override { return "CGameStateContext"; }
	const char* GetCategory() const override { return "GameModules"; }
	bool Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) override
	{
		return true;
	}

	void Pause() override;
	void Resume() override;
	void EndGame() override;

	IGameStatePtr GetState() const override;
	EGameState GetEState() const override;

protected:
	void SetState(IGameStatePtr state) override;

protected:
	IGameStatePtr m_gameContextState = std::make_shared<CGameRunningState>();	//!< current state of the game
};

//! \brief Get game state interface pointer
static IGameStateContextPtr GetGameStateContextPtr()
{
	IGameStateContextPtr pGameStateContext;

	if (!CryCreateClassInstanceForInterface(cryiidof<IGameStateContext>(), pGameStateContext))
	{
		CryWarning(VALIDATOR_MODULE_FLOWGRAPH, VALIDATOR_ERROR, "Could not create class instance of the game state");
	}

	return pGameStateContext;
}

} // namespace GameModules

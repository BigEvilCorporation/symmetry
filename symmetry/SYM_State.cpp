#include "SYM_State.h"
#include "SYM_Engine.h"

namespace sym
{
	void CStateManager::Shutdown()
	{
		//If states still exist, shut them down and pop themm off the stack
		while(!States.empty())
		{
			States.back()->Shutdown();
			PopState();
		}
	}

	void CStateManager::SwitchState(CState *State)
	{
		//If a state is on the stack, pop it back
		if(!States.empty()) PopState();

		//Push new state
		if(State) PushState(State);
	}

	void CStateManager::PushState(CState *State)
	{
		if(State)
		{
			//Pause current state
			if(GetCurrentState()) GetCurrentState()->Pause();

			//Push state onto stack
			States.push_back(State);

			//Init new state
			GetCurrentState()->Init();
		}
	}

	void CStateManager::PopState()
	{
		//Pop state off stack
		if(!States.empty())
		{
			if(GetCurrentState()) GetCurrentState()->Shutdown();
			States.pop_back();
		}

		//If states are still on the stack
		if(GetCurrentState())
		{
			GetCurrentState()->Resume();
		}
		//If not, shut down engine
		else sym::Engine->ShutDown();
	}

	CState * CStateManager::GetCurrentState()
	{
		//If there are states on the stack, return the top state
		if(!States.empty()) return States.back();
		else return 0;
	}

	void CStateManager::HandleInput()
	{
		//Instruct top state to handle input
		if(GetCurrentState()) GetCurrentState()->HandleInput();
	}

	void CStateManager::Update()
	{
		//Instruct current state to update
		if(GetCurrentState()) GetCurrentState()->Update();
	}

	void CStateManager::Render()
	{
		//Instruct current state to render
		if(GetCurrentState()) GetCurrentState()->Render();
	}
}
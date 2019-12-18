#ifndef __SYM_STATE
#define __SYM_STATE

#ifdef SYM_DLL
#   define __declspec(dllexport)
#else
//#   define __declspec(dllimport)
#endif

#include <vector>

namespace sym
{
	class CState
	{
		public:
			virtual void Init() = 0;
			virtual void Shutdown() = 0;

			virtual void Pause() = 0;
			virtual void Resume() = 0;

			virtual void HandleInput() = 0;
			virtual void Update() = 0;
			virtual void Render() = 0;
	};

	class CStateManager
	{
		public:
			void Shutdown();

			void SwitchState(CState *State);
			void PushState(CState *State);
			void PopState();
			CState * GetCurrentState();

			void HandleInput();
			void Update();
			void Render();

		private:
			std::vector<CState*> States;
	};
}

#endif
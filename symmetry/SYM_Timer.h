#ifndef __SYM_TIMER
#define __SYM_TIMER

#ifdef SYM_DLL
#   define __declspec(dllexport)
#else
//#   define __declspec(dllimport)
#endif

#include <SDL/SDL.h>

namespace sym
{
	void SplitTime(int Time, int &Hours, int &Minutes, int &Seconds);

	class CTimer
	{
		public:
			//Constructor
			CTimer();
			CTimer(int Milliseconds, bool Loop, bool StartNow);

			//Set values
			void SetTime(float Milliseconds);
			float GetTime();
			void SetLooping(bool Loop);

			//Start timer
			void Start();

			//Stop timer
			void Stop();

			//Reset timer
			void Reset();

			//Has timer triggered?
			bool HasTriggered();

			//Is timer running?
			bool IsRunning() { return Running; }

			//Get total elapsed time
			int GetElapsed();

		private:
			//The time
			int TimeMilliseconds;

			//Start time
			int StartTime;

			//Current time
			int CurrTime;

			//Should this timer loop
			bool Looping;

			//Is this timer running
			bool Running;
	};

} //Namespace

#endif

#include "SYM_Timer.h"

namespace sym
{
	void SplitTime(int Time, int &Hours, int &Minutes, int &Seconds)
	{
		Hours = Time / 3600;
		Time = Time % 3600;
		Minutes = Time / 60;
		Time = Time % 60;
		Seconds = Time;
	}

	CTimer::CTimer()
	{
		//Initialise variables
		TimeMilliseconds = 0;
		StartTime = 0;
		CurrTime = 0;
		Running = false;
		Looping = false;
	}

	CTimer::CTimer(int Milliseconds, bool Loop, bool StartNow)
	{
		//Initialise variables
		StartTime = 0;
		CurrTime = 0;
		Running = false;

		//Set time
		TimeMilliseconds = Milliseconds;

		//Set looping
		Looping = Loop;

		//Should the timer start immediately
		if(StartNow) Start();
	}

	void CTimer::SetTime(float Milliseconds)
	{
		TimeMilliseconds = Milliseconds;
	}

	float CTimer::GetTime()
	{
		return TimeMilliseconds;
	}

	void CTimer::SetLooping(bool Loop)
	{
		Looping = Loop;
	}

	void CTimer::Start()
	{
		//If the timer is not running
		if(!Running)
		{
			StartTime = SDL_GetTicks();
			Running = true;
		}
	}

	void CTimer::Stop()
	{
		Running = false;
	}

	void CTimer::Reset()
	{
		StartTime = SDL_GetTicks();
	}

	bool CTimer::HasTriggered()
	{
		if(Running == true)
		{
			//Update the current time
			CurrTime = SDL_GetTicks();

			//Check if the timer has elapsed
			if(CurrTime - StartTime > TimeMilliseconds)
			{
				//Set starting time to current time
				StartTime = CurrTime;

				//If timer is a single shot, stop the timer
				if(!Looping) Stop();

				//Return true
				return true;
			}
		}

		//Return false
		return false;
	}

	int CTimer::GetElapsed()
	{
		return SDL_GetTicks() - StartTime;
	}

} //Namespace

#ifndef __SYM_AUDIO
#define __SYM_AUDIO

#ifdef SYM_DLL
#   define __declspec(dllexport)
#else
//#   define __declspec(dllimport)
#endif

#include "SYM_Vector.h"

#include <fmod/fmod.hpp>
#include <string>

namespace sym
{
	const int SOUND_ONCE = 0;
	const int SOUND_LOOP = 1;

	const int SOUND_2D = 0;
	const int SOUND_3D = 1;

	class CAudioSystem
	{
		public:	
			bool Init();
			void Shutdown();
			void Update();

			void SetListenerPosition(CVector3 Position, CVector3 Forward, CVector3 Up);

			void SetMasterVolume(float Volume);

			FMOD::System *GetFMODSystem() { return FmodSystem; }

		private:
			FMOD::System *FmodSystem;
	};

	class CSound
	{
		public:
			CSound();
			~CSound();

			bool Load(std::string Filename, bool Positional = false, bool Looping = false);

			void Play();
			void Stop();

			void SetVolume(float Volume);
			void SetPosition(CVector3 Position);
			void SetConeDegrees(float Inside, float Outside, float Volume);
			void SetMinMaxDistance(float Min, float Max);

		private:
			FMOD::Sound *Sound;
			FMOD::Channel *Channel;
	};
}

#endif
#include "SYM_Audio.h"
#include "SYM_Engine.h"

namespace sym
{
	bool CAudioSystem::Init()
	{
		FMOD_RESULT      Result;
		unsigned int     Version;
		FMOD_SPEAKERMODE SpeakerMode;
		FMOD_CAPS        Caps;

		//Create the Fmod system and initialise
		Result = FMOD::System_Create(&FmodSystem);
		if(Result != FMOD_OK)
		{
			Engine->Logger.AddEntry("Error: Could not create FMod system");
			return false;
		}
	    
		Result = FmodSystem->getVersion(&Version);
		if(Result != FMOD_OK)
		{
			Engine->Logger.AddEntry("Error: Could not get FMod version");
			return false;
		}

		if (Version < FMOD_VERSION)
		if(Result != FMOD_OK)
		{
			Engine->Logger.AddEntry("Error: Incorrect FMod version installed");
			return false;
		}
	    
		Result = FmodSystem->getDriverCaps(0, &Caps, 0, 0, &SpeakerMode);
		Result = FmodSystem->setSpeakerMode(SpeakerMode);

		if(Caps & FMOD_CAPS_HARDWARE_EMULATED)
		{
			Engine->Logger.AddEntry("Warning: Audio hardware acceleration is disabled on this system, check your OS sound settings");
			Engine->Console.PrintLine("Warning: Audio hardware acceleration is disabled on this system, check your OS sound settings");
			Result = FmodSystem->setDSPBufferSize(1024, 10);
		}

		Result = FmodSystem->init(100, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED, 0);
		if (Result == FMOD_ERR_OUTPUT_CREATEBUFFER)         /* Ok, the speaker mode selected isn't supported by this soundcard.  Switch it back to stereo... */
		{
			Engine->Logger.AddEntry("Warning: Speaker mode not supported, defaulting to stereo. Check your OS sound settings");
			Engine->Console.PrintLine("Warning: Speaker mode not supported, defaulting to stereo. Check your OS sound settings");
			Result = FmodSystem->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
			Result = FmodSystem->init(100, FMOD_INIT_NORMAL, 0);
		}

		return true;
	}

	void CAudioSystem::Shutdown()
	{
		if(FmodSystem) FmodSystem->release();
	}

	void CAudioSystem::Update()
	{
		FMOD_RESULT Result = FmodSystem->update();

		if(Result != FMOD_OK)
		{
			Engine->Logger.AddEntry("Error: Could not update sound system");
		}
	}

	void CAudioSystem::SetMasterVolume(float Volume)
	{
		FMOD::ChannelGroup *MasterGroup;

		FMOD_RESULT Result;

		if(FmodSystem) Result = FmodSystem->getMasterChannelGroup(&MasterGroup);

		if(Result != FMOD_OK)
		{
			Engine->Logger.AddEntry("Error: Could not set master volume");
		}
		else
		{
			MasterGroup->setVolume(Volume);
		}
	}

	void CAudioSystem::SetListenerPosition(CVector3 Position, CVector3 Forward, CVector3 Up)
	{
		const FMOD_VECTOR FPos = {Position.x, Position.y, Position.y};
		const FMOD_VECTOR FFwd = {Forward.x, Forward.y, Forward.z};
		const FMOD_VECTOR FUp = {Up.x, Up.y, Up.z};

		if(FmodSystem) FmodSystem->set3DListenerAttributes(0, &FPos, 0, &FFwd, &FUp);
	}

	CSound::CSound()
	{
		Sound = 0;
	}

	CSound::~CSound()
	{

	}

	bool CSound::Load(std::string Filename, bool Positional, bool Looping)
	{
		FMOD_RESULT Result;

		Filename = Engine->AudioPath + Filename;

		int Flags = FMOD_HARDWARE;

		if(Positional) Flags |= FMOD_3D;
		else Flags |= FMOD_2D;

		if(Looping) Flags |= FMOD_LOOP_NORMAL;
		else Flags |= FMOD_LOOP_OFF;

		Result = Engine->Audio.GetFMODSystem()->createSound(Filename.c_str(), Flags, 0, &Sound);

		if(Result != FMOD_OK)
		{
			Engine->Logger.AddEntry("Error: Could not open sound " + Filename);
			return false;
		}
		
		return true;
	}

	void CSound::Play()
	{
		if(Sound)
		{
			FMOD_RESULT Result;
			Result = Engine->Audio.GetFMODSystem()->playSound(FMOD_CHANNEL_FREE, Sound, false, &Channel);

			if(Result != FMOD_OK)
			{
				Engine->Logger.AddEntry("Error: Could not play sound");
			}
		}
	}

	void CSound::Stop()
	{
		if(Channel) Channel->stop();
		Channel = 0;
	}

	void CSound::SetVolume(float Volume)
	{
		if(Channel) Channel->setVolume(Volume);
	}

	void CSound::SetPosition(CVector3 Position)
	{
		const FMOD_VECTOR FPos = {Position.x, Position.y, Position.y};
		if(Channel) Channel->set3DAttributes(&FPos, 0);
	}

	void CSound::SetConeDegrees(float Inside, float Outside, float Volume)
	{
		if(Sound) Sound->set3DConeSettings(Inside, Outside, Volume);
	}

	void CSound::SetMinMaxDistance(float Min, float Max)
	{
		if(Sound) Sound->set3DMinMaxDistance(Min, Max);
	}
}
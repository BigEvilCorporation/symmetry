#ifndef __SYM_MPEG
#define __SYM_MPEG

#ifdef SYM_DLL
#   define __declspec(dllexport)
#else
//#   define __declspec(dllimport)
#endif

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <SDL/SDL.h>

#include <string>

#include "SYM_Texture.h"
#include "SYM_Audio.h"
#include "SYM_Timer.h"

extern "C"
{
	//Suppress 'int64_t to int' warnings
	#pragma warning (push)
	#pragma warning (disable:4244)
	#ifdef WIN32
        #include <ffmpeg/avcodec.h>
        #include <ffmpeg/avformat.h>
        #include <ffmpeg/swscale.h>
    #else
        #include <libavcodec/avcodec.h>
        #include <libavformat/avformat.h>
        #include <libswscale/swscale.h>
	#endif
	#pragma warning (pop)
}

namespace sym
{
	FMOD_RESULT F_CALLBACK GlobalPCMReadCallback(FMOD_SOUND *Sound, void *Data, unsigned int Length);

	class CAudioPacketQueue
	{
		public:
			CAudioPacketQueue();
			bool Add(AVPacket *Packet);
			bool Get(AVPacket *Packet, int Block);
			int DecodeFrame(AVCodecContext *CodecContext, uint8_t *Buffer, int BufferSize);

		private:
			AVPacketList *First, *Last;
			int NumBytes;
			int Size;
	};

	class CMpeg
	{
		public:
			//Constructor
			CMpeg();
			~CMpeg();

			//Load an MPEG file
			bool Load(std::string Filename);

			//Close the mpeg file
			void Close();

			//Start playing / unpause the stream
			void Play();

			//Stop playing the stream
			void Stop();

			//Seek to time
			void SeekForward(int Time);
			void SeekBackward(int Time);

			//Copy next frame to texture
			bool ReadFrame();

			//Render current frame fullscreen
			void RenderFrame();

			//Return true if video is playing
			bool IsPlaying();

			//Texture to store the frame
			CTexture *Texture;

			//FMOD PCM read callback
			void PCMReadCallback(void *Data, unsigned int Length);

		private:

			//Frame timer
			CTimer *FrameTimer;

			//Format context
			AVFormatContext *FormatContext;

			//Codec contexts
			AVCodecContext *VideoCodecContext;
			AVCodecContext *AudioCodecContext;

			//SWScale context
			SwsContext *SWScaleContext;

			//Codecs
			AVCodec *VideoCodec;
			AVCodec *AudioCodec;

			//A frame
			AVFrame *Frame;

			//A frame in RGB format
			AVFrame *FrameRGB;

			//A packet
			AVPacket Packet;

			//Buffers
			uint8_t *VideoBuffer;
			uint8_t *AudioBuffer;

			//Audio packet queue
			std::vector<AVPacket*> AudioPackets;

			//FMOD sound object
			FMOD::Sound *SoundStream;
			FMOD::Channel *SoundChannel;

			//FMOD sound info object
			FMOD_CREATESOUNDEXINFO SoundInfo;

			//Audio queue
			CAudioPacketQueue AudioQueue;

			//Number of bytes
			int NumBytes;

			//Current streams
			int VideoStream;
			int AudioStream;

			//Time between frames
			int FrameWait;

			double TimeBase;

			//Frame finished
			int FrameFinished;

			//Is the video running
			bool IsRunning;

			//Is the video loaded
			bool IsLoaded;

			//Does video contain audio
			bool HasAudio;

			//Video dimentions
			int Width, Height;
	};

} //Namespace

#endif

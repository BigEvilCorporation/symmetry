#include <algorithm>

#include "SYM_MPEG.h"
#include "SYM_Engine.h"

namespace sym
{
	int AudioBufferSize = 1024;

	FMOD_RESULT F_CALLBACK GlobalPCMReadCallback(FMOD_SOUND *Sound, void *Data, unsigned int Length)
	{
		CMpeg *MPEG = 0;

		FMOD::Sound *Snd = (FMOD::Sound*)Sound;
		
		Snd->getUserData((void**)&MPEG);

		if(MPEG) MPEG->PCMReadCallback(Data, Length);
		else memset(Data, 0, Length);

		return FMOD_OK;
	}

	CAudioPacketQueue::CAudioPacketQueue()
	{
		First = Last = 0;
		NumBytes = Size = 0;
	}

	bool CAudioPacketQueue::Add(AVPacket *Packet)
	{
		AVPacketList *PacketList = new AVPacketList;
		if(av_dup_packet(Packet) < 0) return false;
		if(!PacketList) return false;

		PacketList->pkt = *Packet;
		PacketList->next = 0;

		if(!Last) First = PacketList;
		else Last->next = PacketList;

		Last = PacketList;
		NumBytes++;
		Size += PacketList->pkt.size;

		return true;
	}

	bool CAudioPacketQueue::Get(AVPacket *Packet, int Block)
	{
		AVPacketList *PacketList;
		
		PacketList = First;

		if(PacketList)
		{
			First = PacketList->next;

			if(!First) Last = 0;

			NumBytes--;
			Size -= PacketList->pkt.size;

			*Packet = PacketList->pkt;

			delete PacketList;

			return true;
		}
		return false;
	}

	int CAudioPacketQueue::DecodeFrame(AVCodecContext *CodecContext, uint8_t *Buffer, int BufferSize)
	{
		static AVPacket Packet;
		static uint8_t *PacketData = 0;
		static int PacketSize = 0;
		int Length, DataSize;
		//bool Finished = false;

		while(1)
		{
			while(PacketSize > 0)
			{
				DataSize = BufferSize;

				Length = avcodec_decode_audio2(CodecContext, (int16_t*)Buffer, &DataSize, PacketData, PacketSize);

				if(Length < 0)
				{
					PacketSize = 0;
					break;
				}

				PacketData += Length;
				PacketSize -= Length;

				if(DataSize <= 0) continue;

				return DataSize;
			}

			if(Packet.data) av_free_packet(&Packet);

			if(!Get(&Packet, 1)) return -1;

			PacketData = Packet.data;
			PacketSize = Packet.size;
		}
	}

	void CMpeg::PCMReadCallback(void *Data, unsigned int Length)
	{
		uint8_t* FMODBuffer = (uint8_t*)Data;

		int Length1, AudioSize;

		static uint8_t AudioBuffer[(AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2];
		static unsigned AudioBufferSize = 0;
		static unsigned AudioBufferIndex = 0;

		while (Length > 0)
		{
			if(AudioBufferIndex >= AudioBufferSize)
			{
				AudioSize = AudioQueue.DecodeFrame(AudioCodecContext, AudioBuffer, sizeof(AudioBuffer));

				if(AudioSize < 0)
				{
					AudioBufferSize = 1024;
					memset(AudioBuffer, 0, AudioBufferSize);
				}
				else
				{
					AudioBufferSize = AudioSize;
				}

				AudioBufferIndex = 0;
			}

			Length1 = AudioBufferSize - AudioBufferIndex;

			if(Length1 > (int)Length) Length1 = (int)Length;

			memcpy(FMODBuffer, (uint8_t*)AudioBuffer + AudioBufferIndex, Length1);

			Length -= Length1;
			FMODBuffer += Length1;
			AudioBufferIndex += Length1;
		}
	}

	CMpeg::CMpeg()
	{
		IsRunning = false;
		IsLoaded = false;
		HasAudio = true;
		FrameTimer = 0;
		Texture = 0;

		SoundStream = 0;
		SoundChannel = 0;
	}

	CMpeg::~CMpeg()
	{
	    if(IsLoaded) Close();
	}

	void CMpeg::Close()
	{
		if(IsLoaded)
		{
			//Stop playing
			Stop();

			//Free the RGB image
			delete [] VideoBuffer;
			av_free(FrameRGB);

			//Free the YUV frame
			av_free(Frame);

			//Free the SWscale context
			sws_freeContext(SWScaleContext);

			//Close the codec
			avcodec_close(VideoCodecContext);

			//Close the video file
			av_close_input_file(FormatContext);

			IsLoaded = false;

			if(FrameTimer) delete FrameTimer;
			if(Texture) delete Texture;
		}
	}

	bool CMpeg::Load(std::string Filename)
	{
		//Register all formats and codecs
		av_register_all();

		//Close previous video if open
		if(IsLoaded) Close();

		//Convert filename to upper case
		//transform(Filename.begin(), Filename.end(), Filename.begin(), toupper);

		//Prefix filename with path from engine config
		Filename = Engine->VideosPath + Filename;

		// Open video file
		if(av_open_input_file(&FormatContext, Filename.c_str(), NULL, 0, NULL) !=0 )
		{
		    Engine->Logger.AddEntry("Error: Could not load MPEG video " + Filename);
			Engine->Console.PrintLine("> Error: Could not load MPEG video " + Filename);
			return false;
		}

		//Retrieve stream information
		if(av_find_stream_info(FormatContext) < 0)
		{
		    Engine->Logger.AddEntry("Error: Could not retrieve stream info from " + Filename);
			return false;
		}

		//Dump file info
		dump_format(FormatContext, 0, Filename.c_str(), false);

		//Find the first video and audio streams
		VideoStream = -1;
		AudioStream = -1;
		for(int i = 0; i < (int)FormatContext->nb_streams; i++)
		{
			if(FormatContext->streams[i]->codec->codec_type == CODEC_TYPE_VIDEO && VideoStream < 0)
			{
				VideoStream = i;
			}

			if(FormatContext->streams[i]->codec->codec_type == CODEC_TYPE_AUDIO && AudioStream < 0)
			{
				AudioStream = i;
			}
		}

		//No video stream found
		if(VideoStream == -1)
		{
			Engine->Logger.AddEntry("Error: No video stream found in " + Filename);
			return false;
		}

		//No audio stream found
		if(AudioStream == -1)
		{
			Engine->Logger.AddEntry("Warning: No audio stream found in " + Filename);
			HasAudio = false;
		}
		else
		{
			//Get a pointer to the codec context for the audio stream
			AudioCodecContext = FormatContext->streams[AudioStream]->codec;

			//Find audio decoder
			AudioCodec = avcodec_find_decoder(AudioCodecContext->codec_id);
			if(AudioCodec == 0)
			{
				Engine->Logger.AddEntry("Error: Could not find audio codec for " + Filename);
				HasAudio = false;
			}
			else
			{
				// Open audio codec
				if(avcodec_open(AudioCodecContext, AudioCodec) < 0)
				{
					Engine->Logger.AddEntry("Error: Could not open audio codec for " + Filename);
					HasAudio = false;
				}
			}

			//Fill in audio format details for FMOD
			memset(&SoundInfo, 0, sizeof(SoundInfo));
			SoundInfo.cbsize = sizeof(SoundInfo);
			SoundInfo.decodebuffersize = AudioBufferSize;
			SoundInfo.defaultfrequency = AudioCodecContext->sample_rate;
			SoundInfo.format = FMOD_SOUND_FORMAT_PCM16;
			SoundInfo.numchannels = AudioCodecContext->channels;
			SoundInfo.length = AudioCodecContext->sample_rate * AudioCodecContext->channels * sizeof(uint8_t) * 5;
			SoundInfo.pcmreadcallback = GlobalPCMReadCallback;
			SoundInfo.userdata = (void*)this;

			FMOD_MODE Mode = FMOD_2D | FMOD_OPENUSER | FMOD_LOOP_NORMAL | FMOD_HARDWARE | FMOD_CREATESTREAM;
			FMOD_RESULT Result = Engine->Audio.GetFMODSystem()->createSound(0, Mode, &SoundInfo, &SoundStream);

			if(Result != FMOD_OK)
			{
				Engine->Logger.AddEntry("Error: Could not create audio stream for MPEG file " + Filename);
				HasAudio = false;
			}

			Result = Engine->Audio.GetFMODSystem()->playSound(FMOD_CHANNEL_FREE, SoundStream, false, &SoundChannel);

			if(Result != FMOD_OK)
			{
				Engine->Logger.AddEntry("Error: Could not start audio stream for MPEG file " + Filename);
				HasAudio = false;
			}
		}

		//Get a pointer to the codec context for the video stream
		VideoCodecContext = FormatContext->streams[VideoStream]->codec;
		

		//Open the sound stream
		//Engine->Audio.GetFMODSystem()->createStream(AudioBuffer, FMOD_OPENRAW | FMOD_2D | FMOD_LOOP_OFF, &SoundInfo, &SoundStream);

		//Find video decoder
		VideoCodec = avcodec_find_decoder(VideoCodecContext->codec_id);
		if(VideoCodec == 0)
		{
			Engine->Logger.AddEntry("Error: Could not find video codec for " + Filename);
			return false;
		}

		// Open video codec
		if(avcodec_open(VideoCodecContext, VideoCodec) < 0)
		{
			Engine->Logger.AddEntry("Error: Could not open video codec for " + Filename);
			return false;
		}

		//Allocate video frame
		Frame = avcodec_alloc_frame();

		//Allocate an AVFrame structure
		FrameRGB = avcodec_alloc_frame();
		if(FrameRGB == NULL)
		{
			Engine->Logger.AddEntry("Error: Could not allocate an RGB video frame for " + Filename);
			return false;
		}

		//Determine required buffer size and allocate buffer
		NumBytes = avpicture_get_size(PIX_FMT_RGB24, VideoCodecContext->width, VideoCodecContext->height);
		VideoBuffer = new uint8_t[NumBytes];

		//Assign appropriate parts of buffer to image planes in FrameRGB
		avpicture_fill((AVPicture *)FrameRGB, VideoBuffer, PIX_FMT_RGB24, VideoCodecContext->width, VideoCodecContext->height);

		//Get SWScale context
		SWScaleContext = sws_getContext(VideoCodecContext->width, VideoCodecContext->height, VideoCodecContext->pix_fmt, VideoCodecContext->width, VideoCodecContext->height, PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);

		//Get video dimentions
		Width = VideoCodecContext->width;
		Height = VideoCodecContext->height;

		//Get time between frames
		TimeBase = av_q2d(VideoCodecContext->time_base);

		//New timer
		FrameTimer = new CTimer(10, true, false);

		//Create new texture
		Texture = new CTexture;
		Texture->Create(Width, Height, GL_RGB8);

		//Set as loaded
		IsLoaded = true;

		return true;
	}

	void CMpeg::Play()
	{
		if(IsLoaded)
		{
			//Start timer
			FrameTimer->Start();

			if(HasAudio)
			{
				FMOD_RESULT Result = Engine->Audio.GetFMODSystem()->playSound(FMOD_CHANNEL_FREE, SoundStream, false, &SoundChannel);

				if(Result != FMOD_OK)
				{
					Engine->Logger.AddEntry("Error: Could not resume audio stream for MPEG");
					HasAudio = false;
				}
			}

			IsRunning = true;
		}
	}

	void CMpeg::Stop()
	{
		//Stop timer
		FrameTimer->Stop();

		if(HasAudio) SoundChannel->stop();

		IsRunning = false;
	}

	void CMpeg::SeekForward(int Time)
	{
		av_seek_frame(FormatContext, Packet.stream_index, Time, 0);
	}

	void CMpeg::SeekBackward(int Time)
	{
		av_seek_frame(FormatContext, Packet.stream_index, Time, AVSEEK_FLAG_BACKWARD);
	}

	bool CMpeg::IsPlaying()
	{
		return IsRunning;
	}

	bool CMpeg::ReadFrame()
	{
		//If the video is loaded, is running and the framerate timer has elapsed
		if(IsLoaded && IsRunning && FrameTimer->HasTriggered())
		{
			//Read the frame
			if (av_read_frame(FormatContext, &Packet) >= 0)
			{
				//Is this a packet from the video stream?
				if(Packet.stream_index == VideoStream)
				{
					//Decode video frame
					avcodec_decode_video(VideoCodecContext, Frame, &FrameFinished, Packet.data, Packet.size);

					//Did we get a video frame?
					if(FrameFinished)
					{
						//Convert the image from its native format to RGB
						sws_scale(SWScaleContext, Frame->data, Frame->linesize, 0, VideoCodecContext->height, FrameRGB->data, FrameRGB->linesize);

						//Bind texture
						glBindTexture(GL_TEXTURE_2D, Texture->Texture);

						//Copy image data to texture
						glTexSubImage2D(GL_TEXTURE_2D,
										0,
										0,
										0,
										VideoCodecContext->width,
										VideoCodecContext->height,
										GL_RGB,
										GL_UNSIGNED_BYTE,
										FrameRGB->data[0]);
					}

					av_free_packet(&Packet);
				}
				else if(Packet.stream_index == AudioStream && HasAudio)
				{
					AudioQueue.Add(&Packet);
				}
				else
				{
					// Free the packet that was allocated by av_read_frame
					av_free_packet(&Packet);
				}

				//FrameTimer->SetTime((TimeBase * Packet.pts) / 500.0f);
			}
			else
			{
				//Video has finished
				Stop();

				return false;
			}
		}

		return true;
	}

	void CMpeg::RenderFrame()
	{
		//Enable texture mapping
		glEnable(GL_TEXTURE_2D);

		//Bind the texture
		glBindTexture(GL_TEXTURE_2D, Texture->Texture);

		//Draw the frame
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0); glVertex2f(0.0f,					0.0f);
			glTexCoord2f(1, 0); glVertex2f((float)Engine->ScreenWidth, 0.0f);
			glTexCoord2f(1, 1); glVertex2f((float)Engine->ScreenWidth, (float)Engine->ScreenHeight);
			glTexCoord2f(0, 1); glVertex2f(0.0f,					(float)Engine->ScreenHeight);
        glEnd();

		//Disable texture mapping
		glDisable(GL_TEXTURE_2D);
	}

} //Namespace

#ifndef __SYM_ANIMATION
#define __SYM_ANIMATION

#ifdef SYM_DLL
#   define __declspec(dllexport)
#else
//#   define __declspec(dllimport)
#endif

#include <string>
#include <vector>
#include <map>

#include "SYM_Matrix.h"

#define ROOT_SECTION		0x0001
#define BONE_ANIM			0xD000 //No data, header to bones animation sequence
#define 	ANIM_BONE_ID	0xD001 //Int (Bone ID)
#define 	NUM_KEYS		0xD002 //Int (Num keys)
#define 	ANIM_KEYS		0xD003 //16x float (Matrix)

namespace sym
{
	class CSkeletalAnimation
	{
		public:
			//Read from file
			bool Load(std::string Filename);

			//Write to file
			int Write(std::string Filename);

			//Get key based on time (0.0f - 1.0f)
			CMatrix4 GetKey(int BoneID, float Time);
			
			//Map of keys (Indexed by 3ds Max Bone ID)
			std::map<int, std::vector<CMatrix4>> Keys;
	};
}

#endif
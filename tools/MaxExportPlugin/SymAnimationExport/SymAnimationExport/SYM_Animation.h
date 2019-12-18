#ifndef __SYM_ANIMATION
#define __SYM_ANIMATION

#include <string>
#include <vector>
#include <map>

#include "SYM_Matrix.h"

#define ROOT_SECTION		0x0001
#define BONE_ANIM			0xD000 //No data, header to bones animation sequence
#define 	ANIM_BONE_ID	0xD001 //Int (Bone ID)
#define 	NUM_KEYS		0xD002 //Int (Num keys)
#define 	ANIM_KEYS		0xD003 //16x float (4x4 Matrix)

namespace SYM
{
	class SYM_SKELETAL_ANIMATION
	{
		public:
			//Write to file
			int Write(std::string Filename);
			
			//Map of keys belonging to Bone ID's
			std::map<int, std::vector<SYM_MATRIX4>> Keys;
	};
}

#endif
#include "SYM_Animation.h"

namespace SYM
{
	int SYM_SKELETAL_ANIMATION::Write(std::string Filename)
	{
		//Current MAIN section ID and length
		unsigned short MainSection_ID = 0;
		unsigned int MainSection_Length = 0;

		//Current SUB section ID and length
		unsigned short SubSection_ID = 0;
		unsigned int SubSection_Length = 0;
		unsigned int SubSection_Start = 0;

		//Open file for writing
		FILE *File = fopen(Filename.c_str(), "wb");

		//Check for errors
		if (!File) return -1;

		//////////////////////////////////////////////////////////////////
		//ROOT_SECTION
		MainSection_ID = ROOT_SECTION;
		MainSection_Length = 6;

		//Write ROOT_SECTION header
		fwrite(&MainSection_ID, 2, 1, File);

		//Write root section length
		fwrite(&MainSection_Length, 4, 1, File);

		//Loop through Bone ID's
		for(std::map<int, std::vector<SYM::SYM_MATRIX4>>::iterator i = Keys.begin(); i != Keys.end(); i++)
		{
			MainSection_ID = BONE_ANIM;

			//Calculate BONE_ANIM section length
			///       Header 		(6)
			/// + 6 + BoneID		(4)
			/// + 6 + NumKeys		(4)
			/// + 6 + Keys			(64 x NumKeys)

			MainSection_Length = 	  24					//All headers
									+ 8					//All single integers
									+ (64 * i->second.size()); //Keys

			//Write header
			fwrite(&MainSection_ID, 2, 1, File);

			//Write length
			fwrite(&MainSection_Length, 4, 1, File);

			//////////////////////////////////////////////////////////////////
			//ANIM_BONE_ID
			SubSection_ID = ANIM_BONE_ID;
			SubSection_Length = 10;

			//Write header
			fwrite(&SubSection_ID, 2, 1, File);

			//Write length
			fwrite(&SubSection_Length, 4, 1, File);

			//Write Bone ID
			fwrite(&i->first, 4, 1, File);

			//////////////////////////////////////////////////////////////////
			//NUM_KEYS
			SubSection_ID = NUM_KEYS;
			SubSection_Length = 10;

			//Write header
			fwrite(&SubSection_ID, 2, 1, File);

			//Write length
			fwrite(&SubSection_Length, 4, 1, File);

			int Size = i->second.size();

			//Write num keys
			fwrite(&Size, 4, 1, File);

			//////////////////////////////////////////////////////////////////
			//ANIM_KEYS
			SubSection_ID = ANIM_KEYS;
			SubSection_Length = (64 * i->second.size()) + 6;

			//Write header
			fwrite(&SubSection_ID, 2, 1, File);

			//Write length
			fwrite(&SubSection_Length, 4, 1, File);

			//Loop through keys
			for (int k = 0; k < i->second.size(); k++)
			{
				for(int v = 0; v < 16; v++)
				{
					float Value = i->second[k]._Matrix[v];
					fwrite(&Value, 4, 1, File);
				}
			}
		}

		int Filesize = ftell(File);

		fclose(File);

		return Filesize;
	}
}
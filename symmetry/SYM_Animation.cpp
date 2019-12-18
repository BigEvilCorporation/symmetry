#include "SYM_Animation.h"
#include "SYM_Engine.h"

namespace sym
{
	CMatrix4 CSkeletalAnimation::GetKey(int BoneID, float Time)
	{
		CMatrix4 Key;

		//If no bones, return a blank keyframe
		if(Keys[BoneID].size() <= 0) return Key;

		//Check time is within rage 0.0f - 1.0f
		if(Time < 0.0f) Time = 0.0f;
		if(Time > 1.0f) Time = 1.0f;

		//Get exact frame point (0.0f - Keys.size)
		float Exact = Time * (Keys[BoneID].size() - 1);

		//Get nearest whole keyframe
		int Frame1 = (int)(Time * (Keys[BoneID].size() - 1));

		//Get next keyframe
		int Frame2 = Frame1 + 1;

		//If next keyframe is greater than Keys.size, use frame 0
		if(Frame2 > ((int)Keys[BoneID].size() - 1)) Frame2 = 0;

		//Get time distance between keyframe 1 and 2 (0.0f - 1.0f)
		float Factor = Exact - Frame1;

		//Return interpolated key
		Key = (Keys[BoneID][Frame1].GetInterpolatedMatrix(Keys[BoneID][Frame2], Factor));

		return Key;
	}

	bool CSkeletalAnimation::Load(std::string Filename)
	{
		//Used for file seeking
		long int FileSize = 0;

		//Current MAIN section ID and length
		unsigned short MainSection_ID = 0;
		unsigned int MainSection_Length = 0;

		//Current SUB section ID and length
		unsigned short SubSection_ID = 0;
		unsigned int SubSection_Length = 0;
		unsigned int SubSection_Start = 0;

		//Prefix with aimation path from engine config
		Filename = Engine->AnimationsPath + Filename;

		//Open file for reading
		FILE *File = fopen(Filename.c_str(), "rb");

		//Check if file exists
		if (!File)
		{
		    Engine->Logger.AddEntry("Error: Could not load animation " + Filename);
			Engine->Console.PrintLine("> Error: Could not load animation " + Filename);
			return false;
		}

		//Determine filesize (Seek to end, get current position, rewind to start)
		fseek(File, 0, SEEK_END);
		FileSize = ftell(File);
		rewind(File);

		//Loop through file until end
		while (ftell(File) < FileSize)
		{
			//Read the section header
			fread (&MainSection_ID, 2, 1, File);

			//Read the length of the section
			fread (&MainSection_Length, 4, 1, File);

			//Determine section type
			switch (MainSection_ID)
			{
				case ROOT_SECTION:
					break;

				case BONE_ANIM:
				{
					//Store where the subsection was found, so we can return later
					SubSection_Start = ftell(File) - 6;

					//Zero the subsection ID and length, in case they were used previously
					SubSection_ID = 0;
					SubSection_Length = 0;

					//Current bone ID
					int BoneID = 0;

					//Number of keys
					int NumKeys = 0;

					//Loop from subsection Start to subsection End (Mainsection length, + the position it was found)
					while ((unsigned)ftell(File) < (MainSection_Length + SubSection_Start))
					{
						//Read the subchunk ID
						fread (&SubSection_ID, 2, 1, File);

						//Read the length of the subchunk
						fread (&SubSection_Length, 4, 1, File);

						//Determine SubChunk type using its ID
						switch (SubSection_ID)
						{
							case ANIM_BONE_ID:
							{
								fread (&BoneID, 4, 1, File);

								//Initialise map
								std::vector<CMatrix4> BlankVector;
								Keys[BoneID] = BlankVector;

								break;
							}

							case NUM_KEYS:
								fread (&NumKeys, 4, 1, File);
								break;

							case ANIM_KEYS:
							{
								//For all keys
								for (int i = 0; i < NumKeys; i++)
								{
									//Create temp key
									CMatrix4 TempKey;

									for(int v = 0; v < 16; v++)
									{
										//Read value
										float Value;
										fread (&Value, 4, 1, File);
										TempKey._Matrix[v] = Value;
									}

									//Push back to vector
									Keys[BoneID].push_back(TempKey);
								}

								break;
							}

							default:
									//Advance to next subchunk (Current Position, + SubChunk length, - the header (6))
									fseek(File, SubSection_Length - 6, SEEK_CUR);

								break;
						}
					}

					break;
				}

				default:
					//Go to next section (Advance to the end of this section, minus 6)
					fseek(File, MainSection_Length - 6, SEEK_CUR);

					break;
			}
		}

		//Close the file
		fclose(File);

		return true;
	}
}
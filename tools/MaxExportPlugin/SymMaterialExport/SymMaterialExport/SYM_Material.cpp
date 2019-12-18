#include "SYM_Material.h"

namespace SYM
{
	SYM_COLOUR::SYM_COLOUR()
	{
		//Initialise all values
		r = g = b = a = 255;
	}

	SYM_COLOUR::SYM_COLOUR(int R, int G, int B)
	{
		r = R;
		g = G;
		b = B;
		a = 255;
	}

	SYM_MATERIAL::SYM_MATERIAL()
	{
		//Initialise all values
		Shininess = 0;
		Transparency = 0;
		UTile = VTile = 1.0f;
		UOffset = VOffset = 1.0f;
	}

	int SYM_MATERIAL::Write(std::string Filename)
	{
		//Current MAIN section ID and length
		unsigned short MainSection_ID = 0;
		unsigned int MainSection_Length = 0;

		//Current SUB section ID and length
		unsigned short SubSection_ID = 0;
		unsigned int SubSection_Length = 0;
		unsigned int SubSection_Start = 0;

		//Current CHILD section ID and length
		unsigned short ChildSection_ID = 0;
		unsigned int ChildSection_Length = 0;
		unsigned int ChildSection_Start = 0;

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

		MainSection_ID = MATERIAL;

		//Append '\0' to names
		Name += '\0';
		TextureFile += '\0';

		//Calculate MATERIAL section length
		///       Header 		(6)
		/// + 6 + Name (char)		(Chars in name)
		/// + 6 + MaterialID (int)	(4)
		/// + 6 + DiffuseColour (3xint)	(12)
		/// + 6 + AmbientColour (3xint)	(12)
		/// + 6 + SpecularColour (3xint)(12)
		/// + 6 + Shininess (int)	(4)
		/// + 6 + Transparency (int) 	(4)

		//Calculate TEXTURE section length
		///	+ Header		(6)
		/// + 6 + Filename (char) 	(Chars in name)
		/// + 6 + UTile (float)		(sizeof(float))
		/// + 6 + VTile (float)		(sizeof(float))
		/// + 6 + UOffset (float)	(sizeof(float))
		/// + 6 + VOffset (float)	(sizeof(float))


		MainSection_Length = 	  72					//All headers
								+ 48					//All single integers
								+ Name.size()		//Name
								+ TextureFile.size()	//Texture
								+ (4 * (int)sizeof(float));			//All floats

		//Write header
		fwrite(&MainSection_ID, 2, 1, File);

		//Write length
		fwrite(&MainSection_Length, 4, 1, File);

			//////////////////////////////////////////////////////////////
			//MAT_NAME
			SubSection_ID = MAT_NAME;

			SubSection_Length = (int)Name.size() + 6;

			//Write header
			fwrite(&SubSection_ID, 2, 1, File);

			//Write length
			fwrite(&SubSection_Length, 4, 1, File);

			//Write object name
			fwrite(Name.c_str(), 1, Name.size(), File);

			//////////////////////////////////////////////////////////////////
			//DIFFUSE
			SubSection_ID = DIFFUSE;
			SubSection_Length = 18;

			//Write header
			fwrite(&SubSection_ID, 2, 1, File);

			//Write length
			fwrite(&SubSection_Length, 4, 1, File);

			//Write R
			fwrite(&Diffuse.r, 4, 1, File);

			//Write G
			fwrite(&Diffuse.g, 4, 1, File);

			//Write B
			fwrite(&Diffuse.b, 4, 1, File);

			//////////////////////////////////////////////////////////////////
			//AMBIENT
			SubSection_ID = AMBIENT;
			SubSection_Length = 18;

			//Write header
			fwrite(&SubSection_ID, 2, 1, File);

			//Write length
			fwrite(&SubSection_Length, 4, 1, File);

			//Write R
			fwrite(&Ambient.r, 4, 1, File);

			//Write G
			fwrite(&Ambient.g, 4, 1, File);

			//Write B
			fwrite(&Ambient.b, 4, 1, File);

			//////////////////////////////////////////////////////////////////
			//SPECULAR
			SubSection_ID = SPECULAR;
			SubSection_Length = 18;

			//Write header
			fwrite(&SubSection_ID, 2, 1, File);

			//Write length
			fwrite(&SubSection_Length, 4, 1, File);

			//Write R
			fwrite(&Specular.r, 4, 1, File);

			//Write G
			fwrite(&Specular.g, 4, 1, File);

			//Write B
			fwrite(&Specular.b, 4, 1, File);

			//////////////////////////////////////////////////////////////////
			//SHININESS
			SubSection_ID = SHININESS;
			SubSection_Length = 10;

			//Write header
			fwrite(&SubSection_ID, 2, 1, File);

			//Write length
			fwrite(&SubSection_Length, 4, 1, File);

			//Write Shininess
			fwrite(&Shininess, 4, 1, File);

			//////////////////////////////////////////////////////////////////
			//TRANSPARENCY
			SubSection_ID = TRANSPARENCY;
			SubSection_Length = 10;

			//Write header
			fwrite(&SubSection_ID, 2, 1, File);

			//Write length
			fwrite(&SubSection_Length, 4, 1, File);

			//Write Transparency
			fwrite(&Transparency, 4, 1, File);

			//////////////////////////////////////////////////////////////
			//TEXTURE

			SubSection_ID = TEXTURE;

			//Calculate TEXTURE section length
			///	+ Header		(6)
			/// + 6 + Filename (char) 	(Chars in name)
			/// + 6 + UTile (float)		(sizeof(float))
			/// + 6 + VTile (float)		(sizeof(float))
			/// + 6 + UOffset (float)	(sizeof(float))
			/// + 6 + VOffset (float)	(sizeof(float))

			SubSection_Length =   36					    //All headers
                                + TextureFile.size()	    //Texture
                                + (4 * (int)sizeof(float));	//All floats

			//Write header
			fwrite(&SubSection_ID, 2, 1, File);

			//Write length
			fwrite(&SubSection_Length, 4, 1, File);

				//////////////////////////////////////////////////////////////
				//FILENAME

				ChildSection_ID = FILENAME;

				ChildSection_Length = (int)TextureFile.size() + 6;

				//Write header
				fwrite(&ChildSection_ID, 2, 1, File);

				//Write length
				fwrite(&ChildSection_Length, 4, 1, File);

				//Write object name
				fwrite(TextureFile.c_str(), 1, TextureFile.size(), File);

				//////////////////////////////////////////////////////////////////
				//UTILE
				ChildSection_ID = UTILE;
				ChildSection_Length = 10;

				//Write header
				fwrite(&ChildSection_ID, 2, 1, File);

				//Write length
				fwrite(&ChildSection_Length, 4, 1, File);

				//Write Transparency
				fwrite(&UTile, sizeof(float), 1, File);

				//////////////////////////////////////////////////////////////////
				//VTILE
				ChildSection_ID = VTILE;
				ChildSection_Length = 10;

				//Write header
				fwrite(&ChildSection_ID, 2, 1, File);

				//Write length
				fwrite(&ChildSection_Length, 4, 1, File);

				//Write Transparency
				fwrite(&VTile, sizeof(float), 1, File);

				//////////////////////////////////////////////////////////////////
				//UOFFSET
				ChildSection_ID = UOFFSET;
				ChildSection_Length = 10;

				//Write header
				fwrite(&ChildSection_ID, 2, 1, File);

				//Write length
				fwrite(&ChildSection_Length, 4, 1, File);

				//Write Transparency
				fwrite(&UOffset, sizeof(float), 1, File);

				//////////////////////////////////////////////////////////////////
				//UOFFSET
				ChildSection_ID = VOFFSET;
				ChildSection_Length = 10;

				//Write header
				fwrite(&ChildSection_ID, 2, 1, File);

				//Write length
				fwrite(&ChildSection_Length, 4, 1, File);

				//Write Transparency
				fwrite(&VOffset, sizeof(float), 1, File);

		int FileSize = ftell(File);

		//Close the file
		fclose(File);

		return FileSize;
	}

} //Namespace

#ifndef __SYM_MATERIAL
#define __SYM_MATERIAL

#include <string>

#define ROOT_SECTION		0x0001
#define MATERIAL			0xC000 //No data, header to a material
#define 	MAT_NAME		0xC001 //'\0' terminated CHAR string, name of material
#define 	MAT_ID			0xC002 //UNSIGNED SHORT INT, ID of material
#define 	DIFFUSE			0xC003 //3x UNSIGNED SHORT INT, colour R, G and B (0 - 255)
#define 	AMBIENT			0xC013 //3x UNSIGNED SHORT INT, colour R, G and B (0 - 255)
#define 	SPECULAR		0xC023 //3x UNSIGNED SHORT INT, colour R, G and B (0 - 255)
#define 	SHININESS		0xC004 //UNSIGNED SHORT INT, percentage of shininess (0 - 100)
#define 	TRANSPARENCY	0xC005 //UNSIGNED SHORT INT, percentage of transparency (0 - 100)
#define 	TEXTURE			0xC006 //No data, header to texture
#define 		FILENAME	0xC100 //'\0' terminated CHAR string, file name of texture image
#define 		UTILE		0xC101 //FLOAT, U texture tiling
#define 		VTILE		0xC102 //FLOAT, V texture tiling
#define 		UOFFSET		0xC103 //FLOAT, U texture offset
#define 		VOFFSET		0xC104 //FLOAT, V texture offset
#define		SHADER			0xC007 //'\0' terminated CHAR string, file name of shader
//TODO:		Shader parameters

namespace SYM
{
	class SYM_COLOUR
	{
		public:
			//Constructors
			SYM_COLOUR();
			SYM_COLOUR(int R, int G, int B);

			//Red, green, blue, alpha  (0 - 255)
			unsigned int r, g, b, a;
	};

	class SYM_MATERIAL
	{
		public:
			SYM_MATERIAL();

            //Write to .sym.material file
			int Write(std::string Filename);

			//Name
			std::string Name;

			//Texture filename
			std::string TextureFile;

			//Colours
			SYM_COLOUR Ambient;
			SYM_COLOUR Diffuse;
			SYM_COLOUR Specular;

			//Percent of shininess
			int Shininess;

			//Percent of transparency
			int Transparency;

			//Texture tiling
			float UTile, VTile;

			//Texture offset
			float UOffset, VOffset;
	};

} //Namespace

#endif

#ifndef __SYM_MATERIAL
#define __SYM_MATERIAL

#ifdef SYM_DLL
#   define __declspec(dllexport)
#else
//#   define __declspec(dllimport)
#endif

#include "SYM_Shader.h"
#include "SYM_Texture.h"
#include "SYM_MPEG.h"

#define MATERIAL				0xC000 //No data, header to a material
#define 	MAT_NAME			0xC001 //'\0' terminated CHAR string, name of material
#define 	MAT_ID				0xC002 //UNSIGNED SHORT INT, ID of material
#define 	DIFFUSE				0xC003 //3x UNSIGNED SHORT INT, colour R, G and B (0 - 255)
#define 	AMBIENT				0xC013 //3x UNSIGNED SHORT INT, colour R, G and B (0 - 255)
#define 	SPECULAR			0xC023 //3x UNSIGNED SHORT INT, colour R, G and B (0 - 255)
#define 	SHININESS			0xC004 //UNSIGNED SHORT INT, percentage of shininess (0 - 100)
#define 	TRANSPARENCY		0xC005 //UNSIGNED SHORT INT, percentage of transparency (0 - 100)

#define 	TEXTURE				0xC006 //No data, header to texture
#define 		FILENAME		0xC100 //'\0' terminated CHAR string, file name of texture image
#define 		UTILE			0xC101 //FLOAT, U texture tiling
#define 		VTILE			0xC102 //FLOAT, V texture tiling
#define 		UOFFSET			0xC103 //FLOAT, U texture offset
#define 		VOFFSET			0xC104 //FLOAT, V texture offset

#define		SHADER				0xC007 //No data, header to a shader
#define			CGFX_FILENAME	0xC207 //'\0' terminated CHAR string, file name of nVidia CGFX file
#define			PARAM_FLOAT		0xC208 //Variable name + float
#define			PARAM_FLOAT3	0xC209 //Variable name + 3x float
#define			PARAM_FLOAT4	0xC210 //Variable name + 4x float
#define			PARAM_SAMPLER	0xC211 //Variable name + texture filename

namespace sym
{
	class CColour
	{
		public:
			//Constructors
			CColour();
			CColour(int R, int G, int B);
			CColour(int R, int G, int B, int A);

			//Red, green, blue, alpha  (0 - 255)
			unsigned int r, g, b, a;

			//RGBA array
			GLint rgba[4];
	};

	class CMaterial
	{
		public:
			//Constructor
			CMaterial();
			~CMaterial();

			//Load function, to keep compliance with resource manager
			bool Load(std::string Filename);

            //Write to .sym.material file
            bool Write(std::string Filename);

			//Apply material
			void Apply();

			//Load texture and shader
			void Process();

			//Load texture
			bool LoadTexture(std::string Filename);

			//load video
			bool LoadVideo(std::string Filename);

			//Load shader
			bool LoadShader(std::string Filename);
			bool SetShader(CShader *Shad);

			//Name
			std::string Name;

			//Texture filename
			std::string TextureFile;

			//Set colours
			void SetAmbient(int r, int g, int b);
			void SetDiffuse(int r, int g, int b);
			void SetSpecular(int r, int g, int b);

			//Set shininess and transparency
			void SetShininess(int s);
			void SetTransparency(float t);

			//Set UV tiling/offset
			//void SetUVTiling(float u, float v);
			//void SetUVOffset(float u, float v);

			//Colours
			CColour Ambient;
			CColour Diffuse;
			CColour Specular;

			//Percent of shininess
			int Shininess;

			//Transparency
			float Transparency;

			//Texture tiling
			float UTile, VTile;

			//Texture offset
			float UOffset, VOffset;

			//Texture
			CTexture *Texture;

			//Video
			CMpeg *Video;

			//Shader
			CShader *Shader;
	};

} //Namespace

#endif

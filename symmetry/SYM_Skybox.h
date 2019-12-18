#ifndef __SYM_SKYBOX
#define __SYM_SKYBOX

#ifdef SYM_DLL
#   define __declspec(dllexport)
#else
//#   define __declspec(dllimport)
#endif

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <map>

#include "SYM_Texture.h"
#include "SYM_Level.h"

namespace sym
{
	const int SKY_FACE_NORTH	= 0;
	const int SKY_FACE_SOUTH	= 1;
	const int SKY_FACE_WEST		= 2;
	const int SKY_FACE_EAST		= 3;
	const int SKY_FACE_UP		= 4;
	const int SKY_FACE_DOWN		= 5;

	class CSkybox
	{
		public:
			CSkybox();

			//Load six textures
			bool LoadSkyTextures(std::string Front, std::string Back, std::string Left, std::string Right, std::string Up, std::string Down);
			bool LoadSkyTextures(std::map<int, std::string> Textures);

			bool LoadSkyTexture(int Face, std::string Filename);
			std::string GetTextureFilename(int Face);

			//Render skybox
			void Render();
				
			//Dimentions
			float xpos, ypos, zpos, skywidth, skyheight, skylength;
			
			//Six sky textures
			CTexture *SkyTexture[6];

			//Skybox texture names
			std::map<int, std::string> TextureNames;

			std::string TexNames[6];

			//Is loaded
			bool IsLoaded;

			//Texture offset for removing seam
			float Offset;
	};

} //Namespace
#endif

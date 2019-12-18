#ifndef __SYM_TEXTURE
#define __SYM_TEXTURE

#ifdef SYM_DLL
#   define __declspec(dllexport)
#else
//#   define __declspec(dllimport)
#endif

#ifdef WIN32
#include <windows.h>
#endif

#include <stdlib.h>
#include <stdio.h>

#include <GL/gl.h>	// Header File For The OpenGL32 Library
#include <GL/glu.h>	// Header File For The GLu32 Library
#include <GL/glext.h>

#include <SDL/SDL.h> // main SDL header
#include <SDL/SDL_image.h>

#include <vector>
#include <string>

namespace sym
{
	class CTexture
	{
		public:
			//Constructor
			CTexture();
			~CTexture();

			//Load the texture
			bool Load(std::string File);

			//Create blank texture
			void Create(int Width, int Height, int Mode);

			//Create texture from SDL surface
			void CreateFromSDLSurface(SDL_Surface *Surface);

			//The texture
			GLuint Texture;

			//Texture mode
			int Mode;

			//Check if texture contains alpha channel
			bool HasAlphaChannel();

			//Dimentions
			int Width, Height;
	};

} //Namespace


#endif

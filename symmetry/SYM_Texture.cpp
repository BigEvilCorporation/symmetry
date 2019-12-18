#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>

#include "SYM_Texture.h"
#include "SYM_Engine.h"

namespace sym
{
	CTexture::CTexture()
	{
		Texture = 0;
	}

	CTexture::~CTexture()
	{
		//glDeleteTextures(1, &Texture);
	}

	bool CTexture::Load(std::string Filename)
	{
		//Prefix filename with texture path from engine config
		Filename = Engine->TexturesPath + Filename;

		//New SDL surface
		SDL_Surface *Surface;

		//Open the file
		FILE *File = fopen(Filename.c_str(), "r");

		//Check if file exists
		if(!File)
		{
		    Engine->Logger.AddEntry("Error: Could not load texture " + Filename);
			Engine->Console.PrintLine("> Error: Could not load texture " + Filename);
			return false;
		}

		//Close the file
		fclose(File);

		//Load the image
		Surface = IMG_Load(Filename.c_str());

		//Check if image data loaded ok
		if(Surface == 0) return false;

		//Get dimentions
		Width = Surface->w;
		Height = Surface->h;

		// Check that the image's width is a power of 2
		if ( (Surface->w & (Surface->w - 1)) != 0 )
		{
			Engine->Logger.AddEntry("Warning: Non power-of-two texture loaded: " + Filename);
		}
		// Also check if the height is a power of 2
		else if ( (Surface->h & (Surface->h - 1)) != 0 )
		{
			Engine->Logger.AddEntry("Warning: Non power-of-two texture loaded: " + Filename);
		}

        //Check colour format
		if (Surface->format->BytesPerPixel == 4)
        {
            if (Surface->format->Rmask == 0x000000ff) Mode = GL_RGBA;
            else Mode = GL_BGRA;
        }
        else if (Surface->format->BytesPerPixel == 3)
        {
            if (Surface->format->Rmask == 0x000000ff) Mode = GL_RGB;
            else Mode = GL_BGR;
        }
        else
        {
            //Unsupported type
            Engine->Logger.AddEntry("Error: Unsupported texture colour format: " + Filename);
			SDL_FreeSurface(Surface);
			return false;
        }

		//Generate texure
		glGenTextures(1, &Texture);

		//Bind the texture
		glBindTexture(GL_TEXTURE_2D, Texture);

		//Texture filters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); 

		//Copy all pixels in SDL surface to GL texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, Surface->w, Surface->h, 0, Mode, GL_UNSIGNED_BYTE, Surface->pixels);

		//Free SDL surface
		SDL_FreeSurface(Surface);

		return true;
	}

	void CTexture::Create(int Width, int Height, int Mode)
	{
		//Generate texture
		glGenTextures(1, &Texture);

		//Bind texture
		glBindTexture(GL_TEXTURE_2D, Texture);

		//Set filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		//Create
		glTexImage2D(GL_TEXTURE_2D, 0, Mode, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}

	void CTexture::CreateFromSDLSurface(SDL_Surface *Surface)
	{
		int Width, Height;
		SDL_Surface *Image;
		SDL_Rect Area;

		//Get the dimentions of the surface
		Area.x = 0;
		Area.y = 0;
		Area.w = Surface->w;
		Area.h = Surface->h;

		//Make sure the width and height are powers of two
		Width = Maths::NextPowerOfTwo(Surface->w);
		Height = Maths::NextPowerOfTwo(Surface->h);

		//Create an OpenGL compliant surface
		Image = SDL_CreateRGBSurface(SDL_SWSURFACE, Width, Height, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

		//Copy the surface to the OpenGL compatible one
		SDL_BlitSurface(Surface, &Area, Image, &Area);

		glEnable(GL_TEXTURE_2D);

		//Generate an OpenGL texture
		glGenTextures(1, &Texture);

		//Bind the texture ready for use
		glBindTexture(GL_TEXTURE_2D, Texture);

		//Set texture filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); 

		//Copy data from the SDL surface to the OpenGL texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB_S3TC_DXT1_EXT, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Image->pixels);

		glDisable(GL_TEXTURE_2D);

		//Free the SDL surface
		SDL_FreeSurface(Image);
	}

	bool CTexture::HasAlphaChannel()
	{
		return Mode == GL_BGRA;
	}

} //Namespace

#ifndef __SYM_FONT
#define __SYM_FONT

#ifdef SYM_DLL
#   define __declspec(dllexport)
#else
//#   define __declspec(dllimport)
#endif

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#include <string>
#include <vector>
#include <map>

#include "SYM_Maths.h"
#include "SYM_Texture.h"
#include "SYM_Mesh.h"

namespace sym
{
	class CFontGlyph
	{
		public:
			//Glyph attributes
			int MinX, MaxX, MinY, MaxY, Advance;

			//Texture coordinates
			CTexCoord TexCoords[2];
	};

	class CFont
	{
		public:

			//Constructor
			CFont();

			//Load TTF font and process into a glyph sheet
			bool LoadTTFFont(std::string TTF_Filename, int Size);

			//Render text
			void RenderText(std::string Text, float x, float y);

			//Calculates width of text
			float GetTextWidth(std::string Text);

			//Set text colour
			void SetColour(int r, int g, int b);

		private:
			//Vector of glyphs
			std::vector<CFontGlyph> Glyphs;

			//Temporary glyph
			CFontGlyph *TempGlyph;

			//The texture sheet containing the glyphs
			CTexture GlyphSheet;

			//Temporary SDL surface for loading glyphs
			SDL_Surface *TempGlyphSurface;

			//Temporary SDL surface for the glyph sheet
			SDL_Surface *TempGlyphSheet;

			//TTF font
			TTF_Font* Font;

			//Font attributes
			int Height, Ascent, Descent, LineSkip;

			//Colour
			int ColourR, ColourG, ColourB;

			//Current position on the glyph sheet
			SDL_Rect CurrentPos;

			//Size of current glyph
			SDL_Rect GlyphRect;
	};

	class CFontManager
	{
		public:
			//Load font
			bool LoadFont(std::string Name, std::string Filename, int Size);

			//Render text
			void RenderText(std::string Name, std::string Text, float X, float Y);

			//Set colour
			void SetColour(std::string Name, int R, int G, int B);

			//Remove font
			void RemoveFont(std::string Name);

		private:
			//Mapped container of fonts
			std::map<std::string, CFont> Fonts;

			//Temp CFont for loading
			CFont *TempFont;
	};


} //Namespace

#endif


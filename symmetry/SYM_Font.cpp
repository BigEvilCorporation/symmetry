#include <algorithm>

#include "SYM_Font.h"
#include "SYM_Engine.h"

namespace sym
{
	CFont::CFont()
	{
	    Font = 0;

		//Set default colour
		ColourR = 255;
		ColourG = 255;
		ColourB = 255;

		GlyphRect.x = 0;
		GlyphRect.y = 0;
		GlyphRect.w = 0;
		GlyphRect.h = 0;

		CurrentPos.x = 0;
		CurrentPos.y = 0;
		CurrentPos.w = 0;
		CurrentPos.h = 0;
	}

	bool CFont::LoadTTFFont(std::string TTF_Filename, int Size)
	{
		//Convert filename to upper case
		//transform(TTF_Filename.begin(), TTF_Filename.end(), TTF_Filename.begin(), toupper);

        //Prefix font name with path from engine config
		TTF_Filename = Engine->FontsPath + TTF_Filename;

		//SDL_TTF segfaults if file does not exist, check for file explicitly
		FILE *CheckFile = fopen(TTF_Filename.c_str(), "rb");
		if(!CheckFile)
		{
		    Engine->Logger.AddEntry("Error: Font file does not exist: " + TTF_Filename);
		    return false;
		}
		fclose(CheckFile);

		//Load the font
		Font = TTF_OpenFont(TTF_Filename.c_str(), Size);

		//Check font loaded
        if (!Font)
        {
            Engine->Logger.AddEntry("Error: Could not load font " + TTF_Filename);

			//Return
			return false;
        }

		//Get font attributes
		Ascent = TTF_FontAscent(Font);
        Descent = TTF_FontDescent(Font);
        Height = TTF_FontHeight(Font);
        LineSkip = TTF_FontLineSkip(Font);

		//TODO: Pre-calculate the size of the glyph sheet
		TempGlyphSheet = SDL_CreateRGBSurface(SDL_SWSURFACE, 1024, 1024, 32, 0, 0, 0, 0);

		//Loop through all chars
		for (int i = 0; i <= 254; i++)
        {
			//Clear the surface
			TempGlyphSurface = NULL;

			//Render the glyph onto the SDL surface, in white
			SDL_Color GlyphColour = {255, 255, 255, 0};
			TempGlyphSurface = TTF_RenderGlyph_Blended(Font, i, GlyphColour);

			//New temp glyph
			TempGlyph = new CFontGlyph;

			//Get the glyph attributes
			TTF_GlyphMetrics(Font, i, &TempGlyph->MinX, &TempGlyph->MaxX, &TempGlyph->MinY, &TempGlyph->MaxY, &TempGlyph->Advance);

			//Set size of glyph rect
			GlyphRect.w = TempGlyphSurface->w;
			GlyphRect.h = TempGlyphSurface->h;

			//Set size of current position rect
			CurrentPos.w = CurrentPos.x + TempGlyph->MaxX;
			CurrentPos.h = CurrentPos.y +  TempGlyph->MaxY;

			//Blit the glyph onto the glyph sheet
			SDL_BlitSurface(TempGlyphSurface, &GlyphRect, TempGlyphSheet, &CurrentPos);

			//Set texture coordinates for the glyph
			TempGlyph->TexCoords[0].u = (float)CurrentPos.x / 1024;
			TempGlyph->TexCoords[0].v = 1 - ((float)CurrentPos.y / 1024);

			TempGlyph->TexCoords[1].u = (float)(CurrentPos.x + CurrentPos.w) / 1024;
			TempGlyph->TexCoords[1].v = 1 - ((float)(CurrentPos.y + CurrentPos.h) / 1024);

			//Position xpos ready for next glyph
			CurrentPos.x += GlyphRect.w;

			//If the next character will run off the edge of the glyph sheet, advance to next row
			if (CurrentPos.x + CurrentPos.w > 1024)
			{
				CurrentPos.x = 0;
			 	CurrentPos.y += Height;
			}

			//Push back to glyphs vector
			Glyphs.push_back(*TempGlyph);

			//Delete temp glyph
			delete TempGlyph;

			//Free surfaces
			SDL_FreeSurface(TempGlyphSurface);
 		}

		//Close the font
		TTF_CloseFont(Font);

		//Convert the SDL glyph sheet to an OpenGL compliant texture
		GlyphSheet.CreateFromSDLSurface(TempGlyphSheet);

		//Free the temp glyph sheet surface
		SDL_FreeSurface(TempGlyphSheet);

		return true;
	}

	void CFont::RenderText(std::string Text, float x, float y)
	{
		int MinX, MaxX, MinY, MaxY, CharID;

		//Enable texture mapping
		glEnable(GL_TEXTURE_2D);

		//Bind the glyph sheet texture
		glBindTexture(GL_TEXTURE_2D, GlyphSheet.Texture);

		//Enable blending
		glEnable(GL_BLEND);

		//Set blending mode
		glBlendFunc(GL_ONE, GL_ONE);

		//Loop through characters
		for (unsigned int i = 0; i < Text.size(); i++)
		{
			//Get ID of current char
			CharID = (int)Text[i];

			//Get dimentions of current char
			MinX = Glyphs[CharID].MinX;
			MaxX = Glyphs[CharID].MaxX;
			MinY = Glyphs[CharID].MinY;
			MaxY = Glyphs[CharID].MaxY;

			//Set text blend colour
			glColor3ub(ColourR, ColourG, ColourB);

			//Draw the glyph
			glBegin(GL_QUADS);
				glTexCoord2f(Glyphs[CharID].TexCoords[0].u, Glyphs[CharID].TexCoords[1].v); glVertex2f(x + MinX, y + MinY);
				glTexCoord2f(Glyphs[CharID].TexCoords[1].u, Glyphs[CharID].TexCoords[1].v); glVertex2f(x + MaxX, y + MinY);
				glTexCoord2f(Glyphs[CharID].TexCoords[1].u, Glyphs[CharID].TexCoords[0].v); glVertex2f(x + MaxX, y + MaxY);
				glTexCoord2f(Glyphs[CharID].TexCoords[0].u, Glyphs[CharID].TexCoords[0].v); glVertex2f(x + MinX, y + MaxY);
            glEnd();

			//Position x ready for next glyph
			x += Glyphs[CharID].Advance;
		}

        //Set colour to white
        glColor3ub(255, 255, 255);

		//Disable blending
		glDisable(GL_BLEND);

		//Disable texture mapping
		glDisable(GL_TEXTURE_2D);
	}

	void CFont::SetColour(int r, int g, int b)
	{
		ColourR = r;
		ColourG = g;
		ColourB = b;
	}

	float CFont::GetTextWidth(std::string Text)
	{
		int Width = 0;
		int CharID;

		for (unsigned int i = 0; i < Text.size(); i++)
		{
			CharID = (int)Text[i];
			Width += Glyphs[CharID].Advance;
		}

		return (float)Width;
	}

	bool CFontManager::LoadFont(std::string Name, std::string Filename, int Size)
	{
		//New temp CFont
		TempFont = new CFont;

		//Load font
		if (!TempFont->LoadTTFFont(Filename, Size))
		{
			Engine->Console.PrintLine("> ERROR: Font not found");
			delete TempFont;
			return false;
		}

		//Push back to the map
		Fonts.insert(std::pair<std::string, CFont>(Name, *TempFont));

		//Delete the temp font
		delete TempFont;

		return true;
	}

	void CFontManager::RenderText(std::string Name, std::string Text, float X, float Y)
	{
		Fonts[Name].RenderText(Text, X, Y);
	}

	void CFontManager::SetColour(std::string Name, int R, int G, int B)
	{
		Fonts[Name].SetColour(R, G, B);
	}

} //Namespace


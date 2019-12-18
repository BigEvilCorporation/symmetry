#include <iomanip>

#include "SYM_Console.h"
#include "SYM_Engine.h"
#include "SYM_SDL_GL_Utils.h"
#include "SYM_Texture.h"
#include "SYM_Timer.h"

namespace sym
{
	CConsole::CConsole()
	{
		//Setup some default values
		Prompt = " > ";

		Height = 200;
		YPos = 768;
		MaxLines = 10;
		FontSize = 14;
		FontName = "cumberland_b.ttf";
		IsOpen = false;
		CurrentHistoryID = 0;
		Texture = 0;
	}

	CConsole::~CConsole()
	{

	}

	bool CConsole::Init()
	{
		//Set YPos to screen height
		YPos = (float)Engine->ScreenHeight / 3;

		//Load the font
		if (!Font.LoadTTFFont(FontName, FontSize))
		{
			Engine->Logger.AddEntry("Error: Could not load console font " + FontName);
			return false;
		}

		return true;
	}

	bool CConsole::SetBackgroundTexture(std::string Filename)
	{
		Texture = Engine->ResourceManager.Textures.Get(Filename);

		if(!Texture)
		{
		    Engine->Logger.AddEntry("Error: Could not load console background texture");
			PrintLine("> Error: Could not load console background texture");
			return false;
		}

		return true;
	}

	void CConsole::Open()
	{
		//Set ypos to screen height - console height (bottom of screen)
		YPos = (float)Engine->ScreenHeight / 3;

		//Enable keyboard repetition
		SDL_EnableKeyRepeat(500, 50);

		//Set as open
		IsOpen = true;
	}

	void CConsole::Close()
	{
		//Set ypos as screen height (out of view)
		YPos = (float)Engine->ScreenHeight;

		//Disable keyboard repetition
		SDL_EnableKeyRepeat(0, 0);

		//Set as closed
		IsOpen = false;
	}

	void CConsole::Toggle()
	{
		if (IsOpen == true)
		{
			Close();
		}
		else
		{
			Open();
		}
	}

	void CConsole::KeyboardInput(int KeySym, int Unicode)
	{
		//If key is ENTER/RETURN
		if(KeySym == SDLK_RETURN)
		{
			//Process current line
			ProcessCurrentLine();
		}

		//If key is BACKSPACE
		else if(KeySym == SDLK_BACKSPACE)
		{
			//Remove last key, is line is not already empty
			if((CurrentLine.size() > 0)) CurrentLine.erase(CurrentLine.end() - 1, CurrentLine.end());
		}

		//If key is UP
		else if(KeySym == SDLK_UP)
		{
			//If history is not empty
			if(History.size() > 0)
			{
				//Advance current history entry
				CurrentHistoryID++;

				//Check it doesn't exceed maximum
				if(CurrentHistoryID > (int)History.size()) CurrentHistoryID = (int)History.size();

				//Retrieve the current history entry
				CurrentLine = History[History.size() - CurrentHistoryID];
			}
		}

		//If key is DOWN
		else if(KeySym == SDLK_DOWN)
		{
			//If history is not empty
			if(History.size() > 0)
			{
				//Decrease current history entry
				CurrentHistoryID--;

				//Check it doesn't drop below 1
				if(CurrentHistoryID < 1) CurrentHistoryID = 1;

				//Retrieve the current history entry
				CurrentLine = History[History.size() - CurrentHistoryID];
			}
		}

		//If shift keys are down
		else if(KeySym == SDLK_LSHIFT || KeySym == SDLK_RSHIFT)
		{
			//Ignore them
		}

		else
		{
			//If current line will run off the page
			if(!(Font.GetTextWidth(CurrentLine) > Engine->ScreenWidth - 100))
			{
				//Add Unicode version of the key to current line (support for capital letters, etc)
				CurrentLine += (char)Unicode;
			}
		}
	}

	void CConsole::ProcessCurrentLine()
	{
		//If line is not empty
		if(CurrentLine.size() > 0)
		{
			//Add line to rendering queue
			PrintLine(CurrentLine);

			//Add line to command history
			History.push_back(CurrentLine);

			//Send command to the script manager
			Engine->ScriptManager.RunCommand(CurrentLine);

			//Erase current line
			CurrentLine.erase(CurrentLine.begin(), CurrentLine.end());

			//Reset current history line
			CurrentHistoryID = 0;
		}
	}

	void CConsole::PrintLine(std::string Text)
	{
		//Add line to rendering queue
		Lines.push_back(Text);

		//If queue is greater than max lines, delete the first entry
		if((int)Lines.size() > MaxLines)
		{
			Lines.pop_front();
		}
	}

	void CConsole::Render()
	{
	    glPushMatrix();

		//If console has a texture
		if(Texture)
		{
			//Enable textures
			glEnable(GL_TEXTURE_2D);

			//Bind texture
			glBindTexture(GL_TEXTURE_2D, Texture->Texture);
		}

		//Enable blending
		glEnable(GL_BLEND);

		//Blending type
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//Set colour and transparency
	    glColor4f(1.0, 1.0, 1.0, 0.5);

		//Draw the console frame
		glBegin(GL_QUADS);

			glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex2f((GLfloat)Engine->ScreenWidth, 0.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex2f((GLfloat)Engine->ScreenWidth, YPos);
			glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, YPos);

		glEnd();

		//Set colour back
		glColor4f(1.0, 1.0, 1.0, 1.0);

		//Set blending type back
		glBlendFunc(GL_ONE, GL_ONE);

		//Disable texures
		glDisable(GL_TEXTURE_2D);

		//Render FPS text
		FrameRateStream.str("");
		FrameRateStream << "Frames per second: " << GetFPS();
		Font.RenderText(FrameRateStream.str(), 10.0f, (Engine->ScreenHeight / 3) - 20.0f);

		//Render time text
		int Hours, Minutes, Seconds;
		SplitTime(Engine->GetElapsedTime() / 1000, Hours, Minutes, Seconds);
		TimeStream.str("");
		TimeStream << "Time elapsed: " << std::setfill('0') << std::setw(2) << Hours << ":" << std::setw(2) << Minutes << ":" << std::setw(2) << Seconds;
		Font.RenderText(TimeStream.str(), Engine->ScreenWidth - Font.GetTextWidth(TimeStream.str()) - 10.0f, (Engine->ScreenHeight / 3) - 20.0f);

		//Render prompt
		std::string CurrentLineText;
		CurrentLineText = Prompt + CurrentLine;
		Font.RenderText(CurrentLineText, 10, 10);

		//Render all console lines
		int LinePos = 30;
		for (int i = (int)Lines.size(); i > 0; i--)
		{
			Font.RenderText(Lines[i - 1], 10.0f, (float)LinePos);
			LinePos += 20;
		}

		//Disable blending
		glDisable(GL_BLEND);

		glPopMatrix();
	}


} //Namespace

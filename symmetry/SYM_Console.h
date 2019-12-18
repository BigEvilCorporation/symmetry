#ifndef __CConsole
#define __CConsole

#ifdef SYM_DLL
#   define __declspec(dllexport)
#else
//#   define __declspec(dllimport)
#endif

#ifdef WIN32
#include <windows.h>
#endif

#include <sstream>
#include <string>
#include <vector>
#include <deque>

#include "SYM_Font.h"
#include "SYM_Texture.h"

namespace sym
{
	class CConsole
	{
		public:
			//Constructor
			CConsole();
			~CConsole();

			//Load console background image, load history
			bool Init();

			//Open console
			void Open();

			//Close console
			void Close();

			//Toggle the console open/close
			void Toggle();

			//Render the console and text
			void Render();

			//Process keyboard input
			void KeyboardInput(int KeySym, int Unicode);

			//Process a line entered
			void ProcessCurrentLine();

			//Print a line
			void PrintLine(std::string Text);

			//Print an error
			void PrintError(std::string Text);

			//Clear the console
			void Clear();

			//Clear the history
			void ClearHistory();

			//Set text colour
			void SetTextColour(int r, int g, int b);

			//Set background texture
			bool SetBackgroundTexture(std::string Filename);

			//To check if console is open
			bool IsOpen;

		private:
			//Text lines
			std::deque<std::string> Lines;

			//Command history
			std::deque<std::string> History;

			//Current history entry
			unsigned int CurrentHistoryID;

			//The current line being typed
			std::string CurrentLine;

			//String stream for FPS text
			std::stringstream FrameRateStream;

			//String stream for time text
			std::stringstream TimeStream;

			//The command prompt
			std::string Prompt;

			//Font
			CFont Font;

			//Font name
			std::string FontName;

			//Font size
			int FontSize;

			//Y position
			float YPos;

			//Height
			float Height;

			//Max amount of lines
			int MaxLines;

			//Background texture
			CTexture *Texture;
	};

} //Namespace

#endif


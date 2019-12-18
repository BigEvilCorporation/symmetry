#ifndef __SYM_DEBUG
#define __SYM_DEBUG

#ifdef SYM_DLL
#   define __declspec(dllexport)
#else
//#   define __declspec(dllimport)
#endif

#include <sstream>
#include "SYM_Font.h"

namespace sym
{
	class CDebugInfo
	{
		public:
			//Load debug font, etc
			bool Init();

			//Render debug info
			void RenderDebugInfo();

		private:
			//The font
			CFont Arial10;

			//String stream
			std::stringstream Stream;
	};

} //Namespace

#endif

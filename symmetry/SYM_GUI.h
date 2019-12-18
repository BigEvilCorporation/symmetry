#include "SYM_Sprite.h"
#include "SYM_Font.h"

class SYM_API CGuiButton : public CSprite
{
	public:
		bool SetText(std::string FontFile, int FontSize, std::string Text);
		
		void Render();

	private:
		//Font
		CFont *Font;
};
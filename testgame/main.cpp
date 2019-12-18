#include <tchar.h>
#include "SYM_Core.h"
#include "Game.h"

int main(int argc, char *argv[])
//int _tmain(int argc, _TCHAR *argv[])
{
	//Collect command line args
	std::vector<std::string> Args;

	for (int i = 1; i < argc; i++)
	{
		Args.push_back(argv[i]);
	}

	//Initialise game
	Game->Init(Args);

	//Main loop
	while(sym::Engine->Update())
	{
        //Hard coded console key
		if(sym::Engine->Input.IsKeyPressed('`')) sym::Engine->Console.Toggle();
		if(sym::Engine->Input.IsKeyPressed('~')) sym::Engine->Console.Toggle();
	}

	return 0;
}
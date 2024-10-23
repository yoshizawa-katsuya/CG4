#include "MyGame.h"
#include "YKFramework.h"


//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	YKFramework* game = new MyGame();

	game->Run();

	delete game;
	
	return 0;
}
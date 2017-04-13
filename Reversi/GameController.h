#pragma once
#include "GameWindow.h"
class GameWindow;


class GameController {
public:
	GameController(GameWindow* handle);
private:
	GameWindow* handle;
	char gameBoard[8][8];
};
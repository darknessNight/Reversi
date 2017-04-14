#pragma once
#include "GameWindow.h"
class GameWindow;


class GameController {
public:
	GameController(GameWindow* handle);
	void pawnPlaced(int x, int y);
private:
	GameWindow* handle;
	char gameBoard[8][8];
	int playerNumber;
};
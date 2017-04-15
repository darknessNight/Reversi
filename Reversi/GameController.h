#pragma once
#include "GameWindow.h"
#include "PlayerColor.h"
#include "GameBoard.h"

class GameWindow;


class GameController {
public:
	GameController(GameWindow* handle);
	void pawnPlaced(int x, int y);
private:
	GameWindow* handle;
	GameBoard gameBoard;
	PlayerColor playerNumber;
};
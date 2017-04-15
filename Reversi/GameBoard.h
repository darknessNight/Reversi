#pragma once

#include "PlayerColor.h"


class GameBoard {
public:
	PlayerColor getSquare(int x, int y);
	void setSquare(int x, int y, PlayerColor value);
private:
	PlayerColor board[8][8];
};
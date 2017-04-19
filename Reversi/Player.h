#pragma once
#include <functional>
//#include "GameBoard.h"

enum PlayerColor
{
	WhitePlayer,
	BlackPlayer,
	EmptyField
};

class GameBoard
{
public:
	PlayerColor getSquare(int x, int y) const { return WhitePlayer; }
	void setSquare(int x, int y, PlayerColor value) {}
private:
	PlayerColor board[8][8];
};

namespace Reversi
{
	class Player abstract
	{
	public:
		virtual void StartMove(GameBoard) = 0;

		virtual void setCallback(std::function<void(int x, int y)>) = 0;
	};
}
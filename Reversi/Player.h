#pragma once
#include <functional>
#include "GameBoard.h"

namespace Reversi
{
	class Player abstract
	{
	public:
		virtual void StartMove(GameBoard) = 0;

		virtual void setCallback(std::function<void(int x, int y)>) = 0;
	};
}
#pragma once
#include "Player.h"
#include "../ReversiSI/MinMax.h"

namespace Reversi
{
	class GameBoardConverter
	{
	public:
		static SI::Reversi::BoardState ConvertToBoardState(const GameBoard& board)
		{
			SI::Reversi::BoardState result;
			for ( int i = 0; i < result.rowsCount; i++ )
				for ( int j = 0; j < result.colsCount; j++ )
				{
					result.SetFieldState(i, j, ConvertColorToFieldState(board.getSquare(i, j)));
				}
			return result;
		}

		static SI::Reversi::BoardState::FieldState ConvertColorToFieldState(const PlayerColor& color)
		{
			switch ( color )
			{
			case PlayerColor::BlackPlayer:
				return SI::Reversi::BoardState::FieldState::Player1;
			case PlayerColor::WhitePlayer:
				return SI::Reversi::BoardState::FieldState::Player2;
			case PlayerColor::EmptyField:
				return SI::Reversi::BoardState::FieldState::Empty;
			default:return SI::Reversi::BoardState::FieldState::Unknown;
			}
		}
	};
}
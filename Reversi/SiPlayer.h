#pragma once
#include "Player.h"
#include "GameBoardConverter.h"

namespace Reversi
{
	
	class SiPlayer: public Player
	{
		std::function<void(int x, int y)> callBackFunc;
		SI::Reversi::MinMax minmax;
		SI::Reversi::BoardState::FieldState playerState;
		SI::Reversi::BoardState lastState;
	public:
		SiPlayer(GameBoard startState, PlayerColor playerColor, unsigned minDepth)
			:minmax(GameBoardConverter::ConvertToBoardState(startState),
				GameBoardConverter::ConvertColorToFieldState(playerColor), minDepth,
				getStandardHeuristic()), playerState(GameBoardConverter::ConvertColorToFieldState(playerColor)),
			lastState(GameBoardConverter::ConvertToBoardState(startState))
		{

		}

		std::function<double(const SI::Reversi::BoardState &state)> getStandardHeuristic()
		{
			return [&] (SI::Reversi::BoardState state) {return standardHeuristic(state); };
		}

		double standardHeuristic(SI::Reversi::BoardState state)
		{
			double result = 0;

			for ( int i = 0; i < state.rowsCount; i++ )
				for ( int j = 0; j < state.colsCount; j++ )
				{
					auto fieldState = state.GetFieldState(i, j);
					if ( fieldState == SI::Reversi::BoardState::FieldState::Empty )
					{

					}
					else if ( fieldState == playerState ) result += 1;
					else result -= 1;
				}
			return result;
		}

		virtual void StartMove(GameBoard board)
		{
			lastState=GameBoardConverter::ConvertToBoardState(board);
			minmax.SetOpponentMove(lastState);
			minmax.GetBestMoveAsync([&] (const SI::Reversi::BoardState& state) {
				doCallback(state);
			});
		}

		void doCallback(const SI::Reversi::BoardState& state)
		{
			if ( callBackFunc != nullptr )
				for ( int i = 0; i < state.rowsCount; i++ )
					for ( int j = 0; j < state.colsCount; j++ )
						if ( state.GetFieldState(i, j) != SI::Reversi::BoardState::FieldState::Empty &&
							lastState.GetFieldState(i, j) == SI::Reversi::BoardState::FieldState::Empty )
							callBackFunc(i, j);
		}

		virtual void setCallback(std::function<void(int x, int y)> callback)
		{
			callBackFunc = callback;
		}

		unsigned GetTreeDepth()
		{
			minmax.GetCurrentDepth();
		}
	};
}
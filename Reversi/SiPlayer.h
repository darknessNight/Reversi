#pragma once
#include "Player.h"
#include "GameBoardConverter.h"

namespace Reversi
{
	
	class SiPlayer: public Player
	{
		bool lastMoveSI = false;
		std::function<void(int x, int y)> callBackFunc;
		SI::Reversi::MinMax minmax;
		SI::Reversi::BoardState::FieldState playerState;
		SI::Reversi::BoardState lastState;
		const int C = 500;
		const int E = 50;
		const int M = 10;
		const int R = 5;
		const int B = 1;
		const int weightsOfSquares[8][8] = {
			{ C,E,E,E,E,E,E,C },
			{ E,B,B,B,B,B,B,E },
			{ E,B,M,M,M,M,B,E },
			{ E,B,M,R,R,M,B,E },
			{ E,B,M,R,R,M,B,E },
			{ E,B,M,M,M,M,B,E },
			{ E,B,B,B,B,B,B,E },
			{ C,E,E,E,E,E,E,C },
		};

	public:
		SiPlayer(GameBoard startState, PlayerColor playerColor, unsigned minDepth)
			:minmax(GameBoardConverter::ConvertToBoardState(startState),
				GameBoardConverter::ConvertColorToFieldState(playerColor), minDepth,
				getStandardHeuristic()), playerState(GameBoardConverter::ConvertColorToFieldState(playerColor)),
			lastState(GameBoardConverter::ConvertToBoardState(startState))
		{
			lastMoveSI = true;
		}
	private:
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
					else if ( fieldState == playerState ) result += weightsOfSquares[i][j];
					else result -= weightsOfSquares[i][j];
				}
			return result;
		}
	public:
		virtual void StartMove(GameBoard board)
		{
			lastMoveSI = false;
			lastState=GameBoardConverter::ConvertToBoardState(board);
			minmax.SetOpponentMove(lastState);
			minmax.GetBestMoveAsync(getAsyncFunc());
		}

		virtual void PassMove(GameBoard board)
		{
			lastState = GameBoardConverter::ConvertToBoardState(board);
			minmax.SetOpponentMove(lastState);
			minmax.GetBestMove();
		}
	private:
		std::function<void(const SI::Reversi::BoardState&)> getAsyncFunc()
		{
			return [&](const SI::Reversi::BoardState& state) {
				doCallback(state);
				lastMoveSI = true;
			};
		}

		void doCallback(const SI::Reversi::BoardState& state)
		{
			if ( callBackFunc != nullptr )
				for ( int i = 0; i < state.rowsCount; i++ )
					for ( int j = 0; j < state.colsCount; j++ )
						if ( state.GetFieldState(i, j) != SI::Reversi::BoardState::FieldState::Empty &&
							lastState.GetFieldState(i, j) == SI::Reversi::BoardState::FieldState::Empty )
							callBackFunc(i,j);
		}
	public:
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
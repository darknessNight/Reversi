#pragma once
#include <vector>
#include "BoardState.h"

namespace SI::Reversi
{
	class StateGenerator
	{
	protected:
		BoardState currentState;
	public:
		explicit StateGenerator(const BoardState& state, BoardState::FieldState player):currentState(state)
		{
			Reset();
		}
		StateGenerator()
		{
		}

		virtual ~StateGenerator() = default;

		virtual std::vector<BoardState> GetAllNextStates()
		{
			std::vector<BoardState> result;
			while(HasNextState())
			{
				result.push_back(GetNextState());
			}
			return result;
		}

		virtual BoardState GetNextState()
		{
			return BoardState();
		}
		virtual bool HasNextState()
		{
			return false;
		}
		virtual void Reset(){}

		virtual BoardState GetCurrentState()
		{
			return currentState;
		}
	};
}
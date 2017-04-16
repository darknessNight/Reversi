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
		explicit StateGenerator(const BoardState& state):currentState(state)
		{
			Reset();
		}
		StateGenerator()
		{
		}

		virtual ~StateGenerator() = default;

		//virtual std::vector<BoardState> GetAllNextStates(const BoardState& state) = 0;

		virtual BoardState GetNextState()
		{
			return BoardState();
		}
		virtual bool HasNextState()
		{
			return false;
		}
		virtual void Reset(){}

		virtual void SetCurrentState(const BoardState&state) {
			currentState = state;
			Reset();
		}

		virtual BoardState GetCurrentState()
		{
			return currentState;
		}
	};
}
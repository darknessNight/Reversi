#pragma once
#include <vector>
#include "MapState.h"

namespace SI::Reversi
{
	class StateGenerator abstract
	{
	protected:
		MapState currentState;
	protected:
		explicit StateGenerator(const MapState& state):currentState(state)
		{
		}
		~StateGenerator() = default;
	public:
		virtual std::vector<MapState> GetAllNextStates(const MapState& state) = 0;

		virtual MapState GetNextState() = 0;
		virtual bool HasNextState() = 0;
		virtual void Reset()=0;

		virtual void SetCurrentState(const MapState&state) {
			currentState = state;
		}

		virtual MapState GetCurrentState()
		{
			return currentState;
		}
	};
}
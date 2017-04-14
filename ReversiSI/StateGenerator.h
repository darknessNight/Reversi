#pragma once
#include <vector>
#include "MapState.h"

namespace SI::Reversi
{
	class StateGenerator
	{
	protected:
		MapState currentState;
		MapState::State nextPlayer;
	public:
		explicit StateGenerator(const MapState& state, const MapState::State nextPlayerState)
			:currentState(state), nextPlayer(nextPlayerState)
		{}
		~StateGenerator() = default;
	public:
		static std::vector<MapState> GetAllNextStates(const MapState& state, const MapState::State nextPlayerState) {}

		virtual MapState GetNextState() {
			return MapState();
		}
		virtual bool HasNextState() {
			return false;
		}
		virtual void Reset() {}

		virtual void SetCurrentState(const MapState&state)
		{
			currentState = state;
		}

		virtual MapState GetCurrentState()
		{
			return currentState;
		}
	};
}
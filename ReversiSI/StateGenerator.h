#pragma once
#include <vector>
#include "MapState.h"

namespace SI::Reversi
{
	enum LineDirection {
		North, NorthEast,
		East, SouthEast,
		South, SouthWest,
		West, NorthWest
	};

	struct PossibleAndCurrentFields {
		unsigned int newX;
		unsigned int newY;
		unsigned int currX;
		unsigned int currY;
		LineDirection direction;
	};

	struct FieldI {
		unsigned int x;
		unsigned int y;
	};

	typedef int invalidMoveException;

	class StateGenerator
	{
	protected:
		MapState currentState;
		MapState::State nextPlayer;

		static void getAvaliableStatesForGivenField(const MapState & state, 
			const MapState::State nextPlayerState, std::vector<MapState>* mapStates, std::vector<PossibleAndCurrentFields>* foundFields);

		
		static void checkHorizontalLine(unsigned int x, unsigned int y, const MapState & state,
			const MapState::State nextPlayerState, std::vector<PossibleAndCurrentFields>* foundFields);
		static void checkVerticalLine(unsigned int x, unsigned int y, const MapState & state,
			const MapState::State nextPlayerState, std::vector<PossibleAndCurrentFields>* foundFields);
		static void checkDiagonalLineNW_SE(unsigned int x, unsigned int y, const MapState & state,
			const MapState::State nextPlayerState, std::vector<PossibleAndCurrentFields>* foundFields);
		static void checkDiagonalLineNE_SW(unsigned int x, unsigned int y, const MapState & state,
			const MapState::State nextPlayerState, std::vector<PossibleAndCurrentFields>* foundFields);
		
		static bool checkMovePossibillityOnField(MapState::State fieldState, MapState::State nextPlayerState,
			bool* opponentPieceFound, bool* ownPieceFound);

		static void generateNewStatesBasedOnFoundPoints(MapState::State nextPlayerState, std::vector<PossibleAndCurrentFields>* foundFields, const MapState & state,
			std::vector<MapState>* newMapStates);
		static void setNewFieldState(MapState::State nextPlayerState, std::vector<MapState>* newMapStates, std::vector<PossibleAndCurrentFields>* foundFields, int i);
		static bool validateMove(int xDifference, int yDifference);
		static void setIncrementalValuesAccordingToDirection(unsigned int* x, unsigned int* y, LineDirection direction);
		static std::vector<PossibleAndCurrentFields> getDuplicates(std::vector<PossibleAndCurrentFields>* foundFields, PossibleAndCurrentFields currentFields);
	public:
		explicit StateGenerator(const MapState& state, const MapState::State nextPlayerState)
			:currentState(state), nextPlayer(nextPlayerState)
		{}
		~StateGenerator() = default;
	public:
		static std::vector<MapState> GetAllNextStates(const MapState& state, const MapState::State nextPlayerState);

		virtual MapState GetNextState();
		virtual bool HasNextState();
		virtual void Reset();

		virtual void SetCurrentState(const MapState&state)
		{
			currentState = state;
		}

		virtual MapState GetCurrentState()
		{
			return currentState;
		}

		static unsigned int abs(unsigned int a) {
			if (a >= 0)	return a;
			else return -a;
		}
		static unsigned int max(unsigned int a, unsigned int b) {
			if (a > b)
				return a;
			else
				return b;
		}
	};
}
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

		unsigned int currentStateIndex;

		std::vector<PossibleAndCurrentFields>* foundFields = nullptr;
		std::vector<MapState>* nextMapStates = nullptr;

		void emptyAllVectors();
		void getAvaliableStatesForGivenField();

		
		void checkHorizontalLine(unsigned int x, unsigned int y);
		void checkVerticalLine(unsigned int x, unsigned int y);
		void checkDiagonalLineNW_SE(unsigned int x, unsigned int y);
		void checkDiagonalLineNE_SW(unsigned int x, unsigned int y);
		
		bool checkMovePossibillityOnField(MapState::State fieldState, bool* opponentPieceFound, bool* ownPieceFound);

		void generateNewStatesBasedOnFoundPoints();
		void setNewFieldState(int i);
		bool validateMove(int xDifference, int yDifference);
		void setIncrementalValuesAccordingToDirection(unsigned int* x, unsigned int* y, LineDirection direction);
		std::vector<PossibleAndCurrentFields> getAndRemoveDuplicates(PossibleAndCurrentFields currentFields);
		void removeFromFoundFields(unsigned int i);
		
	public:
		explicit StateGenerator(const MapState& state, const MapState::State nextPlayerState)
			:currentState(state), nextPlayer(nextPlayerState)
		{}
		~StateGenerator() = default;
	public:


		std::vector<MapState> GetAllNextStates(const MapState& state, const MapState::State nextPlayerState);

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

		static int abs(int a) {
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
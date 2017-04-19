#pragma once
#include <vector>
#include "BoardState.h"

namespace SI::Reversi
{

	

	typedef int invalidMoveException;

	class StateGenerator
	{
	public:
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

	protected:
		BoardState currentState;
		BoardStateMemoryOptimized::State nextPlayer;

		unsigned int currentStateIndex = 0;

		std::vector<PossibleAndCurrentFields>* foundFields = nullptr;
		std::vector<BoardState>* nextMapStates = nullptr;

		void emptyAllVectors();
		void getAvaliableStatesForGivenField();


		void checkHorizontalLine(unsigned int x, unsigned int y);
		void checkVerticalLine(unsigned int x, unsigned int y);
		void checkDiagonalLineNW_SE(unsigned int x, unsigned int y);
		void checkDiagonalLineNE_SW(unsigned int x, unsigned int y);

		bool checkMovePossibillityOnField(BoardStateMemoryOptimized::State fieldState, bool* opponentPieceFound, bool* ownPieceFound);

		void generateNewStatesBasedOnFoundPoints();
		void setNewFieldState(int i);
		bool validateMove(int xDifference, int yDifference);
		void setIncrementalValuesAccordingToDirection(unsigned int* x, unsigned int* y, LineDirection direction);
		std::vector<PossibleAndCurrentFields> getAndRemoveDuplicates(PossibleAndCurrentFields currentFields);

	protected:
		StateGenerator() = default;
	public:
		explicit StateGenerator(const BoardState& state, const BoardStateMemoryOptimized::State nextPlayerState)
			:currentState(state), nextPlayer(nextPlayerState)
		{}
		~StateGenerator() = default;


		virtual std::vector<BoardState> GetAllNextStates();
// zmieniono na std::vector<BoardState> GetAllNextStates(); musisz to uwzględnić
		virtual BoardState GetNextState();
		virtual bool HasNextState();
		virtual void Reset();

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

				virtual BoardState GetCurrentState()
				{
					return currentState;
				}
			};
}

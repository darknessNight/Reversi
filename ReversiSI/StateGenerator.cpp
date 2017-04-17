#include "StateGenerator.h"
#include "BoardState.h"
#include <vector>

using namespace SI::Reversi;

//zak³adam, ¿e gracz, który wykonuje teraz ruch to nextPlayerState
/*
		|
		|					x
	----|------------------->
		|				|
		|				|
		|				|
		|				|
		|_______________|
		|
	  y	\/

*/


std::vector<BoardState> SI::Reversi::StateGenerator::GetAllNextStates(const BoardState& state, const BoardStateMemoryOptimized::State nextPlayerState)
{
	Reset();

	this->currentState = state;
	this->nextPlayer = nextPlayerState;

	getAvaliableStatesForGivenField();
	generateNewStatesBasedOnFoundPoints();

	return std::vector<BoardState>(*this->nextMapStates);
}

void SI::Reversi::StateGenerator::emptyAllVectors()
{
	if (this->nextMapStates == nullptr)
		this->nextMapStates = new std::vector<BoardState>();
	else
		this->nextMapStates->empty();
	if (this->foundFields == nullptr)
		this->foundFields = new std::vector<PossibleAndCurrentFields>();
	else
		this->foundFields->empty();

	this->currentStateIndex = 0;
}

void SI::Reversi::StateGenerator::getAvaliableStatesForGivenField()
{
	BoardStateMemoryOptimized::State currentFieldsPlayer;
	for (int i = 0; i <this->currentState.rowsCount; i++) {
		for (int j = 0; j < this->currentState.colsCount; j++)
		{
			currentFieldsPlayer = this->currentState.GetFieldState(j, i);
			if (currentFieldsPlayer == this->nextPlayer) {
				checkHorizontalLine(j, i);
				checkVerticalLine(j, i);
				checkDiagonalLineNW_SE(j, i);
				checkDiagonalLineNE_SW(j, i);
			}
		}
	}
	return;
}

void SI::Reversi::StateGenerator::checkHorizontalLine(unsigned int x, unsigned int y)
{
	bool opponentPieceFound = false;
	bool ownPieceFound = false;
	BoardStateMemoryOptimized::State fieldState;
	if (this->currentState.GetFieldState(x - 1, y) != BoardStateMemoryOptimized::State::Empty)
	{
		for (unsigned int i = x - 1; i > 0; i--) {
			fieldState = this->currentState.GetFieldState(i, y);
			if (checkMovePossibillityOnField(fieldState, &opponentPieceFound, &ownPieceFound)) {
				foundFields->push_back({ i, y, x, y, LineDirection::West });
				break;
			}
		}
	}

	opponentPieceFound = false;
	ownPieceFound = false;
	if (this->currentState.GetFieldState(x + 1, y) != BoardStateMemoryOptimized::State::Empty)
	{
		for (unsigned int i = x + 1; i < BoardState::rowsCount; i++) {
			fieldState = this->currentState.GetFieldState(i, y);
			if (checkMovePossibillityOnField(fieldState, &opponentPieceFound, &ownPieceFound)) {
				foundFields->push_back({ i, y, x, y, LineDirection::East });
				break;
			}
		}
	}
	return;
}

void SI::Reversi::StateGenerator::checkVerticalLine(unsigned int x, unsigned int y)
{
	bool opponentPieceFound = false;
	bool ownPieceFound = false;
	BoardStateMemoryOptimized::State fieldState;
	if (this->currentState.GetFieldState(x, y-1) != BoardStateMemoryOptimized::State::Empty)
	{
		for (unsigned int i = y - 1; i > 0; i--) {
			fieldState = this->currentState.GetFieldState(x, i);
			if (checkMovePossibillityOnField(fieldState, &opponentPieceFound, &ownPieceFound)) {
				foundFields->push_back({ x, i, x, y, LineDirection::North });
				break;
			}
		}
	}

	opponentPieceFound = false;
	ownPieceFound = false;
	if (this->currentState.GetFieldState(x, y + 1) != BoardStateMemoryOptimized::State::Empty)
	{
		for (unsigned int i = y + 1; i < this->currentState.rowsCount; i++) {
			fieldState = this->currentState.GetFieldState(x, i);
			if (checkMovePossibillityOnField(fieldState, &opponentPieceFound, &ownPieceFound)) {
				foundFields->push_back({ x, i, x, y, LineDirection::South });
				break;
			}
		}
	}
	return;
}

void SI::Reversi::StateGenerator::checkDiagonalLineNW_SE(unsigned int x, unsigned int y)
{
	bool opponentPieceFound = false;
	bool ownPieceFound = false;
	BoardStateMemoryOptimized::State fieldState;
	if (this->currentState.GetFieldState(x - 1, y - 1) != BoardStateMemoryOptimized::State::Empty)
	{
		for (unsigned int i = 1; (x - i) > 0 && (y - i) > 0; i++) {
			fieldState = this->currentState.GetFieldState(x - i, y - i);
			if (checkMovePossibillityOnField(fieldState, &opponentPieceFound, &ownPieceFound)) {
				foundFields->push_back({ x - i, y - i, x, y, LineDirection::NorthWest });
				break;
			}
		}
	}

	opponentPieceFound = false;
	ownPieceFound = false;
	if (this->currentState.GetFieldState(x + 1, y + 1) != BoardStateMemoryOptimized::State::Empty)
	{
		for (unsigned int i = 1; x + i < this->currentState.colsCount && y + i < this->currentState.rowsCount; i++) {
			fieldState = this->currentState.GetFieldState(x + i, y + i);
			if (checkMovePossibillityOnField(fieldState, &opponentPieceFound, &ownPieceFound)) {
				foundFields->push_back({ x + i, y + i, x, y, LineDirection::SouthEast });
				break;
			}
		}
	}
	return;
}

void SI::Reversi::StateGenerator::checkDiagonalLineNE_SW(unsigned int x, unsigned int y)
{
	bool opponentPieceFound = false;
	bool ownPieceFound = false;
	BoardStateMemoryOptimized::State fieldState;
	if (this->currentState.GetFieldState(x - 1, y + 1) != BoardStateMemoryOptimized::State::Empty)
	{
		for (unsigned int i = 1; (x - i) > 0 && (y + i) < this->currentState.rowsCount; i++) {
			fieldState = this->currentState.GetFieldState(x - i, y - i);
			if (checkMovePossibillityOnField(fieldState, &opponentPieceFound, &ownPieceFound)) {
				foundFields->push_back({ x - i, y + i, x, y, LineDirection::SouthWest });
				break;
			}
		}
	}

	opponentPieceFound = false;
	ownPieceFound = false;
	if (this->currentState.GetFieldState(x + 1, y - 1) != BoardStateMemoryOptimized::State::Empty)
	{
		for (unsigned int i = 1; (x + i) < this->currentState.colsCount && (y - i) > 0; i++) {
			fieldState = this->currentState.GetFieldState(x + i, y - i);
			if (checkMovePossibillityOnField(fieldState, &opponentPieceFound, &ownPieceFound)) {
				foundFields->push_back({ x + i, y - i, x, y, LineDirection::NorthEast });
				break;
			}
		}
	}
	return;
}

bool SI::Reversi::StateGenerator::checkMovePossibillityOnField(BoardStateMemoryOptimized::State fieldState, 
	bool* opponentPieceFound, bool* ownPieceFound)
{
	if (fieldState != this->nextPlayer && fieldState != BoardStateMemoryOptimized::State::Empty && !*opponentPieceFound) {
		*opponentPieceFound = true;
	}
	else if (fieldState == this->nextPlayer && *opponentPieceFound) {
		*ownPieceFound = true;
	}
	else if (fieldState == this->nextPlayer && *opponentPieceFound) {
		return false;
	}
	else if (fieldState == BoardStateMemoryOptimized::State::Empty && *opponentPieceFound && !*ownPieceFound) {
		return true;
	}
	return false;
}

void SI::Reversi::StateGenerator::generateNewStatesBasedOnFoundPoints()
{
	for (unsigned int i = 0; i < foundFields->size(); i++) {
		
		int xDifference = abs(foundFields->at(i).currX - foundFields->at(i).newX);
		int yDifference = abs(foundFields->at(i).currY - foundFields->at(i).newY);
		if (validateMove(xDifference, yDifference)) {
			this->nextMapStates->push_back(BoardState(this->currentState));
			setNewFieldState(i);
		}
	}
}

void SI::Reversi::StateGenerator::setNewFieldState(int i)
{
	PossibleAndCurrentFields currentFields = foundFields->at(i);
	std::vector<PossibleAndCurrentFields> fieldsWithSameDestination = getAndRemoveDuplicates(currentFields);

	unsigned int incrementX, incrementY, x, y, xDifference, yDifference;
	
	for (unsigned int k = 0; k < fieldsWithSameDestination.size(); k++)
	{
		currentFields = fieldsWithSameDestination.at(k);
		xDifference = abs(currentFields.currX - currentFields.newX);
		yDifference = abs(currentFields.currY - currentFields.newY);
		x = currentFields.currX;
		y = currentFields.currY;
		setIncrementalValuesAccordingToDirection(&incrementX, &incrementY, currentFields.direction);
		for (unsigned int i = 0; i < max(xDifference, yDifference); i++) 
		{
			x += incrementX;
			y += incrementY;
			this->nextMapStates->at(this->nextMapStates->size() - 1).SetFieldState(x, y, this->nextPlayer);
		}
	}
}

bool SI::Reversi::StateGenerator::validateMove(int xDifference, int yDifference)
{
	if (xDifference != yDifference) {
		if (xDifference != 0 && yDifference != 0) {
			return false;
		}
	}
	else if (xDifference == yDifference) {
		if (xDifference > 8)
			return false;
		if (xDifference == 0)
			return false;
	}
	return true;
}

void SI::Reversi::StateGenerator::setIncrementalValuesAccordingToDirection(unsigned int* x, unsigned int* y, 
	LineDirection direction)
{
	switch (direction) {
	case North:
		*x = 0;
		*y = -1;
		break;
	case NorthEast:
		*x = 1;
		*y = -1;
		break;
	case East:
		*x = 1;
		*y = 0;
		break;
	case SouthEast:
		*x = 1;
		*y = 1;
		break;
	case South:
		*x = 0;
		*y = 1;
		break;
	case SouthWest:
		*x = -1;
		*y = 1;
		break;
	case West:
		*x = -1;
		*y = 0;
		break;
	case NorthWest:
		*x = -1;
		*y = -1;
	}
}

std::vector<PossibleAndCurrentFields> SI::Reversi::StateGenerator::getAndRemoveDuplicates(PossibleAndCurrentFields currentFields)
{
	std::vector<PossibleAndCurrentFields> Duplicates = std::vector<PossibleAndCurrentFields>();
	for (unsigned int i = 0; i < foundFields->size(); i++)
	{
		if (foundFields->at(i).newX == currentFields.newX && foundFields->at(i).newY == currentFields.newY) 
		{
			//if (i != 0 && foundFields->at(i).currX == currentFields.currX && foundFields->at(i).currY == currentFields.currY)
				//return std::vector<PossibleAndCurrentFields>();
			
			Duplicates.push_back(foundFields->at(i));
			if (Duplicates.size() > 1)
				foundFields->erase(foundFields->begin() + i);
		}
	}
	return std::vector<PossibleAndCurrentFields>(Duplicates);
}

void SI::Reversi::StateGenerator::removeFromFoundFields(unsigned int i)
{

}

BoardState SI::Reversi::StateGenerator::GetNextState()
{
	return this->nextMapStates->at(this->currentStateIndex++);
}

bool SI::Reversi::StateGenerator::HasNextState()
{
	if (this->currentStateIndex < this->nextMapStates->size())
		return true;
	else
		return false;
}

void SI::Reversi::StateGenerator::Reset()
{
	emptyAllVectors();
	this->currentStateIndex = 0;
}
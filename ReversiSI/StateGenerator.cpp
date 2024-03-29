#include "StateGenerator.h"
#include "BoardState.h"
#include <vector>

using namespace SI::Reversi;


std::vector<BoardState> SI::Reversi::StateGenerator::GetAllNextStates()
{
	if (nextMapStates == nullptr) {
		Reset();
		emptyAllVectors();
		getAvaliableStatesForGivenField();
		generateNewStatesBasedOnFoundPoints();
	}
	return std::vector<BoardState>(*this->nextMapStates);
}

void SI::Reversi::StateGenerator::emptyAllVectors()
{
	if (this->nextMapStates == nullptr)
		this->nextMapStates = new std::vector<BoardState>();
	else
		this->nextMapStates->clear();
	if (this->foundFields == nullptr)
		this->foundFields = new std::vector<PossibleAndCurrentFields>();
	else
		this->foundFields->clear();

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
	bool foundGap = false;
	BoardStateMemoryOptimized::State fieldState;
	if (x != 0) 
	{
		if (this->currentState.GetFieldState(x - 1, y) != BoardStateMemoryOptimized::State::Empty)
		{
			for (unsigned int i = 1; x >= i; i++) {
				fieldState = this->currentState.GetFieldState(x - i, y);
				if (checkMovePossibillityOnField(fieldState, &opponentPieceFound, &ownPieceFound, &foundGap)) {
					foundFields->push_back({ x - i, y, x, y, LineDirection::West });
					break;
				}
			}
		}
	}

	opponentPieceFound = false;
	ownPieceFound = false;
	foundGap = false;
	if (x != BoardState::colsCount)
	{
		if (this->currentState.GetFieldState(x + 1, y) != BoardStateMemoryOptimized::State::Empty)
		{
			for (unsigned int i = x + 1; i < BoardState::colsCount; i++) {
				fieldState = this->currentState.GetFieldState(i, y);
				if (checkMovePossibillityOnField(fieldState, &opponentPieceFound, &ownPieceFound, &foundGap)) {
					foundFields->push_back({ i, y, x, y, LineDirection::East });
					break;
				}
			}
		}
	}
	return;
}

void SI::Reversi::StateGenerator::checkVerticalLine(unsigned int x, unsigned int y)
{
	bool opponentPieceFound = false;
	bool ownPieceFound = false;
	bool foundGap = false;
	BoardStateMemoryOptimized::State fieldState;
	if (y != 0)
	{
		if (this->currentState.GetFieldState(x, y - 1) != BoardStateMemoryOptimized::State::Empty)
		{
			for (unsigned int i = 1; y >= i; i++) {
				fieldState = this->currentState.GetFieldState(x, y - i);
				if (checkMovePossibillityOnField(fieldState, &opponentPieceFound, &ownPieceFound, &foundGap)) {
					foundFields->push_back({ x, y - i, x, y, LineDirection::North });
					break;
				}
			}
		}
	}

	opponentPieceFound = false;
	ownPieceFound = false;
	foundGap = false;
	if (y != BoardState::rowsCount)
	{
		if (this->currentState.GetFieldState(x, y + 1) != BoardStateMemoryOptimized::State::Empty)
		{
			for (unsigned int i = y + 1; i < this->currentState.rowsCount; i++) {
				fieldState = this->currentState.GetFieldState(x, i);
				if (checkMovePossibillityOnField(fieldState, &opponentPieceFound, &ownPieceFound, &foundGap)) {
					foundFields->push_back({ x, i, x, y, LineDirection::South });
					break;
				}
			}
		}
	}
	return;
}

void SI::Reversi::StateGenerator::checkDiagonalLineNW_SE(unsigned int x, unsigned int y)
{
	bool opponentPieceFound = false;
	bool ownPieceFound = false;
	bool foundGap = false;
	BoardStateMemoryOptimized::State fieldState;
	if (x != 0 && y != 0) 
	{
		if (this->currentState.GetFieldState(x - 1, y - 1) != BoardStateMemoryOptimized::State::Empty)
		{
			for (unsigned int i = 1; x >= i && y >= i; i++) {
				fieldState = this->currentState.GetFieldState(x - i, y - i);
				if (checkMovePossibillityOnField(fieldState, &opponentPieceFound, &ownPieceFound, &foundGap)) {
					foundFields->push_back({ x - i, y - i, x, y, LineDirection::NorthWest });
					break;
				}
			}
		}
	}

	opponentPieceFound = false;
	ownPieceFound = false;
	foundGap = false;
	if (x != BoardState::colsCount && y != BoardState::rowsCount)
	{
		if (this->currentState.GetFieldState(x + 1, y + 1) != BoardStateMemoryOptimized::State::Empty)
		{
			for (unsigned int i = 1; x + i < this->currentState.colsCount && y + i < this->currentState.rowsCount; i++) {
				fieldState = this->currentState.GetFieldState(x + i, y + i);
				if (checkMovePossibillityOnField(fieldState, &opponentPieceFound, &ownPieceFound, &foundGap)) {
					foundFields->push_back({ x + i, y + i, x, y, LineDirection::SouthEast });
					break;
				}
			}
		}
	}
	return;
}

void SI::Reversi::StateGenerator::checkDiagonalLineNE_SW(unsigned int x, unsigned int y)
{
	
	bool opponentPieceFound = false;
	bool ownPieceFound = false;
	bool foundGap = false;
	BoardStateMemoryOptimized::State fieldState;
	if (x != 0 && y != BoardState::rowsCount)
	{
		if (this->currentState.GetFieldState(x - 1, y + 1) != BoardStateMemoryOptimized::State::Empty)
		{
			for (unsigned int i = 1; x >= i && y + i < this->currentState.rowsCount; i++) {
				fieldState = this->currentState.GetFieldState(x - i, y + i);
				if (checkMovePossibillityOnField(fieldState, &opponentPieceFound, &ownPieceFound, &foundGap)) {
					foundFields->push_back({ x - i, y + i, x, y, LineDirection::SouthWest });
					break;
				}
			}
		}
	}

	opponentPieceFound = false;
	ownPieceFound = false;
	foundGap = false;
	if (x != BoardState::colsCount && y != 0)
	{
		if (this->currentState.GetFieldState(x + 1, y - 1) != BoardStateMemoryOptimized::State::Empty)
		{
			for (unsigned int i = 1; x + i < this->currentState.colsCount && y >= i; i++) {
				fieldState = this->currentState.GetFieldState(x + i, y - i);
				if (checkMovePossibillityOnField(fieldState, &opponentPieceFound, &ownPieceFound, &foundGap)) {
					foundFields->push_back({ x + i, y - i, x, y, LineDirection::NorthEast });
					break;
				}
			}
		}
	}
	return;
}

bool SI::Reversi::StateGenerator::checkMovePossibillityOnField(BoardStateMemoryOptimized::State fieldState, 
	bool* opponentPieceFound, bool* ownPieceFoundAfterOpponentPiece, bool *foundGap)
{
	if (fieldState != this->nextPlayer && fieldState != BoardStateMemoryOptimized::State::Empty && !*opponentPieceFound) {
		*opponentPieceFound = true;
	}
	else if (fieldState == this->nextPlayer && *opponentPieceFound) {
		*ownPieceFoundAfterOpponentPiece = true;
	}
	else if (fieldState == this->nextPlayer && *opponentPieceFound) {
		return false;
	}
	else if (fieldState == BoardStateMemoryOptimized::State::Empty && *opponentPieceFound && !*ownPieceFoundAfterOpponentPiece &&
		!*foundGap) {
		return true;
	}
	else if (fieldState == BoardStateMemoryOptimized::State::Empty && !*opponentPieceFound && !*opponentPieceFound) {
		*foundGap = true;
	}
	return false;
}

void SI::Reversi::StateGenerator::generateNewStatesBasedOnFoundPoints()
{
	for (unsigned int i = 0; i < foundFields->size(); i++) {
		this->nextMapStates->push_back(BoardState(this->currentState));
		setNewFieldState(i);
	}
}

void SI::Reversi::StateGenerator::setNewFieldState(int i)
{
	PossibleAndCurrentFields currentFields = foundFields->at(i);
	std::vector<PossibleAndCurrentFields> fieldsWithSameDestination = getAndRemoveDuplicates(currentFields);

	unsigned int x, y, xDifference, yDifference;
	int incrementX, incrementY;
	
	for (unsigned int k = 0; k < fieldsWithSameDestination.size(); k++)
	{
		currentFields = fieldsWithSameDestination.at(k);
		xDifference = max(currentFields.currX, currentFields.newX) - min(currentFields.currX, currentFields.newX);
		yDifference = max(currentFields.currY, currentFields.newY) - min(currentFields.currY, currentFields.newY);
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

void SI::Reversi::StateGenerator::setIncrementalValuesAccordingToDirection(int* x, int* y, 
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

std::vector<StateGenerator::PossibleAndCurrentFields> SI::Reversi::StateGenerator::getAndRemoveDuplicates(PossibleAndCurrentFields currentFields)
{
	std::vector<PossibleAndCurrentFields> Duplicates = std::vector<PossibleAndCurrentFields>();
	for (unsigned int i = 0; i < foundFields->size(); i++)
	{
		if (foundFields->at(i).newX == currentFields.newX && foundFields->at(i).newY == currentFields.newY) 
		{
			Duplicates.push_back(foundFields->at(i));
			if (Duplicates.size() > 1) {
				foundFields->erase(foundFields->begin() + i);
				i--;
			}
		}
	}
	return std::vector<PossibleAndCurrentFields>(Duplicates);
}

BoardState SI::Reversi::StateGenerator::GetNextState()
{
	if (nextMapStates == nullptr)
		GetAllNextStates();
	return this->nextMapStates->at(this->currentStateIndex++);
}

bool SI::Reversi::StateGenerator::HasNextState()
{
	if (nextMapStates == nullptr)
		GetAllNextStates();
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

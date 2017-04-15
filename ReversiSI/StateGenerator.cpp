#include "StateGenerator.h"
#include "MapState.h"
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


std::vector<MapState> SI::Reversi::StateGenerator::GetAllNextStates(const MapState & state, const MapState::State nextPlayerState)
{
	std::vector<MapState>* nextMapStates = new std::vector<MapState>();
	std::vector<PossibleAndCurrentFields>* foundFields = new std::vector<PossibleAndCurrentFields>();

	getAvaliableStatesForGivenField(state, nextPlayerState, nextMapStates, foundFields);
	generateNewStatesBasedOnFoundPoints(nextPlayerState, foundFields, state, nextMapStates);

	return std::vector<MapState>(*nextMapStates);
}

void SI::Reversi::StateGenerator::getAvaliableStatesForGivenField(
	const MapState & state, const MapState::State nextPlayerState, std::vector<MapState>* newMapStates,
	std::vector<PossibleAndCurrentFields>* foundFields)
{
	MapState::State currentFieldsPlayer;
	for (int i = 0; i < state.rowsCount; i++) {
		for (int j = 0; j < state.colsCount; j++)
		{
			currentFieldsPlayer = state.GetFieldState(j, i);
			if (currentFieldsPlayer == nextPlayerState) {
				checkHorizontalLine(j, i, state, nextPlayerState, foundFields);
				checkVerticalLine(j, i, state, nextPlayerState, foundFields);
				checkDiagonalLineNW_SE(j, i, state, nextPlayerState, foundFields);
				checkDiagonalLineNE_SW(j, i, state, nextPlayerState, foundFields);
			}
		}
	}
	return;
}

void SI::Reversi::StateGenerator::checkHorizontalLine(unsigned int x, unsigned int y, 
	const MapState & state, const MapState::State nextPlayerState, std::vector<PossibleAndCurrentFields>* foundFields)
{
	bool opponentPieceFound = false;
	bool ownPieceFound = false;
	MapState::State fieldState;
	if (state.GetFieldState(x - 1, y) != MapState::State::Empty)
	{
		for (unsigned int i = x - 1; i > 0; i--) {
			fieldState = state.GetFieldState(i, y);
			if (checkMovePossibillityOnField(fieldState, nextPlayerState, &opponentPieceFound, &ownPieceFound)) {
				foundFields->push_back({ i, y, x, y, LineDirection::West });
				break;
			}
		}
	}

	opponentPieceFound = false;
	ownPieceFound = false;
	if (state.GetFieldState(x + 1, y) == MapState::State::Empty)
	{
		for (unsigned int i = x + 1; i < MapState::rowsCount; i++) {
			fieldState = state.GetFieldState(i, y);
			if (checkMovePossibillityOnField(fieldState, nextPlayerState, &opponentPieceFound, &ownPieceFound)) {
				foundFields->push_back({ i, y, x, y, LineDirection::East });
				break;
			}
		}
	}
	return;
}

void SI::Reversi::StateGenerator::checkVerticalLine(unsigned int x, unsigned int y, 
	const MapState & state, const MapState::State nextPlayerState, std::vector<PossibleAndCurrentFields>* foundFields)
{
	bool opponentPieceFound = false;
	bool ownPieceFound = false;
	MapState::State fieldState;
	if (state.GetFieldState(x, y-1) != MapState::State::Empty)
	{
		for (unsigned int i = y - 1; i > 0; i--) {
			fieldState = state.GetFieldState(x, i);
			if (checkMovePossibillityOnField(fieldState, nextPlayerState, &opponentPieceFound, &ownPieceFound)) {
				foundFields->push_back({ x, i, x, y, LineDirection::North });
				break;
			}
		}
	}

	opponentPieceFound = false;
	ownPieceFound = false;
	if (state.GetFieldState(x, y + 1) == MapState::State::Empty)
	{
		for (unsigned int i = y + 1; i < state.rowsCount; i++) {
			fieldState = state.GetFieldState(x, i);
			if (checkMovePossibillityOnField(fieldState, nextPlayerState, &opponentPieceFound, &ownPieceFound)) {
				foundFields->push_back({ x, i, x, y, LineDirection::South });
				break;
			}
		}
	}
	return;
}

void SI::Reversi::StateGenerator::checkDiagonalLineNW_SE(unsigned int x, unsigned int y, 
	const MapState & state, const MapState::State nextPlayerState, std::vector<PossibleAndCurrentFields>* foundFields)
{
	bool opponentPieceFound = false;
	bool ownPieceFound = false;
	MapState::State fieldState;
	if (state.GetFieldState(x - 1, y - 1) != MapState::State::Empty)
	{
		for (unsigned int i = 1; x - i >= 0 || y - i >= 0; i++) {
			fieldState = state.GetFieldState(x - i, y - i);
			if (checkMovePossibillityOnField(fieldState, nextPlayerState, &opponentPieceFound, &ownPieceFound)) {
				foundFields->push_back({ x - i, y - i, x, y, LineDirection::SouthEast});
				break;
			}
		}
	}

	opponentPieceFound = false;
	ownPieceFound = false;
	if (state.GetFieldState(x, y + 1) == MapState::State::Empty)
	{
		for (unsigned int i = 1; x + i < state.colsCount || y + i < state.rowsCount; i++) {
			fieldState = state.GetFieldState(x + i, y + i);
			if (checkMovePossibillityOnField(fieldState, nextPlayerState, &opponentPieceFound, &ownPieceFound)) {
				foundFields->push_back({ x + i, y + i, x, y, LineDirection::NorthWest });
				break;
			}
		}
	}
	return;
}

void SI::Reversi::StateGenerator::checkDiagonalLineNE_SW(unsigned int x, unsigned int y, 
	const MapState & state, const MapState::State nextPlayerState, std::vector<PossibleAndCurrentFields>* foundFields)
{
	bool opponentPieceFound = false;
	bool ownPieceFound = false;
	MapState::State fieldState;
	if (state.GetFieldState(x - 1, y - 1) != MapState::State::Empty)
	{
		for (unsigned int i = 1; x - i >= 0 || y + i < state.rowsCount; i++) {
			fieldState = state.GetFieldState(x - i, y - i);
			if (checkMovePossibillityOnField(fieldState, nextPlayerState, &opponentPieceFound, &ownPieceFound)) {
				foundFields->push_back({ x - i, y + i, x, y, LineDirection::SouthWest });
				break;
			}
		}
	}

	opponentPieceFound = false;
	ownPieceFound = false;
	if (state.GetFieldState(x, y + 1) == MapState::State::Empty)
	{
		for (unsigned int i = 1; x + i < state.colsCount || y - i >= 0; i++) {
			fieldState = state.GetFieldState(x + i, y + i);
			if (checkMovePossibillityOnField(fieldState, nextPlayerState, &opponentPieceFound, &ownPieceFound)) {
				foundFields->push_back({ x + i, y - i, x, y, LineDirection::NorthEast });
				break;
			}
		}
	}
	return;
}

bool SI::Reversi::StateGenerator::checkMovePossibillityOnField(MapState::State fieldState,
	MapState::State nextPlayerState, bool * opponentPieceFound, bool * ownPieceFound)
{
	if (fieldState != nextPlayerState && fieldState != MapState::State::Empty && !opponentPieceFound) {
		*opponentPieceFound = true;
	}
	else if (fieldState == nextPlayerState && !opponentPieceFound) {
		*ownPieceFound = true;
	}
	else if (fieldState == nextPlayerState && opponentPieceFound) {
		return false;
	}
	else if (fieldState == MapState::State::Empty && opponentPieceFound && !ownPieceFound) {
		return true;
	}
	return false;
}

void SI::Reversi::StateGenerator::generateNewStatesBasedOnFoundPoints(MapState::State nextPlayerState, std::vector<PossibleAndCurrentFields>* foundFields, 
	const MapState & state, std::vector<MapState>* newMapStates)
{

	for (int i = 0; i < foundFields->size(); i++) {
		int xDifference = abs(foundFields->at(i).currX - foundFields->at(i).newX);
		int yDifference = abs(foundFields->at(i).currY - foundFields->at(i).newY);
		if (validateMove(xDifference, yDifference)) {
			if (i != 0) {
				if(newMapStates->at(i - 1) != state)	//checking if there was an attempt to generate the same state twice
					newMapStates->push_back(MapState(state));
			}
			else
				newMapStates->push_back(MapState(state));

			setNewFieldState(nextPlayerState, newMapStates, foundFields, i);
		}
	}
}

void SI::Reversi::StateGenerator::setNewFieldState(MapState::State nextPlayerState, std::vector<MapState>* newMapStates,
	std::vector<PossibleAndCurrentFields>* foundFields, int i)
{
	PossibleAndCurrentFields currentFields = foundFields->at(i);
	std::vector<PossibleAndCurrentFields> fieldsWithSameDestination = getDuplicates(foundFields, currentFields);

	if (fieldsWithSameDestination.size() == 0)	//this means, that the state we are about to generate was already generated before
		return;

	unsigned int incrementX, incrementY, x, y, xDifference, yDifference;
	
	for (int k = 0; k < fieldsWithSameDestination.size(); k++) 
	{
		currentFields = fieldsWithSameDestination.at(k);
		xDifference = abs(currentFields.currX - currentFields.newX);
		yDifference = abs(currentFields.currY - currentFields.newY);
		x = currentFields.currX;
		y = currentFields.currY;
		setIncrementalValuesAccordingToDirection(&incrementX, &incrementY, currentFields.direction);
		for (unsigned int i = 0; i < max(xDifference, yDifference); i++) 
		{
			newMapStates->at(newMapStates->size() - 1).SetFieldState(x, y, nextPlayerState);
			x += incrementX;
			y += incrementY;
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

std::vector<PossibleAndCurrentFields> SI::Reversi::StateGenerator::getDuplicates(std::vector<PossibleAndCurrentFields>* foundFields, 
	PossibleAndCurrentFields currentFields)
{
	std::vector<PossibleAndCurrentFields> Duplicates = std::vector<PossibleAndCurrentFields>();
	for (int i = 0; i < foundFields->size(); i++) {
		if (foundFields->at(i).newX == currentFields.newX && foundFields->at(i).newY == currentFields.newY) {
			if (i != 0 && foundFields->at(i).currX == currentFields.currX && foundFields->at(i).currY == currentFields.currY)
				return std::vector<PossibleAndCurrentFields>();
			Duplicates.push_back(foundFields->at(i));
		}
	}
	return std::vector<PossibleAndCurrentFields>(Duplicates);
}

MapState SI::Reversi::StateGenerator::GetNextState()
{
	return MapState();
}

bool SI::Reversi::StateGenerator::HasNextState()
{
	return false;
}

void SI::Reversi::StateGenerator::Reset()
{
}

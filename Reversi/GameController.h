#pragma once
#include "GameWindow.h"
#include "PlayerColor.h"
#include "GameBoard.h"

#include <thread>

class GameWindow;


class GameController {
public:
	GameController(GameWindow* handle);
	void pawnPlaced(int x, int y);
	void closeMainLoop();
	sf::String getCommunicate();
	GameBoard getGameBoard();
private:
	GameWindow* handle;
	GameBoard gameBoard;
	PlayerColor playerNumber;
	bool continueLoop;
	bool pawnWasPlaced;
	int placedPawnLocationX;
	int placedPawnLocationY;
	sf::String communicate;
	void mainLoop();
	bool checkIfMoveIsLegal(int x, int y, PlayerColor color);
	void changeGameBoardState(int x, int y, PlayerColor color);
	int countPawns(PlayerColor color);
};
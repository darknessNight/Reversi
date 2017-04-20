#pragma once
#include "GameWindow.h"
#include "PlayerColor.h"
#include "GameBoard.h"
#include "SiPlayer.h"

#include <thread>

class GameWindow;


class GameController {
private:
	const unsigned minTreeDepth = 2;
	const int gameMode = 1;
public:
	GameController(GameWindow* handle);
	void pawnPlaced(int x, int y);
	void closeMainLoop();
	sf::String getCommunicate();
	GameBoard getGameBoard();
	void reset();
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
	bool hasMove(PlayerColor color);
};
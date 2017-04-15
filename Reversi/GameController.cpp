#include "GameController.h"


GameController::GameController(GameWindow* handle) {
	this->handle = handle;

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			this->gameBoard.setSquare(i, j, PlayerColor::EmptyField);
		}
	}
	this->gameBoard.setSquare(3, 3, PlayerColor::WhitePlayer);
	this->gameBoard.setSquare(4, 4, PlayerColor::WhitePlayer);
	this->gameBoard.setSquare(3, 4, PlayerColor::BlackPlayer);
	this->gameBoard.setSquare(4, 3, PlayerColor::BlackPlayer);

	communicate = L"Pocz¹tek gry. Czarne zaczynaj¹.";
	this->handle->doDraw();

	this->playerNumber = PlayerColor::BlackPlayer;

	this->continueLoop = true;
	this->pawnWasPlaced = false;

	std::thread(&GameController::mainLoop, this).detach();
}


void GameController::pawnPlaced(int x, int y) {
	placedPawnLocationX = x;
	placedPawnLocationY = y;
	pawnWasPlaced = true;
}


void GameController::mainLoop() {
	while(continueLoop) {
		if (pawnWasPlaced) {
			this->gameBoard.setSquare(placedPawnLocationX, placedPawnLocationY, playerNumber);

			if (playerNumber == PlayerColor::BlackPlayer) {
				playerNumber = PlayerColor::WhitePlayer;
			}
			else {
				playerNumber = PlayerColor::BlackPlayer;
			}

			communicate = L"Placeholder.";
			this->handle->doDraw();

			pawnWasPlaced = false;
		}
		_sleep(100);
	}
}


void GameController::closeMainLoop() {
	continueLoop = false;
}


sf::String GameController::getCommunicate(){
	return communicate;
}


GameBoard GameController::getGameBoard() {
	return gameBoard;
}
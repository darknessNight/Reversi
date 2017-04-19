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

	communicate = L"Pocz�tek gry. Czarne zaczynaj�.";
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

			if (checkIfMoveIsLegal(placedPawnLocationX, placedPawnLocationY, playerNumber)) {

				changeGameBoardState(placedPawnLocationX, placedPawnLocationY, playerNumber);


				if (playerNumber == PlayerColor::BlackPlayer) {
					playerNumber = PlayerColor::WhitePlayer;
					communicate = L"Ruch bia�ego.";
				}
				else {
					playerNumber = PlayerColor::BlackPlayer;
					communicate = L"Ruch czarnego.";
				}

				
				this->handle->doDraw();
			}

			pawnWasPlaced = false;

			this->handle->allowToClick();
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


bool GameController::checkIfMoveIsLegal(int x, int y, PlayerColor color) {

	if (gameBoard.getSquare(x, y) != PlayerColor::EmptyField) {
		return false;
	}

	PlayerColor oppositColor;
	if (color == PlayerColor::BlackPlayer) {
		oppositColor = PlayerColor::WhitePlayer;
	}
	else {
		oppositColor = PlayerColor::BlackPlayer;
	}

	bool metOppositColor = false;

	for (int xchange = -1; xchange < 2; xchange++) {
		for (int ychange = -1; ychange < 2; ychange++) {
			int tempx = x, tempy = y;
			metOppositColor = false;
			while (tempx+xchange!=8 && tempx+xchange!=-1 && tempy+ychange!=8 && tempy+ychange!=-1) {
				tempx += xchange;
				tempy += ychange;
				if (gameBoard.getSquare(tempx, tempy) == oppositColor) {
					metOppositColor = true;
				}
				else if (gameBoard.getSquare(tempx, tempy) == PlayerColor::EmptyField) {
					break;
				}
				else if (gameBoard.getSquare(tempx, tempy) == color) {
					if (metOppositColor) {
						return true;
					}
					else{
						break;
					}
				}
			}
		}
	}

	return false;
}


void GameController::changeGameBoardState(int x, int y, PlayerColor color) {

	PlayerColor oppositColor;
	if (color == PlayerColor::BlackPlayer) {
		oppositColor = PlayerColor::WhitePlayer;
	}
	else {
		oppositColor = PlayerColor::BlackPlayer;
	}

	bool metOppositColor = false;

	gameBoard.setSquare(x, y, color);

	for (int xchange = -1; xchange < 2; xchange++) {
		for (int ychange = -1; ychange < 2; ychange++) {
			int tempx = x, tempy = y;
			metOppositColor = false;
			while (tempx + xchange != 8 && tempx + xchange != -1 && tempy + ychange != 8 && tempy + ychange != -1) {
				tempx += xchange;
				tempy += ychange;
				if (gameBoard.getSquare(tempx, tempy) == oppositColor) {
					metOppositColor = true;
				}
				else if (gameBoard.getSquare(tempx, tempy) == PlayerColor::EmptyField) {
					break;
				}
				else if (gameBoard.getSquare(tempx, tempy) == color) {
					if (metOppositColor) {
						while (tempx - xchange != x || tempy - ychange != y) {
							tempx -= xchange;
							tempy -= ychange;
							gameBoard.setSquare(tempx, tempy, color);
						}
						break;
					}
					else {
						break;
					}
				}
			}
		}
	}
}
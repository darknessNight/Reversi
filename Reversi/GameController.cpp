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

	this->handle->draw(gameBoard, L"Pocz¹tek gry. Czarne zaczynaj¹.");

	this->playerNumber = PlayerColor::BlackPlayer;
}


void GameController::pawnPlaced(int x, int y) {
	this->gameBoard.setSquare(x, y, playerNumber);

	if (playerNumber == PlayerColor::BlackPlayer) {
		playerNumber = PlayerColor::WhitePlayer;
	}
	else {
		playerNumber = PlayerColor::BlackPlayer;
	}

	this->handle->draw(gameBoard, L"Placeholder.");
}
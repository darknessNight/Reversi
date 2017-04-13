#include "GameController.h"


GameController::GameController(GameWindow* handle) {
	this->handle = handle;

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			this->gameBoard[i][j] = 0;
		}
	}
	this->gameBoard[3][3] = 1;
	this->gameBoard[4][4] = 1;
	this->gameBoard[3][4] = 2;
	this->gameBoard[4][3] = 2;

	this->handle->draw(gameBoard, L"Pocz¹tek gry");
}
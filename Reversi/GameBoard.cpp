#include "GameBoard.h"



PlayerColor GameBoard::getSquare(int x, int y) const{
	return this->board[x][y];
}


void GameBoard::setSquare(int x, int y, PlayerColor value) {
	this->board[x][y] = value;
}
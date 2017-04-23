#pragma once
#include <SFML/Graphics.hpp>

#include "GameController.h"
#include "GameBoard.h"
#include "PlayerColor.h"

class GameController;

#define SQUARESIZE 60
#define FONTSIZE 20


class GameWindow {
public:
	void start();
	void doDraw();
	void allowToClick();
private:
	sf::RenderWindow window;
	bool shouldRedrawWindow;
	bool canClick;
	void draw(GameBoard gameBoard, sf::String communicate, bool drawHelp = false, GameBoard mapOfPossibleMoves = {});
};
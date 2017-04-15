#pragma once
#include <SFML/Graphics.hpp>

#include "GameController.h"
#include "GameBoard.h"
#include "PlayerColor.h"

#define SQUARESIZE 60
#define FONTSIZE 20


class GameWindow {
public:
	void start();
	void doDraw();
private:
	sf::RenderWindow window;
	bool shouldRedrawWindow;
	void draw(GameBoard gameBoard, sf::String communicate);
};
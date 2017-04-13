#pragma once
#include <SFML/Graphics.hpp>

#include "GameController.h"

#define SQUARESIZE 60
#define FONTSIZE 20


class GameWindow {
public:
	void start();
	void draw(char gameBoard[8][8], sf::String communicate);
private:
	sf::RenderWindow window;
};
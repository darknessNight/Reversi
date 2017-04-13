#include "GameWindow.h"


void GameWindow::Start() {

	this->window.create(sf::VideoMode(8*SQUARESIZE, 8*SQUARESIZE+100), "Reversi");
	window.setPosition(sf::Vector2i(0, 0));


	while (window.isOpen())
	{

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}

	}

}
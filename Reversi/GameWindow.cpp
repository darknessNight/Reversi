#include "GameWindow.h"


void GameWindow::start() {

	this->window.create(sf::VideoMode(8*SQUARESIZE, 8*SQUARESIZE+100), "Reversi");
	this->window.setPosition(sf::Vector2i(0, 0));
	GameController gameController(this);
	int temporaryX, temporaryY;

	while (window.isOpen())
	{

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) {
				window.close();
			}
			if (event.type == sf::Event::MouseButtonPressed) {
				if (event.mouseButton.button == sf::Mouse::Left) {
					temporaryX = event.mouseButton.x/SQUARESIZE;
					temporaryY = event.mouseButton.y/SQUARESIZE;
				}
			}
			if (event.type == sf::Event::MouseButtonReleased) {
				if (event.mouseButton.button == sf::Mouse::Left) {
					if (event.mouseButton.x/SQUARESIZE == temporaryX && event.mouseButton.y/SQUARESIZE == temporaryY) {
						gameController.pawnPlaced(temporaryX, temporaryY);
					}
				}
			}
		}

	}

}


void GameWindow::draw(char gameBoard[8][8], sf::String communicate) {
	sf::RectangleShape board(sf::Vector2f(8*SQUARESIZE, 8*SQUARESIZE));
	sf::Texture boardTexture;
	boardTexture.loadFromFile("BoardTexture.png");
	boardTexture.setRepeated(true);
	board.setTexture(&boardTexture,false);
	board.setTextureRect(sf::IntRect(0, 0, 800, 800));

	sf::CircleShape pawn(SQUARESIZE/2-1);

	sf::Font font;
	sf::Text text;
	font.loadFromFile("Calibri.ttf");
	text.setFont(font);
	text.setCharacterSize(FONTSIZE);
	text.setFillColor(sf::Color::White);
	text.setString(communicate);
	text.setPosition(sf::Vector2f(0,8*SQUARESIZE+20));

	window.clear(sf::Color::Black);

	window.draw(board);

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (gameBoard[i][j] == 1) {
				pawn.setFillColor(sf::Color::White);
				pawn.setPosition(sf::Vector2f(i*SQUARESIZE+1,j*SQUARESIZE+1));
				window.draw(pawn);
			}
			else if (gameBoard[i][j] == 2) {
				pawn.setFillColor(sf::Color::Black);
				pawn.setPosition(sf::Vector2f(i*SQUARESIZE + 1, j*SQUARESIZE + 1));
				window.draw(pawn);
			}
		}
	}

	window.draw(text);

	window.display();
}
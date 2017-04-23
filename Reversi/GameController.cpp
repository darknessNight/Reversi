#include "GameController.h"



GameController::GameController(GameWindow* handle)
{
	this->handle = handle;

	int stany[8][8] = {
		{ 0,0,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,0,0 },
		{ 0,0,0,2,1,0,0,0 },
		{ 0,0,0,1,2,0,0,0 },
		{ 0,0,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,0,0 },
	};
	

	for ( int i = 0; i < 8; i++ )
	{
		for ( int j = 0; j < 8; j++ )
		{
			switch ( stany[j][i] )
			{
			case 0:
				this->gameBoard.setSquare(i, j, PlayerColor::EmptyField);
				break;
			case 1:
				this->gameBoard.setSquare(i, j, PlayerColor::BlackPlayer);
				break;
			case 2:
				this->gameBoard.setSquare(i, j, PlayerColor::WhitePlayer);
				break;
			}
		}
	}

	communicate = L"Pocz¹tek gry. Czarne zaczynaj¹.";
	this->handle->doDraw();

	this->playerNumber = PlayerColor::BlackPlayer;

	this->continueLoop = true;
	this->pawnWasPlaced = false;

	std::thread(&GameController::mainLoop, this).detach();
}


void GameController::pawnPlaced(int x, int y)
{
	placedPawnLocationX = x;
	placedPawnLocationY = y;
	pawnWasPlaced = true;
}


void GameController::mainLoop()
{
	PlayerColor oppsitePlayer;

	if (gameMode == 1) {
		siPlayer = std::make_shared<Reversi::SiPlayer>(gameBoard, PlayerColor::WhitePlayer, minTreeDepth);

		askSI = true;

		siPlayer->setCallback([&](int x, int y) {
			this->pawnPlaced(x, y);
			askSI = true;
		});
	}

	while ( continueLoop )
	{
		if ( pawnWasPlaced )
		{

			if ( checkIfMoveIsLegal(placedPawnLocationX, placedPawnLocationY, playerNumber) )
			{

				changeGameBoardState(placedPawnLocationX, placedPawnLocationY, playerNumber);

				if ( playerNumber == PlayerColor::BlackPlayer )
				{
					oppsitePlayer = PlayerColor::WhitePlayer;
				}
				else
				{
					oppsitePlayer = PlayerColor::BlackPlayer;
				}

				if ( hasMove(oppsitePlayer) )
				{//kolejny gracz mo¿e wykonaæ ruch
					if ( playerNumber == PlayerColor::BlackPlayer )
					{//teraz gra bia³y
						playerNumber = PlayerColor::WhitePlayer;
						communicate = L"Ruch bia³ego. Czarne:";
						communicate += std::to_string(countPawns(PlayerColor::BlackPlayer));
						communicate += L" Bia³e:";
						communicate += std::to_string(countPawns(PlayerColor::WhitePlayer));
					}
					else
					{//teraz gra czarny
						playerNumber = PlayerColor::BlackPlayer;
						communicate = L"Ruch czarnego. Czarne:";
						communicate += std::to_string(countPawns(PlayerColor::BlackPlayer));
						communicate += L" Bia³e:";
						communicate += std::to_string(countPawns(PlayerColor::WhitePlayer));
					}
				}
				else
				{
					if ( hasMove(playerNumber) )
					{//kolejny gracz straci³ ruch, jeszcze raz ten sam
						if ( playerNumber == PlayerColor::BlackPlayer )
						{//znowu czarny
							siPlayer->PassMove(gameBoard);
							communicate = L"Bia³y traci ruch. Ruch czarnego. Czarne:";
							communicate += std::to_string(countPawns(PlayerColor::BlackPlayer));
							communicate += L" Bia³e:";
							communicate += std::to_string(countPawns(PlayerColor::WhitePlayer));
						}
						else
						{//znowu bia³y
							communicate = L"Czarny traci ruch. Ruch bia³ego. Czarne:";
							communicate += std::to_string(countPawns(PlayerColor::BlackPlayer));
							communicate += L" Bia³e:";
							communicate += std::to_string(countPawns(PlayerColor::WhitePlayer));
						}
					}
					else
					{//koniec gry
						if ( countPawns(PlayerColor::BlackPlayer) > countPawns(PlayerColor::WhitePlayer) )
						{//wygra³ czarny
							communicate = L"Wygra³ czarny gracz. Czarne:";
							communicate += std::to_string(countPawns(PlayerColor::BlackPlayer));
							communicate += L" Bia³e:";
							communicate += std::to_string(countPawns(PlayerColor::WhitePlayer));
						}
						else if (countPawns(PlayerColor::BlackPlayer) < countPawns(PlayerColor::WhitePlayer))
						{//wygra³ bia³y
							communicate = L"Wygra³ bia³y gracz. Czarne:";
							communicate += std::to_string(countPawns(PlayerColor::BlackPlayer));
							communicate += L" Bia³e:";
							communicate += std::to_string(countPawns(PlayerColor::WhitePlayer));
						}
						else
						{//remis
							communicate = L"Remis. Czarne:";
							communicate += std::to_string(countPawns(PlayerColor::BlackPlayer));
							communicate += L" Bia³e:";
							communicate += std::to_string(countPawns(PlayerColor::WhitePlayer));
						}
						gameEnded = true;
					}
				}

				this->handle->doDraw();
			}
			else
			{

			}

			if ( !gameEnded )
			{
				if ( gameMode == 2 )
					this->handle->allowToClick();
				else if ( playerNumber == PlayerColor::BlackPlayer )
					this->handle->allowToClick();
				else if ( askSI )
				{
					askSI = false;
					siPlayer->StartMove(gameBoard);
				}
			}

			pawnWasPlaced = false;
		}
		_sleep(100);
	}

	if (gameMode == 1) {
		siPlayer = nullptr;
	}
}


void GameController::closeMainLoop()
{
	continueLoop = false;
}


sf::String GameController::getCommunicate()
{
	return communicate;
}


GameBoard GameController::getGameBoard()
{
	return gameBoard;
}


bool GameController::checkIfMoveIsLegal(int x, int y, PlayerColor color)
{
	if (x > 7 || x < 0 || y>7 || y < 0) {
		return false;
	}

	if ( gameBoard.getSquare(x, y) != PlayerColor::EmptyField )
	{
		return false;
	}

	PlayerColor oppositColor;
	if ( color == PlayerColor::BlackPlayer )
	{
		oppositColor = PlayerColor::WhitePlayer;
	}
	else
	{
		oppositColor = PlayerColor::BlackPlayer;
	}

	bool metOppositColor = false;

	for ( int xchange = -1; xchange < 2; xchange++ )
	{
		for ( int ychange = -1; ychange < 2; ychange++ )
		{
			int tempx = x, tempy = y;
			metOppositColor = false;
			while ( tempx + xchange != 8 && tempx + xchange != -1 && tempy + ychange != 8 && tempy + ychange != -1 )
			{
				tempx += xchange;
				tempy += ychange;
				if ( gameBoard.getSquare(tempx, tempy) == oppositColor )
				{
					metOppositColor = true;
				}
				else if ( gameBoard.getSquare(tempx, tempy) == PlayerColor::EmptyField )
				{
					break;
				}
				else if ( gameBoard.getSquare(tempx, tempy) == color )
				{
					if ( metOppositColor )
					{
						return true;
					}
					else
					{
						break;
					}
				}
			}
		}
	}

	return false;
}


void GameController::changeGameBoardState(int x, int y, PlayerColor color)
{

	PlayerColor oppositColor;
	if ( color == PlayerColor::BlackPlayer )
	{
		oppositColor = PlayerColor::WhitePlayer;
	}
	else
	{
		oppositColor = PlayerColor::BlackPlayer;
	}

	bool metOppositColor = false;

	gameBoard.setSquare(x, y, color);

	for ( int xchange = -1; xchange < 2; xchange++ )
	{
		for ( int ychange = -1; ychange < 2; ychange++ )
		{
			int tempx = x, tempy = y;
			metOppositColor = false;
			while ( tempx + xchange != 8 && tempx + xchange != -1 && tempy + ychange != 8 && tempy + ychange != -1 )
			{
				tempx += xchange;
				tempy += ychange;
				if ( gameBoard.getSquare(tempx, tempy) == oppositColor )
				{
					metOppositColor = true;
				}
				else if ( gameBoard.getSquare(tempx, tempy) == PlayerColor::EmptyField )
				{
					break;
				}
				else if ( gameBoard.getSquare(tempx, tempy) == color )
				{
					if ( metOppositColor )
					{
						while ( tempx - xchange != x || tempy - ychange != y )
						{
							tempx -= xchange;
							tempy -= ychange;
							gameBoard.setSquare(tempx, tempy, color);
						}
						break;
					}
					else
					{
						break;
					}
				}
			}
		}
	}
}


int GameController::countPawns(PlayerColor color)
{
	int counter = 0;

	for ( int x = 0; x < 8; x++ )
	{
		for ( int y = 0; y < 8; y++ )
		{
			if ( gameBoard.getSquare(x, y) == color )
			{
				counter++;
			}
		}
	}

	return counter;
}


bool GameController::hasMove(PlayerColor color)
{
	for ( int x = 0; x < 8; x++ )
	{
		for ( int y = 0; y < 8; y++ )
		{
			if ( checkIfMoveIsLegal(x, y, color) )
			{
				return true;
			}
		}
	}

	return false;
}


void GameController::reset()
{
	for ( int i = 0; i < 8; i++ )
	{
		for ( int j = 0; j < 8; j++ )
		{
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

	if (gameMode == 1) {
		siPlayer = nullptr;
		siPlayer = std::make_shared<Reversi::SiPlayer>(gameBoard, PlayerColor::WhitePlayer, minTreeDepth);
		siPlayer->setCallback([&](int x, int y) {
			this->pawnPlaced(x, y);
			askSI = true;
		});
	}

	handle->allowToClick();

	gameEnded = false;
}
#include "game.h"

Game::Game(UINT M, UINT N, UINT R1, UINT P1, UINT S1, UINT B1, UINT J1, UINT F1,
						   UINT R2, UINT P2, UINT S2, UINT B2, UINT J2, UINT F2)
	:M(M), N(N), R1(R1), P1(P1), S1(S1), B1(B1), J1(J1), F1(F1),
				 R2(R2), P2(P2), S2(S2), B2(B2), J2(J2), F2(F2)
{
	board = new Board(M, N);
	player1 = new Player(R1, P1, S1, B1, J1, F1);
	player2 = new Player(R2, P2, S2, B2, J2, F2);
	turn = 0;
}

void Game::runMove()
{
	if (turn == 0) //player 1's turn
		runSingleMove(player1, player2);
	else //player 2's turn
		runSingleMove(player2, player1);
	turn += 1 % 2;
}

void Game::runSingleMove(Player* player, Player* against)
{
	UINT fromX, fromY, toX, toY;

	if (player->getHasMoreMoves())
	{
		if (player->getNextMove(&fromX, &fromY, &toX, &toY))
			board->movePiece(fromX, fromY, toX, toY);
		else
			player->setHasMoreMoves(false);
	}
}

void Game::checkPlayersFlags(Player * player)
{
	if (player->getTypeCount(FLAG) != player->getTypeMax(FLAG))
	{
		player->setHasLost();
		player->setReason(BAD_POSITIONING_INPUT_FILE);
	}
}

bool Game::endGame()
{
	if (player1->isAlive() && player2->isAlive() && (player1->getHasMoreMoves() 
		|| player2->getHasMoreMoves()))
		return false;
	return true;
}

void Game::flagsCheck()
{
	checkPlayersFlags(player1);
	checkPlayersFlags(player2);
}



#include "game.h"

Game::Game(UINT M, UINT N, UINT R1, UINT P1, UINT S1, UINT B1, UINT J1, UINT F1,
						   UINT R2, UINT P2, UINT S2, UINT B2, UINT J2, UINT F2, FileParser* fileParser)
	:M(M), N(N), R1(R1), P1(P1), S1(S1), B1(B1), J1(J1), F1(F1),
				 R2(R2), P2(P2), S2(S2), B2(B2), J2(J2), F2(F2), fileParser(fileParser)
{
	board = new Board(N, M);
	player1 = new Player(R1, P1, S1, B1, J1, F1, fileParser->getPlayerFileContext(0));
	player2 = new Player(R2, P2, S2, B2, J2, F2, fileParser->getPlayerFileContext(1));
	turn = 0;
}

void Game::runMove()
{
	if (turn == 0) //player 1's turn
		runSingleMove(player1);
	else //player 2's turn
		runSingleMove(player2);
	turn += 1 % 2;
}

void Game::runSingleMove(Player* player)
{
	UINT fromX, fromY, toX, toY;
	bool isJoker;
	UINT jokerX, jokerY;
	ePieceType newRep;

	if (player->getHasMoreMoves())
	{
		if (player->getNextMove(&fromX, &fromY, &toX, &toY, &isJoker, &jokerX, &jokerY, &newRep))
		{
			board->movePiece(fromX, fromY, toX, toY);
			if (isJoker)
				board->changeJokerType(jokerX, jokerY, newRep);
		}
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

void Game::positionAllPieces()
{
	/* Initialize temporary boolean array to check if theres pieces from the same player*/
	bool** tmpBoard = new bool*[N];

	for (int i = 0; i < N; i++)
		tmpBoard[i] = new bool[M]();	//() initializes the elements to false

	positionPlayerPieces(player1, tmpBoard);

	for (int i = 0; i < N; i++)
		for (int j = 0; j < M; j++)
			tmpBoard[i][j] = false;

	positionPlayerPieces(player2, tmpBoard);
}

void Game::positionPlayerPieces(Player* p, bool** tmpBoard)
{
	UINT x, y;
	ePieceType type, jokerType;
	eFileStatus status;
	Piece* piece;
	PlayerFileContext* pfc = p->getPlayerFileContext();

	while (true)
	{
		status = pfc->getNextPiece(&type, &x, &y, &jokerType);

		switch (status)
		{
		case FILE_EOF_REACHED:
			return;

		case FILE_ERROR:
		case FILE_BAD_FORMAT:
			p->setHasLost();
			p->setReason(BAD_POSITIONING_INPUT_FILE_FORMAT);
			break;

		case FILE_SUCCESS:
			if (tmpBoard[y, x])
			{
				p->setHasLost();
				p->setReason(BAD_POSITIONING_INPUT_FILE_DOUBLE_POSITION);
			}
			else
			{
				tmpBoard[y][x] = true;
			}
			break;
		}
	}
}

void Game::positionPiece(int player)
{
	if (player == 0)
		positionSinglePiece(player1);
	else
		positionSinglePiece(player2);
}

void Game::positionSinglePiece(Player* player)
{
	ePieceType type, jokerType;
	UINT x, y;
	player->getPlayerFileContext->getNextPiece(&type, &x, &y, &jokerType);
		
}

void Game::flagsCheck()
{
	checkPlayersFlags(player1);
	checkPlayersFlags(player2);
}

int Game::getWinner(eReason* reason)
{
	if (player1->getHasLost() && !player2->getHasLost())
	{
		*reason = player1->getReason();
		return 2; //player2 wins
	}
	if (!player1->getHasLost() && player2->getHasLost())
	{
		*reason = player2->getReason();
		return 1; //player1 wins
	}
	//TIE scenarios:
	if (!player1->getHasMoreMoves() && !player2->getHasMoreMoves())
	{
		*reason = MOVES_INPUT_FILES_DONE;
		return 0;
	}
	if (player1->getReason() == BAD_POSITIONING_INPUT_FILE &&
		player2->getReason() == BAD_POSITIONING_INPUT_FILE)
	{
		*reason = BOTH_BAD_POSITIONING_INPUT_FILE;
		return 0;
	}
	
	if (player1->getTypeCount(FLAG) == 0 && player2->getTypeCount(FLAG) == 0)
	{
		*reason = BOTH_BAD_POSITIONING_INPUT_FILE;
		return 0;
	}
	return 0;
}



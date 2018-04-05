#include "game.h"

Game::Game(UINT M, UINT N, UINT R1, UINT P1, UINT S1, UINT B1, UINT J1, UINT F1,
						   UINT R2, UINT P2, UINT S2, UINT B2, UINT J2, UINT F2, FileParser* fileParser)
	:M(M), N(N), R1(R1), P1(P1), S1(S1), B1(B1), J1(J1), F1(F1),
				 R2(R2), P2(P2), S2(S2), B2(B2), J2(J2), F2(F2), fileParser(fileParser)
{
	board = new Board(N, M);
	player1 = new Player(0, R1, P1, S1, B1, J1, F1, fileParser->getPlayerFileContext(0));
	player2 = new Player(1, R2, P2, S2, B2, J2, F2, fileParser->getPlayerFileContext(1));
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
			//true means getNextMove succeeded
			if (board->movePiece(fromX, fromY, toX, toY))
			{
				//non zero value means some kind of error in move file
				player->setHasLost();
				player->setReason(BAD_MOVES_INPUT_FILE);
				return;
			}

			if (isJoker)
				board->changeJokerType(jokerX, jokerY, newRep);
		}
		else
		{
			player->setHasMoreMoves(false);
		}
	}
}

void Game::checkPlayersFlags(Player * player)
{
	if (player->getTypeCount(FLAG) != player->getTypeMax(FLAG))
	{
		player->setHasLost();
		player->setReason(BAD_POSITIONING_INPUT_FILE_FLAG_NUMBER);
	}
}

bool Game::endGame()
{
	if (player1->isAlive() && player2->isAlive() && (player1->getHasMoreMoves() 
		|| player2->getHasMoreMoves()))
		return false;
	return true;
}

int Game::validatePositionFiles()
{
	/* Initialize temporary boolean array to check if theres pieces from the same player*/
	bool** tmpBoard = new bool*[N];

	for (UINT i = 0; i < N; i++)
		tmpBoard[i] = new bool[M]();	//() initializes the elements to false

	player1->validatePlayerPositions(tmpBoard, N, M);

	dprint("validatePlayerPositions #1 success\n");

	for (UINT i = 0; i < N; i++)
		for (UINT j = 0; j < M; j++)
			tmpBoard[i][j] = false;

	player2->validatePlayerPositions(tmpBoard, N, M);

	dprint("validatePlayerPositions #2 success\n");


	for (UINT i = 0; i < N; i++)
		delete [] tmpBoard[i];

	delete [] tmpBoard;

	if (player1->getHasLost() || player2->getHasLost())
		return -1;
	return 0;


}

void Game::resetPieceFiles()
{
	player1->getPlayerFileContext()->setPieceFileToStart();
	player2->getPlayerFileContext()->setPieceFileToStart();
}


int Game::positionPiece(int player)
{
	if (player == 0)
		return positionSinglePiece(player1);
	else
		return positionSinglePiece(player2);
}

int Game::positionSinglePiece(Player* player)
{
	ePieceType type, jokerType;
	UINT x, y;

	if (!player->getNextPiece(&type, &x, &y, &jokerType))
	{
		Piece* p = createNewPiece(player, type, jokerType);
		board->positionPiece(p, x, y);
		return 0;
	}

	return -1;
}

void Game::flagsCheck()
{
	checkPlayersFlags(player1);
	checkPlayersFlags(player2);
}

int Game::getWinner(eReason* pReason)
{
	int winner = 0;
	eReason reason = INVALID_REASON;

	if (player1->getHasLost() && !player2->getHasLost())
	{
		reason = player1->getReason();
		winner = 2; //player2 wins
	}

	else if (!player1->getHasLost() && player2->getHasLost())
	{
		reason = player2->getReason();
		winner = 1; //player1 wins
	}

	/*********** Tie Scenarios ***********/

	else if (!player1->getHasMoreMoves() && !player2->getHasMoreMoves())
	{
		reason = MOVES_INPUT_FILES_DONE;
	}

	else if (player1->getReason() == player2->getReason())
	{
		reason = (eReason) MAKE_REASON_DOUBLE(player1->getReason());
	}
	
	else if (player1->getTypeCount(FLAG) == 0 && player2->getTypeCount(FLAG) == 0)
	{
		reason = BOTH_BAD_POSITIONING_INPUT_FILE_FLAG_NUMBER;
	}

	*pReason = reason;

	return winner;
}

string Game::GetReasonString(eReason reason)
{
	char temp[100];
	int playerNumber, line, line2;

	switch (reason)
	{
	case FLAGS_CAPTURED:
		return "All flags of the opponent are captured";

	case PIECES_EATEN:
		return "All moving PIECEs of the opponent are eaten";

	case MOVES_INPUT_FILES_DONE:
		return "A tie - both Moves input files done without a winner";

	case ALL_FLAGS_EATEN_DURING_POSITIONING:
		return "A tie - all flags are eaten by both players in the position files";

	case BAD_POSITIONING_INPUT_FILE_FORMAT:
	case BAD_POSITIONING_INPUT_FILE_DOUBLE_POSITION:
	case BAD_POSITIONING_INPUT_FILE_PIECE_NUMBER:
	case BAD_POSITIONING_INPUT_FILE_FLAG_NUMBER:
		if (player1->getHasLost())
		{
			playerNumber = 0;
			line = player1->getPlayerFileContext()->pieces->getCurrentLineNum();
		}
		else
		{
			playerNumber = 1;
			line = player2->getPlayerFileContext()->pieces->getCurrentLineNum();
		}
		sprintf(temp, "Bad Positioning input file for player %d - line %d", playerNumber, line);
		return (string)temp;

	case BOTH_BAD_POSITIONING_INPUT_FILE_FORMAT:
	case BOTH_BAD_POSITIONING_INPUT_FILE_DOUBLE_POSITION:
	case BOTH_BAD_POSITIONING_INPUT_FILE_PIECE_NUMBER:
	case BOTH_BAD_POSITIONING_INPUT_FILE_FLAG_NUMBER:
		line = player1->getPlayerFileContext()->pieces->getCurrentLineNum();
		line2 = player2->getPlayerFileContext()->pieces->getCurrentLineNum();

		sprintf(temp, "Bad Positioning input file for both players - player 1: line %d, player 2: line %d", line, line2);
		return (string)temp;

	default:
		return "Invalid eReason code";
	}
}

void Game::writeOutputFile()
{
	UINT rows, cols, i, j;
	Piece* piece;
	bool isPlayer1 = false;
	char c;
	int winner;
	eReason reason;

	winner = getWinner(&reason);

	board->getBoardDimensions(&cols, &rows);
	fileParser->output->file.seekp(ios::beg);

	fileParser->output->file << "Winner: " << winner << '\n';
	fileParser->output->file << "Reason: " << GetReasonString(reason) << "\n\n";


	for (i = 0; i < rows; i++)
	{
		for (j = 0; j < cols; j++)
		{
			piece = board->getPieceAt(j, i);
			isPlayer1 = (piece->getOwner() == player1);

			if (!piece)
			{
				c = ' ';
			}
			else
			{
				c = pieceToChar(piece->getOriginalType(), isPlayer1);
				if (c < 0)
				{
					cout << "Bad piece returned from function" << endl;
					return;
				}
			}

			fileParser->output->file << c;
		}
		fileParser->output->file << '\n';
	}
}
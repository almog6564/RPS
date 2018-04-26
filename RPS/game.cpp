#include "game.h"

Game::Game(UINT M, UINT N, UINT R1, UINT P1, UINT S1, UINT B1, UINT J1, UINT F1,
						   UINT R2, UINT P2, UINT S2, UINT B2, UINT J2, UINT F2, FileParser* fileParser)
	:M(M), N(N), R1(R1), P1(P1), S1(S1), B1(B1), J1(J1), F1(F1),
				 R2(R2), P2(P2), S2(S2), B2(B2), J2(J2), F2(F2), fileParser(fileParser)
{
	board = new MyBoard(N, M);
	//player1 = new Player(0, R1, P1, S1, B1, J1, F1, fileParser->getPlayerFileContext(0));
	//player2 = new Player(1, R2, P2, S2, B2, J2, F2, fileParser->getPlayerFileContext(1));
	turn = 0;
}

Game::~Game()
{
	delete board;
	delete player1;
	delete player2;
}

void Game::runMove()
{
	if (turn == 0) //player 1's turn
		runSingleMove(player1);
	else //player 2's turn
		runSingleMove(player2);
	turn += 1;
	turn %= 2;
}

void Game::runSingleMove(Player* player)
{
	UINT fromX, fromY, toX, toY;
	bool isJoker;
	UINT jokerX, jokerY;
	ePieceType newRep;

	dprint("\n\n ************ Starting MOVE: Player #%d ************ \n\n", player->getPlayerId()+1);

	if (player->getHasMoreMoves())
	{
		if (player->getNextMove(&fromX, &fromY, &toX, &toY, &isJoker, &jokerX, &jokerY, &newRep))
		{
			//true means getNextMove succeeded
			if (board->movePiece(player->getPlayerId(), fromX, fromY, toX, toY))
			{
				//non zero value means some kind of error in move file
				player->setHasLost();
				player->setReason(BAD_MOVES_INPUT_FILE);
				return;
			}

			if (isJoker)
			{
				if (board->changeJokerType(jokerX, jokerY, newRep))
				{
					player->setHasLost();
					player->setReason(BAD_MOVES_INPUT_FILE);
					return;
				}
			}

			checkWhetherFlagsWereCaptured();
				
		}
		else
		{
			player->setHasMoreMoves(false);
		}
	}
}

void Game::checkPlayersFlagCountLessThanMax(Player * player)
{
	if (player->getHasLost())
		return;

	if (player->getTypeCount(FLAG) != player->getTypeMax(FLAG) &&		//less flags that were supposed to be
		player->getTypeCount(FLAG) == player->getOriginalFlagCount())	//no flags were eaten
	{
		player->setHasLost();
		player->setReason(BAD_POSITIONING_INPUT_FILE_FLAG_NUMBER);
		printf("ERROR [player %d] - %d flags were supposed to be positioned on screen but only %d were in fact positioned\n", 
			player->getPlayerId()+1, player->getTypeMax(FLAG), player->getTypeCount(FLAG));
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

	dprint("validatePlayerPositions #1 %s\n",player1->getHasLost() ? "FAILED" : "SUCCESS");

	for (UINT i = 0; i < N; i++)
		for (UINT j = 0; j < M; j++)
			tmpBoard[i][j] = false;

	player2->validatePlayerPositions(tmpBoard, N, M);

	dprint("validatePlayerPositions #2 %s\n", player2->getHasLost() ? "FAILED" : "SUCCESS");


	for (UINT i = 0; i < N; i++)
		delete [] tmpBoard[i];

	delete [] tmpBoard;

	if (player1->getHasLost() || player2->getHasLost())
		return -1;
	return 0;


}

void Game::resetPieceFiles()
{
	player1->getPlayerFileContext().setPieceFileToStart();
	player2->getPlayerFileContext().setPieceFileToStart();
}


void Game::checkWhetherFlagsWereCaptured(void)
{
	if (player1->getTypeCount(FLAG) == 0 && player1->getOriginalFlagCount() > 0 &&
		player2->getTypeCount(FLAG) > 0)
	{
		if (player1->getHasLost() == false)
		{
			player1->setHasLost();
			player1->setReason(FLAGS_CAPTURED);
		}
	}
	else if (player2->getTypeCount(FLAG) == 0 && player2->getOriginalFlagCount() > 0 &&
		player1->getTypeCount(FLAG) > 0)
	{
		if (player2->getHasLost() == false)
		{
			player2->setHasLost();
			player2->setReason(FLAGS_CAPTURED);
		}
	}
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
	checkPlayersFlagCountLessThanMax(player1);
	checkPlayersFlagCountLessThanMax(player2);

	checkWhetherFlagsWereCaptured();
	
	if (player1->getTypeCount(FLAG) == 0 && player1->getOriginalFlagCount() > 0 &&
		player2->getTypeCount(FLAG) == 0 && player2->getOriginalFlagCount() > 0)
	{
		if (player1->getHasLost() == false &&
			player2->getHasLost() == false)
		{
			player1->setHasLost();
			player1->setReason(ALL_FLAGS_EATEN_DURING_POSITIONING);
			player2->setHasLost();
			player2->setReason(ALL_FLAGS_EATEN_DURING_POSITIONING);
		}
	}

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

	//if no moves left but both players still alive
	else if (!player1->getHasMoreMoves() && !player2->getHasMoreMoves() &&
		!player1->getHasLost() && !player2->getHasLost())
	{
		reason = MOVES_INPUT_FILES_DONE;
	}

	else if (player1->getReason() == player2->getReason())
	{
		if (player1->getReason() == ALL_FLAGS_EATEN_DURING_POSITIONING)		//identified in checkPlayersFlags
			reason = ALL_FLAGS_EATEN_DURING_POSITIONING;
		else
			reason = (eReason)MAKE_REASON_DOUBLE(player1->getReason());
	}
	
	else if (player1->getReason() != player2->getReason())						//TODO
	{
		printf("BOTH player Got TIE scenario with two different reasons:\n"
			"\tPlayer #1 reason: %s\n"
			"\tPlayer #2 reason: %s\n",
			GetReasonString(player1->getReason()).c_str(), GetReasonString(player2->getReason()).c_str());
		reason = BOTH_LOST_DIFFERENT_REASONS;
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

	case BAD_MOVES_INPUT_FILE:
		if (player1->getHasLost())
		{
			playerNumber = 1;
			line = player1->getPlayerFileContext().moves->getCurrentLineNum();
		}
		else
		{
			playerNumber = 2;
			line = player2->getPlayerFileContext().moves->getCurrentLineNum();
		}
		sprintf(temp, "Bad Moves input file for player %d - line %d", playerNumber, line);
		return (string)temp;

	case BAD_POSITIONING_INPUT_FILE_FORMAT:
	case BAD_POSITIONING_INPUT_FILE_DOUBLE_POSITION:
	case BAD_POSITIONING_INPUT_FILE_PIECE_NUMBER:
	case BAD_POSITIONING_INPUT_FILE_FLAG_NUMBER:
		if (player1->getHasLost())
		{
			playerNumber = 1;
			line = player1->getPlayerFileContext().pieces->getCurrentLineNum();
		}
		else
		{
			playerNumber = 2;
			line = player2->getPlayerFileContext().pieces->getCurrentLineNum();
		}
		sprintf(temp, "Bad Positioning input file for player %d - line %d", playerNumber, line);
		return (string)temp;

	case BOTH_BAD_POSITIONING_INPUT_FILE_FORMAT:
	case BOTH_BAD_POSITIONING_INPUT_FILE_DOUBLE_POSITION:
	case BOTH_BAD_POSITIONING_INPUT_FILE_PIECE_NUMBER:
	case BOTH_BAD_POSITIONING_INPUT_FILE_FLAG_NUMBER:
		line = player1->getPlayerFileContext().pieces->getCurrentLineNum();
		line2 = player2->getPlayerFileContext().pieces->getCurrentLineNum();

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

#if DEBUG == 1
	char debug_c;
#endif

	winner = getWinner(&reason);

	board->getBoardDimensions(&cols, &rows);
	fileParser->output->file->seekp(ios::beg);

	*(fileParser->output->file) << "Winner: " << winner << '\n';	//1st line: winner
	*(fileParser->output->file) << "Reason: " << GetReasonString(reason) << "\n\n"; //2nd line: reason, 3rd line: empty

	dprint("writeOutputFile: got reason string, reason = (%d) [%s]\n", reason, GetReasonString(reason).c_str());
	dprint("\n\n ##### BOARD #####\n\n");
	for (i = 1; i <= rows; i++)
	{
		for (j = 1; j <= cols; j++)
		{
			piece = board->getPieceAt(j, i);

			if (!piece)
			{
				c = ' ';
#if DEBUG == 1
				debug_c = '-';
#endif
			}
			else
			{
				isPlayer1 = (piece->getOwner() == player1);
				c = pieceToChar(piece->getOriginalType(), isPlayer1);	//need to get original type to print joker as joker
#if DEBUG == 1
				debug_c = c;
#endif
				if (c < 0)
				{
					cout << "Bad piece returned from function" << endl;
					return;
				}
			}

			*(fileParser->output->file) << c;
			dprint("%c", debug_c);
		}
		*(fileParser->output->file) << '\n';
		dprint("\n");
	}
	fileParser->output->file->flush();	//actual write to the file
	dprint("\n");

}
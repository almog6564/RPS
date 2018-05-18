
#include "game.h"
#include "MyMove.h"
#include "MyJokerChange.h"

using namespace std;

Game::Game(UINT M, UINT N, UINT R1, UINT P1, UINT S1, UINT B1, UINT J1, UINT F1,
						   UINT R2, UINT P2, UINT S2, UINT B2, UINT J2, UINT F2, FileParser* fileParser,
							eGameMode gameMode)
	:M(M), N(N), R1(R1), P1(P1), S1(S1), B1(B1), J1(J1), F1(F1),
				 R2(R2), P2(P2), S2(S2), B2(B2), J2(J2), F2(F2), fileParser(fileParser), movesCounter(0)
{
	board = new MyBoard(N, M);

	if (gameMode == AUTO_VS_FILE || gameMode == AUTO_VS_AUTO)
	{
		player1Context = new PlayerContext(0, R1, P1, S1, B1, J1, F1);
		player1Algorithm = new AutoPlayerAlgorithm(N, M, R1, P1, S1, B1, J1, F1, 1);
	}

	else
	{
		player1Context = new PlayerContext(0, R1, P1, S1, B1, J1, F1);
		player1Algorithm = new FilePlayerAlgorithm(*fileParser->getPlayerFileContext(0), N, M);
	}


	if (gameMode == FILE_VS_AUTO || gameMode == AUTO_VS_AUTO)
	{
		player2Context = new PlayerContext(1, R2, P2, S2, B2, J2, F2);
		player2Algorithm = new AutoPlayerAlgorithm(N, M, R1, P1, S1, B1, J1, F1, 2);
	}
	else
	{
		player2Context = new PlayerContext(1, R1, P1, S1, B1, J1, F2);
		player2Algorithm = new FilePlayerAlgorithm(*fileParser->getPlayerFileContext(1), N, M);
	}

	turn = 0;
}

Game::~Game()
{
	printf("%d\n", movesCounter);
	delete board;
	delete player1Context;
	delete player2Context;
	delete player1Algorithm;
	delete player2Algorithm;
}

void Game::runMove()
{
	if (turn == 0) //player 1's turn
		runSingleMove(player1Context,player1Algorithm);
	else //player 2's turn
		runSingleMove(player2Context,player2Algorithm);
	turn += 1;
	turn %= 2;
}

void Game::runSingleMove(PlayerContext* playerContext, PlayerAlgorithm* playerAlgo)
{
	ePieceType newRep;
	MyFightInfo fightInfo;

	movesCounter++;

	do 
	{
		dprint("\n\n%02d. ************ Starting MOVE: Player #%d ************ \n\n", movesCounter, playerContext->getPlayerId() + 1);

		/* STEP 1 */
		if (!playerContext->getHasMoreMoves())
			break;

		auto move = playerAlgo->getMove();
		if (!move)
		{
			playerContext->setHasMoreMoves(false);
			dprint("No more moves for player %d !", playerContext->getPlayerId()+1);
			break;
		}
	
		if (move->getFrom().getX() == 0) //illegal move
		{
			playerContext->setHasLost();
			playerContext->setReason(BAD_MOVES_INPUT_FILE);
			break;
		}

		const Point& from = move->getFrom(), &to = move->getTo();

		/* STEP 2  */

		unique_ptr<MyFightInfo> fightInfo; 

		if (board->movePiece(playerContext->getPlayerId(), from.getX(), from.getY(), to.getX(), to.getY(), fightInfo))
		{
			//non zero value means some kind of error in move file
			playerContext->setHasLost();
			playerContext->setReason(BAD_MOVES_INPUT_FILE);
			return;
		}

		gameNotifyOnOpponnentMove(playerContext, move);

		gameNotifyFightResult(fightInfo);
		
		auto jokerChange = playerAlgo->getJokerChange();
		if (jokerChange)
		{
			const Point& jokerPos = jokerChange->getJokerChangePosition();
			newRep = charToPiece(jokerChange->getJokerNewRep());

			if (board->changeJokerType(jokerPos.getX(), jokerPos.getY(), newRep))
			{
				playerContext->setHasLost();
				playerContext->setReason(BAD_MOVES_INPUT_FILE);
				return;
			}
		}

		checkWhetherFlagsWereCaptured();

	} while (false);
	printBoard();
}

void Game::gameNotifyFightResult(unique_ptr<MyFightInfo>& fightInfo)
{
	if (fightInfo) //if there was a fight
	{
		player1Algorithm->notifyFightResult(*fightInfo);
		player2Algorithm->notifyFightResult(*fightInfo);
	}
}

void Game::gameNotifyOnOpponnentMove(PlayerContext* playerContext, unique_ptr<Move>& move)
{
	if (playerContext->getPlayerId() == 0) //player 1
	{
		player2Algorithm->notifyOnOpponentMove(*move);
	}
	else //player 2
	{
		player1Algorithm->notifyOnOpponentMove(*move);
	}
}

void Game::checkPlayersFlagCountLessThanMax(PlayerContext* player)
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
	if (player1Context->isAlive() && player2Context->isAlive() && (player1Context->getHasMoreMoves() 
		|| player2Context->getHasMoreMoves()))
		return false;
	return true;
}

void Game::checkWhetherFlagsWereCaptured(void)
{
	if (player1Context->getTypeCount(FLAG) == 0 && player1Context->getOriginalFlagCount() > 0 &&
		player2Context->getTypeCount(FLAG) > 0)
	{
		if (player1Context->getHasLost() == false)
		{
			player1Context->setHasLost();
			player1Context->setReason(FLAGS_CAPTURED);
		}
	}
	else if (player2Context->getTypeCount(FLAG) == 0 && player2Context->getOriginalFlagCount() > 0 &&
		player1Context->getTypeCount(FLAG) > 0)
	{
		if (player2Context->getHasLost() == false)
		{
			player2Context->setHasLost();
			player2Context->setReason(FLAGS_CAPTURED);
		}
	}
}

void Game::flagsCheck()
{
	if (player1Context->getHasLost() || player2Context->getHasLost())
		return;

	checkPlayersFlagCountLessThanMax(player1Context);
	checkPlayersFlagCountLessThanMax(player2Context);

	checkWhetherFlagsWereCaptured();

	if (player1Context->getTypeCount(FLAG) == 0 && player1Context->getOriginalFlagCount() > 0 &&
		player2Context->getTypeCount(FLAG) == 0 && player2Context->getOriginalFlagCount() > 0)
	{
		if (player1Context->getHasLost() == false &&
			player2Context->getHasLost() == false)
		{
			player1Context->setHasLost();
			player1Context->setReason(ALL_FLAGS_EATEN_DURING_POSITIONING);
			player2Context->setHasLost();
			player2Context->setReason(ALL_FLAGS_EATEN_DURING_POSITIONING);
		}
	}

}

int Game::getWinner(eReason* pReason)
{
	int winner = 0;
	eReason reason = INVALID_REASON;

	if (player1Context->getHasLost() && !player2Context->getHasLost())
	{
		reason = player1Context->getReason();
		winner = 2; //player2 wins
	}

	else if (!player1Context->getHasLost() && player2Context->getHasLost())
	{
		reason = player2Context->getReason();
		winner = 1; //player1 wins
	}

	/*********** Tie Scenarios ***********/

	//if no moves left but both players still alive
	else if (!player1Context->getHasMoreMoves() && !player2Context->getHasMoreMoves() &&
		!player1Context->getHasLost() && !player2Context->getHasLost())
	{
		reason = MOVES_INPUT_FILES_DONE;
	}

	else if (player1Context->getReason() == player2Context->getReason())
	{
		if (player1Context->getReason() == ALL_FLAGS_EATEN_DURING_POSITIONING)		//identified in checkPlayersFlags
			reason = ALL_FLAGS_EATEN_DURING_POSITIONING;
		else
			reason = (eReason)MAKE_REASON_DOUBLE(player1Context->getReason());
	}
	
	else if (player1Context->getReason() != player2Context->getReason())						//TODO
	{
		printf("BOTH player Got TIE scenario with two different reasons:\n"
			"\tPlayer #1 reason: %s\n"
			"\tPlayer #2 reason: %s\n",
			GetReasonString(player1Context->getReason()).c_str(), GetReasonString(player2Context->getReason()).c_str());
		reason = BOTH_LOST_DIFFERENT_REASONS;
	}

	if(pReason)
		*pReason = reason;

	return winner;
}

string Game::GetReasonString(eReason reason)
{
	char temp[100];
	int playerNumber;

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
		sprintf(temp, "Bad Moves input file for player %d", player1Context->getHasLost() ? 1 : 2);
		return (string)temp;

	case BAD_POSITIONING_INPUT_FILE_FORMAT:
	case BAD_POSITIONING_INPUT_FILE_DOUBLE_POSITION:
	case BAD_POSITIONING_INPUT_FILE_PIECE_NUMBER:
	case BAD_POSITIONING_INPUT_FILE_FLAG_NUMBER:
		if (player1Context->getHasLost())
		{
			playerNumber = 1;
		}
		else
		{
			playerNumber = 2;
		}
		sprintf(temp, "Bad Positioning input file for player %d", playerNumber);
		return (string)temp;

	case BOTH_BAD_POSITIONING_INPUT_FILE_FORMAT:
	case BOTH_BAD_POSITIONING_INPUT_FILE_DOUBLE_POSITION:
	case BOTH_BAD_POSITIONING_INPUT_FILE_PIECE_NUMBER:
	case BOTH_BAD_POSITIONING_INPUT_FILE_FLAG_NUMBER:
		sprintf(temp, "Bad Positioning input file for both players");
		return (string)temp;

	default:
		return "Invalid eReason code";
	}
}

void Game::printBoard()
{
	return;

	UINT rows, cols, i, j;
	board->getBoardDimensions(&cols, &rows);
	BoardPiece* piece;
	bool isPlayer1 = false;
	char debug_c;

	dprint("\n\n ##### BOARD #####\n\n");

	for (i = 1; i <= rows; i++)
	{
		for (j = 1; j <= cols; j++)
		{
			piece = board->getPieceAt(j, i);

			if (!piece)
			{
				debug_c = '-';
			}
			else
			{
				isPlayer1 = (piece->getOwner() == player1Context);
				consume(isPlayer1);
#if DEBUG == 1
				debug_c = pieceToChar(piece->getOriginalType(), isPlayer1);	//need to get original type to print joker as joker
#endif
				if (debug_c < 0)
				{
					cout << "Bad piece returned from function" << endl;
					return;
				}
			}

			dprint("%c", debug_c);
		}
		dprint("\n");
	}
	dprint("\n");
}

void Game::writeOutputFile()
{
	UINT rows, cols, i, j;
	BoardPiece* piece;
	bool isPlayer1 = false;
	char c;
	int winner;
	eReason reason;

	winner = getWinner(&reason);

	board->getBoardDimensions(&cols, &rows);
	fileParser->output->file->seekp(ios::beg);

	*(fileParser->output->file) << "Winner: " << winner << '\n';	//1st line: winner
	*(fileParser->output->file) << "Reason: " << GetReasonString(reason) << "\n\n"; //2nd line: reason, 3rd line: empty

	dprint("writeOutputFile: got reason string, reason = (%d) [%s]\n", reason, GetReasonString(reason).c_str());
	for (i = 1; i <= rows; i++)
	{
		for (j = 1; j <= cols; j++)
		{
			piece = board->getPieceAt(j, i);

			if (!piece)
			{
				c = ' ';
			}
			else
			{
				isPlayer1 = (piece->getOwner() == player1Context);
				c = pieceToChar(piece->getOriginalType(), isPlayer1);	//need to get original type to print joker as joker
				if (c < 0)
				{
					cout << "Bad piece returned from function" << endl;
					return;
				}
			}

			*(fileParser->output->file) << c;
		}
		*(fileParser->output->file) << '\n';
	}
	fileParser->output->file->flush();	//actual write to the file

	printBoard();
}


void Game::positionAllPieces()
{
	UINT x, y;
	ePieceType type, jokerType;
	BoardPiece* p = nullptr;
	unique_ptr<MyFightInfo> pFight;
	vector<unique_ptr<PiecePosition>> p1PieceVec(0), p2PieceVec(0);
	vector<unique_ptr<FightInfo>> fightVec(0);

	/* Player1 Positioning */

	do
	{
		player1Algorithm->getInitialPositions(1, p1PieceVec);

		if (p1PieceVec.size() == 0)
		{
			dprint("Player 1 lost because of positioning\n");

			player1Context->setHasLost();
			player1Context->setReason(BAD_POSITIONING_INPUT_FILE_FORMAT);
			break;
		}

		dprint("Player 1 getInitialPositions ended successfully\n");

		for (const auto& piecePos : p1PieceVec)
		{
			x = piecePos->getPosition().getX();
			y = piecePos->getPosition().getY();
			type = charToPiece(piecePos->getPiece());
			jokerType = charToPiece(piecePos->getJokerRep());

			p = createNewPiece(player1Context, type, jokerType);
			board->positionPiece(p, x, y, pFight);

		}
		//no need to check pFight because no fights

		dprint("Player 1 positioned all pieces\n");

		printBoard();

	} while (false);

	do
	{

		player2Algorithm->getInitialPositions(2, p2PieceVec);

		if (p2PieceVec.size() == 0)
		{
			dprint("Player 2 lost because of positioning\n");

			board->clearBoard(); //need to clear the board because player1 positioned already

			player2Context->setHasLost();
			player2Context->setReason(BAD_POSITIONING_INPUT_FILE_FORMAT);
			break;
		}

		//don't position player 2 pieces if player 1 has lost, but check was needed for TIE scenario
		if (player1Context->getHasLost())
			break;

		for (const auto& piecePos : p2PieceVec)
		{
			x = piecePos->getPosition().getX();
			y = piecePos->getPosition().getY();
			type = charToPiece(piecePos->getPiece());
			jokerType = charToPiece(piecePos->getJokerRep());

			p = createNewPiece(player2Context, type, jokerType);
			board->positionPiece(p, x, y, pFight);

			if (pFight)
			{
				fightVec.push_back(move(pFight));	//pFight will hold nullptr after move
			}
		}

		dprint("\nFight vector includes %d fights\n", (int)fightVec.size());

		dprint("Player 2 positioned all pieces\n");

	} while (false);

	printBoard();

	player1Algorithm->notifyOnInitialBoard(*board, fightVec);
	player2Algorithm->notifyOnInitialBoard(*board, fightVec);
}
#include "FilePlayerAlgorithm.h"
#include "MyMove.h"
#include "MyPoint.h"
#include "MyJokerChange.h"
#include "MyPiecePosition.h"

using namespace std;


FilePlayerAlgorithm::FilePlayerAlgorithm(PlayerFileContext & fileContext, UINT rows, UINT cols) :
	playerFileContext(fileContext), rows(rows), cols(cols), hasMoreMoves(true), nextJokerChange(nullptr) {};


/*
	Returns how many pieces are in the file.
	If 0 - no pieces in file or some pieces had error.
*/
UINT FilePlayerAlgorithm::validatePlayerPositions(int player)
{
	UINT x, y;
	ePieceType type, jokerType;
	eFileStatus status;
	UINT pieceCounter = 0;

	/* Initialize temporary boolean array to check if theres pieces from the same player*/
	bool** tmpBoard = new bool*[rows];

	for (UINT i = 0; i < rows; i++)
		tmpBoard[i] = new bool[cols]();	//() initializes the elements to false


	while (true)
	{
		status = playerFileContext.getNextPiece(&type, &x, &y, &jokerType);

		if (x > cols || y > rows || x < 1 || y < 1)
		{
			printf("[Player::validatePlayerPositions] player #%d dimensions check failed: "
				"x=%d, y=%d WHILE: cols=%d and rows=%d\n",
				player, x, y, cols, rows);

			status = FILE_BAD_FORMAT;
		}


		switch (status)
		{
		case FILE_EOF_REACHED:
			break;

		case FILE_ERROR:
		case FILE_BAD_FORMAT:
		//	setHasLost();
		//	setReason(BAD_POSITIONING_INPUT_FILE_FORMAT);

			pieceCounter = 0;

			break;

		case FILE_SUCCESS:
			if (tmpBoard[y - 1][x - 1])
			{
				printf("ERROR [player #%d] - while checking position for piece %c at (%d,%d), position is already occupied\n",
					player, pieceToChar(type), x, y);

				pieceCounter = 0;
			//	setHasLost();
			//	setReason(BAD_POSITIONING_INPUT_FILE_DOUBLE_POSITION);
			}
			else
			{
				dprint("got piece: %c %d %d joker: %c\n", pieceToChar(type), x, y, pieceToChar(jokerType));
				tmpBoard[y - 1][x - 1] = true;
				pieceCounter++;
			}
			break;
		}
	}

	playerFileContext.setPieceFileToStart();

	for (UINT i = 0; i < rows; i++)
		delete[] tmpBoard[i];

	delete[] tmpBoard;

	return pieceCounter;
}


void FilePlayerAlgorithm::getInitialPositions(int player, std::vector<unique_ptr<PiecePosition>>& vectorToFill) 
{
	UINT x, y;
	ePieceType type, jokerType;
	UINT piecesCtr = 0;
	unique_ptr<MyPiecePosition> piece;

	dprint("FilePlayerAlgorithm::getInitialPositions");

	//this function validates there's no collisions between pieces of the same player
	//it must be before the actual positioning
	piecesCtr = validatePlayerPositions(player);

	dprint("validatePlayerPositions pieces counter is %d\n", piecesCtr);

	if (piecesCtr == 0)
		return;

	//if we're here than all pieces are valid

	vectorToFill.resize(piecesCtr);

	for (UINT i = 0; i < piecesCtr; i++)
	{
		//no need to check return values because we know all are valid
		playerFileContext.getNextPiece(&type, &x, &y, &jokerType);

		vectorToFill[i] = make_unique<MyPiecePosition>(x, y, pieceToChar(type), pieceToChar(jokerType));
	}
}

unique_ptr<Move> FilePlayerAlgorithm::getMove()
{
	UINT fromX, fromY, toX, toY, jokerX, jokerY;
	bool isJoker;
	ePieceType newRep;

	if (!hasMoreMoves)
		return nullptr;

	eFileStatus status = playerFileContext.getNextMove(&fromX, &fromY, &toX, &toY, &isJoker, &jokerX, &jokerY, &newRep);
	switch (status)
	{
	case FILE_SUCCESS:
		break;
	case FILE_EOF_REACHED:
		hasMoreMoves = false;
		return nullptr;
	case FILE_ERROR:
	case FILE_BAD_FORMAT:
	default:
		return make_unique<MyMove>(0, 0, 0, 0);
	}
	if (isJoker)
		nextJokerChange = make_unique<MyJokerChange>(jokerX, jokerY, newRep);
	else
		nextJokerChange = nullptr;

	return make_unique<MyMove>(fromX, fromY, toX, toY);
}

void FilePlayerAlgorithm::notifyOnInitialBoard(const Board & b, const std::vector<unique_ptr<FightInfo>>& fights) {/*Ignore call*/ }

void FilePlayerAlgorithm::notifyOnOpponentMove(const Move & move) {/*Ignore call*/ }

void FilePlayerAlgorithm::notifyFightResult(const FightInfo & fightInfo) {/*Ignore call*/ }


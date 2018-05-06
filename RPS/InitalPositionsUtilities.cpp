
#include "AutoPlayerAlgorithm.h"
#include "parser.h"
#include <algorithm>

using namespace std;


void addPieceToVectorAndBoard(BoardSet &boardSet, PieceVector &vectorToFill, 
	int x, int y, char pieceType, char jokerType = '#')
{
	vectorToFill.push_back(make_unique<MyPiecePosition>(x, y, pieceType, jokerType));
	boardSet.emplace(x, y, pieceType, jokerType);
}

void positionBomb(UINT& bombsUsed, int bombX, int bombY, PieceVector& vectorToFill,
	BoardSet& boardSet)
{
	dprint("\t - Bomb #%d choosed position (%d,%d)\n", bombsUsed + 1, bombX, bombY);

	addPieceToVectorAndBoard(boardSet, vectorToFill, bombX, bombY, 'B');

	bombsUsed++;
}

void positionFlag(int i, int x, int y, PieceVector &vectorToFill, BoardSet &boardSet)
{
	dprint("\t - Flag #%d choosed position (%d,%d)\n", i + 1, x, y);

	addPieceToVectorAndBoard(boardSet, vectorToFill, x, y, 'F');
}

void positionMovingPiece(int x, int y, PieceVector &vectorToFill, BoardSet &boardSet, char pieceType, char jokerType)
{
	if (jokerType == '#')
	{
		dprint("\t - Moving Piece (%c) choosed position (%d,%d)\n", pieceType, x, y);
	}
	else
	{
		dprint("\t - Joker from type (%c) choosed position (%d,%d)\n", jokerType, x, y);
	}

	addPieceToVectorAndBoard(boardSet, vectorToFill, x, y, pieceType, jokerType);
}


void positionFlagRandomly(RandomContext& rndCtx, BoardSet &boardSet, int flagIndex, PieceVector& vectorToFill)
{
	int x, y;

	do
	{
		//choose place randomly
		x = rndCtx.getRandomCol();
		y = rndCtx.getRandomRow();

		//if chose already 
		if (boardSet.count(MyPiecePosition(x, y, FLAG)) == 0)
			break;

	} while (true);

	positionFlag(flagIndex, x, y, vectorToFill, boardSet);
}


int generateUniqueCorner(RandomContext &rndCtx, vector<bool>& selectedCorners)
{
	int select;

	do
	{
		select = rndCtx.getRandomCol();

		if (selectedCorners[select] == true) //already flag on this corner
			select = rndCtx.getRandomCol();
		else
		{
			selectedCorners[select] = true;
			break;
		}

	} while (true);

	return select;
}

void AutoPlayerAlgorithm::placeMovingPiecesOnCorners(vector<char> &movingPieceVector,
	int initialMovingCnt, RandomContext &rndCtx, PieceVector& vectorToFill, BoardSet& boardSet,
	int &pieceIndex)
{

	int select, cornerX, cornerY, corners;
	vector<bool> selectedCorners = { false, false, false, false };

	if (scenario->areMovingOnCorners && !scenario->areFlagsOnCorners)
	{
		shuffle(movingPieceVector.begin(), movingPieceVector.end(), rndCtx.getRandomGenerator());

		corners = (initialMovingCnt < 4) ? (initialMovingCnt) : 4;

		for (int i = 0; i < corners; i++)
		{
			select = generateUniqueCorner(rndCtx, selectedCorners);

			chooseCorner(select, rndCtx, cornerX, cornerY);

			positionMovingPiece(cornerX, cornerY, vectorToFill, boardSet, movingPieceVector[pieceIndex++]);

		}
	}
}


void AutoPlayerAlgorithm::chooseCorner(int select, RandomContext& rndCtx,
	int& cornerX, int& cornerY, int* bomb1X, int* bomb1Y, int* bomb2X, int* bomb2Y)
{

#define assign(a,b)		if(a) *(a) = (b);

	switch (select)
	{
	case 0:
		cornerX = 1;
		cornerY = 1;
		assign(bomb1X, 2);
		assign(bomb1Y, 1);
		assign(bomb2X, 1);
		assign(bomb2Y, 2);
		break;
	case 1:
		cornerX = 1;
		cornerY = boardRows;
		assign(bomb1X, 2);
		assign(bomb1Y, boardRows);
		assign(bomb2X, 1);
		assign(bomb2Y, boardRows - 1);
		break;
	case 2:
		cornerX = boardCols;
		cornerY = 1;
		assign(bomb1X, boardCols - 1);
		assign(bomb1Y, 1);
		assign(bomb2X, boardCols);
		assign(bomb2Y, 2);
		break;
	case 3:
		cornerX = boardCols;
		cornerY = boardRows;
		assign(bomb1X, boardCols - 1);
		assign(bomb1Y, boardRows);
		assign(bomb2X, boardCols);
		assign(bomb2Y, boardRows - 1);
		break;
	default:
		cornerX = rndCtx.getRandomCol();
		cornerY = rndCtx.getRandomRow();
	}
}

void AutoPlayerAlgorithm::fillListWithMovingPieces(vector<char>& movingPieceVector, int bombsUsed)
{
	addAllToVector(R, 'R');
	addAllToVector(P, 'P');
	addAllToVector(S, 'S');

	for (UINT i = 0; i < (initPieceCnt.B - bombsUsed); i++)
		movingPieceVector.push_back('B');
}


int AutoPlayerAlgorithm::positionFlagsAndBombs(RandomContext& rndCtx,
	BoardSet& boardSet, PieceVector& vectorToFill)
{
	/* Other static variables */
	int cornerX, cornerY, select, corners, bomb1X, bomb1Y, bomb2X, bomb2Y;

	vector<bool> selectedCorners = { false, false, false, false };

	UINT bombsUsed = 0, chooseBomb;
	bool shouldUseOneBombPerFlag = initPieceCnt.B < initPieceCnt.F;

	/* If randomly chosen in the scenario, place the flags on the corners */
	if (scenario->areFlagsOnCorners)
	{
		corners = (initPieceCnt.F < 4) ? (initPieceCnt.F) : 4;

		for (int i = 0; i < corners; i++)
		{
			select = generateUniqueCorner(rndCtx, selectedCorners);

			chooseCorner(select, rndCtx, cornerX, cornerY,
				&bomb1X, &bomb1Y, &bomb2X, &bomb2Y);

			positionFlag(i, cornerX, cornerY, vectorToFill, boardSet);

			if (bombsUsed < initPieceCnt.B)
			{
				if (shouldUseOneBombPerFlag)
				{
					chooseBomb = rndCtx.getRandomBinary();

					if (chooseBomb == 0)
						positionBomb(bombsUsed, bomb1X, bomb1Y, vectorToFill, boardSet);
					else
						positionBomb(bombsUsed, bomb2X, bomb2Y, vectorToFill, boardSet);

				}
				else //position two bombs beside flags
				{
					positionBomb(bombsUsed, bomb1X, bomb1Y, vectorToFill, boardSet);
					positionBomb(bombsUsed, bomb2X, bomb2Y, vectorToFill, boardSet);
				}
			}
		}

		if (initPieceCnt.F > 4)
		{
			//if there are more flags than corners, must use all flags
			for (UINT i = 4; i < initPieceCnt.F; i++)
			{
				positionFlagRandomly(rndCtx, boardSet, i, vectorToFill);
			}
		}
	}
	else //position flags randomly, not on corners
	{
		for (UINT i = 0; i < initPieceCnt.F; i++)
		{
			positionFlagRandomly(rndCtx, boardSet, i, vectorToFill);
		}
	}

	return bombsUsed;
}


void positionRestOfMovingPiecesRandomly(int pieceIndex, int initialMovingCnt,
	RandomContext &rndCtx, BoardSet &boardSet, vector<char> movingPieceVector, PieceVector& vectorToFill)
{
	int x, y;
	char random_RPSB_jokerType;

	for (int i = pieceIndex; i < initialMovingCnt;)
	{
		do
		{
			//choose place randomly
			x = rndCtx.getRandomCol();
			y = rndCtx.getRandomRow();

			//if chose already 
			if (boardSet.count(MyPiecePosition(x, y, movingPieceVector[i])) == 0)
				break;

		} while (true);

		if (movingPieceVector[i] != 'J')
		{
			positionMovingPiece(x, y, vectorToFill, boardSet, movingPieceVector[i]);
		}
		else
		{
			//getRandomCorner+1 will generate number between 1-4 (ROCK,PAPER,SCISSORS,BOMB in ePieceType enum)

			random_RPSB_jokerType = pieceToChar((ePieceType)(rndCtx.getRandomCorner() + 1));

			positionMovingPiece(x, y, vectorToFill, boardSet, 'J', random_RPSB_jokerType);
		}

		i++;
	}
}

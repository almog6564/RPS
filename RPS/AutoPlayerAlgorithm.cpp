

#include "AutoPlayerAlgorithm.h"
#include <algorithm>
#include "parser.h"

using namespace std;

#define addAllToVector(c,cc)	for (UINT i = 0; i < initPieceCnt.c; i++) movingPieceVector.push_back(cc);


AutoPlayerAlgorithm::AutoPlayerAlgorithm(UINT rows, UINT cols, UINT R, UINT P, UINT S, UINT B, UINT J, UINT F)
{
	random_device				seed;			//Will be used to obtain a seed for the random number engine
	mt19937						gen(seed());	//Standard mersenne_twister_engine seeded with seed()
	uniform_int_distribution<>	dis(0,1);		//Distributed random
	
	scenario = new PositioningScenario(dis(gen), dis(gen));

	boardCols = cols;
	boardRows = rows;

	initPieceCnt.R = R;
	initPieceCnt.P = P;
	initPieceCnt.S = S;
	initPieceCnt.B = B;
	initPieceCnt.J = J;
	initPieceCnt.F = F;

}


AutoPlayerAlgorithm::~AutoPlayerAlgorithm()
{
	delete scenario;
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
		assign(bomb1X,2);
		assign(bomb1Y,1);
		assign(bomb2X,1);
		assign(bomb2Y,2);
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

void positionBomb(UINT& bombsUsed, int bombX, int bombY, PieceVector& vectorToFill, 
	BoardSet& boardSet)
{
	dprint("\t - Bomb #%d choosed position (%d,%d)\n", bombsUsed + 1, bombX, bombY);

	vectorToFill.push_back(make_unique<MyPiecePosition>(bombX, bombY, 'B'));
	boardSet.insert(MyPiecePosition(bombX, bombY, 'B'));
	bombsUsed++;

}


void positionFlag(int i, int x, int y, PieceVector &vectorToFill, BoardSet &boardSet)
{
	dprint("\t - Flag #%d choosed position (%d,%d)\n", i + 1, x, y);

	vectorToFill.push_back(make_unique<MyPiecePosition>(x, y, 'F'));
	boardSet.insert(MyPiecePosition(x, y, 'F'));
}

void positionMovingPiece(int x, int y, PieceVector &vectorToFill, BoardSet &boardSet, char pieceType, char jokerType = '#')
{
	if (jokerType == '#')
	{
		dprint("\t - Moving Piece (%c) choosed position (%d,%d)\n", pieceType, x, y);
	}
	else
	{
		dprint("\t - Joker from type (%c) choosed position (%d,%d)\n", jokerType, x, y);
	}

	vectorToFill.push_back(make_unique<MyPiecePosition>(x, y, pieceType, jokerType));
	boardSet.insert(MyPiecePosition(x, y, pieceType, jokerType));
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

int AutoPlayerAlgorithm::positionFlagsAndBombs(RandomContext& rndCtx,
	BoardSet& boardSet, PieceVector& vectorToFill)
{
	/* Other static variables */
	int cornerX, cornerY, select, corners, bomb1X, bomb1Y, bomb2X, bomb2Y;
	bool selectedCorners[4] = { false };
	UINT bombsUsed = 0, chooseBomb;
	bool shouldUseOneBombPerFlag = initPieceCnt.B < initPieceCnt.F;

	/* If randomly chosen in the scenario, place the flags on the corners */
	if (scenario->areFlagsOnCorners)
	{
		corners = (initPieceCnt.F < 4) ? (initPieceCnt.F) : 4;

		for (int i = 0; i < corners; i++)
		{
			do
			{
				select = rndCtx.getRandomCorner();

				if (selectedCorners[select] == true) //already flag on this corner
					select = rndCtx.getRandomCorner();
				else
				{
					selectedCorners[select] = true;
					break;
				}

			} while (true);

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

void AutoPlayerAlgorithm::getInitialPositions(int player, PieceVector& vectorToFill)
{
	/** Parameters Initialization **/
	BoardSet boardSet;	
	vector<char> movingPieceVector;
	int initialMovingCnt, corners, select, cornerX, cornerY, pieceIndex = 0, bombsUsed, x, y;
	bool selectedCorners[4] = { false };

	/* Random Generators Context */
	random_device				seed;
	mt19937						gen(seed());
	uniform_int_distribution<>	binaryGen(0, 1), cornerGen(0, 3),
		rowNumGen(1, boardRows), colNumGen(1, boardCols);
	RandomContext rndCtx(gen, colNumGen, rowNumGen, cornerGen, binaryGen);

	/****************************************************************/

	dprint("\n#### Initial Positions || Player %d\n", player);
	dprint("\t - Scnario: areFlagsOnCoreners = %d, areMovingOnCorners = %d\n", scenario->areFlagsOnCorners, scenario->areMovingOnCorners);

	vectorToFill.clear(); //sanity

	/**** Position All Flags and Some Bombs *****/

	bombsUsed = positionFlagsAndBombs(rndCtx, boardSet, vectorToFill);

	//all flags are positioned now



	/********* Moving Pieces Section **********/

	fillListWithMovingPieces(movingPieceVector, bombsUsed);
	initialMovingCnt = (int) movingPieceVector.size();
	
	if (scenario->areMovingOnCorners && !scenario->areFlagsOnCorners)
	{
		shuffle(movingPieceVector.begin(), movingPieceVector.end(), gen);

		corners = (initialMovingCnt < 4) ? (initialMovingCnt) : 4;

		for (int i = 0; i < corners; i++)
		{
			do
			{
				select = cornerGen(gen);

				if (selectedCorners[select] == true) //already flag on this corner
					select = cornerGen(gen);
				else
				{
					selectedCorners[select] = true;
					break;
				}

			} while (true);

			chooseCorner(select, rndCtx, cornerX, cornerY);

			positionMovingPiece(cornerX, cornerY, vectorToFill, boardSet, movingPieceVector[pieceIndex++]);

		}
	}

	//Position the rest of the pieces randomly on board
	
	addAllToVector(J, 'J'); //add Jokers, don't place them on the corners

	for (int i = pieceIndex; i < initialMovingCnt;)
	{
		do
		{
			//choose place randomly
			x = colNumGen(gen);
			y = rowNumGen(gen);

			//if chose already 
			if (boardSet.count(MyPiecePosition(x, y, movingPieceVector[i])) == 0)
				break;

		} while (true);

		if(movingPieceVector[i] != 'J')
			positionMovingPiece(x, y, vectorToFill, boardSet, movingPieceVector[i]);
		else
		{
			//cornerGen+1 will generate number between 1-4 (ROCK,PAPER,SCISSORS,BOMB in ePieceType enum)

			positionMovingPiece(x, y, vectorToFill, boardSet, 'J', pieceToChar((ePieceType)(cornerGen(gen) + 1)));
		}

		i++;
	}
}

void AutoPlayerAlgorithm::notifyOnInitialBoard(const Board & b, const vector<unique_ptr<FightInfo>>& fights)
{
	//just zevel for it to compile
	auto a = new MyPoint(1, 1);
	b.getPlayer(*a);
	fights.size();
	delete a;
}

void AutoPlayerAlgorithm::notifyOnOpponentMove(const Move & move)
{
	move.getFrom();
}

void AutoPlayerAlgorithm::notifyFightResult(const FightInfo & fightInfo)
{
	fightInfo.getPiece(1);
}

unique_ptr<Move> AutoPlayerAlgorithm::getMove()
{
	return unique_ptr<Move>();
}

unique_ptr<JokerChange> AutoPlayerAlgorithm::getJokerChange()
{
	return unique_ptr<JokerChange>();
}

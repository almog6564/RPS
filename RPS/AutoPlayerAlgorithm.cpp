

#include "AutoPlayerAlgorithm.h"
#include "parser.h"

using namespace std;


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

void AutoPlayerAlgorithm::getInitialPositions(int player, PieceVector& vectorToFill)
{
	/** Parameters Initialization **/
	BoardSet boardSet;	
	vector<char> movingPieceVector;
	int initialMovingCnt, pieceIndex = 0, bombsUsed, x, y;

	/* Random Generators Context */
	random_device				seed;
	mt19937						gen(seed());
	uniform_int_distribution<>	binaryGen(0, 1), cornerGen(0, 3),
		rowNumGen(1, boardRows), colNumGen(1, boardCols);
	RandomContext rndCtx(gen, colNumGen, rowNumGen, cornerGen, binaryGen);

	dprint("\n#### Initial Positions || Player %d\n", player);
	dprint("\t - Scnario: areFlagsOnCoreners = %d, areMovingOnCorners = %d\n", scenario->areFlagsOnCorners, scenario->areMovingOnCorners);

	vectorToFill.clear(); //sanity

	/**** Position All Flags and Some Bombs *****/

	bombsUsed = positionFlagsAndBombs(rndCtx, boardSet, vectorToFill);

	//all flags are positioned now



	/********* Moving Pieces Section **********/

	fillListWithMovingPieces(movingPieceVector, bombsUsed);
	initialMovingCnt = (int) movingPieceVector.size();

	placeMovingPiecesOnCorners(movingPieceVector, initialMovingCnt, rndCtx, vectorToFill, boardSet, pieceIndex);

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

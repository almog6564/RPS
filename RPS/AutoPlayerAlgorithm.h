#ifndef __AUTO_PLAYER_ALGORITHM_H_
#define __AUTO_PLAYER_ALGORITHM_H_

#include "PlayerAlgorithm.h"
#include "defs.h"
#include <unordered_set>
#include "MyPiecePosition.h"
#include <random>

class PositioningScenario;

typedef std::unordered_set<MyPiecePosition, PiecePositionHasher, PiecePositionComparator> BoardSet;
typedef std::vector<unique_ptr<PiecePosition>> PieceVector;

#define addAllToVector(c,cc)	for (UINT i = 0; i < initPieceCnt.c; i++) movingPieceVector.push_back(cc);


class RandomContext {

	std::mt19937& gen;
	std::uniform_int_distribution<>& colNumGen;
	std::uniform_int_distribution<>& rowNumGen;
	std::uniform_int_distribution<>& cornerGen;
	std::uniform_int_distribution<>& binaryGen;

public:
	RandomContext(std::mt19937& gen, std::uniform_int_distribution<>& colNumGen,
		std::uniform_int_distribution<>& rowNumGen, std::uniform_int_distribution<>& cornerGen,
		std::uniform_int_distribution<>& binaryGen)
		: gen(gen), colNumGen(colNumGen), rowNumGen(rowNumGen), cornerGen(cornerGen), binaryGen(binaryGen) {};

	int getRandomCorner() { return cornerGen(gen); }

	int getRandomCol() { return colNumGen(gen); }

	int getRandomRow() { return rowNumGen(gen); }

	int getRandomBinary() { return binaryGen(gen); }

	std::mt19937& getRandomGenerator() { return gen; }
};


class AutoPlayerAlgorithm : public PlayerAlgorithm
{
private:
	PositioningScenario* scenario;
	UINT boardRows, boardCols;

	struct { 
		UINT R, P, S, B, J, F;
	} initPieceCnt;

	int positionFlagsAndBombs(RandomContext& rndCtx, BoardSet& boardSet, PieceVector& vectorToFill);

	void chooseCorner(int select, RandomContext& rndCtx, int& cornerX, int& cornerY,
		int* bomb1X = nullptr, int* bomb1Y = nullptr, int* bomb2X = nullptr, int* bomb2Y = nullptr);

	void fillListWithMovingPieces(std::vector<char>& movingPieceList, int bombUsed);

public:
	AutoPlayerAlgorithm(UINT boardRows, UINT boardCols, 
		UINT R, UINT P, UINT S, UINT B, UINT J, UINT F);

	~AutoPlayerAlgorithm();

	void getInitialPositions(int player, PieceVector& vectorToFill);

	void placeMovingPiecesOnCorners(vector<char> &movingPieceVector,
		int initialMovingCnt, RandomContext &rndCtx, PieceVector& vectorToFill, BoardSet& boardSet,
		int &pieceIndex);

	void notifyOnInitialBoard(const Board& b, const std::vector<unique_ptr<FightInfo>>& fights);
	void notifyOnOpponentMove(const Move& move); // called only on opponent’s move
	void notifyFightResult(const FightInfo& fightInfo); // called only if there was a fight
	unique_ptr<Move> getMove();
	unique_ptr<JokerChange> getJokerChange(); // nullptr if no change is requested
};

void positionMovingPiece(int x, int y, PieceVector &vectorToFill, BoardSet &boardSet, char pieceType, char jokerType = '#');

class PositioningScenario
{
public:
	bool areFlagsOnCorners;
	bool areMovingOnCorners;

	PositioningScenario(bool areFlagsOnCorners, bool areMovingOnCorners) :
		areFlagsOnCorners(areFlagsOnCorners), areMovingOnCorners(areMovingOnCorners) {};
};

#endif
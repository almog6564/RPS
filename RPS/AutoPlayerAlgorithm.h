#ifndef __AUTO_PLAYER_ALGORITHM_H_
#define __AUTO_PLAYER_ALGORITHM_H_

#include "PlayerAlgorithm.h"
#include "MyJokerChange.h"
#include "MyPiecePosition.h"
#include "MyMove.h"
#include "defs.h"

#include <unordered_set>
#include <random>
#include <bitset>

class PositioningScenario;
class RandomContext;

typedef std::unordered_set<MyPiecePosition, PiecePositionHasher, PiecePositionComparator> BoardSet;
typedef std::vector<unique_ptr<PiecePosition>> PieceVector;

#define addAllToVector(c,cc)	for (UINT i = 0; i < initPieceCnt.c; i++) movingPieceVector.push_back(cc);


class AutoPlayerAlgorithm : public PlayerAlgorithm
{
private:

	int ID;

	struct {
		UINT R, P, S, B, J, F;
	} initPieceCnt;

	PositioningScenario* scenario;
	UINT boardRows, boardCols;
	BoardSet opponentsPieces;
	BoardSet playerPieces;
	//BoardSet boardSet; //serves as playersPieces
	MyPoint lastMove;
	BoardSet::iterator nextPieceToMove;


	int positionFlagsAndBombs(RandomContext& rndCtx, BoardSet& boardSet, PieceVector& vectorToFill);

	void chooseCorner(int select, RandomContext& rndCtx, int& cornerX, int& cornerY,
		int* bomb1X = nullptr, int* bomb1Y = nullptr, int* bomb2X = nullptr, int* bomb2Y = nullptr);

	void fillListWithMovingPieces(std::vector<char>& movingPieceList, int bombUsed);

	MyPiecePosition getNextPieceToMove(void);

	bool checkForAdjecentOpponent(const MyPiecePosition other);

	unique_ptr<Move> checkAllAdjecentOpponents(const MyPiecePosition& piece, std::bitset<4>& boolVec, const int x, const int y);

	unique_ptr<JokerChange> checkAllAdjecentOpponents(const MyPiecePosition & joker, const int x, const int y);

	std::unique_ptr<MyMove> getLegalMove(const MyPoint& point);

	std::unique_ptr<MyMove> getLegalMove(const MyPoint& point, std::bitset<4>& fleeArr);

	char getNewJokerRep(const char oldJokerRep);

	char getRandomJokerChahge(int rand, char jokerRep);

	bool existsOnBoardSet(const MyPoint& point);

	void removeOutOfBoundsDirections(const MyPoint& point, std::bitset<4>& legalFleeDirections);

	int fillCornersWithAlreadyOccupiedCorners(vector<bool>& selectedCorners, BoardSet& boardSet);

public:
	AutoPlayerAlgorithm(UINT boardRows, UINT boardCols,
		UINT R, UINT P, UINT S, UINT B, UINT J, UINT F, int ID);

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

void positionRestOfMovingPiecesRandomly(int pieceIndex, 
	RandomContext &rndCtx, BoardSet &boardSet, vector<char>& movingPieceVector, PieceVector& vectorToFill);

class PositioningScenario
{
public:
	bool areFlagsOnCorners;
	bool areMovingOnCorners;

	PositioningScenario(bool areFlagsOnCorners, bool areMovingOnCorners) :
		areFlagsOnCorners(areFlagsOnCorners), areMovingOnCorners(areMovingOnCorners) {};
};


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


#endif
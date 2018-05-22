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

	int ID; //1 for player 1, 2 for player 2

	struct {
		UINT R, P, S, B, J, F;
	} initPieceCnt;

	PositioningScenario* scenario;
	UINT boardRows, boardCols;
	BoardSet opponentsPieces; //This set will hold all the known data regarding his opponent's pieces
	BoardSet playerPieces; //This set will hold the data regarding player's own pieces
	BoardSet::iterator nextPieceToMove; //An iterator keeping the next piece to move 


	int positionFlagsAndBombs(RandomContext& rndCtx, BoardSet& boardSet, PieceVector& vectorToFill);

	void chooseCorner(int select, RandomContext& rndCtx, int& cornerX, int& cornerY,
		int* bomb1X = nullptr, int* bomb1Y = nullptr, int* bomb2X = nullptr, int* bomb2Y = nullptr);

	void fillListWithMovingPieces(std::vector<char>& movingPieceList, int bombUsed);

	/* The function will iterate on playersPieces Set and return the first moving piece it encounters */
	MyPiecePosition getNextPieceToMove(void);

	/* The function checks if an opponent's piece is present at specific position.
		@return - True if opponent's piece if found, False otherwise.*/
	bool checkForAdjecentOpponent(const MyPiecePosition other);

	/*The function checks for all possible 4 points surrounding a piece to see if there is an opportunity to attack or flee from an adjecent opponent's piece.
		@return - if such a possible move is found, then it is returned as a unique_ptr, else, nullptr is returned*/
	unique_ptr<Move> checkAllAdjecentOpponents(const MyPiecePosition& piece, std::bitset<4>& boolVec, const int x, const int y);

	/* Similarly to previous function, this one checks a "smart" piece type joker change by searching for adjecent opponent's pieces surrounding player's jokers.
	   @return - if an adjecent opponent piece with known type (inticipated in a fight before), than player's joker is changed to become stronger than it.
				 if no such piece is found, nullptr is returned./*/
	unique_ptr<JokerChange> checkAllAdjecentOpponents(const MyPiecePosition & joker, const int x, const int y);

	/* This function will return a random move out of 4 possible potential legal moves for a specific point under the following constrains:
		1. It is inside the board (for point around the edges).
		2. There is no piece owned by the same player at that point.
		3. There is no stronger known opponent's piece at that point (discovered by checkAllAdjecentOpponents fuction call)
		@return - From all (upto 4) possible moves meeting all 3 constrains, a random move is raffled.
				  If no Point that meets all 3 constrains is found, nullptr is returned.*/
	std::unique_ptr<MyMove> getLegalMove(const MyPoint& point);
	std::unique_ptr<MyMove> getLegalMove(const MyPoint& point, std::bitset<4>& fleeArr);

	/* @Pre - the oldJokerRep recieved is considered a loosing type with regard to an adjecent previously found opponent
	   @Post - The new char returned, represents the winning type. 
	   (e.g. if Rock was given as a loosing type, than we know that Paper is the winner of Rock, than Scissors will be returned to win the Paper)*/
	char getNewJokerRep(const char oldJokerRep);

	/* @return - a random piece type (R,P,S or B) other than given jokerRep*/
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

	/* @pre - 1. When function is called, the player already holds a Set (playersPieces) of his own pieces, including those who lost in first positioning.
			  2. The given board consists of only pieces that "survived" initial positioning.
	   @post - 1. playersPieces Set will discard pieces that didn't survive initial positioning.
			   2. opponentsPieces Set will now hold all opponent's pieces that survived initial positioning and known type for those who inticiapted in a fight.*/
	void notifyOnInitialBoard(const Board& b, const std::vector<unique_ptr<FightInfo>>& fights);

	/* @brief - opponentsPieces Set is updated about last opponent's move 
		Because it is a set, and the point is part of the hash function and cannot be changed, the piece needs to be removed and inserted again with new data
		For other data member changes that are not part of the hash function, we used mutable and const setters functions to change data of members of the set without extracting and inserting them.*/
	void notifyOnOpponentMove(const Move& move); 

	/* @brief - Both Sets are updated (playersPieces Set and opponentsPieces Set).
				The function handles 3 scenarios (win, lose, tie) and deals with the special case of Bomb, that needs to be removed, in any case.
				Pieces are removed from the sets according to fight results.
				In the case of opponent's win (player's loss), the piece type is updated in opponentsPieces Set.*/
	void notifyFightResult(const FightInfo& fightInfo); // called only if there was a fight

	/* @brief - The  function has 2 stages:
				1. Find a "smart" move to make, i.e. :
					a. search for a piece that has an opponen't piece that is not stronger in a single move distance from it, and attack it.
				    b. Or, search for a stronger piece in a single move distance, and flee from it to a different direction.
				2. If no such "smart" move is found, perform a random legal move.
	   @return - A unique_ptr<MyMove> of the "smart" or random move. In the very edge case that no legal move can be done (e.g. all moving pieces are surrounded by the same player's non-moving pieces), nullptr is returned.*/
	unique_ptr<Move> getMove();

	/*  @brief -  Just like getMove, the fuction first tries to find a "smart" joker change with checkAllAdjecentOpponents function, and if no such joker change is found, a random change is performed.
		@return - unique_ptr<MyJokerChange> of the "smart" or random joker change, or nullptr if no jokers are left.*/
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
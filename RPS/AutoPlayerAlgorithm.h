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

//typdefs for readability
typedef std::unordered_set<MyPiecePosition, PiecePositionHasher, PiecePositionComparator> BoardSet;
typedef std::vector<unique_ptr<PiecePosition>> PieceVector;

/* Macro for convenient inserting of all elements from type c which are represented by char 'cc' 
	Example usage: addAllToVector(R,'R') - inserts all ROCK pieces to the rpsbVector */
#define addAllToVector(c,cc)	for (UINT i = 0; i < initPieceCnt.c; i++) rpsbVector.push_back(cc);


class AutoPlayerAlgorithm : public PlayerAlgorithm
{
private:

	int ID; //1 for player 1, 2 for player 2

	//holds the piece count as received from main
	struct {
		UINT R, P, S, B, J, F;
	} initPieceCnt;

	random_device	seed;			//Will be used to obtain a seed for the random number engine
	mt19937			gen;			//Standard mersenne_twister_engine seeded with seed()


	PositioningScenario* scenario;		//This member holds the positioning scenario, as described in the class itself
	UINT boardRows, boardCols;			//Size of board
	BoardSet opponentsPieces;			//This set will hold all the known data regarding his opponent's pieces
	BoardSet playerPieces;				//This set will hold the data regarding player's own pieces

	/* The function positions all the flags on the board and possibly some of the bombs,
		according to the positioning scenario randomly chosen.

		@pre - there is enough room for the flags and bombs.
		@var rndCtx							- ref to a randomization context to be used to generate a random corner
		@var boardSet						- ref to the player pieces set
		@var vectorToFill					- ref to the pieces unique_ptr vector returned by getInitialPositions

		@ret number of bombs positioned
	*/
	int positionFlagsAndBombs(RandomContext& rndCtx, BoardSet& boardSet, PieceVector& vectorToFill);


	/*
		Convert corner index (0-3) to an actual board position for a flag.
		It places the results accordingly in cornerX, cornerY.
		If used, bomb1, bomb2 coordinates will be filled with the relevant adjacent positions on the board,
		to place a bomb to guard the flag.

		If there are N rows and M columns then:

		select	|	coordinates
			0	|	1,1
			1	|	1,N
			2	|	M,1
			3	|	M,N
		else	|	random

	*/
	void chooseCorner(int select, RandomContext& rndCtx, int& cornerX, int& cornerY,
		int* bomb1X = nullptr, int* bomb1Y = nullptr, int* bomb2X = nullptr, int* bomb2Y = nullptr);

	/* Fills the rpsbVector with all the R,P,S,B pieces possible, minus bombUsed Bombs that were already placed. */
	void fillListWithRPSBpieces(std::vector<char>& rpsbVector, int bombUsed);

	/* The function will iterate on playersPieces Set and return the first moving piece it encounters */
	unique_ptr<Move> getNextRandomMove(void);

	/* The function checks if an opponent's piece is present at specific position.
		@return - True if opponent's piece if found, False otherwise.*/
	bool checkForAdjecentOpponent(const MyPiecePosition other);

	/*The function checks for all possible 4 points surrounding a piece to see if there is an opportunity to attack or flee from an adjecent opponent's piece.
		@return - if such a possible move is found, then it is returned as a unique_ptr, else, nullptr is returned*/
	unique_ptr<Move> checkAllAdjecentOpponents(const MyPiecePosition& piece, std::bitset<4>& boolVec, const int x, const int y);

	/* Similarly to previous function, this one checks a "smart" piece type joker change by searching for adjacent opponent's pieces surrounding player's jokers.
	   @return - if an adjacent opponent piece with known type (anticipated in a fight before), than player's joker is changed to become stronger than it.
				 if no such piece is found, nullptr is returned./*/
	unique_ptr<JokerChange> checkAllAdjecentOpponents(const MyPiecePosition & joker, const int x, const int y);

	/* This function will return a random move out of 4 possible potential legal moves for a specific point under the following constrains:
		1. It is inside the board (for point around the edges).
		2. There is no piece owned by the same player at that point.
		3. There is no stronger known opponent's piece at that point (discovered by checkAllAdjecentOpponents function call)
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

	/* Checks whether a piece exists on point #point in the playerPieces of the autoPlayerAlgorithm. */
	bool existsOnBoardSet(const MyPoint& point);

	/* Checks whether point is on one of the board's borders or corners, and if so, 
		'false's the relevant directions from the legalFleeDirections bitset. 
		The indices represent the directions of [left, right, up, down]. */
	void removeOutOfBoundsDirections(const MyPoint& point, std::bitset<4>& legalFleeDirections);

	/* Checks whether the corners of the board are already occupied with pieces and if so writes 'true' at
		the relevant index in selectedCorners - see definition on #chooseCorner.

		@ret - the number of empty corners
	*/
	int fillCornersWithAlreadyOccupiedCorners(vector<bool>& selectedCorners, BoardSet& boardSet);

	/* */
	void placeMovingPiecesOnCorners(vector<char> &movingPieceVector,
		int initialMovingCnt, RandomContext &rndCtx, PieceVector& vectorToFill, BoardSet& boardSet,
		int &pieceIndex);

public:
	AutoPlayerAlgorithm(UINT boardRows, UINT boardCols,
		UINT R, UINT P, UINT S, UINT B, UINT J, UINT F, int ID);

	~AutoPlayerAlgorithm();

	/*
		Places all the pieces of the current player on the auto player's board - used in "playerPieces".
		It randomizes a scenario as explained in the class definition of #PositioningScenario below.

		Afterwards it positions all the flags (and maybe some Bombs around them) - if chosen by the scenario
		than they will be places on the corners of the board.
		
		Otherwise, if chosen by scenario, moving pieces will be set on the corners to counter attack flags on corners. 
		
		It then positions the rest of the pieces randomly on board, while jokers will not be places on the corners
		and their first jokerType will be also randomized.

		The 'player' argument is used only for debug.
	*/
	void getInitialPositions(int player, PieceVector& vectorToFill);



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
					a. search for a piece that has an opponent's piece that is not stronger in a single move distance from it, and attack it.
				    b. Or, search for a stronger piece in a single move distance, and flee from it to a different direction.
				2. If no such "smart" move is found, perform a random legal move.
	   @return - A unique_ptr<MyMove> of the "smart" or random move. In the very edge case that no legal move can be done (e.g. all moving pieces are surrounded by the same player's non-moving pieces), nullptr is returned.*/
	unique_ptr<Move> getMove();

	/*  @brief -  Just like getMove, the function first tries to find a "smart" joker change with checkAllAdjecentOpponents function, and if no such joker change is found, a random change is performed.
		@return - unique_ptr<MyJokerChange> of the "smart" or random joker change, or nullptr if no jokers are left.*/
	unique_ptr<JokerChange> getJokerChange(); // nullptr if no change is requested
};


/*
	Wrapper function to position a moving piece that contains debug printing.
	@var boardSet					- ref to the player pieces set
	@var vectorToFill				- ref to the pieces unique_ptr vector returned by getInitialPositions
	@vars x,y,pieceType,jokerType	- properties of the piece
*/
void positionMovingPiece(int x, int y, PieceVector &vectorToFill, BoardSet &boardSet, char pieceType, char jokerType = '#');


/*
	Places all the remaining unpositioned pieces on the board.
	Those contain all the pieces not positioned on the corners and all the player's jokers.
	In this point there should be no flags to position.
	Each position will be unique and random, using the random context.
	This function will not return until all pieces are positioned.

	@pre - there is enough room for all pieces in the board

	@var rndCtx							- ref to a randomization context to be used to generate a random corner
	@var piecesAlreadyPositioned		- count of pieces already positioned before this function
	@var movingPieceVector				- ref to the pieces vector, from which rest of the pieces will be added
	@var boardSet						- ref to the player pieces set
	@var vectorToFill					- ref to the pieces unique_ptr vector returned by getInitialPositions

*/
void positionRestOfMovingPiecesRandomly(const int piecesAlreadyPositioned, RandomContext &rndCtx,
	BoardSet &boardSet, const vector<char>& movingPieceVector, PieceVector& vectorToFill);



/*
	Positioning scenario for the auto player algorithm.
	The booleans areFlagsOnCorners and areMovingOnCorners are both constructed after randomization.

	The possible scenarios:
		
		1. areFlagsOnCorners == true (disregards areMovingOnCorners value):
			All the flags are positioned on the corners of the board. Bombs will be positioned
			as 'guards' near the adjacent to the flags.
			If there are more flags than corners - the rest are positioned randomly.
			The rest of the pieces will be positioned randomly on the board.
		
		2.	areFlagsOnCorners == false && areMovingOnCorners == true
			Moving pieces are positioned on the corners in attempt to counterattack flags positioned on the corners
			in the second player - this is a possible scenario also on other implementations.
			The rest of the pieces will be positioned randomly on the board.

		3.	areFlagsOnCorners == false && areMovingOnCorners == false
			All pieces, including flags and bombs, will be positioned randomly on the board.

*/
class PositioningScenario
{
public:
	bool areFlagsOnCorners;
	bool areMovingOnCorners;

	PositioningScenario(bool areFlagsOnCorners, bool areMovingOnCorners) :
		areFlagsOnCorners(areFlagsOnCorners), areMovingOnCorners(areMovingOnCorners) {};
};


/*
	Random context to hold specific objects and operations carried out by a lot of the above functions.
	This is a helper class, used only as a container for function arguments.
	The objects are constructed outside of it - holding only the reference.
	The class is used only throughout getInitialPositions and the objects are created statically at its beginning.
*/
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

	/* Returns a random "corner" index - a number between [0,3] */
	int getRandomCorner() { return cornerGen(gen); }

	/* Returns a random column index - a number between [1, boardCols] */
	int getRandomCol() { return colNumGen(gen); }

	/* Returns a random row index - a number between [1, boardRows] */
	int getRandomRow() { return rowNumGen(gen); }

	/* Returns a random binary bit - a number between [0,1] */
	int getRandomBinary() { return binaryGen(gen); }

	std::mt19937& getRandomGenerator() { return gen; }

};


#endif
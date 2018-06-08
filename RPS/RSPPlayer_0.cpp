
#include "PlayerAlgorithm.h"
#include "AlgorithmRegistration.h"
#include "MyJokerChange.h"
#include "MyPiecePosition.h"
#include "MyMove.h"
#include "parser.h"

#include <algorithm>
#include <iostream>
#include <list>
#include <memory>
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


class RSPPlayer_0 : public PlayerAlgorithm
{
private:

	int ID; //1 for player 1, 2 for player 2

			//holds the piece count as received from main
	struct {
		UINT R, P, S, B, J, F;
	} initPieceCnt;


	PositioningScenario* scenario;		//This member holds the positioning scenario, as described in the class itself
	UINT boardRows, boardCols;			//Size of board
	BoardSet opponentsPieces;			//This set will hold all the known data regarding his opponent's pieces
	BoardSet playerPieces;				//This set will hold the data regarding player's own pieces
	BoardSet::iterator nextPieceToMove; //An iterator keeping the next piece to move 

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
	MyPiecePosition getNextPieceToMove(void);

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
	RSPPlayer_0(UINT boardRows, UINT boardCols,
		UINT R, UINT P, UINT S, UINT B, UINT J, UINT F, int ID);

	RSPPlayer_0() : RSPPlayer_0(BOARD_SIZE, BOARD_SIZE, R_COUNT, P_COUNT, S_COUNT, B_COUNT, J_COUNT, F_COUNT, 0)
	{
		dprintreg("Created a new RSPPlayer_0\n");
	}

	~RSPPlayer_0();

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


using namespace std;



/*
Generic function that adds a new piece to the auto player's context: BoardSet and PieceVector.
@var boardSet					- ref to the player pieces set
@var vectorToFill				- ref to the pieces unique_ptr vector returned by getInitialPositions
@vars x,y,pieceType,jokerType	- properties of the piece
*/
void addPieceToVectorAndBoard(BoardSet &boardSet, PieceVector &vectorToFill,
	const int x, const int y, const char pieceType, const char jokerType = '#')
{
	vectorToFill.push_back(make_unique<MyPiecePosition>(x, y, pieceType, jokerType));
	boardSet.emplace(x, y, pieceType, jokerType);
}

/*
Adds a new bomb to the auto player's context using #addPieceToVectorAndBoard, incrementing the bombUsed counter.
@var boardSet					- ref to the player pieces set
@var vectorToFill				- ref to the pieces unique_ptr vector returned by getInitialPositions
@var bombsUsed		- bombs counter up until this point
@var bombX, bombY	- bomb position
*/
void positionBomb(UINT& bombsUsed, const int bombX, const int bombY, PieceVector& vectorToFill,
	BoardSet& boardSet)
{
	dprint("\t - Bomb #%d choosed position (%d,%d)\n", bombsUsed + 1, bombX, bombY);
	consume(bombsUsed);

	addPieceToVectorAndBoard(boardSet, vectorToFill, bombX, bombY, 'B');

	bombsUsed++;
}

/*
Adds a new flag to the auto player's context using #addPieceToVectorAndBoard.
@var boardSet					- ref to the player pieces set
@var vectorToFill				- ref to the pieces unique_ptr vector returned by getInitialPositions
@var i							- flag counter for debug purposes
@var x,y						- flag position
*/
void positionFlag(const int i, const int x, const int y, PieceVector &vectorToFill, BoardSet &boardSet)
{
	dprint("\t - Flag #%d choosed position (%d,%d)\n", i + 1, x, y);
	consume(i);

	addPieceToVectorAndBoard(boardSet, vectorToFill, x, y, 'F');
}

void positionMovingPiece(const int x, const int y, PieceVector &vectorToFill, BoardSet &boardSet,
	const char pieceType, const char jokerType)
{
	if (pieceType != 'J')
	{
		dprint("\t - Moving Piece (%c) choosed position (%d,%d)\n", pieceType, x, y);
	}
	else
	{
		dprint("\t - Joker from type (%c) choosed position (%d,%d)\n", jokerType, x, y);
	}

	addPieceToVectorAndBoard(boardSet, vectorToFill, x, y, pieceType, jokerType);
}

/*
Generates a random position on the board, one which is unused by this player.
Afterwards it places a flag in this position, adding it to the auto player's context: BoardSet and PieceVector

@pre - an empty position exists on the board.

@var rndCtx						- ref to a randomization context to be used to generate a random position
@var boardSet					- ref to the player pieces set
@var vectorToFill				- ref to the pieces unique_ptr vector returned by getInitialPositions
@var flagIndex					- index of the flag for debug purposes
*/
void positionFlagRandomly(RandomContext& rndCtx, BoardSet &boardSet, const int flagIndex, PieceVector& vectorToFill)
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

/*
Generates a random unused corner (0-3) on the board using a boolean vector of the current selected corners.

@pre - selectedCorners is not all "true".
@pre - selectedCorners is of size 4

@var rndCtx						- ref to a randomization context to be used to generate a random corner
@var selectedCorners			- ref to the boolean vectors containing information of already occupied board corners
Indices start from the upper left corner, see definition on #chooseCorner.

@ret	The generated corner index.
*/
int generateUniqueCorner(RandomContext &rndCtx, vector<bool>& selectedCorners)
{
	int select;

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

	return select;
}


void positionRestOfMovingPiecesRandomly(const int piecesAlreadyPositioned, RandomContext &rndCtx,
	BoardSet &boardSet, const vector<char>& movingPieceVector, PieceVector& vectorToFill)
{
	int x, y, piecesCounter = (int)movingPieceVector.size();
	char random_RPSB_jokerType;

	for (int i = piecesAlreadyPositioned; i < piecesCounter;)
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



int RSPPlayer_0::fillCornersWithAlreadyOccupiedCorners(vector<bool>& selectedCorners, BoardSet& boardSet)
{
	int count = 4;

	if (selectedCorners[0] || boardSet.count(MyPiecePosition(1, 1)) > 0)
	{
		selectedCorners[0] = true;
		count--;
	}

	if (selectedCorners[1] || boardSet.count(MyPiecePosition(1, boardRows)) > 0)
	{
		selectedCorners[1] = true;
		count--;
	}

	if (selectedCorners[2] || boardSet.count(MyPiecePosition(boardCols, 1)) > 0)
	{
		selectedCorners[2] = true;
		count--;
	}


	if (selectedCorners[3] || boardSet.count(MyPiecePosition(boardCols, boardRows)) > 0)
	{
		selectedCorners[3] = true;
		count--;
	}

	return count;
}

void RSPPlayer_0::placeMovingPiecesOnCorners(vector<char> &movingPieceVector,
	const int initialMovingCnt, RandomContext &rndCtx, PieceVector& vectorToFill, BoardSet& boardSet,
	int &pieceIndex)
{

	int cornerX, cornerY, corners;
	vector<bool> selectedCorners = { false, false, false, false }; //see definition on #chooseCorner.

	corners = fillCornersWithAlreadyOccupiedCorners(selectedCorners, boardSet);

	//corners now hold number of empty corners on board
	//if it was randomized that moving pieces will be places on the corners (see definition of class PositioningScneario)

	if (scenario->areMovingOnCorners && !scenario->areFlagsOnCorners)
	{
		/* choose random piece from the moving piece vector and place it on a unique empty corner */

		shuffle(movingPieceVector.begin(), movingPieceVector.end(), rndCtx.getRandomGenerator());

		corners = (initialMovingCnt < corners) ? (initialMovingCnt) : corners;

		for (int i = 0; i < corners; i++)
		{
			const int select = generateUniqueCorner(rndCtx, selectedCorners);

			chooseCorner(select, rndCtx, cornerX, cornerY);

			positionMovingPiece(cornerX, cornerY, vectorToFill, boardSet, movingPieceVector[pieceIndex++]);

		}
	}
}


void RSPPlayer_0::chooseCorner(int select, RandomContext& rndCtx,
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

void RSPPlayer_0::fillListWithRPSBpieces(vector<char>& rpsbVector, const int bombsUsed)
{
	addAllToVector(R, 'R');
	addAllToVector(P, 'P');
	addAllToVector(S, 'S');

	for (UINT i = 0; i < (initPieceCnt.B - bombsUsed); i++)
		rpsbVector.push_back('B');
}

int RSPPlayer_0::positionFlagsAndBombs(RandomContext& rndCtx,
	BoardSet& boardSet, PieceVector& vectorToFill)
{
	/* Other static variables */
	int select, cornerX, cornerY, corners, bomb1X = 0, bomb1Y = 0, bomb2X = 0, bomb2Y = 0;

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




//assumes at least one moving piece exists
MyPiecePosition RSPPlayer_0::getNextPieceToMove(void)
{
	++nextPieceToMove;
	while (true)
	{
		if (nextPieceToMove == playerPieces.end())
			nextPieceToMove = playerPieces.begin();
		//save iterator of current,iterate until reached current, when reached end return to begin
		if (nextPieceToMove->isMoving())
			break;
		else
			++nextPieceToMove; //move the pointer to the next iterator
	}

	return *nextPieceToMove; //return the piece inside the iterator
}

unique_ptr<Move> RSPPlayer_0::checkAllAdjecentOpponents(const MyPiecePosition & piece, std::bitset<4>& boolVec, const int x, const int y)
{
	if (checkForAdjecentOpponent(MyPiecePosition(x - 1, y)))
	{
		if (piece >= MyPiecePosition(x - 1, y)) //potential win
			return make_unique<MyMove>(x, y, x - 1, y);
		else //potential lose
			boolVec[0] = false;
	}

	if (checkForAdjecentOpponent(MyPiecePosition(x + 1, y)))
	{
		if (piece >= MyPiecePosition(x + 1, y)) //potential win
			return make_unique<MyMove>(x, y, x + 1, y);
		else
			boolVec[1] = false;
	}
	if (checkForAdjecentOpponent(MyPiecePosition(x, y + 1)))
	{
		if (piece >= MyPiecePosition(x, y + 1)) //potential win
			return make_unique<MyMove>(x, y, x, y + 1);
		else
			boolVec[2] = false;
	}
	if (checkForAdjecentOpponent(MyPiecePosition(x, y - 1)))
	{
		if (piece >= MyPiecePosition(x, y - 1)) //potential win
			return make_unique<MyMove>(x, y, x, y - 1);
		else
			boolVec[3] = false;
	}
	return nullptr;
}

char RSPPlayer_0::getNewJokerRep(const char oldJokerRep)
{
	char result;
	switch (oldJokerRep)
	{
	case 'R':
		result = 'S';
		break;
	case 'S':
		result = 'P';
		break;
	case 'P':
		result = 'R';
		break;
	default:
		result = '#';
	}
	return result;
}

unique_ptr<JokerChange> RSPPlayer_0::checkAllAdjecentOpponents(const MyPiecePosition & joker, const int x, const int y)
{
	bool foundAdjecentOpponent = false;
	do
	{
		if (checkForAdjecentOpponent(MyPiecePosition(x - 1, y)))
		{
			if (joker < MyPiecePosition(x - 1, y)) //potential lose
			{
				foundAdjecentOpponent = true;
				break;
			}
		}
		if (checkForAdjecentOpponent(MyPiecePosition(x + 1, y)))
		{
			if (joker < MyPiecePosition(x + 1, y)) //potential lose
			{
				foundAdjecentOpponent = true;
				break;
			}
		}
		if (checkForAdjecentOpponent(MyPiecePosition(x, y + 1)))
		{
			if (joker < MyPiecePosition(x, y + 1)) //potential lose
			{
				foundAdjecentOpponent = true;
				break;
			}

		}
		if (checkForAdjecentOpponent(MyPiecePosition(x, y - 1))) //potential lose
		{
			if (joker < MyPiecePosition(x, y - 1)) //potential win
			{
				foundAdjecentOpponent = true;
				break;
			}
		}
	} while (false);

	if (foundAdjecentOpponent)
		return make_unique<MyJokerChange>(joker.getPosition().getX(), joker.getPosition().getY(), getNewJokerRep(joker.getJokerRep()));

	return nullptr;
}

//pos will be received as a lvalue reference, and other as rvalue reference
bool RSPPlayer_0::checkForAdjecentOpponent(const MyPiecePosition other)
{
	if (opponentsPieces.find(other) != opponentsPieces.end())
		return true;
	return false;
}

unique_ptr<Move> RSPPlayer_0::getMove()
{
	unique_ptr<Move> nextMove;
	std::bitset<4> fleeArr(0xF); //[left, right, up, down]
	bool foundMove = false;

	/*look for potential win or flee*/
	for (auto& piece : playerPieces)
	{
		if (!(piece.isMoving()))
			continue;

		const MyPoint point = piece.getPosition();
		int x = point.getX(), y = point.getY();
		nextMove = move(checkAllAdjecentOpponents(piece, fleeArr, x, y));

		if (nextMove)
		{
			foundMove = true;
			break;
		}
		else if (fleeArr.count() < 4)
		{
			//if not all are true, it means that there is an opponent piece stronger than me,
			//try to flee from it
			nextMove = move(getLegalMove(point, fleeArr));

			if (nextMove)
			{
				foundMove = true;
				break;
			}

		}
	}
	if (!foundMove)
	{
		/*perform random move*/
		MyPiecePosition nextPieceToMove = getNextPieceToMove();
		MyPiecePosition firstPieceToMove = nextPieceToMove;
		do
		{
			MyPoint point = nextPieceToMove.getPosition();
			nextMove = move(getLegalMove(point));
			if (nextMove)
				break;
			nextPieceToMove = getNextPieceToMove();
			if (nextPieceToMove == firstPieceToMove)
				return nullptr; //no moves to do
		} while (true);
	}

	if (!nextMove)
	{
		dprint("/******in getMove got null******/\n");
		return nullptr;
	}


	//update player's board
	auto& t = *playerPieces.find(MyPiecePosition(nextMove->getFrom().getX(), nextMove->getFrom().getY()));
	char type = t.getPiece();
	char jokerType = t.getJokerRep();
	playerPieces.erase(t);
	playerPieces.insert(MyPiecePosition(nextMove->getTo().getX(), nextMove->getTo().getY(), type, jokerType));

	return move(nextMove);
}


char RSPPlayer_0::getRandomJokerChahge(int rand, char jokerRep)
{
	switch (jokerRep)
	{
	case 'R':
		switch (rand)
		{
		case 0:
			return 'S';
		case 1:
			return 'P';
		case 2:
			return 'B';
		default:
			return '#';
		}
	case 'P':
		switch (rand)
		{
		case 0:
			return 'S';
		case 1:
			return 'R';
		case 2:
			return 'B';
		default:
			return '#';
		}
	case 'S':
		switch (rand)
		{
		case 0:
			return 'P';
		case 1:
			return 'R';
		case 2:
			return 'B';
		default:
			return '#';
		}
	case 'B':
		switch (rand)
		{
		case 0:
			return 'S';
		case 1:
			return 'R';
		case 2:
			return 'P';
		default:
			return '#';
		}
	default:
		return '#';
	}
}

unique_ptr<MyMove> RSPPlayer_0::getLegalMove(const MyPoint& point)
{
	std::bitset<4> legalDirections(0xF);

	return move(getLegalMove(point, legalDirections));

}

bool RSPPlayer_0::existsOnBoardSet(const MyPoint& point)
{
	return playerPieces.count(MyPiecePosition(point.getX(), point.getY())) > 0;
}

void RSPPlayer_0::removeOutOfBoundsDirections(const MyPoint& point, std::bitset<4>& legalFleeDirections)
{
	//[left, right, up, down]
	if (point.getX() == 1)
		legalFleeDirections[0] = false;

	else if (point.getX() == (int)boardCols)
		legalFleeDirections[1] = false;

	if (point.getY() == 1)
		legalFleeDirections[2] = false;

	else if (point.getY() == (int)boardRows)
		legalFleeDirections[3] = false;
}

const MyPoint getPointByDirection(const MyPoint& point, int direction)
{
	switch (direction)
	{
	case 0:
		return MyPoint(point.getX() - 1, point.getY()); //left

	case 1:
		return MyPoint(point.getX() + 1, point.getY()); //right

	case 2:
		return MyPoint(point.getX(), point.getY() - 1); //up

	case 3:
		return MyPoint(point.getX(), point.getY() + 1); //down
	}

	//should not get here

	printf("Error: Illegal Direction in getPointByDirection\n");

	return point;
}

unique_ptr<MyMove> RSPPlayer_0::getLegalMove(const MyPoint& point, bitset<4>& legalFleeDirections)
{
	vector<int> possibleMoves(0);
	int chosenDirection = 0;

	removeOutOfBoundsDirections(point, legalFleeDirections);

	/* Get all directions that does not include opponent piece*/
	for (int i = 0; i < 4; i++)
	{
		//if move is legal already and the to Point does not contain a piece of ours
		if (legalFleeDirections[i] && !existsOnBoardSet(getPointByDirection(point, i)))
		{
			possibleMoves.push_back(i);
			//dprint("\tPossible move: index %d\n", i);
		}
	}


	/* If there is no available direction than cannot find legal move which is not a fight */

	if (possibleMoves.size() == 0)
		return nullptr;


	if (possibleMoves.size() > 1)
	{
		random_device	seed;
		mt19937			gen(seed());
		uniform_int_distribution<> genDirection(0, (int)possibleMoves.size() - 1);
		chosenDirection = genDirection(gen);
	}
	//else it already is 0 and it will take the first and only element
	const MyPoint& chosenDirectionPoint = getPointByDirection(point, possibleMoves[chosenDirection]);
	return make_unique<MyMove>(point.getX(), point.getY(),
		chosenDirectionPoint.getX(), chosenDirectionPoint.getY());
}



RSPPlayer_0::RSPPlayer_0(UINT rows, UINT cols, UINT R, UINT P, UINT S, UINT B, UINT J, UINT F, int ID) : ID(ID)
{
	random_device				seed;			//Will be used to obtain a seed for the random number engine
	mt19937						gen(seed());	//Standard mersenne_twister_engine seeded with seed()
	uniform_int_distribution<>	dis(0, 1);		//Distributed random

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


RSPPlayer_0::~RSPPlayer_0()
{
	dprintreg("DELETED RSPPlayer_0\n");
	delete scenario;
}

void RSPPlayer_0::getInitialPositions(int player, PieceVector& vectorToFill)
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	/** Parameters Initialization **/
	vector<char> rpsbVector;
	int initialMovingCnt, pieceIndex = 0;

	////////////////////////////////////////////////////////////////////////////////////////////////////////

	/* Random Generators Context */
	random_device				seed;
	mt19937						gen(seed());
	uniform_int_distribution<>	binaryGen(0, 1), cornerGen(0, 3),
		rowNumGen(1, boardRows), colNumGen(1, boardCols);
	RandomContext rndCtx(gen, colNumGen, rowNumGen, cornerGen, binaryGen);

	////////////////////////////////////////////////////////////////////////////////////////////////////////

	consume(player);
	dprint("\n#### Initial Positions || Player %d\n", player);
	dprint("\t - Scnario: areFlagsOnCoreners = %d, areMovingOnCorners = %d\n", scenario->areFlagsOnCorners, scenario->areMovingOnCorners);

	vectorToFill.clear(); //sanity

						  ////////////////////////////////////////////////////////////////////////////////////////////////////////

						  /**** Position All Flags (and maybe some Bombs around them) *****/

	const int bombsUsed = positionFlagsAndBombs(rndCtx, playerPieces, vectorToFill);

	////////////////////////////////////////////////////////////////////////////////////////////////////////

	/********* Moving Pieces Section **********/

	fillListWithRPSBpieces(rpsbVector, bombsUsed);
	initialMovingCnt = (int)rpsbVector.size();

	////////////////////////////////////////////////////////////////////////////////////////////////////////

	/** if chosen by scenario, moving pieces will be set on the corners to counter attack flags on corners **/

	placeMovingPiecesOnCorners(rpsbVector, initialMovingCnt, rndCtx, vectorToFill, playerPieces, pieceIndex);

	////////////////////////////////////////////////////////////////////////////////////////////////////////

	/********* Position the rest of the pieces randomly on board ***********/

	//add the Jokers, don't place them on the corners (its a waste!)
	addAllToVector(J, 'J');

	positionRestOfMovingPiecesRandomly(pieceIndex, rndCtx, playerPieces, rpsbVector, vectorToFill);

}

void RSPPlayer_0::notifyOnInitialBoard(const Board & b, const vector<unique_ptr<FightInfo>>& fights)
{
	int owner;
	for (UINT i = 1; i <= boardRows; i++)
	{
		for (UINT j = 1; j <= boardCols; j++)
		{
			owner = b.getPlayer(MyPoint(j, i));
			if (owner == 0 || owner == ID) //Point is empty or owned by player
				continue;
			else
				opponentsPieces.emplace(j, i);
		}
	}
	nextPieceToMove = playerPieces.begin(); //start with first piece

	for (auto& fightInfo : fights)
	{
		const MyPoint fightPoint = fightInfo->getPosition();
		const MyPiecePosition fightPos(fightPoint.getX(), fightPoint.getY());
		int fightWinner = fightInfo->getWinner();
		char winningType = fightInfo->getPiece(fightWinner);
		int opponentID = (ID == 1 ? 2 : 1), playerID = ID;

		auto playerPieceIter = playerPieces.find(fightPos);

		if (fightWinner == opponentID) //player lost fight, remove piece from players set, and update piece type and if moving
		{
			//update new date regarding opponent's piece
			if (winningType == 'P' || winningType == 'R' || winningType == 'S')
			{
				if (opponentsPieces.count(fightPos) > 0)	//could be that the piece is no longer there if it lost another fight
				{
					auto opponentPieceIter = opponentsPieces.find(fightPos);
					opponentPieceIter->setPieceType(winningType);
					opponentPieceIter->setMovingPiece(true);
				}
			}

			//in any case, remove player's losing piece
			if (*playerPieceIter == *nextPieceToMove)
				++nextPieceToMove;

			if (playerPieces.count(fightPos) > 0)	//could be that the piece is no longer there if it lost another fight
				playerPieces.erase(playerPieceIter);

		}

		else if (fightWinner == playerID) //player won, remove opponent's piece
		{
			//although won, remove player's piece if it's a bomb
			if (winningType == 'B')
			{
				if (*playerPieceIter == *nextPieceToMove)
					++nextPieceToMove;

				if (playerPieces.count(fightPos) > 0)	//could be that the piece is no longer there if it lost another fight
					playerPieces.erase(playerPieceIter);
			}
		}

		else //TIE - remove both pieces
		{
			if (*playerPieceIter == *nextPieceToMove)
				++nextPieceToMove;

			if (playerPieces.count(fightPos) > 0)		//could be that the piece is no longer there if it lost another fight
				playerPieces.erase(playerPieceIter);
		}
	}
}


void RSPPlayer_0::notifyOnOpponentMove(const Move & move)
{
	auto& pieceToMove = *opponentsPieces.find(MyPiecePosition(move.getFrom().getX(), move.getFrom().getY()));
	char type = pieceToMove.getPiece();
	char jokerRep = pieceToMove.getJokerRep();
	MyPoint getTo(move.getTo());
	opponentsPieces.erase(pieceToMove);
	opponentsPieces.insert(MyPiecePosition(getTo.getX(), getTo.getY(), type, jokerRep));
}

void RSPPlayer_0::notifyFightResult(const FightInfo & fightInfo)
{
	const MyPoint fightPoint = fightInfo.getPosition();
	const MyPiecePosition fightPos(fightPoint.getX(), fightPoint.getY());
	int fightWinner = fightInfo.getWinner();
	char winningType = fightInfo.getPiece(fightWinner);
	int opponentID = (ID == 1 ? 2 : 1), playerID = ID;

	auto opponentPieceIter = opponentsPieces.find(fightPos);
	auto playerPieceIter = playerPieces.find(fightPos);

	if (fightWinner == opponentID) //player lost fight, remove piece from players set, and update piece type and if moving
	{
		//update new date regarding opponent's piece
		if (winningType == 'P' || winningType == 'R' || winningType == 'S')
		{
			opponentPieceIter->setPieceType(winningType);
			opponentPieceIter->setMovingPiece(true);
		}
		//remove opponent's piece if it's a bomb
		else if (winningType == 'B')
		{
			opponentsPieces.erase(opponentPieceIter);
		}
		//in any case, remove player's losing piece
		if (*playerPieceIter == *nextPieceToMove)
			++nextPieceToMove;
		playerPieces.erase(playerPieceIter);
	}

	else if (fightWinner == playerID) //player won, remove opponent's piece
	{
		//although won, remove player's piece if it's a bomb
		if (winningType == 'B')
		{
			if (*playerPieceIter == *nextPieceToMove)
				++nextPieceToMove;
			playerPieces.erase(playerPieceIter);
		}
		//in any case, remove opponent's losing piece
		opponentsPieces.erase(opponentPieceIter);
	}

	else //TIE - remove both pieces
	{
		opponentsPieces.erase(opponentPieceIter);
		if (*playerPieceIter == *nextPieceToMove)
			++nextPieceToMove;
		playerPieces.erase(playerPieceIter);
	}
}

unique_ptr<JokerChange> RSPPlayer_0::getJokerChange()
{
	unique_ptr<JokerChange> nextJokerChange;
	for (auto& piece : playerPieces)
	{
		if (piece.getJokerRep() == '#')
			continue;
		const MyPoint point = piece.getPosition();
		int x = point.getX(), y = point.getY();
		nextJokerChange = move(checkAllAdjecentOpponents(piece, x, y));
		if (nextJokerChange)
		{
			if (piece.getJokerRep() == 'B')
				piece.setMovingPiece(true);
			else if (nextJokerChange->getJokerNewRep() == 'B')
				piece.setMovingPiece(false);
			piece.setJokerRep(nextJokerChange->getJokerNewRep());
			return nextJokerChange;
		}
	}

	/*
	//get random jokerChange
	bool firstLoop = true;
	for (auto piece = nextPieceToMove; *piece != *nextPieceToMove || firstLoop;)
	{
	firstLoop = false;
	if (piece->getJokerRep() == '#')
	{
	++piece;
	if (piece == playerPieces.end())
	piece = playerPieces.begin();
	continue;
	}
	const MyPoint point = piece->getPosition();
	int x = point.getX(), y = point.getY();
	random_device	seed;
	mt19937			gen(seed());
	uniform_int_distribution<> genDirection(0, 2);
	unique_ptr<MyJokerChange> ret = make_unique<MyJokerChange>(x, y, getRandomJokerChahge(genDirection(gen), piece->getJokerRep()));
	if (piece->getJokerRep() == 'B')
	piece->setMovingPiece(true);
	else if (ret->getJokerNewRep() == 'B')
	piece->setMovingPiece(false);
	piece->setJokerRep(ret->getJokerNewRep());
	++piece;
	if (piece == playerPieces.end())
	piece = playerPieces.begin();
	return ret;
	}
	*/
	return nullptr;
}

REGISTER_ALGORITHM(0)



/*
#include "AutoPlayerAlgorithm.h"
#include "AlgorithmRegistration.h"


class RSPPlayer_0 : public AutoPlayerAlgorithm
{
public:
	RSPPlayer_0();
};


RSPPlayer_0::RSPPlayer_0() :
	AutoPlayerAlgorithm(BOARD_SIZE, BOARD_SIZE, R_COUNT, P_COUNT, S_COUNT, B_COUNT, J_COUNT, F_COUNT, 0)
{
	dprintreg("Created a new RSPPlayer_0\n");
}

extern "C" void myprint(void)
{
	printf("In Myprint!!\n");
}

REGISTER_ALGORITHM(0)
*/
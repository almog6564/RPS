

#include "AutoPlayerAlgorithm.h"
#include <algorithm>
#include <list>
using namespace std;


AutoPlayerAlgorithm::AutoPlayerAlgorithm(UINT rows, UINT cols, UINT R, UINT P, UINT S, UINT B, UINT J, UINT F, int ID): ID(ID)
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
	//initialize private Board
	//privateBoard = vector<PieceVector>(rows);
	//for (UINT i = 0; i < rows; i++) privateBoard[i] = vector<unique_ptr<PiecePosition>>(cols);
}


AutoPlayerAlgorithm::~AutoPlayerAlgorithm()
{
	delete scenario;
}

void AutoPlayerAlgorithm::getInitialPositions(int player, PieceVector& vectorToFill)
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	/** Parameters Initialization **/
	//BoardSet boardSet;	
	vector<char> movingPieceVector;
	int initialMovingCnt, pieceIndex = 0, bombsUsed;

	////////////////////////////////////////////////////////////////////////////////////////////////////////

	/* Random Generators Context */
	random_device				seed;
	mt19937						gen(seed());
	uniform_int_distribution<>	binaryGen(0, 1), cornerGen(0, 3),
		rowNumGen(1, boardRows), colNumGen(1, boardCols);
	RandomContext rndCtx(gen, colNumGen, rowNumGen, cornerGen, binaryGen);

	////////////////////////////////////////////////////////////////////////////////////////////////////////

	dprint("\n#### Initial Positions || Player %d\n", player);
	dprint("\t - Scnario: areFlagsOnCoreners = %d, areMovingOnCorners = %d\n", scenario->areFlagsOnCorners, scenario->areMovingOnCorners);

	vectorToFill.clear(); //sanity

	////////////////////////////////////////////////////////////////////////////////////////////////////////

	/**** Position All Flags (and maybe some Bombs around them) *****/

	bombsUsed = positionFlagsAndBombs(rndCtx, boardSet, vectorToFill);

	////////////////////////////////////////////////////////////////////////////////////////////////////////

	/********* Moving Pieces Section **********/

	fillListWithMovingPieces(movingPieceVector, bombsUsed);
	initialMovingCnt = (int) movingPieceVector.size();

	////////////////////////////////////////////////////////////////////////////////////////////////////////

	/** if chosen by scenario, moving pieces will be set on the corners to counter attack flags on corners **/

	placeMovingPiecesOnCorners(movingPieceVector, initialMovingCnt, rndCtx, vectorToFill, boardSet, pieceIndex);

	////////////////////////////////////////////////////////////////////////////////////////////////////////

	/********* Position the rest of the pieces randomly on board ***********/

	//add the Jokers, don't place them on the corners (its a waste!)
	addAllToVector(J, 'J'); 

	positionRestOfMovingPiecesRandomly(pieceIndex, initialMovingCnt, rndCtx, boardSet, movingPieceVector, vectorToFill);
	
}

void AutoPlayerAlgorithm::notifyOnInitialBoard(const Board & b, const vector<unique_ptr<FightInfo>>& fights)
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
	for (auto& fight : fights)
	{
		if (fight->getWinner() != ID) //player lost fight, remove piece from players set, and update piece type
		{
			MyPoint tempPoint = fight->getPosition();
			int x = tempPoint.getX(), y = tempPoint.getY();
			char type = fight->getPiece(ID == 1 ? 2 : 1);
			boardSet.erase(boardSet.find(MyPiecePosition(x, y)));
			auto it = opponentsPieces.find(MyPiecePosition(x, y));
			if (it != opponentsPieces.end())
			{
				it->setPieceType(type);
				if (type == 'P' || type == 'R' || type == 'S')
					it->setMovingPiece(true);
			}
		}
	}
	nextPieceToMove = boardSet.begin(); //start with first piece
	//nextPieceToAttack = opponentsPieces.begin();
}


void AutoPlayerAlgorithm::notifyOnOpponentMove(const Move & move)
{
	/*dprint("notifyOnOpponentMove of player %d: opponentsPieces.size = %d\n", ID+1, (int)opponentsPieces.size());
	for (auto& p : opponentsPieces)
	{
		dprint("(%d,%d)[%c],  ", p.getPosition().getX(), p.getPosition().getY(), p.getPiece());
	}
	dprint("\nSearching for move (%d,%d)->(%d,%d)\n", move.getFrom().getX(), move.getFrom().getY(), move.getTo().getX(), move.getTo().getY());*/
	//update opponent's piece, because the key is imutable, remove and insert a new piece
	auto& t = *opponentsPieces.find(MyPiecePosition(move.getFrom().getX(), move.getFrom().getY()));
	char type = t.getPiece();
	opponentsPieces.erase(t);
	opponentsPieces.insert(MyPiecePosition(move.getTo().getX(), move.getTo().getY(), type, true));
	//save last move for next_move decision
	lastMove = move.getTo(); //call copy c'tor
}

void AutoPlayerAlgorithm::notifyFightResult(const FightInfo & fightInfo)
{
	MyPoint tempPoint = fightInfo.getPosition();
	int winner = fightInfo.getWinner();
	if (winner == (ID == 1 ? 2 : 1)) //player lost fight, remove piece from players set, and update piece type and if moving
	{
		auto& piece = *opponentsPieces.find(MyPiecePosition(tempPoint.getX(), tempPoint.getY()));
		auto it = boardSet.find(MyPiecePosition(tempPoint.getX(), tempPoint.getY()));
		if (it == nextPieceToMove)
			++nextPieceToMove;
		boardSet.erase(it);
		char type = fightInfo.getPiece(winner);
		piece.setPieceType(type);
		if (type == 'P' || type == 'R' || type == 'S')
			piece.setMovingPiece(true);
	}
	else if (winner == ID)//player won, remove opponent's piece
	{
		opponentsPieces.erase(opponentsPieces.find(MyPiecePosition(tempPoint.getX(), tempPoint.getY())));
	}
	else //TIE - remove both pieces
	{
		opponentsPieces.erase(opponentsPieces.find(MyPiecePosition(tempPoint.getX(), tempPoint.getY())));
		boardSet.erase(boardSet.find(MyPiecePosition(tempPoint.getX(), tempPoint.getY())));
	}
}

//assumes at least one moving piece exists
MyPiecePosition AutoPlayerAlgorithm::getNextPieceToMove()
{
	++nextPieceToMove;
	while (true) 
	{
		if (nextPieceToMove == boardSet.end())
			nextPieceToMove = boardSet.begin();
		//save iterator of current,iterate until reached current, when reached end return to begin
		char tempType = (nextPieceToMove)->getPiece();
		char tempJoker = (nextPieceToMove)->getJokerRep();
		if (tempType == 'P' || tempType == 'S' || tempType == 'R' ||
			(tempType == 'J' && (tempJoker == 'P' || tempType == 'S' || tempType == 'R'))) //A moving piece
			break;
		else
			++nextPieceToMove; //move the pointer to the next iterator
	}
	return *nextPieceToMove; //return the piece inside the iterator
}

const MyPiecePosition & AutoPlayerAlgorithm::getNextPieceToAttack()
{
	++nextPieceToAttack;
	while (nextPieceToAttack != opponentsPieces.end())
	{
		if ((nextPieceToAttack)->isMoving()) //prefer to attack a potential flag
			++nextPieceToAttack; //move to the next iterator
		else
			break;
	}
	return *nextPieceToAttack; //return the piece inside the iterator
}

unique_ptr<Move> AutoPlayerAlgorithm::checkAllAdjecentOpponents(const MyPiecePosition & piece, std::bitset<4>& boolVec, int x, int y)
{
	if (checkForAdjecentOpponent(piece, MyPiecePosition(x - 1, y)))
	{
		if (piece >= MyPiecePosition(x - 1, y)) //potential win
			return make_unique<MyMove>(x, y, x - 1, y);
		else //potential lose
			boolVec[0] = false;
	}

	if (checkForAdjecentOpponent(piece, MyPiecePosition(x + 1, y)))
	{
		if (piece >= MyPiecePosition(x + 1, y)) //potential win
			return make_unique<MyMove>(x, y, x + 1, y);
		else
			boolVec[1] = false;
	}
	if (checkForAdjecentOpponent(piece, MyPiecePosition(x, y + 1)))
	{
		if (piece >= MyPiecePosition(x, y + 1)) //potential win
			return make_unique<MyMove>(x, y, x, y + 1);
		else
			boolVec[2] = false;
	}
	if (checkForAdjecentOpponent(piece, MyPiecePosition(x, y - 1)))
	{
		if (piece >= MyPiecePosition(x, y - 1)) //potential win
			return make_unique<MyMove>(x, y, x, y - 1);
		else
			boolVec[3] = false;
	}
	return nullptr;
}

unique_ptr<JokerChange> AutoPlayerAlgorithm::checkAllAdjecentOpponents(const MyPiecePosition & joker, int x, int y)
{
	if (checkForAdjecentOpponent(joker, MyPiecePosition(x - 1, y)))
	{
		if (!(joker >= MyPiecePosition(x - 1, y))) //potential lose
		{};//return make_unique<MyMove>(x, y, x - 1, y);
	}

	if (checkForAdjecentOpponent(joker, MyPiecePosition(x + 1, y)))
	{
		if (!(joker >= MyPiecePosition(x + 1, y))) //potential lose
		{};//return make_unique<MyMove>(x, y, x + 1, y);
	}
	if (checkForAdjecentOpponent(joker, MyPiecePosition(x, y + 1)))
	{
		if (!(joker >= MyPiecePosition(x, y + 1))) //potential lose
		{};// return make_unique<MyMove>(x, y, x, y + 1);

	}
	if (!(checkForAdjecentOpponent(joker, MyPiecePosition(x, y - 1)))) //potential lose
	{
		if (joker >= MyPiecePosition(x, y - 1)) //potential win
		{};// return make_unique<MyMove>(x, y, x, y - 1);

	}
	return nullptr;
}

//pos will be received as a lvalue reference, and other as rvalue reference
bool AutoPlayerAlgorithm::checkForAdjecentOpponent(const MyPiecePosition& pos, const MyPiecePosition other)
{
	MyPoint point = pos.getPosition();
	if (opponentsPieces.find(other) != opponentsPieces.end())
		return true;
	return false;
}

unique_ptr<Move> AutoPlayerAlgorithm::getMove()
{
	unique_ptr<Move> nextMove;
	std::bitset<4> fleeArr(0xF); //[left, right, up, down]
	bool foundMove = false;

	/*look for potential win or flee*/
	for (auto& piece : boardSet)
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
		else if( fleeArr.count() < 4 )
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
	auto& t = *boardSet.find(MyPiecePosition(nextMove->getFrom().getX(), nextMove->getFrom().getY()));
	char type = t.getPiece();
	boardSet.erase(t);
	boardSet.insert(MyPiecePosition(nextMove->getTo().getX(), nextMove->getTo().getY(), type, true));

	return move(nextMove);

	//TODOS: 1. joker change

}

unique_ptr<JokerChange> AutoPlayerAlgorithm::getJokerChange()
{
	return nullptr;
}

unique_ptr<MyMove> AutoPlayerAlgorithm::getLegalMove(const MyPoint& point)
{
	std::bitset<4> legalDirections(0xF);

	return move(getLegalMove(point, legalDirections));

}

bool AutoPlayerAlgorithm::existsOnBoardSet(const MyPoint& point)
{
	return boardSet.count(MyPiecePosition(point.getX(), point.getY())) > 0;
}

void AutoPlayerAlgorithm::removeOutOfBoundsDirections(const MyPoint& point, std::bitset<4>& legalFleeDirections)
{
	//[left, right, up, down]
	if (point.getX() == 1)
		legalFleeDirections[0] = false;

	else if (point.getX() == (int) boardCols)
		legalFleeDirections[1] = false;

	if (point.getY() == 1)
		legalFleeDirections[2] = false;

	else if (point.getY() == (int) boardRows)
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

unique_ptr<MyMove> AutoPlayerAlgorithm::getLegalMove(const MyPoint& point, bitset<4>& legalFleeDirections)
{
	vector<int> possibleMoves(0);
	int chosenDirection = 0;

	//dprint("getLegalMove: point: (%d,%d) legalFleeDirections = %s\n",
	//	point.getX(), point.getY(), legalFleeDirections.to_string().c_str());

	removeOutOfBoundsDirections(point, legalFleeDirections);

	//dprint("removeOutOfBoundsDirections: legalFleeDirections = %s\n", legalFleeDirections.to_string().c_str());

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

	if(possibleMoves.size() == 0)
		return nullptr;


	if (possibleMoves.size() > 1) 
	{
		random_device	seed;
		mt19937			gen(seed());
		uniform_int_distribution<> genDirection(0, (int) possibleMoves.size() - 1);
		chosenDirection = genDirection(gen);
	}
	//else it already is 0 and it will take the first and only element
	//dprint("Had %lu directions to choose from. Chose direction possibleMoves[%d] = %d\n", possibleMoves.size(), chosenDirection, possibleMoves[chosenDirection]);
	const MyPoint& chosenDirectionPoint = getPointByDirection(point, possibleMoves[chosenDirection]);
	//dprint("ChosenDirectionPoint: (%d,%d)\n", chosenDirectionPoint.getX(), chosenDirectionPoint.getY());
	return make_unique<MyMove>(point.getX(), point.getY(),
		chosenDirectionPoint.getX(), chosenDirectionPoint.getY());
}

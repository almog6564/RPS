#include "AutoPlayerAlgorithm.h"
#include <algorithm>
#include <iostream>
#include <list>
#include "MyJokerChange.h"

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
}


AutoPlayerAlgorithm::~AutoPlayerAlgorithm()
{
	delete scenario;
}

void AutoPlayerAlgorithm::getInitialPositions(int player, PieceVector& vectorToFill)
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	/** Parameters Initialization **/
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

	bombsUsed = positionFlagsAndBombs(rndCtx, playerPieces, vectorToFill);

	////////////////////////////////////////////////////////////////////////////////////////////////////////

	/********* Moving Pieces Section **********/

	fillListWithMovingPieces(movingPieceVector, bombsUsed);
	initialMovingCnt = (int) movingPieceVector.size();

	////////////////////////////////////////////////////////////////////////////////////////////////////////

	/** if chosen by scenario, moving pieces will be set on the corners to counter attack flags on corners **/

	placeMovingPiecesOnCorners(movingPieceVector, initialMovingCnt, rndCtx, vectorToFill, playerPieces, pieceIndex);

	////////////////////////////////////////////////////////////////////////////////////////////////////////

	/********* Position the rest of the pieces randomly on board ***********/

	//add the Jokers, don't place them on the corners (its a waste!)
	addAllToVector(J, 'J'); 
	initialMovingCnt = (int)movingPieceVector.size();

	positionRestOfMovingPiecesRandomly(pieceIndex, initialMovingCnt, rndCtx, playerPieces, movingPieceVector, vectorToFill);
	
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

	nextPieceToMove = playerPieces.begin(); //start with first piece

	for (auto& fightInfo : fights)
	{
		const MyPoint fightPoint = fightInfo->getPosition();
		const MyPiecePosition fightPos(fightPoint.getX(), fightPoint.getY());
		int fightWinner = fightInfo->getWinner();
		char winningType = fightInfo->getPiece(fightWinner);
		int opponentID = (ID == 1 ? 2 : 1), playerID = ID;

		auto playerPieceIter = playerPieces.find(fightPos);
		auto opponentPieceIter = opponentsPieces.find(fightPos);

		if (fightWinner == opponentID) //player lost fight, remove piece from players set, and update piece type and if moving
		{
			//update new date regarding opponent's piece
			if (winningType == 'P' || winningType == 'R' || winningType == 'S')
			{
				opponentPieceIter->setPieceType(winningType);
				opponentPieceIter->setMovingPiece(true);
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
		}

		else //TIE - remove both pieces
		{
			if (*playerPieceIter == *nextPieceToMove)
				++nextPieceToMove;
			playerPieces.erase(playerPieceIter);
		}
	}
}


void AutoPlayerAlgorithm::notifyOnOpponentMove(const Move & move)
{
	auto& pieceToMove = *opponentsPieces.find(MyPiecePosition(move.getFrom().getX(), move.getFrom().getY()));
	char type = pieceToMove.getPiece();
	char jokerRep = pieceToMove.getJokerRep();
	MyPoint getTo(move.getTo());
	opponentsPieces.erase(pieceToMove);
	opponentsPieces.insert(MyPiecePosition(getTo.getX(), getTo.getY(), type, jokerRep));
}

void AutoPlayerAlgorithm::notifyFightResult(const FightInfo & fightInfo)
{
	const MyPoint fightPoint = fightInfo.getPosition();
	const MyPiecePosition fightPos(fightPoint.getX(), fightPoint.getY());
	int fightWinner        = fightInfo.getWinner();
	char winningType       = fightInfo.getPiece(fightWinner);
	int opponentID         = (ID == 1 ? 2 : 1), playerID = ID;

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

//assumes at least one moving piece exists
MyPiecePosition AutoPlayerAlgorithm::getNextPieceToMove()
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

char AutoPlayerAlgorithm::getNewJokerRep(char oldJokerRep)
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

unique_ptr<JokerChange> AutoPlayerAlgorithm::checkAllAdjecentOpponents(const MyPiecePosition & joker, int x, int y)
{
	bool foundAdjecentOpponent = false;
	do
	{
		if (checkForAdjecentOpponent(joker, MyPiecePosition(x - 1, y)))
		{
			if (joker < MyPiecePosition(x - 1, y)) //potential lose
			{
				foundAdjecentOpponent = true;
				break;
			}
		}
		if (checkForAdjecentOpponent(joker, MyPiecePosition(x + 1, y)))
		{
			if (joker < MyPiecePosition(x + 1, y)) //potential lose
			{
				foundAdjecentOpponent = true;
				break;
			}
		}
		if (checkForAdjecentOpponent(joker, MyPiecePosition(x, y + 1)))
		{
			if (joker < MyPiecePosition(x, y + 1)) //potential lose
			{
				foundAdjecentOpponent = true;
				break;
			}

		}
		if (checkForAdjecentOpponent(joker, MyPiecePosition(x, y - 1))) //potential lose
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
	auto& t = *playerPieces.find(MyPiecePosition(nextMove->getFrom().getX(), nextMove->getFrom().getY()));
	char type = t.getPiece();
	char jokerType = t.getJokerRep();
	playerPieces.erase(t);
	playerPieces.insert(MyPiecePosition(nextMove->getTo().getX(), nextMove->getTo().getY(), type, jokerType));

	return move(nextMove);
}

unique_ptr<JokerChange> AutoPlayerAlgorithm::getJokerChange()
{
	
	unique_ptr<JokerChange> nextJokerChange;
	for (auto& piece : playerPieces)				//there can be no jokers at all
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
	
	return nullptr;
}

char AutoPlayerAlgorithm::getRandomJokerChahge(int rand, char jokerRep)
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

unique_ptr<MyMove> AutoPlayerAlgorithm::getLegalMove(const MyPoint& point)
{
	std::bitset<4> legalDirections(0xF);

	return move(getLegalMove(point, legalDirections));

}

bool AutoPlayerAlgorithm::existsOnBoardSet(const MyPoint& point)
{
	return playerPieces.count(MyPiecePosition(point.getX(), point.getY())) > 0;
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
	const MyPoint& chosenDirectionPoint = getPointByDirection(point, possibleMoves[chosenDirection]);
	return make_unique<MyMove>(point.getX(), point.getY(),
		chosenDirectionPoint.getX(), chosenDirectionPoint.getY());
}



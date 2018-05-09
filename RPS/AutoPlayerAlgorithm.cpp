

#include "AutoPlayerAlgorithm.h"
#include "MyMove.h"

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
	//global changin MyPoint
	for (UINT i = 1; i < boardRows; i++)
	{
		for (UINT j = 1; j < boardCols; j++)
		{
			owner = b.getPlayer(MyPoint(j, i)); //move constructor will be called
			if (owner == 0 || owner == ID) //Point is empty or owned by player
				continue;
			else
				opponentsPieces.emplace(i, j);
		}
	}
	for (auto& fight : fights)
	{
		if (fight->getWinner() != ID) //player lost fight, remove piece from players set, and update piece type
		{
			MyPoint tempPoint = fight->getPosition();
			char type = fight->getPiece(ID == 1 ? 2 : 1);
			boardSet.erase(boardSet.find(MyPiecePosition(tempPoint.getX(), tempPoint.getY())));
			auto& piece = *opponentsPieces.find(MyPiecePosition(tempPoint.getX(), tempPoint.getY()));
			piece.setPieceType(type);
			if (type == 'P' || type == 'R' || type == 'S')
				piece.setMovingPiece(true);
		}
	}
	nextPieceToMove = boardSet.begin(); //start with first piece
	nextPieceToAttack = opponentsPieces.begin();
}


void AutoPlayerAlgorithm::notifyOnOpponentMove(const Move & move)
{
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
	if (fightInfo.getWinner() != ID) //player lost fight, remove piece from players set, and update piece type and if moving
	{
		char type = fightInfo.getPiece(ID == 1 ? 2 : 1);
		auto& piece = *opponentsPieces.find(MyPiecePosition(tempPoint.getX(), tempPoint.getY()));
		boardSet.erase(boardSet.find(MyPiecePosition(tempPoint.getX(), tempPoint.getY())));
		piece.setPieceType(type);
		if (type == 'P' || type == 'R' || type == 'S')
			piece.setMovingPiece(true);
	}
	else //player won, remove opponent's piece
	{
		opponentsPieces.erase(opponentsPieces.find(MyPiecePosition(tempPoint.getX(), tempPoint.getY())));
	}

	/*TIE!!!*/
}

const MyPiecePosition & AutoPlayerAlgorithm::getNextPieceToMove()
{
	while (nextPieceToMove != boardSet.end())
	{
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
	while (nextPieceToAttack != opponentsPieces.end())
	{
		if ((nextPieceToAttack)->isMoving()) //prefer to attack a potential flag
			++nextPieceToAttack; //move to the next iterator
		else
			break;
	}
	return *nextPieceToAttack; //return the piece inside the iterator
}

//pos will be recieved as a lvalue reference, and other as rvalue reference
bool AutoPlayerAlgorithm::checkForAdjecentOpponent(const MyPiecePosition& pos, const MyPiecePosition other)
{
	MyPoint point = pos.getPosition();
	if (opponentsPieces.find(other) != opponentsPieces.end())
		return true;
	return false;
}

unique_ptr<Move> AutoPlayerAlgorithm::getMove()
{
	//MyPiecePosition nextPieceToMove = getNextPieceToMove();
	//MyPiecePosition nextPieceToAttack = getNextPieceToAttack();
	
	/*look for petential win or flee*/
	for (auto& piece : boardSet)
	{
		MyPoint point = piece.getPosition();
		int x = point.getX(), y = point.getY();
		if (checkForAdjecentOpponent(piece, MyPiecePosition(x - 1, y)));
		{
			if (piece >= MyPiecePosition(x - 1, y)) //potential win
				return make_unique<MyMove>(x, y, x-1, y);
			//else flee(x, left) 
		}
		
		if (checkForAdjecentOpponent(piece, MyPiecePosition(x + 1, y)));
		{
			if (piece >= MyPiecePosition(x + 1, y)) //potential win
				return make_unique<MyMove>(x, y, x + 1, y);
			//else flee(x, right) 
		}
		if (checkForAdjecentOpponent(piece, MyPiecePosition(x, y - 1)));
		{
			if (piece >= MyPiecePosition(x, y - 1)) //potential win
				return make_unique<MyMove>(x, y, x, y - 1);
			//else flee(y, down) 
		}
		if (checkForAdjecentOpponent(piece, MyPiecePosition(x, y + 1)));
		{
			if (piece >= MyPiecePosition(x, y + 1)) //potential win
				return make_unique<MyMove>(x, y, x, y + 1);
			//else flee(y, up) 
		}
	}

	/*perform random move*/

	return unique_ptr<Move>();
}

unique_ptr<JokerChange> AutoPlayerAlgorithm::getJokerChange()
{
	return unique_ptr<JokerChange>();
}


#include "AutoPlayerAlgorithm.h"
#include <algorithm>
#include <iostream>
#include <list>

using namespace std;


AutoPlayerAlgorithm::AutoPlayerAlgorithm(UINT rows, UINT cols, UINT R, UINT P, UINT S, UINT B, UINT J, UINT F, int ID): 
	ID(ID), gen(seed())
{
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
	//update player's ID
	ID = player;

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	/** Parameters Initialization **/
	vector<char> rpsbVector;
	int initialMovingCnt, pieceIndex = 0;

	////////////////////////////////////////////////////////////////////////////////////////////////////////

	/* Random Generators Context */
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
	initialMovingCnt = (int) rpsbVector.size();

	////////////////////////////////////////////////////////////////////////////////////////////////////////

	/** if chosen by scenario, moving pieces will be set on the corners to counter attack flags on corners **/

	placeMovingPiecesOnCorners(rpsbVector, initialMovingCnt, rndCtx, vectorToFill, playerPieces, pieceIndex);

	////////////////////////////////////////////////////////////////////////////////////////////////////////

	/********* Position the rest of the pieces randomly on board ***********/

	//add the Jokers, don't place them on the corners (its a waste!)
	addAllToVector(J, 'J'); 

	positionRestOfMovingPiecesRandomly(pieceIndex, rndCtx, playerPieces, rpsbVector, vectorToFill);
	
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
			if (playerPieces.count(fightPos) > 0)	//could be that the piece is no longer there if it lost another fight
				playerPieces.erase(playerPieceIter);

		}

		else if (fightWinner == playerID) //player won, remove opponent's piece
		{
			//although won, remove player's piece if it's a bomb
			if (winningType == 'B')
			{
				if (playerPieces.count(fightPos) > 0)	//could be that the piece is no longer there if it lost another fight
					playerPieces.erase(playerPieceIter);
			}
		}

		else //TIE - remove both pieces
		{
			if (playerPieces.count(fightPos) > 0)		//could be that the piece is no longer there if it lost another fight
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
		playerPieces.erase(playerPieceIter);
	}

	else if (fightWinner == playerID) //player won, remove opponent's piece
	{
		//although won, remove player's piece if it's a bomb
		if (winningType == 'B')
		{
			playerPieces.erase(playerPieceIter);
		}
		//in any case, remove opponent's losing piece
		opponentsPieces.erase(opponentPieceIter);
	}

	else //TIE - remove both pieces
	{
		opponentsPieces.erase(opponentPieceIter);
		playerPieces.erase(playerPieceIter);
	}
}

unique_ptr<JokerChange> AutoPlayerAlgorithm::getJokerChange()
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
	return nullptr;
}

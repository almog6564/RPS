#ifndef _PLAYER_
#define _PLAYER_

#include "defs.h"
#include "parser.h"
#include <iostream>


class Player
{
	const UINT ID;
	UINT pieceCounters[PIECE_COUNT];
	UINT movingPiecesCtr; 
	UINT score;
	bool hasLost;
	UINT R, P, S, B, J, F;
	eReason reason;
	bool hasMoreMoves;
	PlayerFileContext* fileContext;
	

public:
	Player(UINT ID, UINT R, UINT P, UINT S, UINT B, UINT J, UINT F, PlayerFileContext* fileContext);
	bool isAlive();
	bool getNextMove(UINT * fromX, UINT * fromY, UINT * toX, UINT * toY, bool* isJoker,
		UINT * jokerX, UINT * jokerY, ePieceType* newRep);
	int getNextPiece(ePieceType* type, UINT * toX, UINT * toY, ePieceType* jokerType);
	void updateScore();
	bool getHasMoreMoves();
	void setHasMoreMoves(bool val);

	void decTypeCounter(ePieceType type, ePieceType originalType = UNDEF, bool updateOnlyMovingCounter = false);
	int incTypeCount(ePieceType type, ePieceType originalType, bool updateOnlyMovingCounter  = false );

	PlayerFileContext* getPlayerFileContext();
	void validatePlayerPositions(bool** tmpBoard, UINT rows, UINT cols);

	UINT getPlayerId()const
	{
		return ID;
	}

	bool getHasLost()const
	{
		return hasLost;
	}

	void setHasLost()
	{
		hasLost = true;
	}

	eReason getReason()const
	{
		return reason;
	}

	void setReason(eReason reasonArg)
	{
		reason = reasonArg;
	}

	UINT getTypeCount(ePieceType type)
	{
		return pieceCounters[type];
	}

	UINT getTypeMax(ePieceType type)
	{
		switch (type)
		{
		case ROCK:
			return R;
		case SCISSORS:
			return S;
		case PAPER:
			return P;
		case BOMB:
			return B;
		case FLAG:
			return F;
		case JOKER:
			return J;
		default:
			return UNDEF;
		}
	}







};







#endif //_PLAYER_

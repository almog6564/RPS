#ifndef _PLAYER_
#define _PLAYER_

#include "defs.h"
#include "parser.h"
#include <iostream>


class Player
{
	UINT pieceCounters[PIECE_COUNT];
	UINT movingPiecesCtr; 
	UINT score;
	bool hasLost;
	UINT R, P, S, B, J, F;
	eReason reason;
	bool hasMoreMoves;
	PlayerFileContext* fileContext;
	

public:
	Player(UINT R, UINT P, UINT S, UINT B, UINT J, UINT F, PlayerFileContext* fileContext);
	void decCounter(ePieceType type);
	bool isAlive();
	bool getNextMove(UINT* fromX, UINT* fromY, UINT* toX, UINT* toY);
	bool getNextPosition(ePieceType* type, UINT* toX, UINT* toY);
	void updateScore();
	bool getHasMoreMoves();
	void setHasMoreMoves(bool val);

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

	int updateTypeCount(ePieceType type);



};







#endif //_PLAYER_

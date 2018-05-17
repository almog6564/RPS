#ifndef _PLAYER_
#define _PLAYER_

#include "defs.h"
#include "parser.h"
#include <iostream>


class PlayerContext
{
	const UINT ID;
	UINT pieceCounters[PIECE_COUNT];
	UINT movingPiecesCtr; 
	UINT originalFlagsCnt;
	UINT score;
	bool hasLost;
	UINT R, P, S, B, J, F;
	eReason reason;
	bool hasMoreMoves;

public:
	PlayerContext(UINT ID, UINT R, UINT P, UINT S, UINT B, UINT J, UINT F);

	/*
	This function checks if the Player is still "Alive", i.e. still have at least one flag and one moving piece left. 
		@returns -
			true - Player is Alive (has not lost yet)
			false - Player has lost (no flags or no moving pieces)
	*/
	bool isAlive();

	/*
	This function decreases the counter of a specific Piece type. 
	If the type of the Piece is Joker, than originalType will be set to Joker, and the correct type counter will be
	decreased (e.g. to avoid that the ROCK counter will be decreased if a Joker that is currently a ROCK was removed.
	In any case, the movingPieces global counter will be updated according to the type or current type if it's a Joker
	*/
	void decTypeCounter(ePieceType type, ePieceType originalType = UNDEF, bool updateOnlyMovingCounter = false);

	/*
	Same as decTypeCounter, just for increase in case of new positioning.
	*/
	int incTypeCount(ePieceType type, ePieceType originalType, bool updateOnlyMovingCounter = false);

	bool getHasMoreMoves();

	void setHasMoreMoves(bool val);

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

	/* This is to know whether there were flags before but they were eaten*/
	UINT getOriginalFlagCount(void)
	{
		return originalFlagsCnt;
	}

	void incOriginalFlagCount(void)
	{
		originalFlagsCnt++;
	}

};

#endif //_PLAYER_

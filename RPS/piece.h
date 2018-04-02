
#ifndef _PIECE_
#define _PIECE_

#include "defs.h"
#include "player.h"


class Piece
{
	const ePieceType type;
	ePieceType winAgainst;
	Player* owner;

	
public:

	Piece(ePieceType typeArg, ePieceType winAgainstArg, Player* owner);

	ePieceType getType()const 
	{ 
		return type; 
	}

	ePieceType getWinAgainst()const
	{
		return winAgainst;
	}

	Player* getOwner()const
	{
		return owner;
	}

	eScore match(Piece* p);

	~Piece();

};


Piece* createNewPiece(Player* owner, ePieceType type, ePieceType jokerType = UNDEF);




#endif //_PIECE_


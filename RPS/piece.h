
#ifndef _PIECE_
#define _PIECE_

#include "defs.h"
#include "player.h"

class Piece
{
	const ePieceType type;
	ePieceType winAgainst;
	Player& owner;
	
public:

	Piece(ePieceType typeArg, ePieceType winAgainstArg, Player& playerOwner);

	~Piece();

	ePieceType getType()const 
	{ 
		return type; 
	}

	ePieceType getWinAgainst()const
	{
		return winAgainst;
	}

	eScore match(Piece* p);

};







#endif //_PIECE_


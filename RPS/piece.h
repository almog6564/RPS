
#ifndef _PIECE_
#define _PIECE_

#include "defs.h"


class Piece
{
	const ePieceType type;
	ePieceType winAgainst;
	
public:

	Piece(ePieceType typeArg, ePieceType winAgainstArg);

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


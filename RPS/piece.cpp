
#include "piece.h"
#include <iostream>

using namespace std;

Piece::Piece(ePieceType typeArg, ePieceType winAgainstArg) :
	type(typeArg), winAgainst(winAgainstArg) {};

eScore Piece::match(Piece* p)
{
	eScore s = ERROR;
	ePieceType p2type = p->getType();

	if (type == p2type)
		s = TIE;
	else if (type == BOMB || p2type == winAgainst || p2type == FLAG)
		s = WIN;
	else if (p2type == BOMB || type == p->getWinAgainst() || type == FLAG)
		s = LOSE;

	return s;
}

class Rock : Piece 
{
	Rock() : Piece(ROCK,SCISSORS) {};
};

class Scissors : Piece
{
	Scissors() : Piece(SCISSORS, PAPER) {};
};

class Paper : Piece
{
	Paper() : Piece(PAPER, ROCK) {};
};

class Flag : Piece
{
	Flag() : Piece(ROCK, UNDEF) {};
};

class Bomb : Piece
{
	Bomb() : Piece(ROCK, UNDEF) {};
};

class Joker : Piece
{
	ePieceType currentType;

public:
	Joker(ePieceType currentTypeArg) : Piece(JOKER, UNDEF) 
	{
		currentType = currentTypeArg;
	};

	//override original function, so match function wouldn't have to know if its Joker
	ePieceType getType()const
	{
		return currentType;
	}

	ePieceType getCurrentType()const
	{
		return currentType;
	}

	int setCurrentType(ePieceType newType)
	{
		if (newType == FLAG || newType == UNDEF || newType == JOKER)
		{
			cout << "setCurrentType: invalid newType " << newType << endl;
			return -1;
		}
		
		currentType = newType;

		return 0;
	}
};


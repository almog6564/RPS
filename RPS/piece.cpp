
#include "piece.h"
#include <iostream>

using namespace std;

Piece::Piece(ePieceType typeArg, ePieceType winAgainstArg, Player* owner) :
	type(typeArg), winAgainst(winAgainstArg), owner(owner) {};

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

Piece::~Piece()
{
	owner->decCounter(type);
}

class Rock : Piece 
{
	Rock(Player* owner) : Piece(ROCK, SCISSORS, owner) {};
};

class Scissors : Piece
{
	Scissors(Player* owner) : Piece(SCISSORS, PAPER, owner) {};
};

class Paper : Piece
{
	Paper(Player* owner) : Piece(PAPER, ROCK, owner) {};
};

class Flag : Piece
{
	Flag(Player* owner) : Piece(ROCK, UNDEF, owner) {};
};

class Bomb : Piece
{
	Bomb(Player* owner) : Piece(ROCK, UNDEF, owner) {};
};

class Joker : Piece
{
	ePieceType currentType;

public:
	Joker(ePieceType currentTypeArg, Player* owner) : Piece(JOKER, UNDEF, owner)
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


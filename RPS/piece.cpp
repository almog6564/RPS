
#include "piece.h"
#include <iostream>

using namespace std;


Piece::Piece(ePieceType typeArg, ePieceType winAgainstArg, Player* playerOwner) :
	type(typeArg), winAgainst(winAgainstArg), owner(playerOwner) {};

Piece::~Piece()
{
	owner->decCounter(type);
}



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

bool Piece::isJoker()
{
	return false;
}

Joker::Joker(ePieceType currentTypeArg, Player * owner): Piece(JOKER, UNDEF, owner)
{
	currentType = currentTypeArg;	
}

int Joker::setCurrentType(ePieceType newType)
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

void createNewPiece(Piece** pPiece, Player* owner, ePieceType type, ePieceType jokerType)
{
	/*
	switch (type)
	{
	case ROCK:
		*pPiece = new Rock(owner);
	default:
		break;
	}

	return p;
	*/
}



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
	else if (p2type == winAgainst || p2type == FLAG)
		s = WIN;
	else if (type == p->getWinAgainst() || type == FLAG)
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
};

Piece* createNewPiece(Player* owner, ePieceType type, ePieceType jokerType)
{
	Piece* p = NULL;

	switch (type)
	{
	case ROCK:
		p = new Rock(owner);
		break;
	case PAPER:
		p = new Paper(owner);
		break;
	case SCISSORS:
		p = new Scissors(owner);
		break;
	case BOMB:
		p = new Bomb(owner);
		break;
	case FLAG:
		p = new Flag(owner);
		break;
	case JOKER:
		p = new Joker(jokerType,owner);
		break;
	default:
		break;
	}

	return p;
}


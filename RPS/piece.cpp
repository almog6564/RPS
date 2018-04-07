
#include "piece.h"
#include <iostream>

using namespace std;


Piece::Piece(ePieceType typeArg, ePieceType winAgainstArg, Player* playerOwner) :
	type(typeArg), winAgainst(winAgainstArg), owner(playerOwner) {}

Piece::~Piece()
{
	owner->decTypeCounter(type, type);
}



eScore Piece::match(Piece* p)
{
	eScore s = ERROR;
	ePieceType p1type = this->getType();
	ePieceType p1WinAgainst = this->getWinAgainst();

	ePieceType p2type = p->getType();

	if (p1type == p2type)
		s = TIE;
	else if (p2type == p1WinAgainst || p2type == FLAG)
		s = WIN;
	else if (p1type == p->getWinAgainst() || p1type == FLAG)
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

Joker::~Joker()
{
	owner->decTypeCounter(currentType, JOKER);
}

int Joker::setCurrentType(ePieceType newType)
{
	if (newType == FLAG || newType == UNDEF || newType == JOKER)
	{
		cout << "setCurrentType: invalid newType " << newType << endl;
		return -1;
	}

	/* We are now changing type from moving piece to non-moving piece.
	this is to prevent race condition on which joker type changes after a player "loses",
	but the joker changes type to moving type and saves the player from dying */
	if (currentType != BOMB && newType == BOMB)
	{
		owner->decTypeCounter(currentType, JOKER, true);
	} 
	else if (currentType == BOMB && newType != BOMB)
	{
		owner->incTypeCount(newType, JOKER, true);
	}

	currentType = newType;


	return 0;
}

ePieceType Joker::getWinAgainst() const
{
	ePieceType ret = UNDEF;
	switch (currentType)
	{
	case ROCK:
		ret = SCISSORS;
		break;
	case SCISSORS:
		ret = PAPER;
		break;
	case PAPER:
		ret = ROCK;
		break;
	//BOMB case is being checked by calling function
	default:
		break;
	}
	return ret;
}

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



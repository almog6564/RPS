
#include "BoardPiece.h"
#include <iostream>

using namespace std;


BoardPiece::BoardPiece(ePieceType typeArg, ePieceType winAgainstArg, PlayerContext* playerOwner) :
	type(typeArg), winAgainst(winAgainstArg), owner(playerOwner) 
{
	position = new MyPoint(0, 0);
}

void BoardPiece::setPiecePosition(int col, int row)
{
	position->setNewPosition(col, row);
}


char BoardPiece::getPiece() const
{
	return pieceToChar(getOriginalType());
}

BoardPiece::~BoardPiece()
{
	delete position;
}



eScore BoardPiece::match(BoardPiece* p)
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

 bool BoardPiece::isJoker()
{
	return false;
}

Joker::Joker(ePieceType currentTypeArg, PlayerContext* owner): BoardPiece(JOKER, UNDEF, owner)
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

BoardPiece* createNewPiece(PlayerContext* owner, ePieceType type, ePieceType jokerType)
{
	BoardPiece* p = nullptr;

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



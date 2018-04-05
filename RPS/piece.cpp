
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

ePieceType charToPiece(char c)
{
	ePieceType p;

	switch (c)
	{
	case 'R':
		p = ROCK;
		break;

	case 'P':
		p = PAPER;
		break;

	case 'S':
		p = SCISSORS;
		break;

	case 'B':
		p = BOMB;
		break;

	case 'J':
		p = JOKER;
		break;

	case 'F':
		p = FLAG;
		break;

	default:
		p = UNDEF;
		break;
	}

	return p;
}

char pieceToChar(ePieceType p, bool isUpperCase)
{
	char c;

	switch (p)
	{
	case ROCK:
		c = isUpperCase ? 'R' : 'r';
		break;

	case PAPER:
		c = isUpperCase ? 'P' : 'p';
		break;

	case SCISSORS:
		c = isUpperCase ? 'S' : 's';
		break;

	case BOMB:
		c = isUpperCase ? 'B' : 'b';
		break;

	case JOKER:
		c = isUpperCase ? 'J' : 'j';
		break;

	case 'F':
		c = isUpperCase ? 'F' : 'f';
		break;

	default:
		c = -1;
		break;
	}

	return c;
}


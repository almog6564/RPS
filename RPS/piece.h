
#ifndef _PIECE_
#define _PIECE_

#include "defs.h"
#include "player.h"


class Piece
{
	const ePieceType type;
	ePieceType winAgainst;

	
public:

	Player * owner;

	Piece(ePieceType typeArg, ePieceType winAgainstArg, Player* owner);

	ePieceType getOriginalType()const
	{
		return type;
	}

	virtual ePieceType getType()const 
	{ 
		return type; 
	}

	virtual ePieceType getWinAgainst()const
	{
		return winAgainst;
	}

	Player* getOwner()const
	{
		return owner;
	}

	eScore match(Piece* p);

	virtual bool isJoker();

	virtual ~Piece();

};

class Rock : public Piece
{
public:
	Rock(Player* owner) : Piece(ROCK, SCISSORS, owner) {};
};

class Scissors : public Piece
{
public:
	Scissors(Player* owner) : Piece(SCISSORS, PAPER, owner) {};
};

class Paper : public Piece
{
public:
	Paper(Player* owner) : Piece(PAPER, ROCK, owner) {};
};

class Flag : public Piece
{
public:
	Flag(Player* owner) : Piece(FLAG, UNDEF, owner) {};
};

class Bomb : public Piece
{
public:
	Bomb(Player* owner) : Piece(BOMB, UNDEF, owner) {};
};

class Joker : public Piece
{
	ePieceType currentType;

public:
	Joker(ePieceType currentTypeArg, Player* owner);

	~Joker() override;

	//override original function, so match function wouldn't have to know if its Joker
	ePieceType getType()const override
	{
		return currentType;
	}

	int setCurrentType(ePieceType newType);

	ePieceType getWinAgainst()const override;

	bool isJoker() override
	{
		return true;
	}
};


Piece* createNewPiece(Player* owner, ePieceType type, ePieceType jokerType = UNDEF);

#endif //_PIECE_


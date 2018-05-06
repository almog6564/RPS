
#ifndef _PIECE_
#define _PIECE_

#include "defs.h"
#include "MyPiecePosition.h"
#include "PlayerContext.h"


class Piece : public PiecePosition
{
	const ePieceType type;
	ePieceType winAgainst;

	
public:
	MyPoint * position = nullptr;

	PlayerContext* owner;

	Piece(ePieceType typeArg, ePieceType winAgainstArg, PlayerContext* owner);

	~Piece();

	const Point& getPosition() const;

	//does not check boundaries
	void setPiecePosition(int col, int row);
	char getPiece() const; // R, P, S, B, J or F


	char getJokerRep() const override // ONLY for Joker: R, P, S or B -- non-Joker may return ‘#’
	{
		return '#';
	}

	/*
	This function does a match between the piece and a given second piece according to the rules of Rock, Paper, Scissors.
		@returns -
			WIN - the Piece won p.
			LOSE - the piece lost to p.
			TIE - a tie result.
			ERROR - some invalid values.
	*/
	eScore match(Piece* p);


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

	PlayerContext* getOwner()const
	{
		return owner;
	}


	virtual bool isJoker();

};

class Rock : public Piece
{
public:
	Rock(PlayerContext* owner) : Piece(ROCK, SCISSORS, owner) {};
};

class Scissors : public Piece
{
public:
	Scissors(PlayerContext* owner) : Piece(SCISSORS, PAPER, owner) {};
};

class Paper : public Piece
{
public:
	Paper(PlayerContext* owner) : Piece(PAPER, ROCK, owner) {};
};

class Flag : public Piece
{
public:
	Flag(PlayerContext* owner) : Piece(FLAG, UNDEF, owner) {};
};

class Bomb : public Piece
{
public:
	Bomb(PlayerContext* owner) : Piece(BOMB, UNDEF, owner) {};
};

class Joker : public Piece
{
	ePieceType currentType;

public:
	Joker(ePieceType currentTypeArg, PlayerContext* owner);

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

	char getJokerRep() const override // ONLY for Joker: R, P, S or B -- non-Joker may return ‘#’
	{
		return getType();
	}
};


Piece* createNewPiece(PlayerContext* owner, ePieceType type, ePieceType jokerType = UNDEF);

#endif //_PIECE_



#ifndef _PIECE_
#define _PIECE_

#include "defs.h"
#include "MyPiecePosition.h"
#include "PlayerContext.h"


class BoardPiece
{
	const ePieceType type;
	ePieceType winAgainst;

public:
	MyPoint * position = nullptr;

	PlayerContext* owner;

	BoardPiece(ePieceType typeArg, ePieceType winAgainstArg, PlayerContext* owner);

	virtual ~BoardPiece();

	//does not check boundaries
	void setPiecePosition(int col, int row);
	char getPiece() const; // R, P, S, B, J or F

	/*
	This function does a match between the piece and a given second piece according to the rules of Rock, Paper, Scissors.
		@returns -
			WIN - the BoardPiece won p.
			LOSE - the piece lost to p.
			TIE - a tie result.
			ERROR - some invalid values.
	*/
	eScore match(BoardPiece* p);


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

class Rock : public BoardPiece
{
public:
	Rock(PlayerContext* owner) : BoardPiece(ROCK, SCISSORS, owner) {};
};

class Scissors : public BoardPiece
{
public:
	Scissors(PlayerContext* owner) : BoardPiece(SCISSORS, PAPER, owner) {};
};

class Paper : public BoardPiece
{
public:
	Paper(PlayerContext* owner) : BoardPiece(PAPER, ROCK, owner) {};
};

class Flag : public BoardPiece
{
public:
	Flag(PlayerContext* owner) : BoardPiece(FLAG, UNDEF, owner) {};
};

class Bomb : public BoardPiece
{
public:
	Bomb(PlayerContext* owner) : BoardPiece(BOMB, UNDEF, owner) {};
};

class Joker : public BoardPiece
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
};


BoardPiece* createNewPiece(PlayerContext* owner, ePieceType type, ePieceType jokerType = UNDEF);

#endif //_PIECE_


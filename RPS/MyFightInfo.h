#ifndef __MY_FIGHT_INFO_H_
#define __MY_FIGHT_INFO_H_

#include "FightInfo.h"
#include "defs.h"
#include "MyPoint.h"

using namespace std;

class MyFightInfo : public FightInfo
{
	unique_ptr<Point> position;
	char piece;
	int winner;

public:
	MyFightInfo(UINT x, UINT y, char piece, int winner) : 
		position(make_unique<MyPoint>(x, y)), piece(piece), winner(winner) {}

	MyFightInfo() {}

	MyFightInfo& operator=(MyFightInfo& other)
	{
		// check for self-assignment
		if (&other == this)
			return *this;
		this->piece = other.piece;
		this->position = move(other.position);
		this->winner = other.winner;
		return *this;
	}

	const Point& getPosition() const
	{
		return *(move(position));
	}

	char getPiece(int player) const // R, P, S, B or F (but NOT J)
	{
		return piece;
	}

	int getWinner() const // 0 - both lost / tie, 1 - player 1 won, 2 - player 2 won
	{
		return winner;
	}

	void setPosition() 
	{

	}
};

#endif 


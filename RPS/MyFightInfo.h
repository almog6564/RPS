#ifndef __MY_FIGHT_INFO_H_
#define __MY_FIGHT_INFO_H_

#include "FightInfo.h"
#include "defs.h"
#include "MyPoint.h"

using namespace std;

class MyFightInfo : public FightInfo
{
	unique_ptr<Point> position;
	char p1piece, p2piece;
	int winner;

public:

	MyFightInfo(UINT x, UINT y, char p1piece, char p2piece, int winner) :
		position(make_unique<MyPoint>(x, y)), p1piece(p1piece), p2piece(p2piece), winner(winner) {}

	MyFightInfo()
	{
		position.reset(nullptr);
	}

	operator bool() const { return position != nullptr; }

	MyFightInfo(MyFightInfo& other)
	{
		*this = other;
	}

	const Point& getPosition() const
	{
		return *(move(position));
	}

	char getPiece(int player) const // R, P, S, B or F (but NOT J)
	{
		if (player == 1)
			return p1piece;
		else if (player == 2)
			return p2piece;
		else
			return '#';
	}


	MyFightInfo& operator=(MyFightInfo& other)
	{
		// check for self-assignment
		if (&other == this)
			return *this;
		p1piece = other.p1piece;
		p2piece = other.p2piece;

		position = move(other.position);
		winner = other.winner;
		return *this;
	}

	int getWinner() const // 0 - both lost / tie, 1 - player 1 won, 2 - player 2 won
	{
		return winner;
	}
};

#endif 


#ifndef __MY_PIECE_POSITION_H_
#define __MY_PIECE_POSITION_H_

#include "PiecePosition.h"
#include "defs.h"
#include "MyPoint.h"


using namespace std;

class MyPiecePosition : public PiecePosition
{
	unique_ptr<Point> position;
	char pieceType;
	char jokerRep;

public:
	MyPiecePosition(UINT x, UINT y, char pieceType, char jokerRep = '#');

	const Point& getPosition() const
	{
		return *(position);
	}
	char getPiece() const // R, P, S, B, J or F
	{
		return pieceType;
	}
	char getJokerRep() const // ONLY for Joker: R, P, S or B -- non-Joker may return ‘#’
	{
		return jokerRep;
	}
};

struct PiecePositionHasher
{
	size_t operator()(const PiecePosition & obj) const
	{
		return (size_t)(obj.getPosition().getX() << 16 || obj.getPosition().getY());
	}
};

struct PiecePositionComparator
{
	bool operator()(const PiecePosition & obj1, const PiecePosition & obj2) const
	{
		if (obj1.getPosition().getX() == obj2.getPosition().getX() &&
			obj1.getPosition().getY() == obj2.getPosition().getY())
			return true;
		else
			return false;
	}
};




#endif



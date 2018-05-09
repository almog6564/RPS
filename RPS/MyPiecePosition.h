#ifndef __MY_PIECE_POSITION_H_
#define __MY_PIECE_POSITION_H_

#include "PiecePosition.h"
#include "defs.h"
#include "MyPoint.h"


using namespace std;

class MyPiecePosition : public PiecePosition
{
	mutable char pieceType;
	unique_ptr<Point> position;
	char jokerRep;
	mutable bool movingPiece; //initialized to false


public:

	MyPiecePosition(UINT x, UINT y, char pieceType = '?', char jokerRep = '#', bool movingPiece = false);

	MyPiecePosition(const MyPiecePosition& other) //copy c'tor
	{
		pieceType = other.pieceType;
		jokerRep = other.jokerRep;
		movingPiece = other.movingPiece;
		position = make_unique<MyPoint>(other.getPosition().getX(), other.getPosition().getY());
	}

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
	void setPieceType(char newPieceType) const
	{
		pieceType = newPieceType;
	}

	bool isMoving() const
	{
		return movingPiece;
	}

	void setMovingPiece(bool b) const
	{
		movingPiece = b;
	}

	MyPiecePosition(MyPiecePosition&&) = default; //default move constructor

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



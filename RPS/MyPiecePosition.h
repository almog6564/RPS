#ifndef __MY_PIECE_POSITION_H_
#define __MY_PIECE_POSITION_H_

#include "PiecePosition.h"
#include "defs.h"
#include "MyPoint.h"


using namespace std;

class MyPiecePosition : public PiecePosition
{
	mutable char pieceType;
	shared_ptr<Point> position;
	char jokerRep;
	mutable bool movingPiece; //initialized to false


public:

	MyPiecePosition(UINT x, UINT y, char pieceType = '?', char jokerRep = '#', bool movingPiece = false);

	MyPiecePosition(const MyPiecePosition& other) = default;

	MyPiecePosition(MyPiecePosition& other) = default;


	MyPiecePosition& operator=(MyPiecePosition& other) = default;

	MyPiecePosition& operator=(const MyPiecePosition& other) = default;

	MyPiecePosition(MyPiecePosition&&) = default; //default move constructor


	bool operator==(MyPiecePosition& other)
	{
		if ((*position).getX() == (*other.position).getX() && (*position).getY() == (*other.position).getY())
			return true;
		return false;
	}

	bool operator==(const MyPiecePosition& other)
	{
		if ((*position).getX() == (*other.position).getX() && (*position).getY() == (*other.position).getY())
			return true;
		return false;
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

	bool operator>=(const MyPiecePosition& other) const
	{
		char otherPieceType = other.getPiece();
		if (((pieceType == 'R' || jokerRep == 'R') && (otherPieceType == 'S' || otherPieceType == '?')) ||
			((pieceType == 'S' || jokerRep == 'S') && (otherPieceType == 'P' || otherPieceType == '?')) ||
			((pieceType == 'P' || jokerRep == 'P') && (otherPieceType == 'R' || otherPieceType == '?')))
			return false;
		return true;
	}


};

struct PiecePositionHasher
{
	size_t operator()(const PiecePosition & obj) const
	{
		return (size_t)(obj.getPosition().getX() << 16 | obj.getPosition().getY());
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



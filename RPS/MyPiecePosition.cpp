#include "MyPiecePosition.h"


MyPiecePosition::MyPiecePosition(UINT x, UINT y, char pieceType, char jokerRep) : pieceType(pieceType), jokerRep(jokerRep)
{
	position = make_unique<MyPoint>(x, y);
}

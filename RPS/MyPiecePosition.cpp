#include "MyPiecePosition.h"


MyPiecePosition::MyPiecePosition(UINT x, UINT y, char pieceType /*='?'*/, char jokerRep/*='#'*/, bool movingPiece /*= false*/):
	pieceType(pieceType), jokerRep(jokerRep), movingPiece(movingPiece)
{
	position = make_unique<MyPoint>(x, y);
}

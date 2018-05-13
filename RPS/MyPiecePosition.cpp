#include "MyPiecePosition.h"


MyPiecePosition::MyPiecePosition(UINT x, UINT y, char pieceType /*='?'*/, char jokerRep/*='#'*/) :
	pieceType(pieceType), jokerRep(jokerRep)
{
	if ((pieceType == 'R' || pieceType == 'S' || pieceType == 'P') ||
		(pieceType == 'J' && (jokerRep == 'R' || jokerRep == 'S' || jokerRep == 'P')))
		movingPiece = true;
	else
		movingPiece = false;
	position = make_shared<MyPoint>(x, y);
}

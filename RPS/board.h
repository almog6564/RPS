
#ifndef _BOARD_
#define _BOARD_

#include "piece.h"


using namespace std;

class Board
{
	const UINT rows;
	const UINT cols;
	Piece*** table;	//2D array of pointers to pieces

	//Assumes legal arguments, called after dimensions check
	bool isCellEmpty(UINT col, UINT row)
	{
		return table[row][col] ? true : false;
	}

public:
	Board(UINT N, UINT M);

	~Board();

	Piece* getPieceAt(UINT col, UINT row);
	void setPieceAt(Piece* p, UINT col, UINT row);
	void removePiece(UINT col, UINT row);

	int positionPiece(Piece* p, UINT toX, UINT toY, int moved = 0, UINT fromX = 0, UINT fromY = 0);

	int movePiece(UINT playerID, UINT fromX, UINT fromY, UINT toX, UINT toY);

	int changeJokerType(UINT fromX, UINT fromY, ePieceType newType);

	void getBoardDimensions(UINT* pCols, UINT* pRows);
};






#endif //_BOARD_


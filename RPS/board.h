
#ifndef _BOARD_
#define _BOARD_

#include "piece.h"
#include "cstdio"
#include "cstdlib"
#include <iostream>

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

	void removePiece(UINT col, UINT row);

	eScore movePieceAndMatch(UINT fromX, UINT fromY, UINT toX, UINT toY);
};






#endif //_BOARD_


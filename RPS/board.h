
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
	Board(UINT N, UINT M) : rows(N), cols(M)
	{
		table = (Piece***) malloc(N * sizeof(Piece**));

		if (!table)
		{
			throw "Board constructor: memory error 1";
		}

		for (UINT i = 0; i < N; i++)
		{
			table[i] = (Piece**) calloc(M, sizeof(Piece*));
			
			if (!table[i])
			{
				throw "Board constructor: memory error 1";
			}
		}
	}

	~Board()
	{
		for (UINT i = 0; i < rows; i++)
		{
			free(table[i]);
		}

		free(table);

	}

	/*****************************/

	Piece* getPieceAt(UINT col, UINT row)
	{
		if (col >= cols || row >= rows)
			return NULL;

		return table[row][col];
	}

	eScore movePieceAndMatch(UINT fromX, UINT fromY, UINT toX, UINT toY)
	{
		Piece* p1;
		Piece* p2;
		eScore score;

		if (fromX >= cols || fromY >= rows || toX >= cols || toY >= rows)
		{
			cout << "movePieceAndMatch: Illegal dimensions" << endl;
			return ERROR;
		}

		p1 = getPieceAt(fromX, fromY);

		if (!p1)
		{
			printf("movePieceAndMatch: no piece at fromX <%d> fromY <%d>", fromX, fromY);
			return ERROR;
		}

		p2 = getPieceAt(toX, toY);

		if (!p2)
		{
			table[toY][toX] = p1;
		}
		else //piece found at cell, MATCH!
		{
			score = p1->match(p2);


		}


	}
};






#endif //_BOARD_


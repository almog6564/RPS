#include "board.h"
#include "piece.h"
#include <iostream>

using namespace std;

Board::Board(UINT N, UINT M) : rows(N), cols(M)
{
	table = (Piece***)malloc(N * sizeof(Piece**));

	if (!table)
	{
		throw "Board constructor: memory error 1";
	}

	for (UINT i = 0; i < N; i++)
	{
		table[i] = (Piece**)calloc(M, sizeof(Piece*));

		if (!table[i])
		{
			throw "Board constructor: memory error 1";
		}
	}
}

Piece* Board::getPieceAt(UINT col, UINT row)
{
	if (col >= cols || row >= rows)
		return NULL;

	return table[row][col];
}

void Board::removePiece(UINT col, UINT row)
{
	delete table[row][col];
	table[row][col] = NULL;
}

eScore Board::movePieceAndMatch(UINT fromX, UINT fromY, UINT toX, UINT toY)
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
		table[fromY][fromX] = NULL;

	}
	else //piece found at cell, MATCH!
	{
		score = p1->match(p2);
		switch (score)
		{
		case WIN:
			removePiece(toX, toY);
			table[toY][toX] = p1;
			table[fromY][fromX] = NULL;
			break;

		case LOSE:
			removePiece(fromX, fromY);
			break;

		case TIE:
			removePiece(fromX, fromY);
			removePiece(toX, toY);
			break;

		default: //ERROR
			cout << "invalid match result" << endl;
			return ERROR;

		}

	}
}

Board::~Board()
{
	for (UINT i = 0; i < rows; i++)
	{
		free(table[i]);
	}
	free(table);
}







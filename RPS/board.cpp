#include "board.h"
#include <iostream>
#include "cstdio"
#include "cstdlib"

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
	if (col > cols || row > rows)
		return NULL;

	return table[row-1][col-1];
}

void Board::setPieceAt(Piece* p, UINT col, UINT row)
{
	if (col > cols || row > rows)
		return;

	table[row-1][col-1] = p;
}

void Board::removePiece(UINT col, UINT row)
{
	delete table[row-1][col-1];
	table[row-1][col-1] = NULL;
}

/**
 * This function assumes legal dimensions.
 */
int Board::positionPiece(Piece* p, UINT toX, UINT toY, int moved, UINT fromX, UINT fromY)
{
	Piece* p2;
	eScore score;
	Player*  player;
	ePieceType type;

	type = p->getType();
	player = p->getOwner();

	p2 = getPieceAt(toX, toY);
	if (!p2) //Piece was not found
	{
		setPieceAt(p, toX, toY);
		if (!moved) //first positioning
			return (player->updateTypeCount(type) == -1);
		return 0; 
	}
	else //piece found at cell
	{
		if (player == p2->getOwner())
		{
			//both pieces belong to same player
			player->setHasLost();
			player->setReason(BAD_MOVES_INPUT_FILE);
			return -1; 
		}

		if (type == BOMB || p2->getType() == BOMB)
		{
			removePiece(toX, toY);
			if (moved)
				removePiece(fromX, fromY);
			return 0;
		}

		//pieces belong to different players, MATCH!
		score = p->match(p2);
		switch (score)
		{
		case WIN:
			removePiece(toX, toY);
			table[toY][toX] = p;
			player->updateTypeCount(type);
			break;

		case LOSE:
			if (moved)
				removePiece(fromX, fromY);
			break;

		case TIE:
			if (moved)
				removePiece(fromX, fromY);
			removePiece(toX, toY);
			break;

		default: //ERROR
			cout << "invalid match result" << endl;
			return -1;
		}

	}
	return 0;
}


int Board::movePiece(UINT fromX, UINT fromY, UINT toX, UINT toY)
{
	Piece* p1;
	int ret;

	if (toX > cols || toY > rows)
	{
		printf("[Board::movePiece] TO-dimensions check failed:\n"
			"\t toX=%d, toY=%d WHILE: cols=%d and rows=%d\n",
			toX, toY, cols, rows);

		return ERROR;
	}

	if (fromX > cols || fromY > rows)
	{
		printf("[Board::movePiece] FROM-dimensions check failed.\n"
			"\t fromX=%d, fromY=%d WHILE: cols=%d and rows=%d\n",
			fromX, fromY, cols, rows);

		return ERROR;
	}

	p1 = getPieceAt(fromX, fromY);
	if (!p1) //Piece was not found
	{
		printf("[Board::movePiece] no piece at fromX <%d> fromY <%d>", fromX, fromY);
		return -1;
	}

	ret = positionPiece(p1, toX, toY, 1, fromX, fromY);
	if (!ret)
	{
		return -1;
	}
	return 0;
}

int Board::changeJokerType(UINT fromX, UINT fromY, ePieceType newType)
{
	Piece* p1;

	if (fromX >= cols || fromY >= rows)
	{
		cout << "changeJokerType: Illegal dimensions" << endl;
		return ERROR;
	}

	p1 = getPieceAt(fromX, fromY);
	if (!p1) //Piece was not found
	{
		printf("changeJokerType: no piece at fromX <%d> fromY <%d>", fromX, fromY);
		return -1;
	}

	if (!p1->isJoker())
	{
		printf("changeJokerType: piece at fromX <%d> fromY <%d> is not a Joker", fromX, fromY);
		return -1;
	}

	((Joker*)p1)->setCurrentType(newType);
	return 0;
}

void Board::getBoardDimensions(UINT* pCols, UINT* pRows)
{
	if (pCols)
		*pCols = cols;

	if (pRows)
		*pRows = rows;
}

Board::~Board()
{
	for (UINT i = 0; i < rows; i++)
	{
		free(table[i]);
	}
	free(table);
}







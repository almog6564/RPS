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
	if (col >= cols || row >= rows)
		return NULL;

	return table[row][col];
}

void Board::removePiece(UINT col, UINT row)
{
	delete table[row][col];
	table[row][col] = NULL;
}

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
		table[toY][toX] = p;
		if (!moved) //first positioning
		{
			//player->setPosition();
			player->setTypeCount(type); //increase type count for positioning new piece
			if (player->getTypeCount(type) > player->getTypeMax(type))
			{
				player->setHasLost();
				player->setReason(BAD_POSITIONING_INPUT_FILE);
				return -1;
			}
		}
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

		//pieces belong to different players, MATCH!
		score = p->match(p2);
		switch (score)
		{
		case WIN:
			removePiece(toX, toY);
			//player->setPosition();
			table[toY][toX] = p;
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

	if (fromX >= cols || fromY >= rows || toX >= cols || toY >= rows)
	{
		cout << "movePieceAndMatch: Illegal dimensions" << endl;
		return ERROR;
	}

	p1 = getPieceAt(fromX, fromY);
	if (!p1) //Piece was not found
	{
		printf("movePieceAndMatch: no piece at fromX <%d> fromY <%d>", fromX, fromY);
		return -1;
	}

	ret = positionPiece(p1, toX, toY, 1, fromX, fromY);
	if (!ret)
	{
		return -1;
	}
	return 0;
}

Board::~Board()
{
	for (UINT i = 0; i < rows; i++)
	{
		free(table[i]);
	}
	free(table);
}







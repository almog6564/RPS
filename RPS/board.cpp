#include "board.h"
#include <iostream>
#include "cstdio"

using namespace std;

Board::Board(UINT N, UINT M) : rows(N), cols(M)
{
	table = new Piece**[N];

	for (UINT i = 0; i < N; i++)
	{
		table[i] = new Piece*[M]();
	}
}

Piece* Board::getPieceAt(UINT col, UINT row)
{
	if (col > cols || row > rows || row < 1 || col < 1)
		return NULL;

	return table[row-1][col-1];
}

void Board::setPieceAt(Piece* p, UINT col, UINT row)
{
	if (col > cols || row > rows || row < 1 || col < 1)
		return;

	table[row-1][col-1] = p;
}

void Board::removePiece(UINT col, UINT row)
{
	if (col > cols || row > rows || row < 1 || col < 1)
		return;

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
	ePieceType type, originalType;

	type = p->getType();
	originalType = p->getOriginalType();
	player = p->getOwner();

	p2 = getPieceAt(toX, toY);
	if (!p2) //Piece was not found
	{
		setPieceAt(p, toX, toY);
		if (!moved) //first positioning
			return (player->incTypeCount(type, originalType) == -1);
		else
		{
			dprint("Player #%d moved its %c [if J: %c] from (%d,%d) to (%d,%d)\n",
				player->getPlayerId(), pieceToChar(originalType), pieceToChar(type),
				fromX, fromY, toX, toY);
			setPieceAt(NULL, fromX, fromY);
		}
		return 0; 
	}
	else //piece found at cell
	{
		if (player == p2->getOwner())
		{
			//both pieces belong to same player
			player->setHasLost();
			if (moved)
				player->setReason(BAD_MOVES_INPUT_FILE);
			else
				player->setReason(BAD_POSITIONING_INPUT_FILE_DOUBLE_POSITION);

			return -1; 
		}

		dprint("MATCH: Player#%d [%c already at (%d,%d)] VS Player #%d [%c from (%d,%d)] ... ",
			p2->getOwner()->getPlayerId(), pieceToChar(p2->getType()), toX, toY, p->getOwner()->getPlayerId(), pieceToChar(type), fromX, fromY);

		if (type == BOMB || p2->getType() == BOMB)
		{
			if(type != p2->getType())
				dprint("Player #%d WINS!\n", type == BOMB ? player->getPlayerId() : p2->getOwner()->getPlayerId());
			else
				dprint("It's a TIE!\n");

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
			setPieceAt(p, toX, toY);
			setPieceAt(NULL, fromX, fromY);
			if (!moved)
			{
				if (player->incTypeCount(type, originalType))
					return -1;
			}

			dprint("Player #%d WINS!\n", p->getOwner()->getPlayerId());
			break;

		case LOSE:
			dprint("Player #%d LOSES!\n", p->getOwner()->getPlayerId());
			if (moved)
				removePiece(fromX, fromY);
			break;

		case TIE:
			dprint("It's a TIE!\n");
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
	ePieceType type;

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
		printf("[Board::movePiece] no piece at fromX <%d> fromY <%d>\n", fromX, fromY);
		return ERROR;
	}

	type = p1->getType();
	if (type == BOMB || type == FLAG || type == UNDEF || type == JOKER)
	{
		printf("[Board::movePiece] illegal piece type <%c> for move at fromX <%d> fromY <%d> toX <%d> toY <%d> \n", pieceToChar(type), fromX, fromY, toX, toY);
		return ERROR;
	}

	if (fromX == toX && fromY == toY)
	{
		printf("[Board::movePiece] piece %c of player #%d at <%d> fromY <%d> same position: toX <%d> toY <%d>\n",
			pieceToChar(p1->getOriginalType()), p1->getOwner()->getPlayerId(), fromX, fromY, toX, toY);

		return ERROR;
	}
	//can move only one cell at a time
	if (fromX >= (toX - 1) && fromX <= (toX + 1) && fromY >= (toY - 1) && fromY <= (toY + 1))
	{
		if (fromX != toX && fromY != toY)	//diagonal
		{
			printf("[Board::movePiece] piece %c of player #%d at <%d> fromY <%d> cannot move diagonally to toX <%d> toY <%d>\n",
				pieceToChar(p1->getOriginalType()), p1->getOwner()->getPlayerId(), fromX, fromY, toX, toY);

			return ERROR;
		}
		return positionPiece(p1, toX, toY, 1, fromX, fromY);

	}

	printf("[Board::movePiece] piece %c of player #%d at <%d> fromY <%d> tried to move more than one cell to: toX <%d> toY <%d>\n",
		pieceToChar(p1->getOriginalType()), p1->getOwner()->getPlayerId(), fromX, fromY, toX, toY);

	return ERROR;

}

int Board::changeJokerType(UINT fromX, UINT fromY, ePieceType newType)
{
	Piece* p1;

	if (fromX >= cols || fromY >= rows)
	{
		cout << "changeJokerType: Illegal dimensions" << endl;
		return -1;
	}

	p1 = getPieceAt(fromX, fromY);
	if (!p1) //Piece was not found
	{
		printf("changeJokerType: no piece at fromX <%d> fromY <%d>\n", fromX, fromY);
		return -1;
	}

	if (!p1->isJoker())
	{
		printf("changeJokerType: player #%d piece <%c> at fromX <%d> fromY <%d> is not a Joker\n", 
			p1->getOwner()->getPlayerId(), charToPiece(p1->getType()), fromX, fromY);
		return -1;
	}

	dprint("CHANGED JOKER at (%d,%d) from %c to %c\n", 
		fromX, fromY, pieceToChar(p1->getType()), pieceToChar(newType));

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
		delete[] table[i];
	}
	delete[] table;
}







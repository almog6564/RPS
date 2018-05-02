#include "myBoard.h"
#include <iostream>
#include "cstdio"

using namespace std;

MyBoard::MyBoard(UINT N, UINT M) : rows(N), cols(M)
{
	table = new Piece**[N];

	for (UINT i = 0; i < N; i++)
	{
		table[i] = new Piece*[M]();
	}
}

int MyBoard::getPlayer(const Point& pos) const
{
	UINT col, row;
	int ret = 0;
	Piece* p = nullptr;

	do 
	{
		col = (UINT)pos.getX();
		row = (UINT)pos.getY();

		if (col > cols || row > rows || row < 1 || col < 1)
			break;

		p = table[row - 1][col - 1];

		if (!p)
			break;

		ret = p->getOwner()->getPlayerId() + 1;

	} while (false);

	return ret;
}

Piece* MyBoard::getPieceAt(UINT col, UINT row) 
{
	if (col > cols || row > rows || row < 1 || col < 1)
		return nullptr;

	return table[row-1][col-1];
}

void MyBoard::setPieceAt(Piece* p, UINT col, UINT row)
{
	if (col > cols || row > rows || row < 1 || col < 1)
		return;

	table[row-1][col-1] = p;

	if (p)
	{
		p->setPiecePosition(col, row);
	}
}

void MyBoard::removePiece(UINT col, UINT row)
{
	if (col > cols || row > rows || row < 1 || col < 1)
		return;
	
	Piece* p = table[row - 1][col - 1];

	if (p)
	{
		p->owner->decTypeCounter(p->getType(), p->getOriginalType());

		delete table[row - 1][col - 1];
		table[row - 1][col - 1] = nullptr;
	}
}

/**
 * This function assumes legal dimensions.
*/
int MyBoard::positionPiece(Piece* newPiece, UINT toX, UINT toY, unique_ptr<MyFightInfo>& fight, int moved, UINT fromX, UINT fromY)
{
	Piece* existingPiece;
	eScore score;
	PlayerContext* newPieceOwner, *existingPieceOwner;
	ePieceType type, originalType, existingPieceType;

	type = newPiece->getType();
	originalType = newPiece->getOriginalType();
	newPieceOwner = newPiece->getOwner();

	existingPiece = getPieceAt(toX, toY);
	if (!existingPiece) //Piece was not found
	{
		setPieceAt(newPiece, toX, toY);

		if (!moved) //first positioning
			return (newPieceOwner->incTypeCount(type, originalType) == -1);
		else
		{
			dprint("Player #%d moved its %c [if J: %c] from (%d,%d) to (%d,%d)\n",
				newPieceOwner->getPlayerId()+1, pieceToChar(originalType), pieceToChar(type),
				fromX, fromY, toX, toY);
			setPieceAt(nullptr, fromX, fromY);
		}
		return 0; 
	}
	else //piece found at cell
	{

		existingPieceType = existingPiece->getType();
		existingPieceOwner = existingPiece->getOwner();

		if (newPieceOwner == existingPieceOwner)
		{
			//both pieces belong to same player
			newPieceOwner->setHasLost();
			if (moved)
				newPieceOwner->setReason(BAD_MOVES_INPUT_FILE);
			else
				newPieceOwner->setReason(BAD_POSITIONING_INPUT_FILE_DOUBLE_POSITION);

			return -1; 
		}

		dprint("MATCH: Player#%d [%c already at (%d,%d)] VS Player #%d [%c from (%d,%d)] ... ",
			existingPieceOwner->getPlayerId()+1, pieceToChar(existingPiece->getType()), toX, toY, newPieceOwner->getPlayerId(), pieceToChar(type), fromX, fromY);

		if (type == BOMB || existingPieceType == BOMB)
		{
			if (type != existingPieceType)
			{
				fight.reset(new MyFightInfo(toX, toY, type, existingPieceType, type == BOMB ? newPieceOwner->getPlayerId() + 1 : existingPieceOwner->getPlayerId() + 1));

				dprint("Player #%d WINS!\n", type == BOMB ? newPieceOwner->getPlayerId()+1 : existingPieceOwner->getPlayerId()+1);
			}
			else
			{
				fight.reset(new MyFightInfo(toX, toY, type, existingPieceType, 0));
				dprint("It's a TIE!\n");
			}

			removePiece(toX, toY);
			if (moved)
				removePiece(fromX, fromY);
			else
				delete newPiece;

			if (!moved && type == FLAG)
			{
				//flag was positioned but lost, need to know that flag existed
				newPieceOwner->incOriginalFlagCount();
			} 

			return 0;
		}


		//pieces belong to different players, MATCH!

		score = newPiece->match(existingPiece);
		switch (score)
		{
		case WIN:
			removePiece(toX, toY);
			setPieceAt(newPiece, toX, toY);
			setPieceAt(nullptr, fromX, fromY);
			if (!moved)
			{
				if (newPieceOwner->incTypeCount(type, originalType))
					return -1;
			}
			fight.reset(new MyFightInfo(toX, toY, type, existingPieceType, newPieceOwner->getPlayerId() + 1));
			dprint("Player #%d WINS!\n", newPieceOwner->getPlayerId()+1);
			break;

		case LOSE:

			fight.reset(new MyFightInfo(toX, toY, type, existingPieceType, existingPieceOwner->getPlayerId() + 1));

			dprint("Player #%d LOSES!\n", newPieceOwner->getPlayerId()+1);
			if (moved)
				removePiece(fromX, fromY);
			else
				delete newPiece;
			break;

		case TIE:
			dprint("It's a TIE!\n");

			fight.reset(new MyFightInfo(toX, toY, type, existingPieceType, 0));

			if (moved)
				removePiece(fromX, fromY);
			else
				delete newPiece;

			removePiece(toX, toY);

			break;

		default: //ERROR
			delete newPiece;
			cout << "invalid match result" << endl;
			return -1;
		}

		if (score != WIN && type == FLAG)
		{
			//flag was positioned but lost, need to know that flag existed
			newPieceOwner->incOriginalFlagCount();
		}

	}

	return 0;
}


int MyBoard::movePiece(UINT playerID, UINT fromX, UINT fromY, UINT toX, UINT toY, unique_ptr<MyFightInfo>& fightInfo)
{
	Piece* p1;
	UINT p1PlayerId;
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
	p1PlayerId = p1->getOwner()->getPlayerId();
	if (p1PlayerId != playerID)
	{
		printf("[Board::movePiece] player #%d tried to move piece %c at fromX <%d> fromY <%d> which belongs to player #%d \n", 
			playerID+1, pieceToChar(type), fromX, fromY, p1PlayerId+1);

		return ERROR;
	}

	if (type == BOMB || type == FLAG || type == UNDEF || type == JOKER)
	{
		printf("[Board::movePiece] illegal piece type <%c> for move at fromX <%d> fromY <%d> toX <%d> toY <%d> \n", pieceToChar(type), fromX, fromY, toX, toY);
		return ERROR;
	}

	if (fromX == toX && fromY == toY)
	{
		printf("[Board::movePiece] piece %c of player #%d at <%d> fromY <%d> same position: toX <%d> toY <%d>\n",
			pieceToChar(p1->getOriginalType()), p1->getOwner()->getPlayerId()+1, fromX, fromY, toX, toY);

		return ERROR;
	}
	//can move only one cell at a time
	if (fromX >= (toX - 1) && fromX <= (toX + 1) && fromY >= (toY - 1) && fromY <= (toY + 1))
	{
		if (fromX != toX && fromY != toY)	//diagonal
		{
			printf("[Board::movePiece] piece %c of player #%d at <%d> fromY <%d> cannot move diagonally to toX <%d> toY <%d>\n",
				pieceToChar(p1->getOriginalType()), p1->getOwner()->getPlayerId()+1, fromX, fromY, toX, toY);

			return ERROR;
		}
		return positionPiece(p1, toX, toY, fightInfo, 1, fromX, fromY);

	}

	printf("[Board::movePiece] piece %c of player #%d at <%d> fromY <%d> tried to move more than one cell to: toX <%d> toY <%d>\n",
		pieceToChar(p1->getOriginalType()), p1->getOwner()->getPlayerId()+1, fromX, fromY, toX, toY);

	return ERROR;

}

int MyBoard::changeJokerType(UINT fromX, UINT fromY, ePieceType newType)
{
	Piece* p1;

	if (fromX > cols || fromY > rows || fromX < 1 || fromY < 1)
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
			p1->getOwner()->getPlayerId()+1, charToPiece(p1->getType()), fromX, fromY);
		return -1;
	}

	dprint("CHANGED JOKER at (%d,%d) from %c to %c\n", 
		fromX, fromY, pieceToChar(p1->getType()), pieceToChar(newType));

	((Joker*)p1)->setCurrentType(newType);
	return 0;
}

void MyBoard::getBoardDimensions(UINT* pCols, UINT* pRows)
{
	if (pCols)
		*pCols = cols;

	if (pRows)
		*pRows = rows;
}

MyBoard::~MyBoard()
{
	for (UINT i = 0; i < rows; i++)
	{
		for (UINT j = 0; j < cols; j++)
		{
			if(table[i][j])
				delete table[i][j];
		}

		delete[] table[i];
	}
	delete[] table;
}








#ifndef _MY_BOARD_
#define _MY_BOARD_

#include "piece.h"
#include "Board.h"
#include "MyPoint.h"
#include "MyFightInfo.h"

using namespace std;

class MyBoard : public Board
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
	MyBoard(UINT N, UINT M);

	~MyBoard();

	virtual int getPlayer(const Point& pos) const;

	/*
	This function receives a position on board (x,y) and return a pointer to the piece, if there is one.
	@returns -
			a pointer to a Piece, if there is one in (col, row)
			a null pointer, if cell is empty, or illegal (col, row) input (exceeds the size of the board)
	*/
	Piece* getPieceAt(UINT col, UINT row);

	/*
	This function sets a given pointer to Piece at the postion (col, row) on board, does nothing if illegal arguments.
	*/
	void setPieceAt(Piece* p, UINT col, UINT row);

	/*
	If input (col, row) is  legal (within boundries of board), delete the Piece in position and reinitiaize position
	to nullptr
	*/
	void removePiece(UINT col, UINT row);

	void clearBoard();
	/*
	This function recieves a playerID (0 or 1), coordinates (fromX, fromY, toX, toY).
	This function will find the Piece placed at (fromX, fromY) on board, and if the piece is there, and owned
	by the given playerID, will move the piece to (toX, toY) and call positionPiece() function.
		@returns -
		0 - function finished with no errors.
		1 - function finished with error, or a player has lost due to an illegal action
	*/
	int movePiece(UINT playerID, UINT fromX, UINT fromY, UINT toX, UINT toY, unique_ptr<MyFightInfo>& fightInfo);

	/*
	This function recieves a pointer to Piece and a position to place it on board.
	This function is robust, and can be used for first positioning phase and for the moving phase of the game.
	In case that it is used during the moving phase, the moved argument will be set to 1, and fromX, fromY arguments
	will be passed as well as the toX,toY.
	If the position is already taken by a different Piece of a different Player, than a match will take place.
		@returns -
			0 - function finished with no errors.
			1 - function finished with error, or a player has lost due to an illegal action
	*/
	int positionPiece(Piece* p, UINT toX, UINT toY, unique_ptr<MyFightInfo>& fight, int moved = 0, UINT fromX = 0, UINT fromY = 0);
	int changeJokerType(UINT fromX, UINT fromY, ePieceType newType);

	void getBoardDimensions(UINT* pCols, UINT* pRows);
};






#endif //_MY_BOARD_


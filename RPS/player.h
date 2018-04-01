#ifndef _PLAYER_
#define _PLAYER_

#include "defs.h"
#include <iostream>


class Player
{
	UINT pieceCounters[PIECE_COUNT];
	UINT movingPiecesCtr; 
	UINT score;
	bool hasMoreMoves;

public:
	Player(UINT R, UINT P, UINT S, UINT B, UINT J, UINT F);
	void decCounter(ePieceType type);
	bool isAlive();
	bool getNextMove(int* fromX, int* fromY, int* toX, int* toY);
	void updateScore();
	bool getHasMoreMoves();
	void setHasMoreMoves(bool val);
	~Player();
};







#endif //_PLAYER_

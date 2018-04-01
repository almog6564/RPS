#ifndef _GAME_
#define _GAME_

#include "defs.h"
#include "player.h"
#include "board.h"

class Move
{
	UINT fromX;
	UINT fromY;
	UINT toX;
	UINT toY;
};

class Game
{
	Player* player1, *player2;
	Board* board;
	UINT M, N;
	UINT R1, P1, S1, B1, J1, F1;
	UINT R2, P2, S2, B2, J2, F2;
	
	void runSingleMove(Player* player, Player* against);

public:
	Game(UINT M, UINT N, UINT R1, UINT P1, UINT S1, UINT B1, UINT J1, UINT F1,
						 UINT R2, UINT P2, UINT S2, UINT B2, UINT J2, UINT F2);

	void runMove();

	bool endGame();

	int getWinner();

	~Game();







};







#endif //_GAME_

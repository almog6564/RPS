#ifndef _GAME_
#define _GAME_

#include "defs.h"
#include "player.h"
#include "board.h"
#include "parser.h"

using namespace std;

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
	FileParser* fileParser;
	int turn; //0=player1, 1=player2
	
	void runSingleMove(Player* player);

	void checkPlayersFlags(Player* player);

	int positionSinglePiece(Player* player);


public:
	Game(UINT M, UINT N, UINT R1, UINT P1, UINT S1, UINT B1, UINT J1, UINT F1,
						 UINT R2, UINT P2, UINT S2, UINT B2, UINT J2, UINT F2, FileParser* fileParser);

	void runMove();

	bool endGame();

	bool positioningDone();

	int positionPiece(int player);

	void flagsCheck();

	int getWinner(eReason* reason);

	string GetReasonString(eReason reason);
	void writeOutputFile();
	int validatePositionFiles();

	void resetPieceFiles();

};







#endif //_GAME_

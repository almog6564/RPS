#include "game.h"

#include "parser.h"


int main()
{
	int M, N, R, P, S, B, J, F;
	M = N = 10;
	R = 2; P = 5; S = 1; B = 2; J = 2; F = 1;
	string p1p = "player1.rps_board";
	string p1m = "player1.rps_moves";
	string p2p = "player2.rps_board";
	string p2m = "player2.rps_moves";
	string output = "rps.output";

	eReason reason;

	FileParser* fileParser = new FileParser(p1p, p2p, p1m, p2m, output);
	if (fileParser->initializeFiles()) //initialization failed
	{
		//print error
		return -1;
	}

	Game* game = new Game(M, N, R, P, S, B, J, F,
								R, P, S, B, J, F, fileParser);
	/* First positioning */
	if (game->validatePositionFiles())
	{
		//end game
		return -1;
	}
		
	game->resetPieceFiles();

	while (!game->positionPiece(0));
	while (!game->positionPiece(1));

	/* Check Flags counters*/
	game->flagsCheck();

	/* Start Game */
	while (!game->endGame())
		game->runMove();
	game->getWinner(&reason);

	return 0;
}
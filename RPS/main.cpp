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
	int	winner;
	eReason reason;

	FileParser* fileParser = new FileParser(p1p, p2p, p1m, p2m, output);
	if (fileParser->initializeFiles()) //initialization failed
	{
		//print error
		return -1;
	}

	Game* game = new Game(M, N, R, P, S, B, J, F,
								R, P, S, B, J, F, fileParser);

	dprint("created new game\n");

	/* First positioning */
	if (game->validatePositionFiles())
	{
		//end game
		return -1;
	}
		
	dprint("validated position files\n");

	game->resetPieceFiles();

	while (!game->positionPiece(0));
	while (!game->positionPiece(1));

	dprint("positioned files on screen\n");

	/* Check Flags counters*/
	game->flagsCheck();

	dprint("checked flags\n");

	/* Start Game */
	while (!game->endGame())
		game->runMove();

	winner = game->getWinner(&reason);

	dprint("winner is %d\n",winner);

	game->writeOutputFile();

	dprint("wrote output file\n");

	return 0;
}
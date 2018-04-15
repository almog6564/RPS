#include "game.h"

#include "parser.h"


int main()
{
	Game* game = nullptr;
	int M, N, R, P, S, B, J, F;
	M = N = 10;
	R = 2; P = 5; S = 1; B = 2; J = 2; F = 1;
	string p1p = "player1.rps_board";
	string p1m = "player1.rps_moves";
	string p2p = "player2.rps_board";
	string p2m = "player2.rps_moves";
	string output = "rps.output";

#if DEBUG == 1
	eReason reason;
	int	winner;
#endif

	FileParser* fileParser = new FileParser(p1p, p2p, p1m, p2m, output);
	if (fileParser->initializeFiles()) //initialization failed
	{
		cout << "Initialization of game failed because of file error" << endl;
		return -1;
	}

	do 
	{
		 game = new Game(M, N, R, P, S, B, J, F,
			R, P, S, B, J, F, fileParser);

		/* First positioning */
		if (game->validatePositionFiles())
		{
			//end game
			dprint("Validation of piece files FAILED\n");
			break;
		}

		dprint("Validated of position files SUCCESS\n");

		game->resetPieceFiles();

		while (!game->positionPiece(0));
		dprint("Positioning of pieces 0 on board SUCCEEDED!\n");

		while (!game->positionPiece(1));

		dprint("Positioning of pieces on board ENDED!\n");

		/* Check Flags counters*/
		game->flagsCheck();
		
		/* Start Game */
		while (!game->endGame())
			game->runMove();

	} while (false);

#if DEBUG == 1
	winner = game->getWinner(&reason);
	dprint("\n\n ########## FINISHED GAME - RESULTS ##########\n\nWINNER is : %d\n", winner);
#endif

	game->writeOutputFile();

	delete game;
	delete fileParser;

	return 0;
}
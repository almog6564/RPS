

#include "game.h"
#include "parser.h"

using namespace std;

int main()
{
	Game* game = nullptr;
	int M, N, R, P, S, B, J, F;
	M = N = 10;
	R = 2; P = 5; S = 1; B = 2; J = 2; F = 1;
	bool bIsPlayer1Auto = true ,  bIsPlayer2Auto = true;
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
			R, P, S, B, J, F, fileParser, bIsPlayer1Auto, bIsPlayer2Auto);

		 game->positionAllPieces();

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
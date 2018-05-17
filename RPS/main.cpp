

#include "game.h"
#include "parser.h"
#include <algorithm>

using namespace std;

int main(int argc, char* argv[])
{
	FileParser* fileParser = nullptr;
	Game* game = nullptr;
	int M, N, R, P, S, B, J, F;
	M = N = 30;
	R = 2; P = 5; S = 1; B = 2; J = 2; F = 1;
	bool printUsageAndExit = false;
	vector<string> usages = { "file-vs-file", "file-vs-auto", "auto-vs-file", "auto-vs-auto" };
	eGameMode gameMode;

#if DEBUG == 1

#endif

	do
	{
		if (argc != 2)
		{
			printUsageAndExit = true;
			break;
		}

		const auto usage = find(usages.begin(), usages.end(), argv[1]);

		if(usage == usages.end())
		{
			printUsageAndExit = true;
			break;
		}

		gameMode = (eGameMode) distance(usages.begin(), usage);

		fileParser = new FileParser(gameMode);

		if (fileParser->initializeFiles()) //initialization failed
		{
			cout << "Initialization of game failed because of file error" << endl;
			break;
		}

		 game = new Game(M, N, R, P, S, B, J, F,
			R, P, S, B, J, F, fileParser, gameMode);

		 game->positionAllPieces();

		dprint("Positioning of pieces on board ENDED!\n");

		/* Check Flags counters*/
		game->flagsCheck();
		
		/* Start Game */
		while (!game->endGame())
			game->runMove();
		
		dprint("\n\n ########## FINISHED GAME - RESULTS ##########\n\nWINNER is : %d\n", game->getWinner());

		game->writeOutputFile();

	} while (false);

	if (printUsageAndExit)
	{
		cout << "\nUsage: ex2 mode\nOptions for \"mode\":"
			"\n - auto-vs-file\n - file-vs-auto\n - auto-vs-auto\n - file-vs-file\n" << endl;
	}

	delete game;
	delete fileParser;

	return 0;
}
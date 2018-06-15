

#include "game.h"
#include "parser.h"
#include <algorithm>

using namespace std;

int main(int argc, char* argv[])
{
	FileParser* fileParser = nullptr;
	Game* game = nullptr;
	bool printUsageAndExit = false;
	vector<string> usages = { "file-vs-file", "file-vs-auto", "auto-vs-file", "auto-vs-auto" };
	eGameMode gameMode;

#if DEBUG == 1

#endif

	do
	{
		//must have arg and exactly one
		if (argc != 2)
		{
			printUsageAndExit = true;
			break;
		}

		//find arg in possible args and if not found print usage
		const auto usage = find(usages.begin(), usages.end(), argv[1]);

		if(usage == usages.end())
		{
			printUsageAndExit = true;
			break;
		}

		//index of the arg in array usage is exactly the the eGameMode value according
		gameMode = (eGameMode) distance(usages.begin(), usage);

		fileParser = new FileParser(gameMode);

		if (fileParser->initializeFiles()) //initialization failed
		{
			cout << "Initialization of game failed because of file error" << endl;
			break;
		}

		 game = new Game(BOARD_SIZE, BOARD_SIZE, 
			 R_COUNT, P_COUNT, S_COUNT, B_COUNT, J_COUNT, F_COUNT,
			 R_COUNT, P_COUNT, S_COUNT, B_COUNT, J_COUNT, F_COUNT,
			 fileParser, gameMode);

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
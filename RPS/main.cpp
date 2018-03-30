
#include "parser.h"


int main()
{
	char* p1p = "player1.rps_board";
	char* p1m = "player1.rps_moves";
	char* p2p = "player2.rps_board";
	char* p2m = "player2.rps_moves";
	ePieceType type, jokerType;
	UINT x, y;
	FileParser* f = new FileParser(p1p, p2p, p1m, p2m);
	eFileStatus status;

	char* names[] = { "UNDEF", "ROCK", "PAPER", "SCISSORS", "BOMB", "JOKER", "FLAG" };

	if (f->initializeFiles() < 0)
	{
		printf("Error file open\n");
		return -1;
	}

	while ((status = f->p1->getNextPiece(&type, &x, &y, &jokerType)) != FILE_EOF_REACHED)
	{
		if (status == FILE_SUCCESS)
		{
			if (type != JOKER)
				printf("Got piece: %s (%d,%d)\n", names[type], x, y);
			else
				printf("Got JOKER: %s (%d,%d)\n", names[jokerType], x, y);
		}
		else
		{
			printf("Bad line : %s\n", f->p1->pieces->getLastReadLine().c_str());
		}
	}

	delete f;

	return 0;
}
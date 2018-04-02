#include "game.h"

#include "parser.h"


int main()
{
	int M, N, R, P, S, B, J, F;
	M = N = 10;
	R = 2; P = 5; S = 1; B = 2; J = 2; F = 1;

	Game* game = new Game(M, N, R, P, S, B, J, F,
								R, P, S, B, J, F);
	/* First positioning */
	

	/* Check Flags counters*/
	game->flagsCheck();

	/* Start Game */
	while (!game->endGame())
		game->runMove();

	return 0;
}
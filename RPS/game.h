#ifndef _GAME_
#define _GAME_

#include "defs.h"
#include "PlayerContext.h"
#include "myBoard.h"
#include "parser.h"
#include "AutoPlayerAlgorithm.h"
#include "FilePlayerAlgorithm.h"

using namespace std;

class Game
{
	PlayerContext*		player1Context,		*player2Context;
	PlayerAlgorithm*	player1Algorithm,	*player2Algorithm;

	MyBoard* board;
	UINT M, N;
	UINT R1, P1, S1, B1, J1, F1;
	UINT R2, P2, S2, B2, J2, F2;
	FileParser* fileParser;
	int turn; //0=player1, 1=player2
	
	void runSingleMove(PlayerContext* playerContext, PlayerAlgorithm* playerAlgo);

	void checkPlayersFlagCountLessThanMax(PlayerContext* player);

public:
	Game(UINT M, UINT N, UINT R1, UINT P1, UINT S1, UINT B1, UINT J1, UINT F1,
						 UINT R2, UINT P2, UINT S2, UINT B2, UINT J2, UINT F2, FileParser* fileParser,
						 bool autoPlayer1, bool autoPlayer2);


	~Game();
	/*
	Before starting the actual game (i.e. positioning, matching and moving pieces), this function checks
	wheather the board files are valid (i.e. correct formats and no two pieces at the same location by the 
	same player).
		@returns -
			0 - board files are valid, can start game.
			-1 - one or both board files are not valid, the player(s) with the invalid board will be set has
				a losing player with the corresponding result.
	*/
	int validatePositionFiles();

	/*
	This function is called during the first positioning phase.
	We assume that both player's boards are valid at this point.
	This function reads the pieces files to get the next piece, creates the piece, and postions it on board.
		@returns -
			0 - Piece was extracted from file, created and positioned on board.
			-1 - No pieces are left in file
	*/
	int positionPiece(int player);

	/*
	Before starting to move pieces according to moving files, this function makes sure that for both players
	the number of flags that was placed is mathcing the number of flags that should have been placed (1 in this
	specific exercise).
	If not, the illegal player(s) will be set as a losing player, with the corresponding reason.
	*/
	void flagsCheck();

	/*
	This function is called by main function, and is simply calling an inner private
	function: runSingleMove(Player* p) with the right player according to currnet turn.
	*/
	void runMove();


	/*
	This function is called by main function at the beginning of every turn, checking if the condition to end 
	the game (e.g. one or both players as lost during the last turn) has met.
		@returns -
			true - if conditions to end the game has met (stop the game)
			false - if conditions to end the game has not been met (continue the game)

	*/
	bool endGame();

	/*
	This function is called by main function at the final stage of the game (i.e. after the first time
	endGame() function returned true).
	The function checks the status of both players and decides who is the winner, and from what reason.
		@returns -
			0 - if player 1 won.
			1 - if player 2 won.
			2 - if it's a tie.
			The function will also update the reason given as a pointer with the right reason.
	*/
	int getWinner(eReason* reason);

	string GetReasonString(eReason reason);

	void writeOutputFile();

	void positionAllPieces();
	void resetPieceFiles();

private:
	void checkWhetherFlagsWereCaptured(void);
};







#endif //_GAME_

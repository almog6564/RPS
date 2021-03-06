#ifndef _GAME_
#define _GAME_

#include "defs.h"
#include "PlayerContext.h"
#include "myBoard.h"
#include "parser.h"
#include "AutoPlayerAlgorithm.h"
#include "FilePlayerAlgorithm.h"
#include <utility>      // std::pair



using namespace std;

class Game
{
	PlayerContext*		player1Context,		*player2Context;
	unique_ptr<PlayerAlgorithm>	player1Algorithm,	player2Algorithm;

	MyBoard* board;
	UINT M, N;
	UINT R1, P1, S1, B1, J1, F1;
	UINT R2, P2, S2, B2, J2, F2;
	FileParser* fileParser;
	int turn; //0=player1, 1=player2
	int movesCounter;
	eGameMode gameMode;

	int player1GlobalID; //for scoreBoard
	int player2GlobalID; //for scoreBoard

	void runSingleMove(PlayerContext* playerContext, unique_ptr<PlayerAlgorithm>& playerAlgo);
	//void runSingleMove(PlayerContext* playerContext, PlayerAlgorithm* playerAlgo);

	void gameNotifyFightResult(unique_ptr<MyFightInfo>& fightInfo);

	void gameNotifyOnOpponnentMove(PlayerContext* playerContext, unique_ptr<Move>& move);

	void checkPlayersFlagCountLessThanMax(PlayerContext* player);


	bool isPlayerAuto(int ID);
	int gameID;

public:
	Game(UINT M, UINT N, UINT R1, UINT P1, UINT S1, UINT B1, UINT J1, UINT F1,
						 UINT R2, UINT P2, UINT S2, UINT B2, UINT J2, UINT F2, 
							FileParser* fileParser,	eGameMode gameMode);

	Game(unique_ptr<PlayerAlgorithm>& algo1, unique_ptr<PlayerAlgorithm>& algo2, int player1GlobalID, int player2GlobalID, int gameID);

	~Game();

	pair<int, int> getPlayersGlobalIDs()
	{
		return pair<int, int>(player1GlobalID, player2GlobalID);
	}

	int getGameID()
	{
		return gameID;
	}

	/*
	Before starting to move pieces according to moving files, this function makes sure that for both players
	the number of flags that was placed is matching the number of flags that should have been placed (1 in this
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
	int getWinner(eReason* reason = nullptr);

	string GetReasonString(eReason reason);

	/* Function for debug */
	void printBoard();

	void writeOutputFile();

	/* Positions all the pieces on the game board using the player's algorithms. */
	void positionAllPieces();

	void run(int *winnerToFill);

private:
	void checkWhetherFlagsWereCaptured(void);
};






#endif //_GAME_

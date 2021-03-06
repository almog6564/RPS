#ifndef __FILE_PLAYER_ALGORITHM_H_
#define __FILE_PLAYER_ALGORITHM_H_

#include "PlayerAlgorithm.h"
#include "parser.h"

class FilePlayerAlgorithm : public PlayerAlgorithm
{
	PlayerFileContext& playerFileContext;
	UINT rows, cols;
	bool hasMoreMoves;
	unique_ptr<JokerChange> nextJokerChange;
	
	

	UINT validatePlayerPositions(int player);
public:
	FilePlayerAlgorithm(PlayerFileContext& fileContext, UINT rows, UINT cols);

	~FilePlayerAlgorithm() {};

	void getInitialPositions(int player, std::vector<unique_ptr<PiecePosition>>& vectorToFill);
	void notifyOnInitialBoard(const Board& b, const std::vector<unique_ptr<FightInfo>>& fights);
	void notifyOnOpponentMove(const Move& move); // called only on opponent�s move
	void notifyFightResult(const FightInfo& fightInfo); // called only if there was a fight
	unique_ptr<Move> getMove();
	unique_ptr<JokerChange> getJokerChange() // nullptr if no change is requested
	{
		return move(nextJokerChange);
	}
};

#endif

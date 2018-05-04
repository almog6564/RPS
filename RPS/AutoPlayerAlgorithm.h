#ifndef __AUTO_PLAYER_ALGORITHM_H_
#define __AUTO_PLAYER_ALGORITHM_H_

#include "PlayerAlgorithm.h"
#include "defs.h"
#include <random>

class PositioningScenario;

class AutoPlayerAlgorithm : public PlayerAlgorithm
{
private:
	PositioningScenario* scenario;
	UINT boardRows, boardCols;

	struct { 
		UINT R, P, S, B, J, F;
	} initPieceCnt;

public:
	AutoPlayerAlgorithm(UINT boardRows, UINT boardCols, 
		UINT R, UINT P, UINT S, UINT B, UINT J, UINT F);

	~AutoPlayerAlgorithm();

	void getInitialPositions(int player, std::vector<unique_ptr<PiecePosition>>& vectorToFill);
	void notifyOnInitialBoard(const Board& b, const std::vector<unique_ptr<FightInfo>>& fights);
	void notifyOnOpponentMove(const Move& move); // called only on opponent’s move
	void notifyFightResult(const FightInfo& fightInfo); // called only if there was a fight
	unique_ptr<Move> getMove();
	unique_ptr<JokerChange> getJokerChange(); // nullptr if no change is requested
};


class PositioningScenario
{
public:
	bool areFlagsOnCoreners;
	bool areMovingOnCorners;

	PositioningScenario(bool areFlagsOnCoreners, bool areMovingOnCorners) :
		areFlagsOnCoreners(areFlagsOnCoreners), areMovingOnCorners(areMovingOnCorners) {};
};

#endif
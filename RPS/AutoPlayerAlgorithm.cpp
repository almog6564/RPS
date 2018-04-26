#include "AutoPlayerAlgorithm.h"



AutoPlayerAlgorithm::AutoPlayerAlgorithm()
{
}


AutoPlayerAlgorithm::~AutoPlayerAlgorithm()
{
}

void AutoPlayerAlgorithm::getInitialPositions(int player, std::vector<unique_ptr<PiecePosition>>& vectorToFill)
{
}

void AutoPlayerAlgorithm::notifyOnInitialBoard(const Board & b, const std::vector<unique_ptr<FightInfo>>& fights)
{
}

void AutoPlayerAlgorithm::notifyOnOpponentMove(const Move & move)
{
}

void AutoPlayerAlgorithm::notifyFightResult(const FightInfo & fightInfo)
{
}

unique_ptr<Move> AutoPlayerAlgorithm::getMove()
{
	return unique_ptr<Move>();
}

unique_ptr<JokerChange> AutoPlayerAlgorithm::getJokerChange()
{
	return unique_ptr<JokerChange>();
}

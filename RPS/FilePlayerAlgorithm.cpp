#include "FilePlayerAlgorithm.h"



FilePlayerAlgorithm::FilePlayerAlgorithm()
{
}


FilePlayerAlgorithm::~FilePlayerAlgorithm()
{
}

void FilePlayerAlgorithm::getInitialPositions(int player, std::vector<unique_ptr<PiecePosition>>& vectorToFill)
{
}

void FilePlayerAlgorithm::notifyOnInitialBoard(const Board & b, const std::vector<unique_ptr<FightInfo>>& fights)
{
}

void FilePlayerAlgorithm::notifyOnOpponentMove(const Move & move)
{
}

void FilePlayerAlgorithm::notifyFightResult(const FightInfo & fightInfo)
{
}

unique_ptr<Move> FilePlayerAlgorithm::getMove()
{
	return unique_ptr<Move>();
}

unique_ptr<JokerChange> FilePlayerAlgorithm::getJokerChange()
{
	return unique_ptr<JokerChange>();
}

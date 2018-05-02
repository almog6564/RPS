

#include "AutoPlayerAlgorithm.h"
#include "defs.h"
#include "MyPoint.h"


AutoPlayerAlgorithm::AutoPlayerAlgorithm()
{
}


AutoPlayerAlgorithm::~AutoPlayerAlgorithm()
{
}

void AutoPlayerAlgorithm::getInitialPositions(int player, std::vector<unique_ptr<PiecePosition>>& vectorToFill)
{
	player = player;
	vectorToFill.size();
}

void AutoPlayerAlgorithm::notifyOnInitialBoard(const Board & b, const std::vector<unique_ptr<FightInfo>>& fights)
{
	//just zevel for it to compile
	auto a = new MyPoint(1, 1);
	b.getPlayer(*a);
	fights.size();
	delete a;
}

void AutoPlayerAlgorithm::notifyOnOpponentMove(const Move & move)
{
	move.getFrom();
}

void AutoPlayerAlgorithm::notifyFightResult(const FightInfo & fightInfo)
{
	fightInfo.getPiece(1);
}

unique_ptr<Move> AutoPlayerAlgorithm::getMove()
{
	return unique_ptr<Move>();
}

unique_ptr<JokerChange> AutoPlayerAlgorithm::getJokerChange()
{
	return unique_ptr<JokerChange>();
}

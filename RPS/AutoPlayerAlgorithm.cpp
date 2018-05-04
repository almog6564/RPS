

#include "AutoPlayerAlgorithm.h"
#include "MyPoint.h"
#include <random>



AutoPlayerAlgorithm::AutoPlayerAlgorithm(UINT rows, UINT cols, UINT R, UINT P, UINT S, UINT B, UINT J, UINT F)
{
	std::random_device				seed;			//Will be used to obtain a seed for the random number engine
	std::mt19937					gen(seed());	//Standard mersenne_twister_engine seeded with seed()
	std::uniform_int_distribution<> dis(0,1);		//Distributed random
	
	scenario = new PositioningScenario(dis(gen), dis(gen));

	dprint("Randomized scenario: areFlagsOnCoreners = %d, areMovingOnCorners = %d\n", 
		scenario->areFlagsOnCoreners, scenario->areMovingOnCorners);

	boardCols = cols;
	boardRows = rows;

	initPieceCnt.R = R;
	initPieceCnt.P = P;
	initPieceCnt.S = S;
	initPieceCnt.B = B;
	initPieceCnt.J = J;
	initPieceCnt.F = F;

}


AutoPlayerAlgorithm::~AutoPlayerAlgorithm()
{
	delete scenario;
}

void AutoPlayerAlgorithm::getInitialPositions(int player, std::vector<unique_ptr<PiecePosition>>& vectorToFill)
{
	std::random_device				seed;			
	std::mt19937					gen(seed());	
	std::uniform_int_distribution<> pina(0, 3);	

	//take the flag and place randomly on board
	//on 4 places around it try to put jokers as bomb and real bombs around flag
	//all the rest RPS put randomly on board
	//randomize pina and randomize if pina
	//randomize scenario

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

#include "FilePlayerAlgorithm.h"
#include "MyMove.h"
#include "MyPoint.h"
#include "MyJokerChange.h"

using namespace std;


FilePlayerAlgorithm::FilePlayerAlgorithm(PlayerFileContext & fileContext): fileContext(fileContext)
{
	hasMoreMoves = true;
}

FilePlayerAlgorithm::~FilePlayerAlgorithm()
{
}

void FilePlayerAlgorithm::getInitialPositions(int player, std::vector<unique_ptr<PiecePosition>>& vectorToFill) 
{
}

unique_ptr<Move> FilePlayerAlgorithm::getMove()
{
	UINT fromX, fromY, toX, toY, jokerX, jokerY;
	bool isJoker;
	ePieceType newRep;

	eFileStatus status = fileContext.getNextMove(&fromX, &fromY, &toX, &toY, &isJoker, &jokerX, &jokerY, &newRep);
	switch (status)
	{
	case FILE_SUCCESS:
		hasMoreMoves = true;
		break;
	case FILE_EOF_REACHED:
		//hasMoreMoves = false? not done in ex.1
	case FILE_ERROR:
	case FILE_BAD_FORMAT:
		//hasLost = 1; 
		//reason = BAD_MOVES_INPUT_FILE; 
	default:
	    return make_unique<MyMove>(0,0,0,0); //invalid move will represent an invalid move
		//can simply return nullptr
	}
	if (isJoker)
		//nextJokerChange = make_unique<MyJokerChange>(jokerX, jokerY, newRep);
	//else
		//nextJokerChange = nullptr;

	//instead can be done by holding a uniqe_ptr<MyMove> as field in class, and using reset to update it
	return make_unique<MyMove>(fromX, fromY, toX, toY);
}

void FilePlayerAlgorithm::notifyOnInitialBoard(const Board & b, const std::vector<unique_ptr<FightInfo>>& fights) {/*Ignore call*/ }

void FilePlayerAlgorithm::notifyOnOpponentMove(const Move & move) {/*Ignore call*/ }

void FilePlayerAlgorithm::notifyFightResult(const FightInfo & fightInfo) {/*Ignore call*/ }


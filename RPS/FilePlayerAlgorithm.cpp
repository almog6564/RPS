#include "FilePlayerAlgorithm.h"
#include "MyMove.h"
#include "MyPoint.h"
#include "MyJokerChange.h"

using namespace std;


FilePlayerAlgorithm::FilePlayerAlgorithm(PlayerFileContext & fileContext): fileContext(fileContext)
{
	hasMoreMoves = true;
	//nextJokerChange = make_unique<MyJokerChange>(0, 0, 'R');
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
		break;
	case FILE_EOF_REACHED:
		hasMoreMoves = false;
	case FILE_ERROR:
	case FILE_BAD_FORMAT:
		//hasLost = 1; 
		//reason = BAD_MOVES_INPUT_FILE; 
	default:
		return nullptr;
	}
	if (isJoker)
		nextJokerChange = make_unique<MyJokerChange>(jokerX, jokerY, newRep);
	else
		nextJokerChange = nullptr;

	return make_unique<MyMove>(fromX, fromY, toX, toY);
}

void FilePlayerAlgorithm::notifyOnInitialBoard(const Board & b, const std::vector<unique_ptr<FightInfo>>& fights) {/*Ignore call*/ }

void FilePlayerAlgorithm::notifyOnOpponentMove(const Move & move) {/*Ignore call*/ }

void FilePlayerAlgorithm::notifyFightResult(const FightInfo & fightInfo) {/*Ignore call*/ }


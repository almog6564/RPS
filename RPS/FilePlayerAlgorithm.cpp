#include "FilePlayerAlgorithm.h"



FilePlayerAlgorithm::FilePlayerAlgorithm(PlayerFileContext & fileContext): fileContext(fileContext)
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
	/*bool Player::getNextMove(UINT * fromX, UINT * fromY, UINT * toX, UINT * toY, bool* isJoker,
		UINT * jokerX, UINT * jokerY, ePieceType* newRep)
	{
		eFileStatus status = fileContext.getNextMove(fromX, fromY, toX, toY, isJoker, jokerX, jokerY, newRep);
		switch (status)
		{
		case FILE_SUCCESS:
			hasMoreMoves = true;
			return true;
		case FILE_EOF_REACHED:
		case FILE_ERROR:
			return false;
		case FILE_BAD_FORMAT:
			hasLost = 1;
			reason = BAD_MOVES_INPUT_FILE;
			return false;
		default:
			return false;
		}
	}*/
	return unique_ptr<Move>();
}

unique_ptr<JokerChange> FilePlayerAlgorithm::getJokerChange()
{
	return unique_ptr<JokerChange>();
}

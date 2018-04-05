#include "player.h"

using namespace std;

Player::Player(UINT R, UINT P, UINT S, UINT B, UINT J, UINT F, PlayerFileContext* fileContext) 
	: R(R), P(P), S(S), B(B), J(J), F(F), fileContext(fileContext)
{
	pieceCounters[ROCK] = 0; pieceCounters[SCISSORS] = 0; pieceCounters[PAPER] = 0;
	pieceCounters[BOMB] = 0; pieceCounters[JOKER] = 0; pieceCounters[FLAG] = 0;
	movingPiecesCtr = R + P + S + B + J;
	score = 0;
	hasMoreMoves = true;
	hasLost = false;
}

void Player::decCounter(ePieceType type)
{
	pieceCounters[type]--;
	switch (type)
	{
	case ROCK:
	case SCISSORS:
	case PAPER:
	case BOMB:
	case JOKER:
		movingPiecesCtr--;
	case FLAG:
		break;
	default:
		cout << "setCurrentType: invalid newType " << type << endl;
	}
}

bool Player::isAlive()
{
	if (pieceCounters[FLAG] == 0)
	{
		hasLost = 1;
		reason = FLAGS_CAPTURED;
		return false;
	}
	if (movingPiecesCtr == 0)
	{
		hasLost = 1;
		reason = PIECES_EATEN;
		return false;
	}
	if (hasLost)
		return false;
	return true;
}

bool Player::getNextMove(UINT * fromX, UINT * fromY, UINT * toX, UINT * toY, bool* isJoker,
						 UINT * jokerX, UINT * jokerY, ePieceType* newRep)
{
	eFileStatus status = fileContext->getNextMove(fromX, fromY, toX, toY, isJoker, jokerX, jokerY, newRep);
	switch (status)
	{
	case FILE_SUCCESS:
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
}

int Player::getNextPiece(ePieceType* type, UINT * toX, UINT * toY, ePieceType* jokerType)
{
	if (fileContext->getNextPiece(type, toX, toY, jokerType) != FILE_SUCCESS)
		return -1;
	return 0;
}

void Player::updateScore()
{
	score++;
}

bool Player::getHasMoreMoves()
{
	return hasMoreMoves;
}

void Player::setHasMoreMoves(bool val)
{
	hasMoreMoves = val;
}

int Player::updateTypeCount(ePieceType type)
{
	pieceCounters[type]++;
	if (pieceCounters[type] > getTypeMax(type))
	{
		hasLost = 1;
		reason = BAD_POSITIONING_INPUT_FILE_PIECE_NUMBER;
		return -1;
	}
	return 0;
}

PlayerFileContext * Player::getPlayerFileContext()
{
	return fileContext;
}

void Player::validatePlayerPositions(bool** tmpBoard)
{
	UINT x, y;
	ePieceType type, jokerType;
	eFileStatus status;

	while (true)
	{
		status = fileContext->getNextPiece(&type, &x, &y, &jokerType);

		switch (status)
		{
		case FILE_EOF_REACHED:
			return;

		case FILE_ERROR:
		case FILE_BAD_FORMAT:
			setHasLost();
			setReason(BAD_POSITIONING_INPUT_FILE_FORMAT);
			break;

		case FILE_SUCCESS:
			if (tmpBoard[y][x])
			{
				setHasLost();
				setReason(BAD_POSITIONING_INPUT_FILE_DOUBLE_POSITION);
			}
			else
			{
				tmpBoard[y][x] = true;
			}
			break;
		}
	}
}



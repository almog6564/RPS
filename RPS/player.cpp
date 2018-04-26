#include "player.h"

using namespace std;

Player::Player(UINT ID, UINT R, UINT P, UINT S, UINT B, UINT J, UINT F, PlayerFileContext* fileContext, bool automaticPlayer) 
	: ID(ID), R(R), P(P), S(S), B(B), J(J), F(F), fileContext(fileContext)
{
	algorithm = automaticPlayer ? new AutoPlayerAlgorithm() : new FilePlayerAlgorithm();
		

		
	pieceCounters[ROCK] = 0; pieceCounters[SCISSORS] = 0; pieceCounters[PAPER] = 0;
	pieceCounters[BOMB] = 0; pieceCounters[JOKER] = 0; pieceCounters[FLAG] = 0;
	movingPiecesCtr = 0;
	originalFlagsCnt = 0;
	score = 0;
	hasMoreMoves = true;
	hasLost = false;
}

bool Player::isAlive()
{
	if (hasLost || pieceCounters[FLAG] == 0 || movingPiecesCtr == 0)
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
}

int Player::getNextPiece(ePieceType* type, UINT * toX, UINT * toY, ePieceType* jokerType)
{
	if (fileContext->getNextPiece(type, toX, toY, jokerType) != FILE_SUCCESS)
		return -1;
	return 0;
}

void Player::updateScore()	//for future use
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

void Player::decTypeCounter(ePieceType type, ePieceType originalType /* = UNDEF */, bool updateOnlyMovingCounter /* = false */)
{
	if (!updateOnlyMovingCounter)
	{
		if (originalType == JOKER) 
		{
			if(pieceCounters[originalType] > 0)
				pieceCounters[originalType]--;
		}
		else
		{
			//this check is so the Piece interface wouldn't "know" there's "original" and
			// "not original" types, only type
			if(pieceCounters[type] > 0)
				pieceCounters[type]--;
		}
	}

	switch (type)
	{
	case ROCK:
	case SCISSORS:
	case PAPER:
		movingPiecesCtr--;

	case JOKER:
	case BOMB:
	case FLAG:
		break;

	default:
		cout << "decCounter: invalid type " << type << endl;
	}

	if (movingPiecesCtr == 0)
	{
		hasLost = 1;
		reason = PIECES_EATEN;
	}
}


int Player::incTypeCount(ePieceType type, ePieceType originalType, bool updateOnlyMovingCounter /* = false */)
{
	if(!updateOnlyMovingCounter)
		pieceCounters[originalType]++;

	if (type != BOMB && type != FLAG && type != UNDEF)
	{
		movingPiecesCtr++;
		
		if (originalType == JOKER && (type == ROCK || type == SCISSORS || type == PAPER)
			&& hasLost && reason == PIECES_EATEN)
		{
			/* We are now changing type from moving piece to non-moving piece.
			this is to prevent race condition on which joker type changes after a player "loses",
			but the joker changes type to moving type and saves the player from dying */

			hasLost = 0;	//we need to revert the loss
			reason = INVALID_REASON;
		}
	}

	if (type == FLAG)
		incOriginalFlagCount();

	if (pieceCounters[originalType] > getTypeMax(type))
	{
		hasLost = 1;
		reason = BAD_POSITIONING_INPUT_FILE_PIECE_NUMBER;
		printf("\n** ERROR: Player #%d piece %c count is <%d> bigger than MAX <%d>\n",
			ID+1, pieceToChar(originalType), pieceCounters[originalType], getTypeMax(type));
		return -1;
	}
	return 0;
}

PlayerFileContext * Player::getPlayerFileContext()
{
	return fileContext;
}

void Player::validatePlayerPositions(bool** tmpBoard, UINT rows, UINT cols)
{
	UINT x, y;
	ePieceType type, jokerType;
	eFileStatus status;

	while (true)
	{
		status = fileContext->getNextPiece(&type, &x, &y, &jokerType);

		if (x > cols || y > rows || x < 1 || y < 1)
		{
			printf("[Player::validatePlayerPositions] player #%d dimensions check failed: "
				"x=%d, y=%d WHILE: cols=%d and rows=%d\n",
				ID+1, x, y, cols, rows);

			status = FILE_BAD_FORMAT;
		}


		switch (status)
		{
		case FILE_EOF_REACHED:
			return;

		case FILE_ERROR:
		case FILE_BAD_FORMAT:
			setHasLost();
			setReason(BAD_POSITIONING_INPUT_FILE_FORMAT);
			return;

		case FILE_SUCCESS:
			if (tmpBoard[y-1][x-1])
			{
				printf("ERROR [player #%d] - while checking position for piece %c at (%d,%d), position is already occupied\n",
					ID+1, pieceToChar(type), x, y);
				setHasLost();
				setReason(BAD_POSITIONING_INPUT_FILE_DOUBLE_POSITION);
			}
			else
			{
				dprint("got piece: %c %d %d joker: %c\n", pieceToChar(type), x, y, pieceToChar(jokerType));
				tmpBoard[y-1][x-1] = true;
			}
			break;
		}
	}
}



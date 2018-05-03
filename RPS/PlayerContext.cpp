#include "PlayerContext.h"

using namespace std;

PlayerContext::PlayerContext(UINT ID, UINT R, UINT P, UINT S, UINT B, UINT J, UINT F, PlayerFileContext* fileContext /*=NULL*/, bool autoPlayer /*=true*/) 
	: ID(ID), R(R), P(P), S(S), B(B), J(J), F(F), fileContext(fileContext)
{

	pieceCounters[ROCK] = 0; pieceCounters[SCISSORS] = 0; pieceCounters[PAPER] = 0;
	pieceCounters[BOMB] = 0; pieceCounters[JOKER] = 0; pieceCounters[FLAG] = 0;
	movingPiecesCtr = 0;
	originalFlagsCnt = 0;
	score = 0;
	hasLost = false;
	hasMoreMoves = true;
	autoPlayer = autoPlayer;
}

bool PlayerContext::isAlive()
{
	if (hasLost || pieceCounters[FLAG] == 0 || movingPiecesCtr == 0)
		return false;

	return true;
}

bool PlayerContext::getHasMoreMoves()
{
	return hasMoreMoves;
}

void PlayerContext::setHasMoreMoves(bool val)
{
	hasMoreMoves = val;
}

void PlayerContext::decTypeCounter(ePieceType type, ePieceType originalType /* = UNDEF */, bool updateOnlyMovingCounter /* = false */)
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


int PlayerContext::incTypeCount(ePieceType type, ePieceType originalType, bool updateOnlyMovingCounter /* = false */)
{
	if (!updateOnlyMovingCounter) 
	{
		pieceCounters[originalType]++;
		//dprint("\t###\t incTypeCount: Player %d ||  type %c || originalType %c || CNT = %d\n", 
		//	ID+1, pieceToChar(type), pieceToChar(originalType), pieceCounters[originalType]);
	}

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

PlayerFileContext* PlayerContext::getPlayerFileContext()
{
	return fileContext;
}





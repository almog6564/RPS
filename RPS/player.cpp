#include "player.h"

using namespace std;

Player::Player(UINT R, UINT P, UINT S, UINT B, UINT J, UINT F) : R(R), P(P), S(S), B(B), J(J), F(F)
{
	pieceCounters[ROCK] = 0; pieceCounters[SCISSORS] = 0; pieceCounters[PAPER] = 0;
	pieceCounters[BOMB] = 0; pieceCounters[JOKER] = 0; pieceCounters[FLAG] = 0;
	movingPiecesCtr = R + P + S + B + J;
	score = 0;
	hasMoreMoves = true;
	hasLost = 0;
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
	if (pieceCounters[FLAG] > 0 && movingPiecesCtr > 0 && !hasLost) 
		return true;
	return false;
}

bool Player::getNextMove(UINT * fromX, UINT * fromY, UINT * toX, UINT * toY)
{
	// if next move exists in file, update pointers and return true, else return false
	return true;
}

bool Player::getNextPosition(ePieceType* type, UINT * toX, UINT * toY)
{
	//update pointers
	return true;
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


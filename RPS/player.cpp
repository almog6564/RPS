#include "player.h"

using namespace std;

Player::Player(UINT R, UINT P, UINT S, UINT B, UINT J, UINT F)
{
	pieceCounters[ROCK] = R; pieceCounters[SCISSORS] = S; pieceCounters[PAPER] = P;
	pieceCounters[BOMB] = B; pieceCounters[JOKER] = J; pieceCounters[FLAG] = F;
	movingPiecesCtr = R + P + S + B + J;
	score = 0;
	hasMoreMoves = true;
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
	if (pieceCounters[FLAG] > 0 && movingPiecesCtr > 0) 
		return true;
	return false;
}

bool Player::getNextMove(int * fromX, int * fromY, int * toX, int * toY)
{
	// if next move exists in file, update pointers and return true, else return false
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

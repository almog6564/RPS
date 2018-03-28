
#ifndef _DEFS_
#define _DEFS_

typedef unsigned int UINT;

typedef enum _pieceType
{
	UNDEF,
	ROCK,
	PAPER,
	SCISSORS,
	BOMB,
	JOKER,
	FLAG,

	PIECE_COUNT

} ePieceType;

typedef enum _score
{
	WIN,
	LOSE,
	TIE,
	ERROR

} eScore;





#endif //_DEFS_


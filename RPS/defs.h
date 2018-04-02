
#ifndef _DEFS_
#define _DEFS_

typedef unsigned int UINT;

typedef enum _ePieceType
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

typedef enum _eScore
{
	WIN,
	LOSE,
	TIE,
	ERROR
} eScore;

typedef enum  _reason
{
	OPPONENT_FLAGS_CAPTURED,
	OPPONENT_PIECES_EATEN,
	MOVES_INPUT_FILES_DONE,
	ALL_FLAGS_EATEN_DURING_POSITIONING,
	BAD_POSITIONING_INPUT_FILE,
	BOTH_BAD_POSITIONING_INPUT_FILE,
	BAD_MOVES_INPUT_FILE
} eReason;

/*
typedef struct _move
{
	UINT fromX;
	UINT fromY;
	UINT toX;
	UINT toY;
} sMove;
*/



#endif //_DEFS_


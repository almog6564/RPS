
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

#define MAKE_REASON(r,n)					r = n

#define BAD_POS_SINGLE_REASON_MASK			(1 << 10)
#define IS_BAD_POS_SINGLE_REASON(r)			r && BAD_POS_SINGLE_REASON_MASK
#define MAKE_BAD_POS_SINGLE_REASON(r,n)		r = ((n) | BAD_POS_SINGLE_REASON_MASK)

#define BAD_POS_DOUBLE_REASON_MASK			(1 << 20)
#define IS_BAD_POS_DOUBLE_REASON(r)			(r && BAD_POS_DOUBLE_REASON_MASK)
#define MAKE_BAD_POS_DOUBLE_REASON(r,n)		r = ((n) | BAD_POS_DOUBLE_REASON_MASK)

typedef enum  _reason
{
	MAKE_REASON(FLAGS_CAPTURED, 0),
	MAKE_REASON(PIECES_EATEN, 1),
	MAKE_REASON(MOVES_INPUT_FILES_DONE, 2),
	MAKE_REASON(ALL_FLAGS_EATEN_DURING_POSITIONING, 3),
	MAKE_BAD_POS_SINGLE_REASON(BAD_POSITIONING_INPUT_FILE_FORMAT, 4),
	MAKE_BAD_POS_SINGLE_REASON(BAD_POSITIONING_INPUT_FILE_DOUBLE_POSITION, 5),
	MAKE_BAD_POS_SINGLE_REASON(BAD_POSITIONING_INPUT_FILE_PIECE_NUMBER, 6),
	MAKE_BAD_POS_SINGLE_REASON(BAD_POSITIONING_INPUT_FILE_FLAG_NUMBER, 7),
	MAKE_BAD_POS_DOUBLE_REASON(BOTH_BAD_POSITIONING_INPUT_FILE_FORMAT, 8),
	MAKE_BAD_POS_DOUBLE_REASON(BOTH_BAD_POSITIONING_INPUT_FILE_DOUBLE_POSITION, 9),
	MAKE_BAD_POS_DOUBLE_REASON(BOTH_BAD_POSITIONING_INPUT_FILE_PIECE_NUMBER, 10),
	MAKE_BAD_POS_DOUBLE_REASON(BOTH_BAD_POSITIONING_INPUT_FILE_FLAG_NUMBER, 11),
	MAKE_REASON(BAD_MOVES_INPUT_FILE, 12),

	REASON_COUNT
	
} eReason;


#endif //_DEFS_


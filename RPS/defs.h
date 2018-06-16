
#ifndef _DEFS_
#define _DEFS_

#include "cstdio"
#include <memory>
#include <iostream>

typedef unsigned int UINT;

/***** For debug printing ******/
#define DEBUG 0

#if DEBUG == 1
#define consume(x)						
#define dprint(...) printf(__VA_ARGS__)
#else
/* Some of the arguments exist only for debug printing, this trick "uses" the argument without actually using it,
	whilst disabling the warning of unused variable. */
#define consume(x)	std::ignore = (x)		
#define dprint(...) 
#endif
/******************************/

/***** For debug printing ******/
#define NEW_DEBUG 0

#if NEW_DEBUG == 1	
#define dprintreg(...) printf(__VA_ARGS__)
#else
#define dprintreg(...)
#endif
/******************************/

#define BOARD_SIZE		10
#define R_COUNT			2
#define P_COUNT			5
#define S_COUNT			1
#define B_COUNT			2
#define J_COUNT			2
#define F_COUNT			1

typedef enum _ePieceType
{
	UNDEF,
	ROCK,
	PAPER,
	SCISSORS,
	BOMB,
	JOKER,
	FLAG,

	PIECE_COUNT	//to declare array the size of enum 

} ePieceType;

typedef enum _eScore
{
	WIN,
	LOSE,
	TIE,
	ERROR
} eScore;

/* Those macros are to make easily a "double" reason to indicate the same loss reason for both players */
#define DOUBLE_REASON_MASK 0x80
#define MAKE_REASON_DOUBLE(r) (r | DOUBLE_REASON_MASK)

typedef enum  _reason
{
	INVALID_REASON,

	FLAGS_CAPTURED,
	PIECES_EATEN,
	MOVES_INPUT_FILES_DONE,
	ALL_FLAGS_EATEN_DURING_POSITIONING,
	BAD_POSITIONING_INPUT_FILE_FORMAT,
	BAD_POSITIONING_INPUT_FILE_DOUBLE_POSITION,
	BAD_POSITIONING_INPUT_FILE_PIECE_NUMBER,
	BAD_POSITIONING_INPUT_FILE_FLAG_NUMBER,
	BAD_MOVES_INPUT_FILE,

	BOTH_PIECES_EATEN								= MAKE_REASON_DOUBLE(PIECES_EATEN),

	BOTH_BAD_POSITIONING_INPUT_FILE_FORMAT			= MAKE_REASON_DOUBLE(BAD_POSITIONING_INPUT_FILE_FORMAT),
	BOTH_BAD_POSITIONING_INPUT_FILE_DOUBLE_POSITION = MAKE_REASON_DOUBLE(BAD_POSITIONING_INPUT_FILE_DOUBLE_POSITION),
	BOTH_BAD_POSITIONING_INPUT_FILE_PIECE_NUMBER	= MAKE_REASON_DOUBLE(BAD_POSITIONING_INPUT_FILE_PIECE_NUMBER),
	BOTH_BAD_POSITIONING_INPUT_FILE_FLAG_NUMBER		= MAKE_REASON_DOUBLE(BAD_POSITIONING_INPUT_FILE_FLAG_NUMBER),
	
	MORE_THAN_100_MOVES,

	BOTH_LOST_DIFFERENT_REASONS

} eReason;


typedef enum _eFileStatus
{
	FILE_SUCCESS,
	FILE_ERROR,
	FILE_EOF_REACHED,
	FILE_BAD_FORMAT

} eFileStatus;

typedef enum _eGameMode
{
	FILE_VS_FILE,
	FILE_VS_AUTO,
	AUTO_VS_FILE,
	AUTO_VS_AUTO

} eGameMode;

#endif //_DEFS_


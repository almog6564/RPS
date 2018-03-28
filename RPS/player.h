#ifndef _PLAYER_
#define _PLAYER_

#include "defs.h"


class Player
{
	UINT pieceCounters[PIECE_COUNT];
	UINT globalCounter;

public:

	Player(UINT R, UINT P, UINT S, UINT B, UINT J, UINT F);

	void decCounter(ePieceType type);
	UINT getGlobalCounter()const;

};







#endif //_PLAYER_

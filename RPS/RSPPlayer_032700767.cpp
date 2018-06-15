
#include "AutoPlayerAlgorithm.h"
#include "AlgorithmRegistration.h"

class RSPPlayer_032700767 : public AutoPlayerAlgorithm
{
public:
	RSPPlayer_032700767();
};


RSPPlayer_032700767::RSPPlayer_032700767() :
	AutoPlayerAlgorithm(BOARD_SIZE, BOARD_SIZE, R_COUNT, P_COUNT, S_COUNT, B_COUNT, J_COUNT, F_COUNT, 0)
{
	//dprintreg("Created a new RSPPlayer_032700767\n");
}

REGISTER_ALGORITHM(032700767)
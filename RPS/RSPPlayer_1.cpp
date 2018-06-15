
#include "AutoPlayerAlgorithm.h"
#include "AlgorithmRegistration.h"

class RSPPlayer_1 : public AutoPlayerAlgorithm
{
public:
	RSPPlayer_1();
};


RSPPlayer_1::RSPPlayer_1() :
	AutoPlayerAlgorithm(BOARD_SIZE, BOARD_SIZE, R_COUNT, P_COUNT, S_COUNT, B_COUNT, J_COUNT, F_COUNT, 0)
{
	//dprintreg("Created a new RSPPlayer_1\n");
}

REGISTER_ALGORITHM(1)
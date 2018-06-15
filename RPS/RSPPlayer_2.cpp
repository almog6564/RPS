
#include "AutoPlayerAlgorithm.h"
#include "AlgorithmRegistration.h"

class RSPPlayer_2 : public AutoPlayerAlgorithm
{
public:
	RSPPlayer_2();
};


RSPPlayer_2::RSPPlayer_2() :
	AutoPlayerAlgorithm(BOARD_SIZE, BOARD_SIZE, R_COUNT, P_COUNT, S_COUNT, B_COUNT, J_COUNT, F_COUNT, 0)
{
	//dprintreg("Created a new RSPPlayer_2\n");
}

REGISTER_ALGORITHM(2)
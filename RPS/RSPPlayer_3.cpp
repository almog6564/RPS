
#include "AutoPlayerAlgorithm.h"
#include "AlgorithmRegistration.h"

class RSPPlayer_3 : public AutoPlayerAlgorithm
{
public:
	RSPPlayer_3();
};


RSPPlayer_3::RSPPlayer_3() :
	AutoPlayerAlgorithm(BOARD_SIZE, BOARD_SIZE, R_COUNT, P_COUNT, S_COUNT, B_COUNT, J_COUNT, F_COUNT, 0)
{
	//dprintreg("Created a new RSPPlayer_3\n");
}

REGISTER_ALGORITHM(3)
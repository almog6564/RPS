
#include "AutoPlayerAlgorithm.h"
#include "AlgorithmRegistration.h"

class RSPPlayer_4 : public AutoPlayerAlgorithm
{
public:
	RSPPlayer_4();
};


RSPPlayer_4::RSPPlayer_4() :
	AutoPlayerAlgorithm(BOARD_SIZE, BOARD_SIZE, R_COUNT, P_COUNT, S_COUNT, B_COUNT, J_COUNT, F_COUNT, 0)
{
//	dprintreg("Created a new RSPPlayer_4\n");
}

REGISTER_ALGORITHM(4)
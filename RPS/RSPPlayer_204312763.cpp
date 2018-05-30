
#include "AutoPlayerAlgorithm.h"
#include "AlgorithmRegistration.h"

class RSPPlayer_204312763 : public AutoPlayerAlgorithm
{
public:
	RSPPlayer_204312763();
};


RSPPlayer_204312763::RSPPlayer_204312763() :
	AutoPlayerAlgorithm(BOARD_SIZE, BOARD_SIZE, R_COUNT, P_COUNT, S_COUNT, B_COUNT, J_COUNT, F_COUNT, 0)
{
	dprintreg("Created a new RSPPlayer_204312763\n");
}

REGISTER_ALGORITHM(204312763)
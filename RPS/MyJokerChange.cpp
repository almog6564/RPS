#include "MyJokerChange.h"

MyJokerChange::MyJokerChange(UINT fromX, UINT fromY, const char jokerNewRep) : jokerNewRep(jokerNewRep)
{
	jokerChangePosition = std::make_unique<MyPoint>(fromX, fromY);
}

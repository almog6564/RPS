#include "MyJokerChange.h"

MyJokerChange::MyJokerChange(UINT fromX, UINT fromY, char jokerNewRep) : jokerNewRep(jokerNewRep)
{
	jokerChangePosition = new MyPoint(fromX, fromY);
}

MyJokerChange::~MyJokerChange()
{
	delete jokerChangePosition;
}
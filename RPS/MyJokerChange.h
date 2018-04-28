#ifndef __MY_JOKER_CHANGE_H_
#define __MY_JOKER_CHANGE_H_

#include "JokerChange.h"
#include "defs.h"
#include "MyPoint.h"

class MyJokerChange : public JokerChange
{
	Point* jokerChangePosition;
	char jokerNewRep;// R, P, S or B (but NOT J and NOT F)
public:
	MyJokerChange(UINT fromX, UINT fromY, char jokerNewRep);
	~MyJokerChange();

	const Point& getJokerChangePosition() const
	{
		return *jokerChangePosition;
	}

	char getJokerNewRep() const
	{
		return jokerNewRep;
	}
};

#endif


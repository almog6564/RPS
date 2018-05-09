#ifndef __MY_JOKER_CHANGE_H_
#define __MY_JOKER_CHANGE_H_

#include "JokerChange.h"
#include "defs.h"
#include "MyPoint.h"

class MyJokerChange : public JokerChange
{
	std::unique_ptr<Point> jokerChangePosition;
	char jokerNewRep;// R, P, S or B (but NOT J and NOT F)
public:
	MyJokerChange(UINT fromX, UINT fromY, char jokerNewRep);

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


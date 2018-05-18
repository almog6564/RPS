#ifndef _MY_MOVE_H_
#define _MY_MOVE_H_

#include "Move.h"
#include "defs.h"
#include "MyPoint.h"

class MyMove :public Move
{
	const MyPoint from;
	const MyPoint to;
public:
	MyMove(const UINT fromX, const UINT fromY, const UINT toX, const UINT toY);

	const Point& getFrom() const
	{
		return from;
	}
	const Point& getTo() const
	{
		return to;
	}
};

#endif

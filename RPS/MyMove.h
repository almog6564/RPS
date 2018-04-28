#ifndef _MY_MOVE_H_
#define _MY_MOVE_H_

#include "Move.h"
#include "defs.h"
#include "MyPoint.h"

class MyMove :public Move
{
	Point* from;
	Point* to;
public:
	MyMove(UINT fromX, UINT fromY, UINT toX, UINT toY);
	~MyMove();

	const Point& getFrom() const
	{
		return *from;
	}
	const Point& getTo() const
	{
		return *to;
	}
};

#endif

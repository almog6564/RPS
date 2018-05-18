#include "MyMove.h"

MyMove::MyMove(const UINT fromX, const UINT fromY, const UINT toX, const UINT toY) :
	from(MyPoint(fromX, fromY)), to(MyPoint(toX, toY)) {}
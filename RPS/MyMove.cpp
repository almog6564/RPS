#include "MyMove.h"

MyMove::MyMove(UINT fromX, UINT fromY, UINT toX, UINT toY)
{
	from = new MyPoint(fromX, fromY); //make sure the fields are in the right order
	to = new MyPoint(toX, toY);
}

MyMove::~MyMove()
{
	delete from;
	delete to;
}

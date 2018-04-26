#pragma once
#include "Point.h"

class MyPoint : public Point
{
	int col;
	int row;
	
public:
	MyPoint(int col, int row) : col(col), row(row) {};

	int getX() const override
	{
		return col;
	}

	int getY() const override
	{
		return row;
	}

	void setNewPosition(int colArg, int rowArg)
	{
		col = colArg;
		row = rowArg;
	}

	~MyPoint() {}
};


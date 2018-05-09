
#ifndef _MY_POINT_
#define _MY_POINT_

#include "Point.h"

class MyPoint : public Point
{
	int col;
	int row;
	
public:
	MyPoint(int col, int row) : col(col), row(row) {};

	MyPoint() {} //empty c'tor

	MyPoint& operator=(const Point& other) 
	{
		col = other.getX();
		row = other.getY();
		return *this;
	}

	MyPoint(const Point& other) //copy c'tor
	{
		col = other.getX();
		row = other.getY();
	}

	MyPoint(const MyPoint& other) //copy c'tor
	{
		col = other.getX();
		row = other.getY();
	}

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

	MyPoint(MyPoint&&) = default; //default move constructor
};

#endif


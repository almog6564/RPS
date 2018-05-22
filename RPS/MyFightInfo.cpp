
#include "MyFightInfo.h"

MyFightInfo::MyFightInfo(UINT x, UINT y, char p1piece, char p2piece, int winner) :
	position(make_unique<MyPoint>(x, y)), p1piece(p1piece), p2piece(p2piece), winner(winner) {}

MyFightInfo::MyFightInfo()
{
	position.reset(nullptr);
}
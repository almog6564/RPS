
#include "AutoPlayerAlgorithm.h"
#include <random>
#include <algorithm>


//assumes at least one moving piece exists
unique_ptr<Move> AutoPlayerAlgorithm::getNextRandomMove(void)
{
	random_device				seed;
	mt19937						gen(seed());
	uniform_int_distribution<>	rand_gen(0, (int)playerPieces.size() - 1);
	int rand;
	vector<bool> checkList(playerPieces.size(), false);

	do
	{
		rand = rand_gen(gen);
		//cout << rand << endl;
		int i = 0;
		for (auto it = playerPieces.begin();; i++, it++)
		{
			if (i == rand)
			{
				if (!checkList[i] && it->isMoving())
				{
					MyPoint point = (*it).getPosition();
					unique_ptr<MyMove> nextMove = move(getLegalMove(point));
					if (nextMove)
						return nextMove;
				}
				checkList[i] = true;
				break;
			}
		}
		if (all_of(checkList.begin(), checkList.end(), [](bool i) {return i;})) //check if all pieces were checked yet
		{
			dprint("Did not find a legal moving piece");
			return nullptr; //invalid Move
		}
	} while (true);
}

unique_ptr<Move> AutoPlayerAlgorithm::checkAllAdjecentOpponents(const MyPiecePosition & piece, std::bitset<4>& boolVec, const int x, const int y)
{
	if (checkForAdjecentOpponent(MyPiecePosition(x - 1, y)))
	{
		if (piece >= MyPiecePosition(x - 1, y)) //potential win
			return make_unique<MyMove>(x, y, x - 1, y);
		else //potential lose
			boolVec[0] = false;
	}

	if (checkForAdjecentOpponent(MyPiecePosition(x + 1, y)))
	{
		if (piece >= MyPiecePosition(x + 1, y)) //potential win
			return make_unique<MyMove>(x, y, x + 1, y);
		else
			boolVec[1] = false;
	}
	if (checkForAdjecentOpponent(MyPiecePosition(x, y + 1)))
	{
		if (piece >= MyPiecePosition(x, y + 1)) //potential win
			return make_unique<MyMove>(x, y, x, y + 1);
		else
			boolVec[2] = false;
	}
	if (checkForAdjecentOpponent(MyPiecePosition(x, y - 1)))
	{
		if (piece >= MyPiecePosition(x, y - 1)) //potential win
			return make_unique<MyMove>(x, y, x, y - 1);
		else
			boolVec[3] = false;
	}
	return nullptr;
}

char AutoPlayerAlgorithm::getNewJokerRep(const char oldJokerRep)
{
	char result;
	switch (oldJokerRep)
	{
	case 'R':
		result = 'S';
		break;
	case 'S':
		result = 'P';
		break;
	case 'P':
		result = 'R';
		break;
	default:
		result = '#';
	}
	return result;
}

unique_ptr<JokerChange> AutoPlayerAlgorithm::checkAllAdjecentOpponents(const MyPiecePosition & joker, const int x, const int y)
{
	bool foundAdjecentOpponent = false;
	do
	{
		if (checkForAdjecentOpponent(MyPiecePosition(x - 1, y)))
		{
			if (joker < MyPiecePosition(x - 1, y)) //potential lose
			{
				foundAdjecentOpponent = true;
				break;
			}
		}
		if (checkForAdjecentOpponent(MyPiecePosition(x + 1, y)))
		{
			if (joker < MyPiecePosition(x + 1, y)) //potential lose
			{
				foundAdjecentOpponent = true;
				break;
			}
		}
		if (checkForAdjecentOpponent(MyPiecePosition(x, y + 1)))
		{
			if (joker < MyPiecePosition(x, y + 1)) //potential lose
			{
				foundAdjecentOpponent = true;
				break;
			}

		}
		if (checkForAdjecentOpponent(MyPiecePosition(x, y - 1))) //potential lose
		{
			if (joker < MyPiecePosition(x, y - 1)) //potential win
			{
				foundAdjecentOpponent = true;
				break;
			}
		}
	} while (false);

	if (foundAdjecentOpponent)
		return make_unique<MyJokerChange>(joker.getPosition().getX(), joker.getPosition().getY(), getNewJokerRep(joker.getJokerRep()));

	return nullptr;
}

//pos will be received as a lvalue reference, and other as rvalue reference
bool AutoPlayerAlgorithm::checkForAdjecentOpponent(const MyPiecePosition other)
{
	if (opponentsPieces.find(other) != opponentsPieces.end())
		return true;
	return false;
}

unique_ptr<Move> AutoPlayerAlgorithm::getMove()
{
	unique_ptr<Move> nextMove;
	std::bitset<4> fleeArr(0xF); //[left, right, up, down]
	bool foundMove = false;

	/*look for potential win or flee*/
	for (auto& piece : playerPieces)
	{
		if (!(piece.isMoving()))
			continue;

		const MyPoint point = piece.getPosition();
		int x = point.getX(), y = point.getY();
		nextMove = move(checkAllAdjecentOpponents(piece, fleeArr, x, y));

		if (nextMove)
		{
			foundMove = true;
			break;
		}
		else if (fleeArr.count() < 4)
		{
			//if not all are true, it means that there is an opponent piece stronger than me,
			//try to flee from it
			nextMove = move(getLegalMove(point, fleeArr));

			if (nextMove)
			{
				foundMove = true;
				break;
			}

		}
	}
	if (!foundMove)
	{
		/*perform random move*/
		nextMove = getNextRandomMove();
		if (!nextMove)
			return nullptr;
	}
		//update player's board
	auto& t = *playerPieces.find(MyPiecePosition(nextMove->getFrom().getX(), nextMove->getFrom().getY()));
	char type = t.getPiece();
	char jokerType = t.getJokerRep();
	playerPieces.erase(t);
	playerPieces.insert(MyPiecePosition(nextMove->getTo().getX(), nextMove->getTo().getY(), type, jokerType));

	return nextMove;
}


char AutoPlayerAlgorithm::getRandomJokerChahge(int rand, char jokerRep)
{
	switch (jokerRep)
	{
	case 'R':
		switch (rand)
		{
		case 0:
			return 'S';
		case 1:
			return 'P';
		case 2:
			return 'B';
		default:
			return '#';
		}
	case 'P':
		switch (rand)
		{
		case 0:
			return 'S';
		case 1:
			return 'R';
		case 2:
			return 'B';
		default:
			return '#';
		}
	case 'S':
		switch (rand)
		{
		case 0:
			return 'P';
		case 1:
			return 'R';
		case 2:
			return 'B';
		default:
			return '#';
		}
	case 'B':
		switch (rand)
		{
		case 0:
			return 'S';
		case 1:
			return 'R';
		case 2:
			return 'P';
		default:
			return '#';
		}
	default:
		return '#';
	}
}

unique_ptr<MyMove> AutoPlayerAlgorithm::getLegalMove(const MyPoint& point)
{
	std::bitset<4> legalDirections(0xF);

	return move(getLegalMove(point, legalDirections));

}

bool AutoPlayerAlgorithm::existsOnBoardSet(const MyPoint& point)
{
	return playerPieces.count(MyPiecePosition(point.getX(), point.getY())) > 0;
}

void AutoPlayerAlgorithm::removeOutOfBoundsDirections(const MyPoint& point, std::bitset<4>& legalFleeDirections)
{
	//[left, right, up, down]
	if (point.getX() == 1)
		legalFleeDirections[0] = false;

	else if (point.getX() == (int)boardCols)
		legalFleeDirections[1] = false;

	if (point.getY() == 1)
		legalFleeDirections[2] = false;

	else if (point.getY() == (int)boardRows)
		legalFleeDirections[3] = false;
}

const MyPoint getPointByDirection(const MyPoint& point, int direction)
{
	switch (direction)
	{
	case 0:
		return MyPoint(point.getX() - 1, point.getY()); //left

	case 1:
		return MyPoint(point.getX() + 1, point.getY()); //right

	case 2:
		return MyPoint(point.getX(), point.getY() - 1); //up

	case 3:
		return MyPoint(point.getX(), point.getY() + 1); //down
	}

	//should not get here

	printf("Error: Illegal Direction in getPointByDirection\n");

	return point;
}

unique_ptr<MyMove> AutoPlayerAlgorithm::getLegalMove(const MyPoint& point, bitset<4>& legalFleeDirections)
{
	vector<int> possibleMoves(0);
	int chosenDirection = 0;

	removeOutOfBoundsDirections(point, legalFleeDirections);

	/* Get all directions that does not include opponent piece*/
	for (int i = 0; i < 4; i++)
	{
		//if move is legal already and the to Point does not contain a piece of ours
		if (legalFleeDirections[i] && !existsOnBoardSet(getPointByDirection(point, i)))
		{
			possibleMoves.push_back(i);
			//dprint("\tPossible move: index %d\n", i);
		}
	}


	/* If there is no available direction than cannot find legal move which is not a fight */

	if (possibleMoves.size() == 0)
		return nullptr;


	if (possibleMoves.size() > 1)
	{
		random_device	seed;
		mt19937			gen(seed());
		uniform_int_distribution<> genDirection(0, (int)possibleMoves.size() - 1);
		chosenDirection = genDirection(gen);
	}
	//else it already is 0 and it will take the first and only element
	const MyPoint& chosenDirectionPoint = getPointByDirection(point, possibleMoves[chosenDirection]);
	return make_unique<MyMove>(point.getX(), point.getY(),
		chosenDirectionPoint.getX(), chosenDirectionPoint.getY());
}

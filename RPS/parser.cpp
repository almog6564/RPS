
#include "parser.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cerrno>
#include <cstring>

using namespace std;

PlayerFileContext::PlayerFileContext(string& pieceFileName, string& movesFileName)
{
	pieces = new FileContext(pieceFileName, true);
	moves = new FileContext(movesFileName, true);
}

FileParser::FileParser(string& p1PiecesFileName, string& p2PiecesFilename,
	string& p1MovesFileName, string& p2MovesFilename, string& outputFilename)
{
	p1 = new PlayerFileContext(p1PiecesFileName,p1MovesFileName);
	p2 = new PlayerFileContext(p2PiecesFilename, p2MovesFilename);
	output = new FileContext(outputFilename, false);
}

FileContext::~FileContext()
{
	if (file.is_open())
		file.close();
}

bool FileContext::openFile()
{
	bool ret = false;

	do
	{
		file.open(fileName, isInputFile ? (ios::in) : (ios::out | ios::trunc));

		if (file.is_open())
		{
			ret = true;
			break;
		}

		printf("Error while opening file: %s, %s\n", fileName.c_str(), strerror(errno));

	} while (false);

	return ret;
}

int FileParser::initializeFiles()
{
	//open files and check if file was opened successfully
	
	if (!p1->pieces->openFile())
		return -1;

	if (!p1->moves->openFile())
		return -1;

	if (!p2->pieces->openFile())
		return -1;

	if (!p2->moves->openFile())
		return -1;

	if (!output->openFile())
		return -1;
	
	return 0;
}


PlayerFileContext* FileParser::getPlayerFileContext(int playerNumber)
{
	if (playerNumber == 0)
		return p1;
	if (playerNumber == 1)
		return p2;

	return NULL;
}

eFileStatus PlayerFileContext::getNextPiece(ePieceType* type, UINT* x, UINT* y, ePieceType* jokerType)
{
	eFileStatus status = FILE_SUCCESS;
	UINT length, xTmp, yTmp;
	string line;
	stringstream ss;
	char typeTmp;

	do
	{
		if (!type || !x || !y)
		{
			cout << "Illegal arguments to getNextPiece" << endl;
			status = FILE_ERROR;
			break;
		}

		//While loop to skip empty lines

		while (true)
		{
			getline(pieces->file, line);
			length = (UINT)line.length();
			pieces->setCurrentLine(&line); //save the current line to print if there was error

			if (length > 0)
			{
				break;
			}
			else if (pieces->file.eof())
			{
				status = FILE_EOF_REACHED;
				break;
			}
		}

		if (status)
			break;


		if (line[0] == 'J')
		{
			/* put the relevant substring, without the J into a stream*/
			ss << line.substr(1, length - 1);

			/* parse the line, while "ws" ignores whitespace */
			ss >> ws >> xTmp >> ws >> yTmp >> ws >> typeTmp >> ws;

			/* if there was fail during one or other conversions
			or if there are anymore characters in the line after then its bad format */
			if (ss.fail() || ss.rdbuf()->in_avail() > 0)
			{
				status = FILE_BAD_FORMAT;
				break;
			}

			/* if we're here, no error - put relevant data into pointers*/
			*type = JOKER;
			*jokerType = charToPiece(typeTmp);
			*x = xTmp;
			*y = yTmp;

			if(*jokerType == UNDEF)
			{
				status = FILE_BAD_FORMAT;
			}
			break;
		}

		/* regular line, no joker*/
		ss << line;

		/* parse line */
		ss >> ws >> typeTmp >> ws >> xTmp >> ws >> yTmp >> ws;

		/* if there was fail during one or other conversions
		or if there are anymore characters in the line after then it's bad format */
		if (ss.fail() || ss.rdbuf()->in_avail() > 0)
		{
			status = FILE_BAD_FORMAT;
			break;
		}

		/* if we're here, no error - put relevant data into pointers*/
		*type = charToPiece(typeTmp);
		*x = xTmp;
		*y = yTmp;
		*jokerType = UNDEF; //joker type undefined

		if (*type == UNDEF || *type == JOKER)
		{
			status = FILE_BAD_FORMAT;
		}
		break;

	} while (false);

	return status;
}


eFileStatus PlayerFileContext::getNextMove(UINT* fromX, UINT* fromY, UINT* toX, UINT* toY,
											bool* isJoker, UINT* jokerX, UINT* jokerY, 
											ePieceType* newRep)
{
	eFileStatus status = FILE_SUCCESS;
	UINT fX, fY, tX, tY, jX, jY;
	string line;
	stringstream ss;
	char typeTmp;
	char chk[3];

	do
	{
		if (!fromX || !fromY || !toX || !toY || 
			!isJoker || !jokerX || !jokerY || !newRep)
		{
			cout << "Illegal arguments to getNextMove" << endl;
			status = FILE_ERROR;
			break;
		}

		//While loop to skip empty lines

		while (true)
		{
			getline(moves->file, line);

			moves->setCurrentLine(&line); //save the current line to print if there was error

			if (line.length() > 0)
			{
				break;
			}
			else if (moves->file.eof())
			{
				status = FILE_EOF_REACHED;
				break;
			}
		}

		if (status)
			break;


		/* put the line into the stream */
		ss << line;

		/* parse the line, while "ws" ignores whitespace */
		ss >> ws >> fX >> ws >> fY >> ws >> tX >> ws >> tY >> ws;

		/* if there was fail during one or other conversions then its bad format */
		if (ss.fail())
		{
			status = FILE_BAD_FORMAT;
			break;
		}

		/* if we're here, no error - put relevant data into pointers*/
		*fromX = fX;
		*fromY = fY;
		*toX = tX;
		*toY = tY;
		*isJoker = false;

		/* line ended, no Joker details */
		if (ss.rdbuf()->in_avail() == 0)
			break;

		/* not enough characters to represent the right format */
		streamsize chkSize = (sizeof("J: 0 0 N") - 1);

		if (ss.rdbuf()->in_avail() < chkSize) 
		{
			status = FILE_BAD_FORMAT;
			break;
		}

		/* check first 3 characters in line */
		ss.read(chk, 3);

		/* more characters on line */
		if (ss.fail() || chk[0] != 'J' || 
			chk[1] != ':' || !isspace(chk[2]))
		{
			/* if its not "J: " then bad format */
			status = FILE_BAD_FORMAT;
			break;
		}

		/* parse line */
		ss >> ws >> jX >> ws >> jY >> ws >> typeTmp >> ws;

		/* if there was fail during one or other conversions
		or if there are anymore characters in the line after then it's bad format */
		if (ss.fail() || ss.rdbuf()->in_avail() > 0)
		{
			status = FILE_BAD_FORMAT;
			break;
		}

		*jokerX = jX;
		*jokerY = jY;
		*newRep = charToPiece(typeTmp);
		*isJoker = true;
	} while (false);

	return status;
}

void PlayerFileContext::setPieceFileToStart()
{
	pieces->file.clear();
	pieces->file.seekg(0, ios::beg);
}

string GetReasonString(eReason reason, int arg0, int arg1)
{
	char temp[100];

	switch (reason)
	{
	case FLAGS_CAPTURED:
		return "All flags of the opponent are captured";
		
	case PIECES_EATEN:
		return "All moving PIECEs of the opponent are eaten";

	case MOVES_INPUT_FILES_DONE:
		return "A tie - both Moves input files done without a winner";

	case ALL_FLAGS_EATEN_DURING_POSITIONING:
		return "A tie - all flags are eaten by both players in the position files";

	case BAD_POSITIONING_INPUT_FILE_FORMAT:
	case BAD_POSITIONING_INPUT_FILE_DOUBLE_POSITION:
	case BAD_POSITIONING_INPUT_FILE_PIECE_NUMBER:
	case BAD_POSITIONING_INPUT_FILE_FLAG_NUMBER:
		sprintf(temp, "Bad Positioning input file for player %d - line %d", arg0, arg1);
		return (string)temp;

	case BOTH_BAD_POSITIONING_INPUT_FILE_FORMAT:
	case BOTH_BAD_POSITIONING_INPUT_FILE_DOUBLE_POSITION:
	case BOTH_BAD_POSITIONING_INPUT_FILE_PIECE_NUMBER:
	case BOTH_BAD_POSITIONING_INPUT_FILE_FLAG_NUMBER:
		sprintf(temp, "Bad Positioning input file for both players - player 1: line %d, player 2: line %d", arg0, arg1);
		return (string)temp;

	default: 
		return "Invalid eReason code";
	}
}


ePieceType charToPiece(char c)
{
	ePieceType p;

	switch (c)
	{
	case 'R':
		p = ROCK;
		break;

	case 'P':
		p = PAPER;
		break;

	case 'S':
		p = SCISSORS;
		break;

	case 'B':
		p = BOMB;
		break;

	case 'J':
		p = JOKER;
		break;

	case 'F':
		p = FLAG;
		break;

	default:
		p = UNDEF;
		break;
	}

	return p;
}

char pieceToChar(ePieceType p, bool isUpperCase)
{
	char c;

	switch (p)
	{
	case ROCK:
		c = isUpperCase ? 'R' : 'r';
		break;

	case PAPER:
		c = isUpperCase ? 'P' : 'p';
		break;

	case SCISSORS:
		c = isUpperCase ? 'S' : 's';
		break;

	case BOMB:
		c = isUpperCase ? 'B' : 'b';
		break;

	case JOKER:
		c = isUpperCase ? 'J' : 'j';
		break;

	case FLAG:
		c = isUpperCase ? 'F' : 'f';
		break;

	default:
		c = -1;
		break;
	}

	return c;
}


/* Unit Test */
/*
int main()
{
	char* p1p = "player1.rps_board";
	char* p1m = "player1.rps_moves";
	char* p2p = "player2.rps_board";
	char* p2m = "player2.rps_moves";
	ePieceType type, jokerType;
	UINT x, y, fromx, fromy, tox, toy, jokerx, jokery;
	bool isJoker;
	FileParser* f = new FileParser(p1p, p2p, p1m, p2m);
	eFileStatus status;

	char* names[] = { "UNDEF", "ROCK", "PAPER", "SCISSORS", "BOMB", "JOKER", "FLAG" };

	if (f->initializeFiles() < 0)
	{
		printf("Error file open\n");
		return -1;
	}

	while ((status = f->p1->getNextPiece(&type, &x, &y, &jokerType)) != FILE_EOF_REACHED)
	{
		if (status == FILE_SUCCESS)
		{
			if (type != JOKER)
				printf("Got piece: %s (%d,%d)\n", names[type], x, y);
			else
				printf("Got JOKER: %s (%d,%d)\n", names[jokerType], x, y);
		}
		else
		{
			printf("Bad line : %s\n", f->p1->pieces->getLastReadLine()->c_str());
		}
	}
	printf("\n\n");
	while ((status = f->p1->getNextMove(&fromx, &fromy, &tox, &toy, &isJoker,
		&jokerx, &jokery, &type)) != FILE_EOF_REACHED)
	{
		if (status == FILE_SUCCESS)
		{
			if (isJoker)
				printf("Got Joker: (%d,%d)->(%d,%d) [J: %d %d (%s)]\n", fromx, fromy, tox, toy,
					jokerx, jokery, names[type]);
			else
				printf("Got Move : (%d,%d)->(%d,%d)\n", fromx, fromy, tox, toy);
		}
		else
		{
			printf("Bad line : %s\n", f->p1->moves->getLastReadLine()->c_str());
		}
	}

	delete f;

	return 0;
}
*/
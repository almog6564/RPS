
#include "parser.h"
#include <sstream>
#include <cerrno>
#include <cstring>
#include <iostream>

using namespace std;


PlayerFileContext::PlayerFileContext(const char* pieceFileName, const char* movesFileName)
{
	pieces = new FileContext(pieceFileName, true);	//input file
	moves = new FileContext(movesFileName, true);	//input file
}

PlayerFileContext::~PlayerFileContext()
{
	delete pieces;
	delete moves;
}

FileParser::FileParser(eGameMode gameMode)
{
	output = new FileContext(OUTPUT_FILENAME, false);	//output file
	p1 = nullptr;
	p2 = nullptr;

	if(gameMode == FILE_VS_AUTO || gameMode == FILE_VS_FILE)
		p1 = new PlayerFileContext(PLAYER1_POSITION_FILENAME, PLAYER1_MOVES_FILENAME);

	if (gameMode == AUTO_VS_FILE || gameMode == FILE_VS_FILE)
		p2 = new PlayerFileContext(PLAYER2_POSITION_FILENAME, PLAYER2_MOVES_FILENAME);
}

FileParser::~FileParser()
{
	delete p1;
	delete p2;
	delete output;
}

/*
	Close file to free resources of file stream only if file was opened.
*/
FileContext::~FileContext()
{
	if (file->is_open())
		file->close();

	delete file;
	delete fileName;
}

int FileContext::openFile()
{
	int ret = -1;

	do
	{
		file = new std::fstream();

		//if output file also truncate file
		file->open(*fileName, isInputFile ? (ios::in) : (ios::out | ios::trunc));

		if (file->is_open())
		{
			ret = 0;
			break;
		}

		printf("Error while opening file: %s, %s\n", fileName->c_str(), strerror(errno));

	} while (false);

	return ret;
}

int FileParser::initializeFiles()
{
	//open files and check if file was opened successfully

	do 
	{
		if (output->openFile())
			break;

		if (p1)
		{
			if (p1->pieces->openFile())
				break;

			if (p1->moves->openFile())
				break;
		}

		if (p2)
		{
			if (p2->pieces->openFile())
				break;

			if (p2->moves->openFile())
				break;
		}

	} while (false);

	if (errno == ENOENT)	//one of files not found, print usage
	{
		cout << "To start the game, please make sure all input files are present in the working directory:\n"
			<< "\t* player1.rps_board \t* player2.rps_board \t* player1.rps_moves \t* player2.rps_moves\n" << endl;
	}

	return 0;
}


PlayerFileContext* FileParser::getPlayerFileContext(int playerNumber)
{
	if (playerNumber == 0)
		return p1;
	else
		return p2;
}



eFileStatus PlayerFileContext::getNextPiece(ePieceType* type, UINT* x, UINT* y, ePieceType* jokerType)
{
	eFileStatus status = FILE_SUCCESS;
	UINT length, xTmp, yTmp;
	string temp, line;
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
			getline(*(pieces->file), temp);
			length = (UINT)temp.length();
			pieces->incCurrentLineNum();

			if (length > 0)
			{
				if (temp[0] == '\n' || temp[0] == '\r') // '\n' not supposed to happen
					continue;
				else
				{
					line = (temp[length - 1] == '\r') ? temp.substr(0, length - 1) : temp;
					break;		//regular line
				}
			}
			else if (pieces->file->eof() || pieces->file->fail())
			{
				pieces->decCurrentLineNum();	//this line was incremented incorrectly
				status = FILE_EOF_REACHED;
				break;
			}
		}

		if (status)
			break;

		/* put the relevant substring into a stream and ignore whitespace at beginning of line */
		ss << (line[length-1] == '\r' ? line.substr(0, length-1) : line);
		ss >> ws;
		
		/* if the first character is J, its a joker line */
		if (ss.peek() == 'J')
		{
			/* move past the J */
			ss.ignore(1);

			/* parse the line, while "ws" ignores whitespace */
			ss >> ws >> xTmp >> ws >> yTmp >> ws >> typeTmp >> ws;

			/* if there was fail during one or other conversions
			or if there are anymore characters in the line after then its bad format */
			if (ss.fail() || ss.rdbuf()->in_avail() > 0)
			{
				printf("File \"%s\" has illegal format on joker line %d [%s]\n",
					pieces->getFileName().c_str(), pieces->getCurrentLineNum(), line.c_str());
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
				printf("File \"%s\" has illegal format on line %d [%s] - illegal joker type '%c'\n",
					pieces->getFileName().c_str(), pieces->getCurrentLineNum(), line.c_str(), typeTmp);
				status = FILE_BAD_FORMAT;
			}
			break;
		}

		/* parse line without joker */
		ss >> typeTmp >> ws >> xTmp >> ws >> yTmp >> ws;

		/* if there was fail during one or other conversions
		or if there are anymore characters in the line after then it's bad format */
		if (ss.fail() || ss.rdbuf()->in_avail() > 0)
		{
			printf("File \"%s\" has illegal format on line %d [%d]\n",
				pieces->getFileName().c_str(), pieces->getCurrentLineNum(), line[0]);
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
			printf("File \"%s\" has illegal format on line %d [%s] - illegal type\n",
				pieces->getFileName().c_str(), pieces->getCurrentLineNum(), line.c_str());
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
	UINT fX, fY, tX, tY, jX, jY, length;
	string temp, line;
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
			getline(*(moves->file), temp);
			length = (UINT)temp.length();
			moves->incCurrentLineNum();

			if (length > 0)
			{
				if (temp[0] == '\n' || temp[0] == '\r') // '\n' not supposed to happen
					continue;
				else
				{
					line = (temp[length - 1] == '\r') ? temp.substr(0, length - 1) : temp;
					break;		//regular line
				}
			}
			else if (moves->file->eof())
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
			printf("File \"%s\" has illegal format on line %d [%s]\n",
				moves->getFileName().c_str(), moves->getCurrentLineNum(), line.c_str());
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
			printf("File \"%s\" has illegal characters on line %d [%s]\n",
				moves->getFileName().c_str(), moves->getCurrentLineNum(), line.c_str());
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
			printf("File \"%s\" has illegal format on joker line %d [%s]\n",
				moves->getFileName().c_str(), moves->getCurrentLineNum(), line.c_str());
			status = FILE_BAD_FORMAT;
			break;
		}

		/* parse line */
		ss >> ws >> jX >> ws >> jY >> ws >> typeTmp >> ws;

		/* if there was fail during one or other conversions
		or if there are anymore characters in the line after then it's bad format */
		if (ss.fail() || ss.rdbuf()->in_avail() > 0)
		{
			printf("File \"%s\" has illegal format on joker line %d [%s]\n",
				moves->getFileName().c_str(), moves->getCurrentLineNum(), line.c_str());
			status = FILE_BAD_FORMAT;
			break;
		}

		*jokerX = jX;
		*jokerY = jY;
		*newRep = charToPiece(typeTmp);

		if (*newRep == UNDEF)
		{
			printf("File \"%s\" has illegal format on line %d [%s] - illegal joker type '%c'\n",
				moves->getFileName().c_str(), moves->getCurrentLineNum(), line.c_str(), typeTmp);
			status = FILE_BAD_FORMAT;
		}

		*isJoker = true;
		
	} while (false);

	return status;
}

void PlayerFileContext::setPieceFileToStart()
{
	pieces->file->clear();
	pieces->file->seekg(0, ios::beg);
	pieces->zeroCurrentLineNum();
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

	default:	//indicates invalid character
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

	default:	//indicates invalid enum
		c = '#';
		break;
	}

	return c;
}
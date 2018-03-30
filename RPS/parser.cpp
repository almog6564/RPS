
#include "parser.h"
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cerrno>
#include <cstring>

using namespace std;

PlayerFileContext::PlayerFileContext(string pieceFileName, string movesFileName)
{
	pieces = new FileContext(pieceFileName);
	moves = new FileContext(movesFileName);
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
			length = (UINT) line.length();
			pieces->setCurrentLine(&line);

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
		break;
		
	} while (false);

	return status;
}

FileParser::FileParser(string p1PiecesFileName, string p2PiecesFilename,
	string p1MovesFileName, string p2MovesFilename)
{
	p1 = new PlayerFileContext(p1PiecesFileName,p1MovesFileName);
	p2 = new PlayerFileContext(p2PiecesFilename, p2MovesFilename);
}

static bool openFile(FileContext* fc)
{
	bool ret = false;

	do 
	{
		if (!fc)
			break;

		fc->file.open(fc->fileName);

		if (fc->file.is_open())
		{
			ret = true;
			break;
		}

		printf("Error while opening file: %s, %s\n", fc->fileName.c_str(), strerror(errno));

	} while (false);

	return ret;
}

int FileParser::initializeFiles()
{
	//open files and check if file was opened successfully
	
	if (!openFile(p1->pieces))
		return -1;

	if (!openFile(p1->moves))
		return -1;

	if (!openFile(p2->pieces))
		return -1;

	if (!openFile(p2->moves))
		return -1;
	
	return 0;
}


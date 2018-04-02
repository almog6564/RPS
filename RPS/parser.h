
#ifndef _PARSER_
#define _PARSER_

#include <string>
#include <fstream>
#include <iostream>
#include "defs.h"


typedef enum _eFileStatus
{
	FILE_SUCCESS,
	FILE_ERROR,
	FILE_EOF_REACHED,
	FILE_BAD_FORMAT

} eFileStatus;

class FileContext
{
public:
	std::string fileName;
	std::ifstream file;

private:
	std::string* currentLine;

public:
	FileContext(std::string fname) : fileName(fname), currentLine(NULL) {};

	std::string& getLastReadLine()
	{
		return *currentLine;
	}

	void setCurrentLine(std::string* line)
	{
		currentLine = line;
	}
};

class PlayerFileContext
{
public:
	FileContext* pieces;
	FileContext* moves;

	PlayerFileContext(std::string pieceFileName, std::string movesFileName);

	eFileStatus getNextPiece(ePieceType* type, UINT* x, UINT* y, ePieceType* jokerType);

	eFileStatus getNextMove(UINT* fromX, UINT* fromY, UINT* toX, UINT* toY,
							bool* isJoker, UINT* jokerX, UINT* jokerY,
							ePieceType* newRep);

};

class FileParser
{

public:
	PlayerFileContext* p1;
	PlayerFileContext* p2;

	FileParser(std::string p1PiecesFileName, std::string p2PiecesFilename,
		std::string p1MovesFileName, std::string p2MovesFilename);

	int initializeFiles(void);
};

#endif //_PARSER_

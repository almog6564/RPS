
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
	std::fstream file;

private:
	std::string fileName;
	std::string* currentLine = nullptr;
	bool isInputFile;
	int currentLineNumber = -1;	//number of last line that was read

public:
	FileContext(std::string& fname, bool isInputFile) :
		fileName(fname), isInputFile(isInputFile) {};

	~FileContext();

	bool openFile();

	std::string* getLastReadLine()
	{
		return currentLine;
	}

	void setCurrentLine(std::string* line)
	{
		currentLine = line;
	}

	int getCurrentLineNum()
	{
		return currentLineNumber;
	}

	void incCurrentLineNum()
	{
		currentLineNumber++;
	}

	void zeroCurrentLineNum()
	{
		currentLineNumber = 0;
	}
};

class PlayerFileContext
{
public:
	FileContext* pieces;
	FileContext* moves;

	PlayerFileContext(std::string& pieceFileName, std::string& movesFileName);

	eFileStatus getNextPiece(ePieceType* type, UINT* x, UINT* y, ePieceType* jokerType);

	eFileStatus getNextMove(UINT* fromX, UINT* fromY, UINT* toX, UINT* toY,
							bool* isJoker, UINT* jokerX, UINT* jokerY,
							ePieceType* newRep);

	void setPieceFileToStart();

	~PlayerFileContext();

};

class FileParser
{
public:
	PlayerFileContext* p1;
	PlayerFileContext* p2;
	FileContext* output;

	FileParser(std::string& p1PiecesFileName, std::string& p2PiecesFilename,
		std::string& p1MovesFileName, std::string& p2MovesFilename, 
		std::string& outputFilename);

	int initializeFiles(void);

	PlayerFileContext* getPlayerFileContext(int playerNumber);

	~FileParser();
};

ePieceType charToPiece(char c);
char pieceToChar(ePieceType p, bool isUpperCase = true);

#endif //_PARSER_

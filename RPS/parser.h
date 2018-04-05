
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
	std::fstream file;
	bool isInputFile;

private:
	std::string* currentLine = nullptr;

public:
	FileContext(std::string& fname, bool isInputFile) :
		fileName(fname), isInputFile(isInputFile), currentLine(NULL) {};

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
};

std::string GetReasonString(eReason reason, int arg0 = -1, int arg1 = -1);
ePieceType charToPiece(char c);
char pieceToChar(ePieceType p, bool isUpperCase);

#endif //_PARSER_

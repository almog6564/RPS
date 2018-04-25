
#ifndef _PARSER_
#define _PARSER_

#include <string>
#include <fstream>
#include "defs.h"


/**
	Class FileContext
	Encloses all the file specific operations.
*/
class FileContext
{
public:
	/* File itself (either input or output) */
	std::fstream* file;

private:
	std::string* fileName;

	/* boolean holding is input file */
	bool isInputFile;

	/* number of last line that was read */
	int currentLineNumber = -1;	

public:
	/*
	Initializes a FileContext object.
	After initialization the function @openFile must be run in order to use the file.
	*/
	FileContext(std::string& fname, bool isInputFile) :
		fileName(new std::string(fname)), isInputFile(isInputFile) {};

	~FileContext();

	/**
	Opens a file for read/write according to @isInputFile from initialization of FileContext.
	If the file cannot open for some reason it prints the errno reason as set by the open function.

	@return 0 if file was opened successfully, otherwise (-1).
	*/
	int openFile();

	std::string& getFileName()
	{
		return *fileName;
	}

	int getCurrentLineNum()
	{
		return currentLineNumber + 1;	//so it won't start from 0
	}

	void incCurrentLineNum()
	{
		currentLineNumber++;
	}

	void decCurrentLineNum()
	{
		currentLineNumber--;
	}

	void zeroCurrentLineNum()
	{
		currentLineNumber = -1;
	}
};

/**
	Class PlayerFileContext
	Encloses all the file specific operations for each player.
	
	@var pieces - a FileContext holding file operations on the positioning (pieces) file of this player
	@var moves	- a FileContext holding file operations on the moves file of this player
*/
class PlayerFileContext
{
public:
	FileContext* pieces;
	FileContext* moves;

	/*
		Initializes a PlayerFileContext object accordingly by getting only the filenames.
	*/
	PlayerFileContext(std::string& pieceFileName, std::string& movesFileName);

	~PlayerFileContext();
	/*
		This function gets the next piece available in the player positioning file.
		It also checks for all the possible #format violations only. This includes piece char
		representation checks. Namely, the function does not do a dimension check on the coordinates
		nor does not access the board itself.
		The function ignores redundant empty lines or whitespace characters.
		The function fills the pointers given as arguments with the relevant data from the line.
		The data is valid only if @FILE_SUCCESS is returned.

		@pre			- pieces->openFile was run beforehand
		@vars type, x, y - pointers to the relevant parameters returned by the file row
		@var jokerType - if type is Joker this will hold the joker type

		@returns -
			* FILE_SUCCESS - if file had another row with data and the parsing operation succeeded completely
			* FILE_EOF_REACHED - if file had no lines to read from. If the function is used again after this was returned,
								 this value will be returned again.

			* FILE_ERROR - if one of the parameters are NULL
			* FILE_BAD_FORMAT - if the parsing did not succeed due to format violations
	*/
	eFileStatus getNextPiece(ePieceType* type, UINT* x, UINT* y, ePieceType* jokerType);


	/*
	This function gets the next move available in the player moves file.
	It also checks for all the possible #format violations only. This includes piece char
	representation checks. Namely, the function does not do a dimension check on the coordinates 
	nor does not access the board itself.
	The function ignores redundant empty lines or whitespace characters.
	The function fills the pointers given as arguments with the relevant data from the line.
	The data is valid only if @FILE_SUCCESS is returned.
	The joker parameters are valid only if true is set in *isJoker.

	@pre							- moves->openFile was run beforehand
	@vars fromX, fromY, toX, toY	- pointers to the relevant parameters returned by the file row
	@var isJoker					- pointer to a boolean indicating whether the line has joker move additionaly
	@vars jokerX, jokerY			- pointers to the coordinates of the joker place. Valid only if *isJoker == true
	@var newRep						- new representation of the joker. Valid only if *isJoker == true

	@returns -
	* FILE_SUCCESS - if file had another row with data and the parsing operation succeeded completely
	* FILE_EOF_REACHED - if file had no lines to read from. If the function is used again after this was returned,
							this value will be returned again.

	* FILE_ERROR - if one of the parameters are NULL
	* FILE_BAD_FORMAT - if the parsing did not succeed due to format violations
	*/
	eFileStatus getNextMove(UINT* fromX, UINT* fromY, UINT* toX, UINT* toY,
							bool* isJoker, UINT* jokerX, UINT* jokerY,
							ePieceType* newRep);

	/*
		Seeks the beginning of the piece file.
		Valid only if file was opened.
	*/
	void setPieceFileToStart();

};

/**
Class FileParser
Encloses all the file specific operations for both players, and the game.

@var p1		- a PlayerFileContext holding file operations of player 1
@var p2		- a PlayerFileContext holding file operations of player 2
@var output - a FileContext holding file operations of the output file
*/
class FileParser
{
public:
	PlayerFileContext* p1;
	PlayerFileContext* p2;
	FileContext* output;

	FileParser(std::string& p1PiecesFileName, std::string& p2PiecesFilename,
		std::string& p1MovesFileName, std::string& p2MovesFilename, 
		std::string& outputFilename);

	/*
		Open all files of both players and the output file.
		@ret 0 if all file opened correctly, (-1) otherwise
	*/
	int initializeFiles(void);

	/*
		Returns the relevant PlayerFileContext by player ID.
		@var playerNumber ID 
		@ret PlayerFileContext of player1 if playerNumber == 0,
			 PlayerFileContext of player2 if playerNumber == 1,	
			 otherwise nullptr
	*/
	PlayerFileContext& getPlayerFileContext(int playerNumber);

	~FileParser();
};

/*
	Converts UPPERCASE char to a piece of ePieceType.

	@ret relevant ePieceType enum value, UNDEF if character is not one of:
			R, P, S, B, F, J
*/
ePieceType charToPiece(char c);

/*
	Converts ePieceType enum to a char.

	@ret relevant char, 'U' if ePieceType is not one of
	ROCK, PAPER, SCISSORS, BOMB, FLAG, JOKER
*/
char pieceToChar(ePieceType p, bool isUpperCase = true);

#endif //_PARSER_

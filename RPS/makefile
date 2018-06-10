COMP = g++-5.3.0
#put all your object files here
OBJS = parser.o MyPoint.o MyJokerChange.o MyMove.o MyFightInfo.o PlayerContext.o \
	MyPiecePosition.o BoardPiece.o myBoard.o InitalPositionsUtilities.o GetMoveUtilites.o \
	AutoPlayerAlgorithm.o FilePlayerAlgorithm.o game.o

#The executabel filename DON'T CHANGE
EXEC = ex2

CPP_COMP_FLAG = -std=c++14 -DNDEBUG -g -Wall -Wextra -pedantic-errors -Werror -fPIC 

$(EXEC): $(OBJS)
	$(COMP) $(OBJS) -o $@


#a rule for building a simple c++ source file
#use g++ -MM main.cpp to see dependencies

parser.o: parser.cpp parser.h defs.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

MyPoint.o: MyPoint.h Point.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

MyJokerChange.o: MyJokerChange.cpp MyJokerChange.h JokerChange.h defs.h \
 MyPoint.h Point.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

MyMove.o: MyMove.h Move.h defs.h MyPoint.h Point.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

MyFightInfo.o: MyFightInfo.h FightInfo.h defs.h MyPoint.h Point.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

PlayerContext.o: PlayerContext.cpp PlayerContext.h defs.h parser.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

MyPiecePosition.o: MyPiecePosition.cpp MyPiecePosition.h PiecePosition.h \
 defs.h MyPoint.h Point.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

BoardPiece.o: BoardPiece.cpp BoardPiece.h defs.h MyPiecePosition.h PiecePosition.h \
 MyPoint.h Point.h PlayerContext.h parser.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

myBoard.o: myBoard.cpp myBoard.h BoardPiece.h defs.h PiecePosition.h \
 PlayerContext.h parser.h MyPoint.h Point.h Board.h MyFightInfo.h \
 FightInfo.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

GetMoveUtilites.o: GetMoveUtilites.cpp AutoPlayerAlgorithm.h \
 PlayerAlgorithm.h Point.h PiecePosition.h Board.h FightInfo.h Move.h \
 JokerChange.h MyJokerChange.h defs.h MyPoint.h MyPiecePosition.h \
 MyMove.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

InitalPositionsUtilities.o: InitalPositionsUtilities.cpp \
 AutoPlayerAlgorithm.h PlayerAlgorithm.h Point.h PiecePosition.h Board.h \
 FightInfo.h Move.h JokerChange.h defs.h MyPiecePosition.h MyPoint.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

	
AutoPlayerAlgorithm.o: AutoPlayerAlgorithm.cpp AutoPlayerAlgorithm.h \
 PlayerAlgorithm.h Point.h PiecePosition.h Board.h FightInfo.h Move.h \
 JokerChange.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

FilePlayerAlgorithm.o: FilePlayerAlgorithm.cpp FilePlayerAlgorithm.h \
 PlayerAlgorithm.h Point.h PiecePosition.h Board.h FightInfo.h Move.h \
 JokerChange.h parser.h defs.h MyMove.h MyPoint.h MyJokerChange.h \
 MyPiecePosition.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

game.o: game.cpp game.h defs.h PlayerContext.h parser.h myBoard.h BoardPiece.h \
 PiecePosition.h MyPoint.h Point.h Board.h MyFightInfo.h FightInfo.h \
 AutoPlayerAlgorithm.h PlayerAlgorithm.h Move.h JokerChange.h \
 FilePlayerAlgorithm.h MyMove.h MyJokerChange.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

main.o: main_ex2.cpp game.h defs.h PlayerContext.h parser.h myBoard.h BoardPiece.h \
 PiecePosition.h MyPoint.h Point.h Board.h MyFightInfo.h FightInfo.h \
 AutoPlayerAlgorithm.h PlayerAlgorithm.h Move.h JokerChange.h \
 FilePlayerAlgorithm.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp



clean:
	rm -f $(OBJS) $(EX3_OBJS) $(EXEC) *.o *.so ex3


#########################################################
##### E X 3						#
#########################################################

#-fPIC -rdynamic  -ldl
SO_COMP_FLAG = -std=c++14 -shared -g 
EX3_OBJS = $(OBJS) main_ex3.o MultiGameManager.o AlgorithmRegistration.o  
SP_FILE = RSPPlayer_0.so

main_ex3.o: main_ex3.cpp MultiGameManager.h defs.h PlayerAlgorithm.h \
 Point.h PiecePosition.h Board.h FightInfo.h Move.h JokerChange.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

RSPPlayer_0.o: RSPPlayer_0.cpp AlgorithmRegistration.h PlayerAlgorithm.h \
 Point.h PiecePosition.h Board.h FightInfo.h Move.h JokerChange.h \
 MyJokerChange.h defs.h MyPoint.h MyPiecePosition.h MyMove.h parser.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

RSPPlayer_1.o: RSPPlayer_1.cpp AlgorithmRegistration.h PlayerAlgorithm.h \
 Point.h PiecePosition.h Board.h FightInfo.h Move.h JokerChange.h \
 MyJokerChange.h defs.h MyPoint.h MyPiecePosition.h MyMove.h parser.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

RSPPlayer_2.o: RSPPlayer_2.cpp AlgorithmRegistration.h PlayerAlgorithm.h \
 Point.h PiecePosition.h Board.h FightInfo.h Move.h JokerChange.h \
 MyJokerChange.h defs.h MyPoint.h MyPiecePosition.h MyMove.h parser.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

RSPPlayer_3.o: RSPPlayer_3.cpp AlgorithmRegistration.h PlayerAlgorithm.h \
 Point.h PiecePosition.h Board.h FightInfo.h Move.h JokerChange.h \
 MyJokerChange.h defs.h MyPoint.h MyPiecePosition.h MyMove.h parser.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

MultiGameManager.o: MultiGameManager.cpp MultiGameManager.h defs.h \
 PlayerAlgorithm.h Point.h PiecePosition.h Board.h FightInfo.h Move.h \
 JokerChange.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
	
AlgorithmRegistration.o: AlgorithmRegistration.cpp \
 AlgorithmRegistration.h PlayerAlgorithm.h Point.h PiecePosition.h \
 Board.h FightInfo.h Move.h JokerChange.h MultiGameManager.h defs.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
	
rps_tournament: $(EX3_OBJS) 
	$(COMP) -rdynamic -pthread -lpthread -o ex3 $(EX3_OBJS) -ldl

LIB_OBJS = RSPPlayer_0.o RSPPlayer_1.o RSPPlayer_2.o RSPPlayer_3.o \
 parser.o MyPoint.o MyJokerChange.o MyMove.o \
 MyFightInfo.o MyPiecePosition.o BoardPiece.o myBoard.o PlayerContext.o MultiGameManager.o


RSPPlayer_0.so: $(LIB_OBJS)
	$(COMP) $(SO_COMP_FLAG) -Wl,-soname,lib$@ -o  $@ RSPPlayer_0.o

RSPPlayer_1.so: $(LIB_OBJS)
	$(COMP) $(SO_COMP_FLAG) -Wl,-soname,lib$@ -o  $@ RSPPlayer_1.o

RSPPlayer_2.so: $(LIB_OBJS)
	$(COMP) $(SO_COMP_FLAG) -Wl,-soname,lib$@ -o  $@ RSPPlayer_2.o

RSPPlayer_3.so: $(LIB_OBJS)
	$(COMP) $(SO_COMP_FLAG) -Wl,-soname,lib$@ -o  $@ RSPPlayer_3.o

rps_lib: $(LIB_OBJS) RSPPlayer_0.so RSPPlayer_1.so RSPPlayer_2.so RSPPlayer_3.so

all: rps_tournament rps_lib
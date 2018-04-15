COMP = g++-5.3.0
#put all your object files here
OBJS = parser.o player.o piece.o board.o game.o main.o
#The executabel filename DON'T CHANGE
EXEC = ex1
CPP_COMP_FLAG = -std=c++14 -Wall -Wextra \
-Werror -pedantic-errors -DNDEBUG

$(EXEC): $(OBJS)
	$(COMP) $(OBJS) -o $@

#a rule for building a simple c++ source file
#use g++ -MM main.cpp to see dependencies

parser.o: parser.cpp parser.h defs.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

player.o: player.cpp player.h defs.h parser.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

piece.o: piece.cpp piece.h defs.h player.h parser.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

board.o: board.cpp board.h piece.h defs.h player.h parser.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

game.o: game.cpp game.h defs.h player.h parser.h board.h piece.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

main.o: main.cpp game.h defs.h player.h parser.h board.h piece.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp


clean:
	rm -f $(OBJS) $(EXEC)

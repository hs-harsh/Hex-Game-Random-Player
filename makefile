OBJS = main.o RandomPlayer.o HumanPlayer.o BoardCoord_Piece.o Graph.o Board.o 
CC = g++
CFLAGS = -std=c++11 -c
LFLAGS = -std=c++11  

a.exe : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o play.exe

main.o : main.cpp
	$(CC) $(CFLAGS) main.cpp

RandomPlayer.o : RandomPlayer.cpp Player.h Board.cpp BoardCoord_Piece.cpp
	$(CC) $(CFLAGS) RandomPlayer.cpp

HumanPlayer.o : HumanPlayer.cpp Player.h Board.cpp BoardCoord_Piece.cpp
	$(CC) $(CFLAGS) HumanPlayer.cpp
    
BoardCoord_Piece.o : BoardCoord_Piece.cpp
	$(CC) $(CFLAGS) BoardCoord_Piece.cpp

Graph.o : Graph.cpp
	$(CC) $(CFLAGS) Graph.cpp

Board.o : Board.cpp BoardCoord_Piece.cpp Graph.cpp
	$(CC) $(CFLAGS) Board.cpp

clean:
	rm -f *.o a.exe play.exe .stackdump *.h.gch

#ifndef HEX_RANDOMPLAYER_H_
#define HEX_RANDOMPLAYER_H_

#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "Player.h" 
#include "Board.cpp" 
#include "BoardCoord_Piece.cpp" 

/* An implementation of Player that makes random moves on the board */
class RandomPlayer: public Player {
    public:
        BoardCoord takeTurn(Board& board) const{
            // Count all the empty squares
            int empty = 0;
            for(unsigned int x = 0; x < board.WIDTH; x++) {
                for(unsigned int y = 0; y < board.HEIGHT; y++) {
                    if(board.isEmpty(BoardCoord(x, y))) {
                        empty++;
                    }
                } 
            }
            // Randomly Select one of the empty squares
            srand (time(NULL));
            int index = rand() % empty;
            int count = 0;
            for(unsigned int x = 0; x < board.WIDTH; x++) {
                for(unsigned int y = 0; y < board.HEIGHT; y++) {
                    if(board.isEmpty(BoardCoord(x, y))) {
                        if(index == count) {
                            // Return the random square
                            return BoardCoord(x, y);
                        }
                        count++;
                    }
                } 
            }
            // Unreachable statement
            assert(false);
        }
};

#endif  // HEX_RANDOMPLAYER_H_
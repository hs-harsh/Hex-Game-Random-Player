#ifndef HEX_PLAYER_H_
#define HEX_PLAYER_H_

#include "Board.cpp" 
#include "BoardCoord_Piece.cpp" 

class Player {
    public:
        virtual BoardCoord takeTurn(Board& board) const = 0;
};

#endif  // HEX_PLAYER_H_
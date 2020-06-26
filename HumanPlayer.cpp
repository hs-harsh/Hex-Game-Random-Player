#ifndef HEX_HUMANPLAYER_H_
#define HEX_HUMANPLAYER_H_

#include <stdexcept>
#include "Player.h" 
#include "Board.cpp" 
#include "BoardCoord_Piece.cpp" 

using namespace std;

class HumanPlayer: public Player {
    public:
        BoardCoord takeTurn(Board& board) const{
            while (true) {
                cout<<endl<<("Enter your move (Row Col): ");
                try {
                    int x,y;
                    cin>>x>>y;
                    BoardCoord boardCoord = BoardCoord(y-1,x-1);

                    if (!board.isValid(boardCoord)) {
                        cout<<("Not on the board! Try again...");
                    } else if (!board.isEmpty(boardCoord)) {
                        cout<<("Already occupied! Try again...");
                    } else { 
                        return boardCoord;
                    }
                } catch (const invalid_argument&) {
                    cout<<("Invalid coordinate! Try again...");
                }
            }
        }
};

#endif  // HEX_HUMANPLAYER_H_
#include <assert.h> 
#include "Board.cpp"
#include "Player.h"
#include "HumanPlayer.cpp"
#include "RandomPlayer.cpp"



using namespace std;

void print_board(const Board& b){
    // Go through each row
    for(unsigned int row = 0; row < b.HEIGHT; row++) {
        string line = "";
        string next = "";
        // For each row, go through each cell
        for(unsigned int col = 0; col < b.WIDTH; col++) {
            // Find the corresponding piece on the board
            BoardCoord bc = BoardCoord(col, row);
            // Add the corresponding character to line
            if (b.isEmpty(bc)) {
                line += ".";            
            } else {
                Piece p = b.getPiece(bc);
                if (p == RED) {
                    line += 'O';
                } else {
                    line += 'X';
                }
            }
            // Add the aesthetic padding between pieces and lines
            if (col == b.WIDTH - 1) {
                next += " \\";        
            } else {
                line += " - ";
                next +=  " \\ /";
            }
        } 
        // Display the line padding at the start of the line and coord numbers
        ostringstream iStm;
        iStm << (row + 1);
        string iStr = iStm.str() + ": ";
        string pad((row * 2) + 3 - iStr.size(), ' ');
        string nextPad((row * 2) + 3, ' ');
        cout << pad << iStr << line << endl;
        if (row != b.HEIGHT - 1) {
            cout << nextPad << next << endl;
        } else {
            // Display the coord letters at the bottom
            int noOfRows;
            if(b.WIDTH<27)      noOfRows=1;
            else                noOfRows=2;
            for(int col = 0; col < noOfRows; col++) {
                string lettersPad(col + 1, ' ');
                cout << nextPad << lettersPad;
                for(unsigned int k = 0; k < b.WIDTH; k++) {
                    cout << to_string(k+1)<< "   ";
                }
                cout << endl;
            }        
        }
    }
}

const Player* play(Board& board, const Player& player1, const Player& player2, const Piece p1Piece, const Piece p2Piece){
    Board* B=&board;
    int size = B->WIDTH * B->HEIGHT;
    for(int turn = 0; turn < size; turn++) {
        const Player* player;
        Piece piece;
        if(turn % 2 == 0) {
            print_board(board);
            player = &player1;
            piece = p1Piece;
        } else {
            player = &player2;
            piece = p2Piece;
        }
        BoardCoord coord = player->takeTurn(*B);
        B->addPiece(piece, coord);
        if (B->isWinner(piece, true)) {
            return player;
        }
    }
    // Unreachable statement
    assert(false);
}

int main() {
    const Piece piece1 = RED;
    const Piece piece2 = BLACK;
    int N,M;
    cout<<"Enter Number dimension (Rows Col) of Hex. Eg- 3 4 (for 3 * 4 board): ";
    cin>>M>>N;
    Board board = Board(N, M, piece1, piece2);
    HumanPlayer player1 = HumanPlayer();
    RandomPlayer player2 = RandomPlayer();
    string Info = "";
    Info += "How to play: Players take turns to place their piece on the board. Player 1's piece is an 'O' and player 2's an 'X'. The\n";
    Info += "first player to link their edges of the board wins. Player 1 must link horizontally and player 2 vertically. Coordinates\n";
    Info += "should be entered as the column number(s) followed by space and then the row number. Think it as Hex cordinate, input move should be 'X Y'";
    Info += ", e.g. '7 7'\n";
    Info += "\n";
    cout<<(Info);
    const Player* winner = play(board, player1, player2, piece1, piece2);
    print_board(board);
    if (winner == &player1) {
        cout<<("Well done, you win!")<<endl;
    } else {
        cout<<("Bad luck, you lose!")<<endl;
    }
}

#ifndef HEX_BOARD_H_
#define HEX_BOARD_H_

#include <stdexcept>
#include "Board.cpp" 
#include "BoardCoord_Piece.cpp" 
#include "Graph.cpp"

using namespace std;

class Board {  
    public:
        const unsigned int WIDTH;
        const unsigned int HEIGHT;
        const Piece PLAYER1;
        const Piece PLAYER2;
    private:
        Graph<bool>* p1Graph;
        Graph<bool>* p2Graph;
        unsigned int p1Start;
        unsigned int p1End;
        unsigned int p2Start;
        unsigned int p2End;
        class BoardSquare {
            public:
                bool empty;
                Piece piece;
                BoardSquare(): empty(true), piece(RED) {}
                BoardSquare(const bool edge, const Piece piece): empty(edge), piece(piece) {}
        };
        BoardSquare* board;
    public:
        Board(unsigned int width, unsigned int height, Piece player1, Piece player2): 
            WIDTH(width), HEIGHT(height), PLAYER1(player1), PLAYER2(player2) {
            if (width <= 0u || height <= 0u) {
                throw invalid_argument("Width and height must not be less than 1");
            }
            board = new BoardSquare[WIDTH * HEIGHT];
            // Each players connectivity graph
            p1Graph = new Graph<bool>(false, WIDTH*HEIGHT);
            p2Graph = new Graph<bool>(false, WIDTH*HEIGHT);

            // Connect all starts/ends together to make only 1 of each
            p1Start = coord2graph(BoardCoord(0, 0));
            p1End = coord2graph(BoardCoord(WIDTH - 1, 0));
            for (unsigned int i = 1; i < HEIGHT; i++) {
                p1Graph->setWeight(p1Start, coord2graph(BoardCoord(0, i)), true);
                p1Graph->setWeight(p1End, coord2graph(BoardCoord(WIDTH - 1, i)), true);
            }

            p2Start = coord2graph(BoardCoord(0, 0));
            p2End = coord2graph(BoardCoord(0, HEIGHT - 1));
            for (unsigned int i = 1; i < WIDTH; i++) {
                p2Graph->setWeight(p2Start, coord2graph(BoardCoord(i, 0)), true);
                p2Graph->setWeight(p2End, coord2graph(BoardCoord(i, HEIGHT - 1)), true);
            }
        }

        ~Board(){
            delete[] board;
        }
        bool isValid(BoardCoord coord) const{
            return isValid(coord.col, coord.row);
        }
        /* Returns true if this BoardCoord is empty.
           BoardCoord coord: The BoardCoord to be checked
           throws: out_of_range if the BoardCoord does not lie on the board
        */
        bool isEmpty(BoardCoord coord) const{
            return get(coord.col, coord.row).empty;     
        }
        /* Returns the Piece at this BoardCoord.
           BoardCoord coord: The BoardCoord to be checked
           throws: out_of_range if the BoardCoord is empty or not on the board
        */
        Piece getPiece(BoardCoord coord) const{
            BoardSquare square = get(coord.col, coord.row);
            if (square.empty) {
                throw out_of_range("BoardCoord is empty");
            }
            return square.piece;     
        }

        void addPiece(Piece piece, BoardCoord coord){
            if(!isValid(coord)) {
                throw out_of_range("Invalid BoardCoord");
            }
            if(!isEmpty(coord)) {
                throw out_of_range("BoardCoord not empty");
            }
            set(coord.col, coord.row, BoardSquare(false, piece));
            // Update the pieces graphs
            Graph<bool>* graph;
            if (piece == PLAYER1) {
                graph = p1Graph;
            } else {
                graph = p2Graph;
            }
            unsigned int index = coord2graph(coord);
            // Adjacent connections
            int adj[12] = {0, -1, 1, -1, -1, 0, 1, 0, -1, 1, 0, 1};
            for (int i = 0; i < 12; i += 2) {
                unsigned int adjCol = coord.col + adj[i];
                unsigned int adjRow = coord.row + adj[i+1];
                BoardCoord adjCoord = BoardCoord(adjCol, adjRow);
                if (isValid(adjCoord) && !isEmpty(adjCoord) &&
                        getPiece(adjCoord) == piece) {
                    unsigned int adjIndex = coord2graph(adjCoord);
                    graph->setWeight(index, adjIndex, true);
                }
            }      
        }
        // If cache is true then this will increase performance if called
        // repeatedivly
        bool isWinner(Piece piece, bool cache){
            if (piece == PLAYER1) {
                return p1Graph->isConnected(p1Start, p1End, cache);
            } else {
                return p2Graph->isConnected(p2Start, p2End, cache);
            }
        }
    private:
        bool isValidCol(unsigned int col) const{
            return col >= 0u && col < this->WIDTH;
        }
        bool isValidRow(unsigned int row) const{
            return row >= 0u && row < this->HEIGHT;
        }
        bool isValid(unsigned int col, unsigned int row) const{
            return isValidCol(col) && isValidRow(row);
        }
        /* Returns the BoardSquare for the specified col and row.
           throws: out_of_range if the col or row is outwith the bounds of this 
           Board
        */
        BoardSquare get(unsigned int col, unsigned int row) const{
            if(!isValid(col, row)) {
                throw out_of_range("Invalid col or row");
            }
            return board[row * WIDTH + col];
        }
        void set(unsigned int col, unsigned int row, BoardSquare square){
            if(!isValid(col, row)) {
                throw out_of_range("Invalid col or row");
            }
            board[row * WIDTH + col] = square;
        }
        // Returns a graph index
        unsigned int coord2graph(BoardCoord coord){
            return coord.row * WIDTH + coord.col;
        }

};

#endif  // HEX_BOARD_H_
#ifndef HEX_BOARDCOORD_H_
#define HEX_BOARDCOORD_H_

#include <bits/stdc++.h> 
#include <boost/algorithm/string.hpp> 
#include <string>
#include <iostream>
#include <stdexcept>

using namespace std;

/* A row and col coordinate corresponding to a position on the board */
class BoardCoord {
    public:
        int col;
        int row;
        BoardCoord(): col(0), row(0) {}
        BoardCoord(int col, int row): col(col), row(row) {}
};

enum Piece: const bool
{
    RED=false,BLACK=true
};


#endif  // HEX_BOARDCOORD_H_
//
// Created by anton on 10/23/2026.
//

#ifndef SOKOBAN_POSITION_H
#define SOKOBAN_POSITION_H
#include <cstdint>

// the position can range from 0 to 32 so it is stored on 8 bits
struct Pair {
    int x;
    int y;
    bool operator==(const Pair& o) const
    {
        if (x == o.x && y == o.y)
            return true;
        return false;
    };
    Pair operator+(const Pair& o) const
    {
        return {x + o.x , y + o.y};
    };
    Pair operator-(const Pair& o)const
    {
        return {x - o.x, y - o.y};
    }

};
enum Tile {FLOOR , WALL , GOAL};

struct Move { Pair dir; bool pushedBox; };

#endif //SOKOBAN_POSITION_H

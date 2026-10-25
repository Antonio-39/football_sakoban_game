//
// Created by anton on 10/24/2026.
//

#ifndef SOKOBAN_LEVEL_H
#define SOKOBAN_LEVEL_H

#include <optional>
#include <vector>
#include <string>

#include "types.h"


class Level {
public:
    static std::optional<Level> fromFile(const std::string& path);

    Tile tileAt(Pair p) const;
    int boxIndexAt(Pair p) const;
    bool inBounds(Pair p) const;
    bool isBlocked(Pair p) const;

    bool tryMove(Pair dir);
    bool undo();
    void reset();
    bool isSolved() const;


    int w = 0, h = 0;
    std::vector<Tile> tiles;
    std::vector<Pair> boxes;
    Pair player{};
    std::vector<Move> history; // functioneaza ca o stiva

};


#endif //SOKOBAN_LEVEL_H

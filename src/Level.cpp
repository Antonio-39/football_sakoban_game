//
// Created by anton on 10/24/2026.
//

#include "Level.h"
#include <fstream>

std::optional<Level> Level :: fromFile(const std::string& path)
{
    std::ifstream f(path);
    if ( !f ) return std::nullopt;

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(f, line))
    {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        lines.push_back(line);
    }

    Level level;
    level.h = (int)lines.size();
    level.w = 0;
    for (const auto &l : lines) level.w = std::max(level.w , (int)l.size());
    level.tiles.assign(level.w * level.h, Tile::FLOOR);

    for (int y = 0; y < level.h; ++y)
    {
        for (int x = 0; x < (int)lines[y].size(); ++x)
        {
            Pair p{x, y};
            int idx = x + y * level.w;
            switch (lines[y][x]){
                case '#':
                    level.tiles[idx] = WALL;
                    break;
                case ' ':
                    level.tiles[idx] = FLOOR;
                    break;
                case '.':
                    level.tiles[idx] = GOAL;
                    break;
                case '@': // player
                    level.tiles[idx] = FLOOR;
                    level.player = p;
                    break;
                case '$': // box
                    level.tiles[idx] = FLOOR;
                    level.boxes.push_back(p);
                    break;
                case '*': // box on goal
                    level.tiles[idx] = GOAL;
                    level.boxes.push_back(p);
                    break;
                case '+': // player on goal
                    level.tiles[idx] = GOAL;
                    level.player = p;
                    break;
                default: return std::nullopt;
            }
        }
    }

    return level;
}

// returneaza ce tip de tile este la positia p
Tile Level::tileAt(Pair p) const
{
    return tiles[p.y * w + p.x];
}

// returneaza indexul cutiei de la posita p
// sau -1 daca nu este cutie la p
int Level::boxIndexAt(Pair p) const
{
    for (int i = 0; i < (int)boxes.size(); i++)
        if (boxes[i] == p) return i;
    return -1;
}

// returneaza 1 daca pozitia p este in joc
// 0 altfel
bool Level::inBounds(Pair p) const
{
    return p.x >= 0 && p.x < w && p.y >= 0 && p.y < h;
}

// returneaza 1 daca pos p nu e in joc sau daca la pos p e un wall
// 0 altfel ( pozitia p este valida pt player sau box)
bool Level::isBlocked(Pair p) const
{
    return !inBounds(p) || tileAt(p) == Tile::WALL;
}

// returneaza 1 daca miscarea e valida
// si efectueaza schimbarile la player
// 0 altfel
bool Level::tryMove(Pair dir)
{
    // unde vrea player sa mearga
    Pair target = player + dir;

    if (isBlocked(target))
        return false;

    // verificam daca pe target este o cutie
    // si daca da facem logica de push
    int boxIdx = boxIndexAt(target);
    bool pushed = false;

    if (boxIdx >= 0)
    {
        Pair beyond = target + dir;

        // cutia trebe sa aiba celula libera in partea in care e impinsa
        // verificam sa nu aiba perete sau alta cutie
        if (isBlocked(beyond) || boxIndexAt(beyond) >= 0)
            return false;

        boxes[boxIdx] = beyond;
        pushed = true;
    }

    player = target;
    history.push_back({dir , pushed});
    return true;
}

// da reverse la ultima miscare
bool Level::undo()
{
    if (history.empty())
        return false;

    Move ultima = history[history.size() - 1];
    history.pop_back();

    // daca la ultima miscare a fost mutata o cutie
    if (ultima.pushedBox)
    {
        int movedBox = boxIndexAt(player + ultima.dir);
        boxes[movedBox] = player;
    }

    player = player - ultima.dir;
    return true;
}

// reseteaza nivelul
void Level::reset()
{
    while (undo()) {}
}

// verifica daca nivelul e complet
bool Level::isSolved() const
{
    for (auto box: boxes)
    {
        int pos = box.y * w + box.x;
        if(tiles[pos] != Tile::GOAL)
            return false;
    }
    return true;
}

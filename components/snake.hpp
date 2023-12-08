#pragma once

#ifndef _SNAKE_
#define _SNAKE_
#include <stdlib.h>
#include <vector>

class Coordinates {
public:
    Coordinates(int x, int y);
    int x;
    int y;
};

class Player {
public:
    Player(int size);
    int size;
    int score;
    std::vector<Coordinates> units;
    void move (int x, int y);
    bool colliding();
};

class Food {
public:
    Food(int max_x, int max_y);
    int pos_x;
    int pos_y;
};


#endif
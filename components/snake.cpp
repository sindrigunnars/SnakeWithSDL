#include "snake.hpp"
#include <stdlib.h>

Coordinates::Coordinates(int x, int y) : x(x), y(y) {};
Player::Player(int size) : size(size), score(0) {};
Food::Food(int max_x, int max_y) : pos_x(rand() % max_x - 20), pos_y(rand() % max_y - 20) {};


void Player::move(int x, int y){
    this->units.insert(units.begin(), Coordinates(x,y));
    int snake_size = this->units.size();
    // this->units.push_back(this->units[snake_size - 1]);
    // for (int i = snake_size - 1; i > 0; i--) {
    //     this->units[i] = this->units[i-1];
    // }
    // this->units[0] = Coordinates(x,y);
    if (snake_size > score + 1) {
        this->units.pop_back();
    }
}

bool Player::colliding() {
    for (int i = 1; i < this->units.size(); i++) {
        if (this->units[0].x == this->units[i].x && this->units[0].y == this->units[i].y) {
            return true; // Collision with the body
        }
    }
    return false;
}

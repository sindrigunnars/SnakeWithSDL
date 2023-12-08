#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include "components/snake.hpp"

SDL_Texture* createTextureFromPixels(SDL_Renderer* renderer, Uint8* pixels, int width, int height) {
    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(pixels, width, height, 24, width * 3, 0xFF0000, 0x00FF00, 0x0000FF, 0);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

int main(int argc, char* argv[]) {
    int WIDTH = 1000;
    int HEIGHT = 600;
    int frame_rate = 1000 / 15;

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }

    SDL_Window* win = SDL_CreateWindow("GAME", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    Uint32 render_flags = SDL_RENDERER_ACCELERATED;
    SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);

    if (TTF_Init() < 0) {
        std::cerr << "SDL_ttf initialization failed: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    TTF_Font* Sans = TTF_OpenFont("assets/lazy.ttf", 24);
    if (!Sans) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        TTF_Quit();
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    const int numPixels = WIDTH * HEIGHT;
    Uint8* pixels = new Uint8[numPixels * 3];

    // Fill the pixel array with RGB values (for example, white)
    for (int i = 0; i < numPixels; ++i) {
        pixels[i * 3] = 255; // Red
        pixels[i * 3 + 1] = 255; // Green
        pixels[i * 3 + 2] = 255; // Blue
    }

    SDL_Texture* player_tex = createTextureFromPixels(rend, pixels, WIDTH, HEIGHT);

    for (int i = 0; i < numPixels; ++i) {
        pixels[i * 3] = 255; // Red
        pixels[i * 3 + 1] = 0; // Green
        pixels[i * 3 + 2] = 0; // Blue
    }

    SDL_Texture* food_tex = createTextureFromPixels(rend, pixels, WIDTH, HEIGHT);

    SDL_Surface* surfaceMessage;
    SDL_Texture* Message;

    SDL_Rect player_dest;
    SDL_Rect food_dest;
    SDL_Rect Message_rect;

    Food food = Food(WIDTH, HEIGHT);
    food_dest.w = food_dest.h = 20;

    Player player = Player(20);
    player_dest.w = player_dest.h = player.size;
    player_dest.x = (WIDTH - player_dest.w) / 2;
    player_dest.y = (HEIGHT - player_dest.h) / 2;
    player.units.push_back(Coordinates(player_dest.x, player_dest.y));

    Message_rect.x = 10;
    Message_rect.y = 10;
    Message_rect.w = 100;
    Message_rect.h = 100;

    int close = 0;
    int velocity = 20;
    int speed_x = 0;
    int speed_y = velocity;

    enum Direction { UP, DOWN, LEFT, RIGHT };
    Direction currentDirection = RIGHT;

    while (!close) {
        SDL_Event event;
        food_dest.x = food.pos_x;
        food_dest.y = food.pos_y;

        player.move(player.units[0].x + speed_x, player.units[0].y + speed_y);

        if ((food.pos_x >= player.units[0].x - player.size) && (food.pos_x <= player.units[0].x + player.size) &&
            (food.pos_y >= player.units[0].y - player.size) && (food.pos_y <= player.units[0].y + player.size)) {
            std::cout << player.units.size() << std::endl;
            player.score += 1;
            food = Food(WIDTH, HEIGHT);;

            SDL_Color rand_color = {(Uint8)(rand() % 255), (Uint8)(rand() % 255), (Uint8)(rand() % 255)};
            std::string updatedMessage = "Score: " + std::to_string(player.score);
            surfaceMessage = TTF_RenderText_Solid(Sans, updatedMessage.c_str(), rand_color);
            Message = SDL_CreateTextureFromSurface(rend, surfaceMessage);
            SDL_FreeSurface(surfaceMessage);
        }

        // right boundary
        if (player.units[0].x + player.size > WIDTH)
            player.units[0].x = 0;

        // left boundary
        if (player.units[0].x < 0)
            player.units[0].x = WIDTH - player.size;

        // bottom boundary
        if (player.units[0].y + player.size > HEIGHT)
            player.units[0].y = 0;

        // upper boundary
        if (player.units[0].y < 0)
            player.units[0].y = HEIGHT - player.size;

        if (player.colliding()) {close = true;}

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                close = 1;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.scancode) {
                case SDL_SCANCODE_W:
                case SDL_SCANCODE_UP:
                    if (currentDirection != DOWN) {
                        speed_x = 0;
                        speed_y = -velocity;
                        currentDirection = UP;
                    }
                    break;
                case SDL_SCANCODE_A:
                case SDL_SCANCODE_LEFT:
                    if (currentDirection != RIGHT) {
                        speed_x = -velocity;
                        speed_y = 0;
                        currentDirection = LEFT;
                    }
                    break;
                case SDL_SCANCODE_S:
                case SDL_SCANCODE_DOWN:
                    if (currentDirection != UP) {
                        speed_x = 0;
                        speed_y = velocity;
                        currentDirection = DOWN;
                    }
                    break;
                case SDL_SCANCODE_D:
                case SDL_SCANCODE_RIGHT:
                    if (currentDirection != LEFT) {
                        speed_x = velocity;
                        speed_y = 0;
                        currentDirection = RIGHT;
                    }
                    break;
                default:
                    break;
                }
            }
        }
 
        SDL_RenderClear(rend);

        SDL_RenderCopy(rend, Message, NULL, &Message_rect);
        SDL_RenderCopy(rend, food_tex, NULL, &food_dest);
        for(int i = 0; i < player.units.size(); i++) {
            SDL_Rect player_dest = {player.units[i].x, player.units[i].y, player.size, player.size};
            SDL_RenderCopy(rend, player_tex, nullptr, &player_dest);
        }
        SDL_RenderPresent(rend);

        SDL_Delay(frame_rate);
    }
 
    SDL_DestroyTexture(player_tex);
    SDL_DestroyTexture(food_tex);
    SDL_DestroyTexture(Message);
    TTF_CloseFont(Sans);
    TTF_Quit();
 
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
 
    return 0;
}

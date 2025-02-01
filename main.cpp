#include <iostream>
#include <windows.h>
#include <vector>
#include <map>
#include "libs/SDL2/include/SDL.h"

constexpr int WINDOW_WIDTH = 640;
constexpr int WINDOW_HEIGHT = 480;

struct Coordinate
{
    Coordinate(int x, int y) : x(x), y(y) {}
    virtual ~Coordinate() = default;
    int x;
    int y;
};

const auto TILE_SIZE = Coordinate(20, 20);
const auto HORIZONTAL_TILE_COUNT = WINDOW_WIDTH / TILE_SIZE.x;
const auto VERTICAL_TILE_COUNT = WINDOW_HEIGHT / TILE_SIZE.y;

struct Snake
{
    Coordinate direction{0, 0};
    std::vector<Coordinate> tiles{Coordinate(HORIZONTAL_TILE_COUNT / 2, VERTICAL_TILE_COUNT / 2)};
    std::map<int, SDL_Color> colors{};
};

void render_snake_tile(SDL_Renderer* renderer, const Coordinate& tile, const SDL_Color color)
{
    SDL_Surface* surface = SDL_CreateRGBSurface(0, TILE_SIZE.x, TILE_SIZE.y, 32, 0, 0, 0, 0);
    const Uint32 uint_color = SDL_MapRGB(surface->format, color.r, color.g, color.b);
    SDL_FillRect(surface, nullptr, uint_color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    const SDL_Rect destination = {tile.x * TILE_SIZE.x, tile.y * TILE_SIZE.y, TILE_SIZE.x, TILE_SIZE.y};
    SDL_RenderCopy(renderer, texture, nullptr, &destination);
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    SDL_Init(SDL_INIT_EVERYTHING);

    // setup window
    SDL_Window* window = SDL_CreateWindow(
        "Snake",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        640, 480,
        SDL_WINDOW_SHOWN
        );

    // setup renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    Snake snake;
    snake.colors[0] = SDL_Color{255, 255, 255};

    bool running = true;
    SDL_Event event;
    // main loop
    while (running)
    {
        // events
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
        }

        // prepare for rendering
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // render snake
        for (int i = 0; i < snake.tiles.size(); ++i)
            render_snake_tile(renderer, snake.tiles[i], snake.colors[i]);

        // update window
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyWindowSurface(window);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
};
#include <iostream>
#include <map>
#include <memory>
#include <random>
#include <vector>
#include <windows.h>
#include "libs/SDL2/include/SDL.h"

int random_int(const int min, const int max)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

struct TextureDeleter {
    void operator()(SDL_Texture* texture) const
    {
        if (texture) {
            SDL_DestroyTexture(texture);
        }
    }
};

constexpr int WINDOW_WIDTH = 640;
constexpr int WINDOW_HEIGHT = 480;

constexpr int GAME_SPEED = 70;

struct Coordinate final
{
    Coordinate(const int x, const int y) : x(x), y(y) {}
    ~Coordinate() = default;
    int x;
    int y;
};

const auto TILE_SIZE = Coordinate(20, 20);
const auto HORIZONTAL_TILE_COUNT = WINDOW_WIDTH / TILE_SIZE.x;
const auto VERTICAL_TILE_COUNT = WINDOW_HEIGHT / TILE_SIZE.y;

struct Snake
{
    Coordinate direction{0, -1};
    std::vector<Coordinate> tiles{Coordinate(HORIZONTAL_TILE_COUNT / 2, VERTICAL_TILE_COUNT / 2)};
    SDL_Texture* head_texture{};
    SDL_Texture* tail_texture{};
    std::map<int, std::unique_ptr<SDL_Texture, TextureDeleter>> textures;
    Uint32 last_update = 0;
};

SDL_Texture* create_tile_texture(SDL_Renderer* renderer, const SDL_Color color)
{
    SDL_Surface* surface = SDL_CreateRGBSurface(0, TILE_SIZE.x, TILE_SIZE.y, 32, 0, 0, 0, 0);
    const Uint32 uint_color = SDL_MapRGB(surface->format, color.r, color.g, color.b);
    SDL_FillRect(surface, nullptr, uint_color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
};

void consume_fruit(Snake& snake)
{
    int max_index = snake.tiles.size() - 1;
    Coordinate difference {snake.tiles[max_index].x - snake.tiles[max_index - 1].x, snake.tiles[max_index].y - snake.tiles[max_index - 1].y};
    snake.tiles.emplace_back(snake.tiles[max_index].x + difference.x, snake.tiles[max_index].y + difference.y);
    snake.textures[snake.tiles.size() - 1] = std::unique_ptr<SDL_Texture, TextureDeleter>(snake.tail_texture);
}

struct Fruit
{
    Fruit(const Coordinate& position, const int texture) : position(position), texture(texture) {}
    ~Fruit() = default;
    Coordinate position;
    int texture;
};

void reset_snake(Snake& snake, SDL_Renderer* renderer)
{
    SDL_Delay(1000);
    snake = Snake();
    snake.head_texture = create_tile_texture(renderer, SDL_Color{255, 0, 0});
    snake.tail_texture = create_tile_texture(renderer, SDL_Color{0, 0, 255});
    snake.textures[0] = std::unique_ptr<SDL_Texture, TextureDeleter>(snake.head_texture);
    for (int i = 1; i < 5; i++)
    {
        snake.tiles.emplace_back(snake.tiles[snake.tiles.size() - 1].x + 0, snake.tiles[snake.tiles.size() - 1].y + 1);
        snake.textures[i] = std::unique_ptr<SDL_Texture, TextureDeleter>(snake.tail_texture);
    }
}

void update_snake_tiles(SDL_Renderer* renderer, Snake& snake, std::vector<std::unique_ptr<Fruit>>& fruits)
{
    const Uint32 current_time = SDL_GetTicks();
    if (current_time - snake.last_update > GAME_SPEED)
    {
        for (int i = 0; i < snake.tiles.size(); i++)
        {
            int current_index = (snake.tiles.size() - i) - 1;
            if (current_index != 0)
            {
                snake.tiles[current_index].x = snake.tiles[current_index - 1].x;
                snake.tiles[current_index].y = snake.tiles[current_index - 1].y;
            } else
            {
                snake.tiles[current_index].x += snake.direction.x;
                snake.tiles[current_index].y += snake.direction.y;
            }

            if (snake.tiles[i].y > VERTICAL_TILE_COUNT)
            {
                snake.tiles[i].y = 0;
            }
            if (snake.tiles[i].y < 0)
            {
                snake.tiles[i].y = VERTICAL_TILE_COUNT;
            }
            if (snake.tiles[i].x > HORIZONTAL_TILE_COUNT)
            {
                snake.tiles[i].x = 0;
            }
            if (snake.tiles[i].x < 0)
            {
                snake.tiles[i].x = HORIZONTAL_TILE_COUNT;
            }
            if (i != 0 && snake.tiles[0].x == snake.tiles[i].x && snake.tiles[0].y == snake.tiles[i].y)
            {
                reset_snake(snake, renderer);
            }
        }
        for (int j = 0; j < fruits.size(); j++)
        {
            if (snake.tiles[0].x == fruits[j]->position.x && snake.tiles[0].y == fruits[j]->position.y)
            {
                fruits[j]->position = Coordinate(random_int(0, HORIZONTAL_TILE_COUNT), random_int(0, VERTICAL_TILE_COUNT));
                consume_fruit(snake);
            }
        }
        snake.last_update = SDL_GetTicks();
    }
}

void render_tile(SDL_Renderer* renderer, const Coordinate& tile, SDL_Texture* texture)
{
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

    // setup snake
    Snake snake;
    snake.head_texture = create_tile_texture(renderer, SDL_Color{255, 0, 0});
    snake.tail_texture = create_tile_texture(renderer, SDL_Color{0, 0, 255});

    // generate snake head and tail
    snake.textures[0] = std::unique_ptr<SDL_Texture, TextureDeleter>(snake.head_texture);
    for (int i = 1; i < 5; i++)
    {
        snake.tiles.emplace_back(snake.tiles[snake.tiles.size() - 1].x + 0, snake.tiles[snake.tiles.size() - 1].y + 1);
        snake.textures[i] = std::unique_ptr<SDL_Texture, TextureDeleter>(snake.tail_texture);
    }

    // setup fruits
    std::vector<std::unique_ptr<Fruit>> fruits;
    std::map<int, std::unique_ptr<SDL_Texture, TextureDeleter>> fruit_textures;
    fruit_textures[0] = std::unique_ptr<SDL_Texture, TextureDeleter>(create_tile_texture(renderer, SDL_Color{150, 150, 150}));
    int max_fruits = 8;
    for (int j = 0; j < max_fruits; j++)
    {
        fruits.push_back(std::unique_ptr<Fruit>(new Fruit(Coordinate(random_int(0, HORIZONTAL_TILE_COUNT), random_int(0, VERTICAL_TILE_COUNT)), 0)));
    }

    bool running = true;
    SDL_Event event;
    Coordinate temp_direction{0, -1};
    // main loop
    SDL_Delay(1000);
    while (running)
    {
        // events
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            } else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_UP && snake.direction.y != 1)
                {
                    temp_direction.x = 0;
                    temp_direction.y = -1;
                }
                else if (event.key.keysym.sym == SDLK_DOWN && snake.direction.y != -1)
                {
                    temp_direction.x = 0;
                    temp_direction.y = 1;
                }
                else if (event.key.keysym.sym == SDLK_LEFT && snake.direction.x != 1)
                {
                    temp_direction.x = -1;
                    temp_direction.y = 0;
                }
                else if (event.key.keysym.sym == SDLK_RIGHT && snake.direction.x != -1)
                {
                    temp_direction.x = 1;
                    temp_direction.y = 0;
                }
            }
        }
        if (SDL_GetTicks() - snake.last_update > GAME_SPEED)
        {
            snake.direction = temp_direction;
        }

        // update snake
        update_snake_tiles(renderer, snake, fruits);

        // prepare for rendering
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // render snake
        for (int i = 0; i < snake.tiles.size(); ++i)
        {
            render_tile(renderer, snake.tiles[i], snake.textures[i].get());
        }
        for (int j = 0; j < fruits.size(); j++)
        {
            render_tile(renderer, fruits[j]->position, fruit_textures[fruits[j]->texture].get());
        }

        // update window
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyWindowSurface(window);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
};
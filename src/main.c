#include "raylib.h"
#include <stdbool.h>
// #include <stdio.h>
#include <stdio.h>
#include <stdlib.h>

#define FPS 60
#define HIGHT_RESOLUTION 800
#define WIDTH_RESOLUTION 800
#define N_COLUMNS                                                              \
  25 // allowed [2, 4, 5, 8, 10, 16, 20, 25, 32, 40, 50, 80, 100, 160, 200]
#define N_LINES                                                                \
  25 // allowed [2, 4, 5, 8, 10, 16, 20, 25, 32, 40, 50, 80, 100, 160, 200]
#define tile_width HIGHT_RESOLUTION / N_COLUMNS
#define tile_height WIDTH_RESOLUTION / N_LINES
#define MUTE_GRAY (Color){82, 82, 82, 255}
#define MUTE_WHITE (Color){112, 112, 112, 255}
#define MUTE_RED (Color){212, 112, 112, 255}
#define MUTE_GREEN (Color){112, 172, 102, 255}

typedef struct {
  Color color;
  bool food;
  bool snake;
} Tile;

typedef struct {
  int row;
  int col;
} SnakeSegment;

SnakeSegment *init_snake() {
  SnakeSegment *snake = malloc(N_COLUMNS * N_LINES * sizeof(SnakeSegment));
  return snake;
}

void handle_boundaries(SnakeSegment *snake, int snake_size) {
  if ((snake[snake_size - 1].row > N_LINES) ||
      (snake[snake_size - 1].col > N_COLUMNS)) {
    printf("OUT OF BOUNDARIES\n");
  }
}

int start_snake(SnakeSegment *snake) {
  int snake_size = 2;
  snake[0].row = 0;
  snake[0].col = 0;
  snake[1].row = 0;
  snake[1].col = 1;
  return snake_size;
}

// Dont code, just try to understand what is this (math : movment propagation)
// Also figure out why tf the snake is being erased at the start

void input_handler(SnakeSegment *snake, int snake_size) {
  SnakeSegment head = snake[0];
  for (int i = snake_size - 1; i == 1; i--) {
    snake[i] = snake[i - 1];
  }
  if (IsKeyPressed(KEY_D)) {
    snake[0].col = head.col + 1;
    snake[0].row = head.row;
  }
  if (IsKeyPressed(KEY_S)) {
    snake[0].col = head.col;
    snake[0].row = head.row + 1;
  }
  if (IsKeyPressed(KEY_A)) {
    snake[0].col = head.col - 1;
    snake[0].row = head.row;
  }
  if (IsKeyPressed(KEY_W)) {
    snake[0].col = head.col;
    snake[0].row = head.row - 1;
  }
}

void test_straight_path(SnakeSegment *snake, int snake_size) {
  for (int i = 0; i < snake_size; i++) {
    snake[i].col += 1;
  }
}
void update_grid(Tile **grid, SnakeSegment *snake, int snake_size) {
  for (int i = 0; i < N_LINES; i++) {
    for (int j = 0; j < N_COLUMNS; j++) {
      grid[i][j].snake = false;
    }
  }
  for (int i = 0; i < snake_size; i++) {
    int row = snake[i].row;
    int col = snake[i].col;
    grid[row][col].snake = true;
  }
}

Tile **init_grid() {
  Tile **grid = malloc(N_LINES * sizeof(Tile *));
  for (int i = 0; i < N_LINES; i++) {
    grid[i] = malloc(N_COLUMNS * sizeof(Tile));
    for (int j = 0; j < N_COLUMNS; j++) {
      grid[i][j].color = MUTE_GRAY;
      grid[i][j].food = false;
      grid[i][j].snake = false;
    }
  }
  return grid;
}

void draw_grid(Tile **grid) {
  for (int i = 0; i < N_LINES; i++) {
    for (int j = 0; j < N_COLUMNS; j++) {
      int x_start = j * tile_width;
      int y_start = i * tile_height;
      Color color = ((i + j) % 2 == 0) ? grid[i][j].color : MUTE_WHITE;
      if (grid[i][j].snake) {
        color = MUTE_RED;
      } else if (grid[i][j].food) {
        color = MUTE_GREEN;
      }
      DrawRectangle(x_start, y_start, tile_width, tile_height, color);
    }
  }
}

void run() {
  SetTargetFPS(FPS);
  InitWindow(HIGHT_RESOLUTION, WIDTH_RESOLUTION, "title");
  Tile **grid = init_grid();
  SnakeSegment *snake = init_snake();
  int snake_size = start_snake(snake);
  int frames = 0;

  while (!WindowShouldClose()) {
    BeginDrawing();
    frames++;
    input_handler(snake, snake_size);
    update_grid(grid, snake, snake_size);
    draw_grid(grid);
    EndDrawing();
  }
  CloseWindow();
  free(grid);
}

int main() {
  run();
  return 0;
}
#include "raylib.h"
#include <stdbool.h>
// #include <stdio.h>
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
  grid[4][4].snake = true;
  grid[2][3].food = true;
  while (!WindowShouldClose()) {
    BeginDrawing();
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
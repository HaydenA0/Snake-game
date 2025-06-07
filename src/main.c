#include "raylib.h"
// #include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>

#define FPS 60
#define HEIGHT_RESOLUTION 800
#define WIDTH_RESOLUTION 800
#define N_COLUMNS                                                              \
  20 // allowed [2, 4, 5, 8, 10, 16, 20, 25, 32, 40, 50, 80, 100, 160, 200]
#define N_LINES                                                                \
  20 // allowed [2, 4, 5, 8, 10, 16, 20, 25, 32, 40, 50, 80, 100, 160, 200]
#define tile_width WIDTH_RESOLUTION / N_COLUMNS
#define tile_height HEIGHT_RESOLUTION / N_LINES
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
} Segment;

void debug(Segment *snake, int snake_size, char *last_move) {
  for (int i = 0; i < snake_size; i++) {
    printf("The position of the %d th element of the snake is : (%d, %d)\n", i,
           snake[i].row, snake[i].col);
  }
  printf("---------\n");
  printf("%s\n", last_move);
}
Segment *init_snake() {
  Segment *snake = malloc(N_COLUMNS * N_LINES * sizeof(Segment));
  return snake;
}

void boundaries_check(Segment *snake, int snake_size) {
  for (int i = 0; i < snake_size; i++) {
    if (snake[i].row >= N_LINES || snake[i].col >= N_COLUMNS ||
        snake[i].row < 0 || snake[i].col < 0) {
      CloseWindow();
      exit(0);
    }
  }
}

int same_segmant(Segment segment1, Segment segment2) {
  return segment1.row == segment2.row && segment1.col == segment2.col;
}

void collision_check(Segment *snake, int snake_size) {
  for (int i = 0; i < snake_size - 2; i++) {
    if (same_segmant(snake[snake_size - 1], snake[i])) {
      printf("Collision happened\n");
      CloseWindow();
      exit(0);
    }
  }
}

void exiter(Segment *snake, int snake_size) {

  boundaries_check(snake, snake_size);
  collision_check(snake, snake_size);
}

int start_snake(Segment *snake) {
  int snake_size = 3;
  snake[0].row = N_LINES / 2;
  snake[0].col = N_COLUMNS / 2;
  snake[1].row = snake[0].row + 1;
  snake[1].col = snake[0].col;
  snake[2].row = snake[1].row + 1;
  snake[2].col = snake[0].col;
  return snake_size;
}

void move_left(Segment *snake, int snake_size) {
  Segment remember = snake[snake_size - 1];
  snake[snake_size - 1].col += 1;
  for (int i = snake_size - 2; i >= 0; i--) {
    Segment temp = snake[i];
    snake[i] = remember;
    remember = temp;
  }
}
void move_right(Segment *snake, int snake_size) {
  Segment remember = snake[snake_size - 1];
  snake[snake_size - 1].col -= 1;
  for (int i = snake_size - 2; i >= 0; i--) {
    Segment temp = snake[i];
    snake[i] = remember;
    remember = temp;
  }
}
void move_up(Segment *snake, int snake_size) {
  Segment remember = snake[snake_size - 1];
  snake[snake_size - 1].row -= 1;
  for (int i = snake_size - 2; i >= 0; i--) {
    Segment temp = snake[i];
    snake[i] = remember;
    remember = temp;
  }
}
void move_down(Segment *snake, int snake_size) {
  Segment remember = snake[snake_size - 1];
  snake[snake_size - 1].row += 1;
  for (int i = snake_size - 2; i >= 0; i--) {
    Segment temp = snake[i];
    snake[i] = remember;
    remember = temp;
  }
}
void input_handler(Segment *snake, int snake_size, char *last_move) {
  if (IsKeyPressed(KEY_D) && strcmp(last_move, "right") != 0) {
    move_left(snake, snake_size);
    strncpy(last_move, "left", 5);
  }
  if (IsKeyPressed(KEY_A) && strcmp(last_move, "left") != 0) {
    move_right(snake, snake_size);
    strncpy(last_move, "right", 5);
  }
  if (IsKeyPressed(KEY_W) && strcmp(last_move, "down") != 0) {
    move_up(snake, snake_size);
    strncpy(last_move, "up", 5);
  }
  if (IsKeyPressed(KEY_S) && strcmp(last_move, "up") != 0) {
    move_down(snake, snake_size);
    strncpy(last_move, "down", 5);
  }
}

void movement_handler(Segment *snake, int snake_size, char *last_move) {
  // snake[snake_size-1] != snake[snake_size-2]
  if ((strcmp(last_move, "left") == 0)) {
    move_left(snake, snake_size);
  }
  if ((strcmp(last_move, "right") == 0)) {
    move_right(snake, snake_size);
  }
  if ((strcmp(last_move, "up") == 0)) {
    move_up(snake, snake_size);
  }
  if ((strcmp(last_move, "down") == 0)) {
    move_down(snake, snake_size);
  }
}

int exists_in_snake(Segment tocheck, Segment *snake, int snake_size) {
  for (int i = 0; i < snake_size; i++) {
    if (same_segmant(snake[i], tocheck)) {
      return 1;
    }
  }
  return 0;
}

Segment random_food_position(Segment *snake, int snake_size) {
  Segment result;
  result.col = GetRandomValue(0, N_COLUMNS - 1);
  result.row = GetRandomValue(0, N_LINES - 1);
  while (exists_in_snake(result, snake, snake_size)) {
    result.col = GetRandomValue(0, N_COLUMNS - 1);
    result.row = GetRandomValue(0, N_LINES - 1);
  }
  return result;
}

void update_food(Segment *food_ptr, Segment *snake, int *snake_size_ptr,
                 int *eaten_food) {
  if (same_segmant(*food_ptr, snake[*snake_size_ptr - 1])) {
    *eaten_food += 1;
    *snake_size_ptr += 1;
    snake[*snake_size_ptr - 1] = *food_ptr;
    *food_ptr = random_food_position(snake, *snake_size_ptr);
  }
}

void update_grid(Tile **grid, Segment food, Segment *snake, int snake_size) {
  for (int i = 0; i < N_LINES; i++) {
    for (int j = 0; j < N_COLUMNS; j++) {
      grid[i][j].snake = false;
      grid[i][j].food = false;
    }
  }
  for (int i = 0; i < snake_size; i++) {
    int row = snake[i].row;
    int col = snake[i].col;
    grid[row][col].snake = true;
  }
  grid[food.row][food.col].food = true;
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

void draw_grid(Tile **grid, int Score) {
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
  char Score_string[120];
  sprintf(Score_string, "%d", Score);
  DrawText(Score_string, 10, 10, 30, WHITE); // drawing the score
}

void collect_garbage(Tile **grid, Segment *snake) {

  free(snake);
  for (int i = 0; i < N_LINES; i++) {
    free(grid[i]);
  }
  free(grid);
}

void debugger(Tile **grid, Segment *snake, Segment food, int snake_size,
              int frames, const char *last_move, int Score, float alpha,
              int eaten_food, float quota, int round_frames) {
  printf("=== RUN PARAMETERS ===\n");
  printf("snake_size: %d\n", snake_size);
  printf("frames: %d\n", frames);
  printf("last_move: %s\n", last_move);
  printf("food position: (%d, %d)\n", food.row, food.col);
  printf("Score: %d\n", Score);
  printf("alpha: %.2f\n", alpha);
  printf("eaten_food: %d\n", eaten_food);
  printf("quota: %.2f\n", quota);
  printf("round_frames: %d\n", round_frames);
  printf("======================\n");
}

void run() {
  // Set frame rate and initialize window
  SetTargetFPS(FPS);
  InitWindow(HEIGHT_RESOLUTION, WIDTH_RESOLUTION, "title");

  // Initialize game elements
  Tile **grid = init_grid();
  Segment *snake = init_snake();
  Segment food;
  int snake_size = start_snake(snake);
  int frames = 0;
  char last_move[6] = "none";
  food.col = 2;
  food.row = 4;
  int Score = 0;
  float alpha = 2.0;
  float beta = 0.0012;
  int eaten_food = 0;
  float quota = 2;
  int round_frames = 300;

  // Main game loop
  while (!WindowShouldClose()) {
    frames++;
    if (frames % 60 == 0) {
      debugger(grid, snake, food, snake_size, frames, last_move, Score, alpha,
               eaten_food, quota, round_frames);
    }
    BeginDrawing();

    // Increment frame count
    // Update game logic
    update_grid(grid, food, snake, snake_size);
    input_handler(snake, snake_size, last_move);
    update_food(&food, snake, &snake_size, &eaten_food);

    // Move snake at fixed intervals
    if (frames % 10 == 0) {
      movement_handler(snake, snake_size, last_move);
    }

    // Score calculation and frame reset
    if (frames == round_frames) {
      if (eaten_food < quota) {
        Score -= (int)(Score / 2 + Score / (2 * (1 + beta * Score)));
      } else {
        Score += floorf(1000.0 * (float)eaten_food /
                        log(1.0 + alpha + (float)frames));
      }
      frames = 0;
      eaten_food = 0;
    }

    // Check for exit conditions
    exiter(snake, snake_size);

    // Render the game
    draw_grid(grid, Score);
    EndDrawing();
  }
  // Debugger :
  // Clean up and show final score
  printf("Score is : %d \n", Score);
  CloseWindow();
  collect_garbage(grid, snake);
}

int main() {
  run();
  return 0;
}

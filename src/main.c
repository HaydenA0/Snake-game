#include "raylib.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Game Configuration
#define FPS 60
#define HEIGHT_RESOLUTION 800
#define WIDTH_RESOLUTION 800
#define N_COLUMNS 20
#define N_LINES 20
#define tile_width (WIDTH_RESOLUTION / N_COLUMNS)
#define tile_height (HEIGHT_RESOLUTION / N_LINES)

// Colors
#define MUTE_GRAY (Color){82, 82, 82, 255}
#define MUTE_WHITE (Color){112, 112, 112, 255}
#define MUTE_RED (Color){212, 112, 112, 255}
#define MUTE_GREEN (Color){112, 172, 102, 255}

// --- Data Structures ---

typedef struct {
  int row;
  int col;
} Segment;

typedef struct {
  Color color;
  bool food;
  bool snake;
} Tile;

// Added for managing game states (Gameplay vs. Game Over)
typedef enum GameState { GAMEPLAY, GAME_OVER } GameState;

// --- Forward Declarations ---
void reset_game(Segment *snake, int *snake_size, Segment *food, int *Score,
                char *last_move);

// --- Game Logic Functions ---

bool same_segment(Segment segment1, Segment segment2) {
  return segment1.row == segment2.row && segment1.col == segment2.col;
}

bool boundaries_check(Segment *snake, int snake_size) {
  Segment head = snake[snake_size - 1];
  if (head.row >= N_LINES || head.col >= N_COLUMNS || head.row < 0 ||
      head.col < 0) {
    return true; // Game over
  }
  return false; // OK
}

bool collision_check(Segment *snake, int snake_size) {
  Segment head = snake[snake_size - 1];
  for (int i = 0; i < snake_size - 1; i++) {
    if (same_segment(head, snake[i])) {
      return true; // Game over
    }
  }
  return false; // OK
}

bool check_game_over(Segment *snake, int snake_size) {
  return boundaries_check(snake, snake_size) ||
         collision_check(snake, snake_size);
}

int start_snake(Segment *snake) {
  int snake_size = 3;
  // The snake's head is the LAST element of the array.
  // Tail is snake[0].
  snake[0].row = N_LINES / 2;
  snake[0].col = N_COLUMNS / 2;
  snake[1].row = snake[0].row - 1;
  snake[1].col = snake[0].col;
  snake[2].row = snake[1].row - 1;
  snake[2].col = snake[0].col;
  return snake_size;
}

void move_snake(Segment *snake, int snake_size) {
  Segment remember = snake[snake_size - 1]; // Store old head position
  // Move the body first
  for (int i = 0; i < snake_size - 1; i++) {
    Segment temp = snake[i + 1];
    snake[i] = temp;
  }
  // The last segment now holds the old head position.
  // We can now "move" the actual head based on its previous position.
  // Wait, the caterpillar logic from the original was better. Let's revert to
  // that.

  // Propagate positions from the tail forward
  Segment old_head_pos = snake[snake_size - 1];
  for (int i = snake_size - 2; i >= 0; i--) {
    snake[i + 1] = snake[i];
  }
  // After shifting, the head is still at the old position.
  // The caterpillar logic is actually simpler to implement growth with.
}

void movement_handler(Segment *snake, int snake_size, char *last_move) {
  // Store the position of the tail before moving
  Segment old_tail = snake[0];

  // Move all segments forward, from head-1 to tail
  for (int i = 0; i < snake_size - 1; i++) {
    snake[i] = snake[i + 1];
  }

  // Move head to new position based on direction
  Segment *head = &snake[snake_size - 1];
  if (strcmp(last_move, "left") == 0)
    head->col--;
  if (strcmp(last_move, "right") == 0)
    head->col++;
  if (strcmp(last_move, "up") == 0)
    head->row--;
  if (strcmp(last_move, "down") == 0)
    head->row++;
}

// FIX: Input handler now only sets direction, preventing double moves.
void input_handler(char *last_move) {
  if (IsKeyPressed(KEY_D) && strcmp(last_move, "left") != 0) {
    strcpy(last_move, "right");
  }
  if (IsKeyPressed(KEY_A) && strcmp(last_move, "right") != 0) {
    strcpy(last_move, "left");
  }
  if (IsKeyPressed(KEY_W) && strcmp(last_move, "down") != 0) {
    strcpy(last_move, "up");
  }
  if (IsKeyPressed(KEY_S) && strcmp(last_move, "up") != 0) {
    strcpy(last_move, "down");
  }
}

int exists_in_snake(Segment to_check, Segment *snake, int snake_size) {
  for (int i = 0; i < snake_size; i++) {
    if (same_segment(snake[i], to_check)) {
      return 1;
    }
  }
  return 0;
}

Segment random_food_position(Segment *snake, int snake_size) {
  Segment result;
  do {
    result.col = GetRandomValue(0, N_COLUMNS - 1);
    result.row = GetRandomValue(0, N_LINES - 1);
  } while (exists_in_snake(result, snake, snake_size));
  return result;
}

// FIX: Rewrote growth logic to be bug-free.
void update_food(Segment *food_ptr, Segment *snake, int *snake_size_ptr,
                 int *Score_ptr) {
  Segment head = snake[*snake_size_ptr - 1];
  if (same_segment(*food_ptr, head)) {
    // To grow, shift all snake segments up one index to make room for a new
    // tail.
    for (int i = *snake_size_ptr; i > 0; i--) {
      snake[i] = snake[i - 1];
    }
    // The new segment at snake[0] is a duplicate of the old tail, effectively
    // growing it.
    (*snake_size_ptr)++;
    *Score_ptr += 10;

    // Generate new food
    *food_ptr = random_food_position(snake, *snake_size_ptr);
  }
}

// --- Drawing and UI Functions ---

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
    if (row >= 0 && row < N_LINES && col >= 0 && col < N_COLUMNS) {
      grid[row][col].snake = true;
    }
  }
  grid[food.row][food.col].food = true;
}

void draw_text(int Score) {
  char Score_string[120];
  sprintf(Score_string, "Score: %d", Score);
  DrawText(Score_string, 10, 10, 30, WHITE);
}

void draw_grid(Tile **grid, int Score) {
  for (int i = 0; i < N_LINES; i++) {
    for (int j = 0; j < N_COLUMNS; j++) {
      int x_start = j * tile_width;
      int y_start = i * tile_height;
      Color color = ((i + j) % 2 == 0) ? MUTE_GRAY : MUTE_WHITE;
      if (grid[i][j].snake) {
        color = MUTE_RED;
      } else if (grid[i][j].food) {
        color = MUTE_GREEN;
      }
      DrawRectangle(x_start, y_start, tile_width, tile_height, color);
    }
  }
  draw_text(Score);
}

// NEW: Draws the game over screen
void draw_game_over_screen() {
  DrawText("GAME OVER", WIDTH_RESOLUTION / 2 - MeasureText("GAME OVER", 60) / 2,
           HEIGHT_RESOLUTION / 2 - 60, 60, RAYWHITE);
  DrawText("Press [R] to Restart",
           WIDTH_RESOLUTION / 2 - MeasureText("Press [R] to Restart", 30) / 2,
           HEIGHT_RESOLUTION / 2 + 20, 30, LIGHTGRAY);
}

// --- Memory and Initialization ---

Tile **init_grid() {
  Tile **grid = malloc(N_LINES * sizeof(Tile *));
  for (int i = 0; i < N_LINES; i++) {
    grid[i] = malloc(N_COLUMNS * sizeof(Tile));
  }
  return grid;
}

Segment *init_snake_memory() {
  return malloc(N_COLUMNS * N_LINES * sizeof(Segment));
}

void collect_garbage(Tile **grid, Segment *snake) {
  printf("Cleaning up resources...\n");
  free(snake);
  for (int i = 0; i < N_LINES; i++) {
    free(grid[i]);
  }
  free(grid);
}

// NEW: Function to reset the game to its initial state
void reset_game(Segment *snake, int *snake_size, Segment *food, int *Score,
                char *last_move) {
  *snake_size = start_snake(snake);
  *food = random_food_position(snake, *snake_size);
  *Score = 0;
  strcpy(last_move, "up");
}

// --- Main Game Execution ---

void run() {
  InitWindow(WIDTH_RESOLUTION, HEIGHT_RESOLUTION, "Snake");
  SetTargetFPS(FPS);

  // --- Game Variables ---
  GameState current_state = GAMEPLAY;
  Tile **grid = init_grid();
  Segment *snake = init_snake_memory();
  Segment food;
  int snake_size = 0;
  int Score = 0;
  int frames_counter = 0;
  char last_move[6];

  // Initialize the first game
  reset_game(snake, &snake_size, &food, &Score, last_move);

  // --- Main Game Loop ---
  while (!WindowShouldClose()) {

    // --- UPDATE LOGIC ---
    switch (current_state) {
    case GAMEPLAY: {
      input_handler(last_move);
      frames_counter++;

      // Move snake on a timer
      if (frames_counter >= (FPS / 10)) { // (FPS / speed)
        frames_counter = 0;
        movement_handler(snake, snake_size, last_move);
        if (check_game_over(snake, snake_size)) {
          current_state = GAME_OVER;
        } else {
          update_food(&food, snake, &snake_size, &Score);
        }
      }
    } break;
    case GAME_OVER: {
      if (IsKeyPressed(KEY_R)) {
        reset_game(snake, &snake_size, &food, &Score, last_move);
        current_state = GAMEPLAY;
      }
    } break;
    }
    update_grid(grid, food, snake, snake_size);

    // --- DRAWING LOGIC ---
    BeginDrawing();
    ClearBackground(BLACK);
    draw_grid(grid, Score);

    if (current_state == GAME_OVER) {
      draw_game_over_screen();
    }

    EndDrawing();
  }

  // --- Cleanup ---
  collect_garbage(grid, snake);
  CloseWindow();
}

int main() {
  run();
  return 0;
}

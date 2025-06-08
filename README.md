# Snake Game (Raylib, C)

A minimal, grid-based Snake game built using [Raylib](https://www.raylib.com/). The game runs at a fixed resolution with simple tile graphics and clean movement mechanics.

## Features

- Simple and efficient game loop with state management
- Clear separation of logic (input, movement, collisions, UI)
- Solid snake movement and growth logic
- Grid-based rendering using muted color palettes
- Basic score tracking and game over screen
- Press `R` to restart the game after dying

![Uploading image.png…]()
![Uploading image.png…]()


## Controls

- `W` / `A` / `S` / `D` — Control snake direction  
- `R` — Restart after game over  
- `ESC` — Exit the game

## Requirements

- [Raylib](https://www.raylib.com/) (C library for game development)

Tested with:
- Raylib version 5.0
- GCC on Linux (should work on Windows/Mac with minimal changes)

## Build & Run

Make sure Raylib is installed and properly linked. Then compile with:

```bash
gcc -o snake main.c -lraylib -lm
./snake

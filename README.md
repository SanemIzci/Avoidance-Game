# 🚀 Avoidance Game

## 🎮 Arcade Style Dodge Game (Raylib)

This project is a *fast-paced avoidance game* developed using *Raylib*. The player controls a green square and must dodge falling obstacles while collecting power-ups to survive as long as possible and achieve high scores.

## ✨ Features
- Developed using *Raylib* with 2D graphics
- Randomly generated obstacles with different shapes:
  - Squares (Red)
  - Circles (Red)
  - Triangles (Red)
- Dynamic difficulty system:
  - Level increases every 15 seconds
  - Obstacle speed increases with each level
  - Zigzag movement pattern for obstacles after level 4
- Power-up system with 6 different types:
  - Speed Boost (Green)
  - Block Reset (Blue)
  - Invisibility (Yellow)
  - Time Slow (Cyan)
  - Shield (Magenta)
  - Extra Life (Pink/Purple)
- Particle effects:
  - Health pickup particles (Pink)
  - Shield break particles (Sky Blue)
- Smooth gameplay at 60 FPS
- Score system based on dodged obstacles
- Maximum of 5 lives
- Maximum of 5 power-ups on screen at once

## 🛠 Requirements
To run this project, you need the following installed on your system:

- *C++ Compiler* 
- *Raylib* library

### 📌 Windows Setup:
1. Download and install *Raylib*
2. Link the library in your C++ project
3. Compile with the following command:
```sh
g++ -std=c++17 main.cpp -o raylib_game.exe -I C:/raylib/raylib/src -L C:/raylib/raylib/src -lraylib -lopengl32 -lgdi32 -lwinmm -lstdc++
$ ./raylib_game.exe
```

## 🎯 Game Mechanics

### Level Progression
- Each level lasts 15 seconds
- Obstacle speed increases with each level
- After level 4, obstacles gain zigzag movement
- Power-up spawn rates adjust based on level:
  - Level 1-3: 1/500 chance
  - Level 4-5: 1/300 chance
  - Level 6-8: 1/200 chance
  - Level 9+: 1/100 chance

### Power-Up Mechanics
| Power-Up | Duration | Effect |
|:---------|:---------|:-------|
| Speed Boost | 20 seconds | Increases movement speed by 3 units |
| Block Reset | 20 seconds | Clears and respawns all obstacles |
| Invisibility | 20 seconds | Makes player immune to obstacles |
| Time Slow | 15 seconds | Slows down obstacle movement |
| Shield | 10 seconds | Absorbs one hit |
| Extra Life | Instant | Adds one life (max 5) |

### Scoring System
- +1 point for each successfully dodged obstacle
- Score increases with level progression
- Final score displayed on game over screen

## 🎮 Controls
| Key | Action |
|-----|--------|
| Left Arrow | Move left |
| Right Arrow | Move right |
| Enter | Start game / Return to menu |

## 📂 Project Structure
The game is contained in a single `main.cpp` file with the following logical sections:

- Game initialization and reset functions
- Obstacle management functions
- Power-up system functions
- Particle effect functions
- Main game loop and rendering

## 🐞 Known Issues
- No sound effect
- No pause functionality

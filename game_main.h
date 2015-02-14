#ifndef GAME_MAIN_H
#define GAME_MAIN_H

#include "game_window.h"
#include "vec2d.h"
#include <SDL.h>

#define QUIT_GAME 1
#define LEFT_DOWN 2

class Entity
{
 public:
  vec2d position;
  vec2d velocity;
  int sprite;

  void update(float dt);
  
  virtual bool check_in_bounds(int screenWidth, int screenHeight);

  //Entity(vec2d position);
  //Entity(vec2d position, int sprite);
  Entity(vec2d position, vec2d velocity, int sprite);

};

class AABB : public Entity
{
 public:

  vec2d dimensions;
  bool check_in_bounds(int screenWidth, int screenHeight);
  AABB(vec2d position, vec2d dimensions, vec2d velocity, int sprite);

};

class Game
{
 private:

  bool gameOver;

  GameWindow gameWindow;
  
  void handle_input();
  void update(float dt);
  void render();

  SDL_Event event;
  
 public:

  Game();
  void run();

};

#endif //GAME_MAIN_H

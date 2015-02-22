#ifndef GAME_MAIN_H
#define GAME_MAIN_H

#include "game_window.h"
#include "vec2d.h"
#include <SDL.h>
#include <vector>

class Entity
{
 public:
  vec2d position;
  vec2d velocity;
  int sprite;

  void update(float dt);
  
  virtual bool check_in_bounds(float x, float y, float w, float h);

  //Entity(vec2d position);
  //Entity(vec2d position, int sprite);
  Entity(vec2d position, vec2d velocity, int sprite);

};

class Quadtree
{
 private:

  float _x;
  float _y;
  float _w;
  float _h;

  Quadtree *northEast;
  Quadtree *northWest;
  Quadtree *southEast;
  Quadtree *southWest;

  int maxCapacity;

  std::vector<Entity *> contents;

  void subdivide();

 public:

  Quadtree(float x, float y, float w, float h);
  ~Quadtree();
  void insert(Entity *element);
  std::vector<Entity *> get_in_range(float x, float y, float w, float h);

};

class AABB : public Entity
{
 public:

  vec2d dimensions;
  virtual bool check_in_bounds(float x, float y, float w, float h);
  AABB(vec2d position, vec2d dimensions, vec2d velocity, int sprite);

};

class KeyboardState
{

 public:

  bool left;
  bool right;
  bool up;
  bool down;
  bool space;

  KeyboardState();

};

class Game
{
 private:

  bool gameOver;

  std::vector<Entity *> gameEntities;
  Quadtree *quadtree;

  KeyboardState keyboardState;
  GameWindow gameWindow;
  
  void handle_input();
  void update(float dt);
  void render();
  
 public:

  Game();
  void run();

};

#endif //GAME_MAIN_H

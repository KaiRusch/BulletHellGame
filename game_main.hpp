#ifndef GAME_MAIN_H
#define GAME_MAIN_H

#include "game_window.hpp"
#include "vec2d.hpp"
#include "quadtree.hpp"
#include "entity.hpp"
#include <vector>
#include <stdint.h>


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

#define ENTITY 0
#define BULLET 1
#define SPACESHIP 2

class Bullet : public Entity
{
protected:

  int allegiance;
  int damage;

public:

  Bullet(vec2d position, vec2d dimensions, vec2d velocity, int sprite, int damage, int allegiance);
  int damage_done(int spaceshipAllegiance); 
 
};

class Spaceship : public Entity
{
protected:
  
  int allegiance;
  int health;

public:

  Spaceship(vec2d position, vec2d dimensions, vec2d velocity, int sprite, int health, int allegiance);

  void check_collisions(Quadtree *quadtree);

};


class Game
{
 private:

  bool gameOver;

  std::vector<Entity *> gameEntities;
  std::vector<Entity *> newEntities;
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

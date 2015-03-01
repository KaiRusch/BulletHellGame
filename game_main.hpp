#ifndef GAME_MAIN_H
#define GAME_MAIN_H

#include "game_window.hpp"
#include "vec2d.hpp"
#include "quadtree.hpp"
#include "entity.hpp"
#include <vector>

class Animation
{
public:

  std::vector<int> frames;
  bool playing;
  float speed;
  float delay;
  int currentFrameIndex;

  Animation(std::vector<int> frames, float speed) : frames(frames), playing(false), speed(speed), delay(0.0f), currentFrameIndex(0)
  {
  };
  
  void start()
  {
    playing = true;
  };
  
  int update(float dt)
  {
    if(playing)
      {
	delay += dt;
	if(delay >= speed)
	  {
	    currentFrameIndex++;
	    delay = 0.0f;
	  }
      }

    if(currentFrameIndex == frames.size())
      {
	return -1;
      }

     return frames[currentFrameIndex];

  };
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

//Types of entities
#define ENTITY 0
#define BULLET 1
#define SPACESHIP 2
#define PLAYER 4
#define PICKUP 8

//Allegiances
#define GOOD 0
#define EVIL 1

//Pickups
#define LIFE 0
#define DOUBLE 1
#define TRIPLE 2

//Firing Modes
#define SINGLE_FIRE 0
#define DOUBLE_FIRE 1
#define TRIPLE_FIRE 2

class Bullet : public Entity
{

public:

  int allegiance;
  int damage;

  Bullet(vec2d position, vec2d dimensions, vec2d velocity, int sprite, int damage, int allegiance);
};

class Pickup : public Entity
{

protected:

public:

  int pickupType;

  Pickup(vec2d position, vec2d dimensions, vec2d velocity, int sprite, int pickupType);
};

class Spaceship : public Entity
{

protected:

  std::vector<Entity *> *gameEntities;
  
public:

  int allegiance;
  int health;

  Spaceship(vec2d position, vec2d dimensions, vec2d velocity, int sprite, int health, int allegiance, std::vector<Entity *> *gameEntities);

  virtual void check_collisions(Quadtree *quadtree);

};

class Player : public Spaceship
{

protected:

  int firingMode;
  float delay;

public:

  Player(vec2d position, vec2d dimensions, int sprite, int health, int allegiance, std::vector<Entity *> *gameEntities);

  void handle_input(KeyboardState *keyboardState);
  void update(float dt);
  void check_collisions(Quadtree *quadtree);
  void keep_in_screen(float x, float y, float w, float h);

};


class Game
{
 private:

  bool gameOver;

  std::vector<Entity *> gameEntities;

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

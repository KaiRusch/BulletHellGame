#ifndef ENTITY_H
#define ENTITY_H

#include "game_window.hpp"
#include "vec2d.hpp"

class Entity
{

public:

  vec2d position;
  vec2d velocity;
  vec2d dimensions;
  int sprite;
  bool destroy;
  int type;

  virtual void update(float dt);
  virtual void render(GameWindow *gameWindow);  
  virtual bool check_in_bounds(float x, float y, float w, float h);

  Entity(vec2d position, vec2d dimensions, vec2d velocity, int sprite);

};


#endif //ENTITY_H

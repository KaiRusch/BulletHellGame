#ifndef QUADTREE_H
#define QUADTREE_H

#include "entity.hpp"
#include <vector>

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

#endif //QUADTREE_H

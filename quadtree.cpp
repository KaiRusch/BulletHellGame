#include "quadtree.hpp"

Quadtree::Quadtree(float x, float y, float w, float h) : maxCapacity(5), northEast(NULL), northWest(NULL), southEast(NULL), southWest(NULL), _x(x), _y(y), _w(w), _h(h)
{
}

Quadtree::~Quadtree()
{
  delete(northEast);
  delete(northWest);
  delete(southEast);
  delete(southWest);
}

void Quadtree::subdivide()
{
  this->northEast = new Quadtree(_x+_w/2.0f,_y,_w/2.0f,_h/2.0f);
  this->northWest = new Quadtree(_x,_y,_w/2.0f,_h/2.0f);  
  this->southEast = new Quadtree(_x+_w/2.0f,_y+_h/2.0f,_w/2.0f,_h/2.0f);
  this->southWest = new Quadtree(_x,_y+_h/2.0f,_w/2.0f,_h/2.0f);
}

void Quadtree::insert(Entity *element)
{
  if(element->check_in_bounds(_x,_y,_w,_h))
    {
      if(contents.size() < maxCapacity)
	{
	  contents.push_back(element);
	}
      else
	{
	  if(northEast == NULL)
	    {
	      subdivide();
	    }

	  northEast->insert(element);
	  northWest->insert(element);      
	  southEast->insert(element);
	  southWest->insert(element);
	}
    }
}

bool rect_intersect(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2)
{
  if(x1 + w1 < x2)
    {
      return false;
    }
  if(x1 > x2 + w2)
    {
      return false;
    }
  if(y1 + h1 < y2)
    {
      return false;
    }
  if(y1 > y2 + h2)
    {
      return false;
    }

  return true;
}

std::vector<Entity *> Quadtree::get_in_range(float x, float y, float w, float h)
{
  std::vector<Entity *> inRange;

  if(!rect_intersect(_x,_y,_w,_h,x,y,w,h))
    {
      return inRange;
    }

  for(int i = 0; i < contents.size(); ++i)
    {
      if(contents[i]->check_in_bounds(x,y,w,h))
	{
	  inRange.push_back(contents[i]);
	}
    }

  if(northEast == NULL)
    {
      return inRange;
    }

  std::vector<Entity *> northEastInRange = northEast->get_in_range(x,y,w,h);
  inRange.insert(inRange.end(),northEastInRange.begin(),northEastInRange.end());
  std::vector<Entity *> northWestInRange = northWest->get_in_range(x,y,w,h);
  inRange.insert(inRange.end(),northWestInRange.begin(),northWestInRange.end());
  std::vector<Entity *> southEastInRange = southEast->get_in_range(x,y,w,h);
  inRange.insert(inRange.end(),southEastInRange.begin(),southEastInRange.end());
  std::vector<Entity *> southWestInRange = southWest->get_in_range(x,y,w,h);
  inRange.insert(inRange.end(),southWestInRange.begin(),southWestInRange.end());

  return inRange;
}

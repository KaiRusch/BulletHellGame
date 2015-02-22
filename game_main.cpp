#include "game_main.h"
/*Entity::Entity(vec2d position)
{
  this->position = position;
  this->velocity = vec2d(0,0);
  this->sprite = -1;
}

Entity::Entity(vec2d position, int sprite)
{
  this->position = position;
  this->velocity = vec2d(0,0);
  this->sprite = sprite;
  }*/

Entity::Entity(vec2d position, vec2d velocity, int sprite)
{
  this->position = position;
  this->velocity = velocity;
  this->sprite = sprite;
}

void Entity::update(float dt)
{
  position += velocity * dt;
}

bool Entity::check_in_bounds(float x, float y, float w, float h)
{
  if(position.x < x)
    {
      return false;
    }
  if(position.x > x+w)
    {
      return false;
    }
  if(position.y < y)
    {
      return false;
    }
  if(position.y > y+h)
    {
      return false;
    }
  return true;
}

Quadtree::Quadtree(float x, float y, float w, float h)
{
  maxCapacity = 5;
  
  northEast = NULL;
  northWest = NULL;
  southEast = NULL;
  southWest = NULL;

  _x = x;
  _y = y;
  _w = w;
  _h = h;

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

AABB::AABB(vec2d position, vec2d dimensions, vec2d velocity, int sprite) : Entity(position,velocity,sprite)
{
  this->dimensions = dimensions;
}

bool AABB::check_in_bounds(float x, float y, float w, float h)
{
  if(position.x + dimensions.x < x)
    {
      return false;
    }
  if(position.x - dimensions.x > x + w)
    {
      return false;
    }
  if(position.y + dimensions.y < y)
    {
      return false;
    }
  if(position.y - dimensions.y > y + h)
    {
      return false;
    }
  return true;  
}

KeyboardState::KeyboardState()
{
  left = false;
  right = false;
  up = false;
  down = false;
  space = false;
}

Game::Game()
{
  gameWindow.init("The Quest Fork", 500, 500);
  gameOver = false;
}

//----------------GAME LOOP------------------

void Game::handle_input()
{
  SDL_Event event;
  while(SDL_PollEvent(&event))
    {
      if(event.type == SDL_QUIT)
	{
	  gameOver = true;
	}
      if(event.type == SDL_KEYDOWN)
	{
	  switch(event.key.keysym.sym)
	    {
	    case SDLK_LEFT: keyboardState.left = true; break;
	    case SDLK_RIGHT: keyboardState.right = true; break;
	    case SDLK_SPACE: keyboardState.space = true; break;
	    }
	}
      if(event.type == SDL_KEYUP)
	{
	  switch(event.key.keysym.sym)
	    {
	    case SDLK_LEFT: keyboardState.left = false; break;
	    case SDLK_RIGHT: keyboardState.right = false; break;
	    case SDLK_SPACE: keyboardState.space = false; break;
	    }
	}
    }
}

int bulletSprite = 0;
float angle = 0.0f;
float delay = 0.0f;

void Game::update(float dt)
{
  angle += 6*M_PI*dt;
  delay += dt;

  if(delay > 0.01f)
    {
      gameEntities.push_back
	(new AABB(vec2d(250,250),
		  vec2d(2.5f,2.5f),
		  vec2d(100*cosf(angle),100*sinf(angle)),
		  bulletSprite));
      gameEntities.push_back
	(new AABB(vec2d(250,250),
		  vec2d(2.5f,2.5f),
		  vec2d(100*cosf(angle + M_PI/2),100*sinf(angle + M_PI/2)),
		  bulletSprite));
      gameEntities.push_back
	(new AABB(vec2d(250,250),
		  vec2d(2.5f,2.5f),
		  vec2d(100*cosf(angle + M_PI),100*sinf(angle + M_PI)),
		  bulletSprite));
      gameEntities.push_back
	(new AABB(vec2d(250,250),
		  vec2d(2.5f,2.5f),
		  vec2d(100*cosf(angle + 3*M_PI/2),100*sinf(angle + 3*M_PI/2)),
		  bulletSprite));
      

      delay = 0.0f;
    }
  

  delete(quadtree);
  quadtree = new Quadtree(0,0,500,500);
  for(int i = 0; i < gameEntities.size(); ++i)
    {
      gameEntities[i]->update(dt);
      quadtree->insert(gameEntities[i]);
    }
}

void Game::render()
{
  gameWindow.render_clear();
  for(int i = 0; i < gameEntities.size(); ++i)
    {
      gameWindow.render_sprite(gameEntities[i]->sprite,
			       gameEntities[i]->position.x,
			       gameEntities[i]->position.y);
    }

  gameWindow.render_present();
}

void Game::run()
{

  int spriteSheet = gameWindow.load_texture("sprite_sheet.png");
  int playerSprite = gameWindow.create_sprite(spriteSheet,0,0,60,20);
  bulletSprite = gameWindow.create_sprite(spriteSheet,60,0,5,5);

  while(!gameOver)
    {
      handle_input();
      update(gameWindow.get_dt());
      render();
    }
}

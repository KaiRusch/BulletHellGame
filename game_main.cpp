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
  this->maxCapacity = 5;

  this->northEast = NULL;
  this->northWest = NULL;
  this->southEast = NULL;
  this->southWest = NULL;

  this->x = x;
  this->y = y;
  this->w = w;
  this->h = h;
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
  this->northEast = new Quadtree(x+w/2.0f,y,w/2.0f,h/2.0f);
  this->northWest = new Quadtree(x,y,w/2.0f,h/2.0f);  
  this->southEast = new Quadtree(x+w/2.0f,y+h/2.0f,w/2.0f,h/2.0f);
  this->southWest = new Quadtree(x,y+h/2.0f,w/2.0f,h/2.0f);
}

void Quadtree::insert(Entity *element)
{
  if(element->check_in_bounds(x,y,w,h))
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

AABB::AABB(vec2d position, vec2d dimensions, vec2d velocity, int sprite) : Entity(position,velocity,sprite)
{
  this->dimensions = dimensions;
}

bool AABB::check_in_bounds(float x, float y, float w, float h)
{
  if(position.x + dimensions.x < 0)
    {
      return false;
    }
  if(position.x - dimensions.x > x + w)
    {
      return false;
    }
  if(position.y + dimensions.y < 0)
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



Entity *player = NULL;
int bulletSprite;
float angle = 0.0f;
float delay = 0.0f;

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

void Game::update(float dt)
{

  
  angle+= M_PI*dt;
  delay += dt;

  if(delay > 0.2f)
    {
  gameEntities.push_back(new AABB(vec2d(250,250),vec2d(5,5),vec2d(100*sinf(angle),100*cosf(angle)),bulletSprite));
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
      Entity *entity = gameEntities[i];
      gameWindow.render_sprite(entity->sprite,entity->position.x,entity->position.y);
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

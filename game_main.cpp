#include "game_main.h"

std::vector<Entity *> entities;

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

bool Entity::check_in_bounds(int screenWidth, int screenHeight)
{
  if(position.x < 0)
    {
      return false;
    }
  if(position.x > screenWidth)
    {
      return false;
    }
  if(position.y < 0)
    {
      return false;
    }
  if(position.y > screenHeight)
    {
      return false;
    }
  return true;
}

AABB::AABB(vec2d position, vec2d dimensions, vec2d velocity, int sprite) : Entity(position,velocity,sprite)
{
  this->dimensions = dimensions;
}

bool AABB::check_in_bounds(int screenWidth, int screenHeight)
{
  if(position.x - dimensions.x < 0)
    {
      position.x = dimensions.x;
    }
  if(position.x + dimensions.x > screenWidth)
    {
      position.x = screenWidth - dimensions.x;
    }
  if(position.y - dimensions.y < 0)
    {
      position.y = dimensions.y;
    }
  if(position.y + dimensions.y > screenHeight)
    {
      position.y = screenHeight - dimensions.y;
    }
  return true;  
}

Game::Game()
{
  gameWindow.init("The Quest Fork", 500, 500);
  gameOver = false;
}

Entity * spaceShipID;
bool leftDown = false, rightDown = false, upDown = false, downDown = false;

//----------------GAME LOOP------------------

void Game::handle_input()
{
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
	    case SDLK_LEFT: leftDown = true; break; 
	    case SDLK_RIGHT: rightDown = true; break; 
	    case SDLK_UP: upDown = true; break;
	    case SDLK_DOWN: downDown = true; break;
	    }
	}
      if(event.type == SDL_KEYUP)
	{
	  switch(event.key.keysym.sym)
	    {
	    case SDLK_LEFT: leftDown = false; break; 
	    case SDLK_RIGHT: rightDown = false; break;
	    case SDLK_UP: upDown = false; break;
	    case SDLK_DOWN: downDown = false; break;
	    }
	}
    }

  spaceShipID->velocity = vec2d(0,0);

  if(leftDown)
    {
      spaceShipID->velocity.x += -100;
    }
  if(rightDown)
    {
      spaceShipID->velocity.x += 100;
    }
  if(upDown)
    {
      spaceShipID->velocity.y += -100;
    }
  if(downDown)
    {
      spaceShipID->velocity.y += 100;
    }

}

int bullet = 0;
float angle = 0.0f;

void Game::update(float dt)
{

  angle += 1000000000000000*M_PI * dt;

  entities.push_back(new Entity(
			      vec2d(250,250),
			      vec2d(100*sinf(angle),100*cosf(angle)),bullet));

  std::vector<Entity *> newEntities;
  for(int i = 0; i < entities.size(); ++i)
    {
      entities[i]->update(dt);
      if(entities[i]->check_in_bounds(500,500))
	{
	  newEntities.push_back(entities[i]);
	}
      else
	{
	  delete(entities[i]);
	}
    }
  
  entities = newEntities;

}

void Game::render()
{
  gameWindow.render_clear();
  for(int i = 0; i < entities.size(); ++i)
    {
      gameWindow.render_sprite(entities[i]->sprite,entities[i]->position.x,entities[i]->position.y);
    }
  gameWindow.render_present();
}

void Game::run()
{

  int spriteSheet = gameWindow.load_texture("test.png");
  int spaceShip = gameWindow.create_sprite(spriteSheet,0,0,60,20);
  bullet = gameWindow.create_sprite(spriteSheet,60,5,15,15);

  entities.push_back(new AABB(vec2d(250,250),vec2d(30,10),vec2d(0,0),spaceShip));
  spaceShipID = entities[entities.size()-1];

  while(!gameOver)
    {
      handle_input();
      update(gameWindow.get_dt());
      render();
    }
}

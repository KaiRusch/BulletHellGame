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
  if(position.x + dimensions.x < 0)
    {
      return false;
    }
  if(position.x - dimensions.x > screenWidth)
    {
      return false;
    }
  if(position.y + dimensions.y < 0)
    {
      return false;
    }
  if(position.y - dimensions.y > screenHeight)
    {
      return false;
    }
  return true;  
}

Player::Player(vec2d position, vec2d dimensions, int hitRadius, int sprite) : AABB(position,dimensions,vec2d(0,0),sprite)
{
  this->hitRadius = hitRadius;
}

bool Player::check_in_bounds(int screenWidth, int screenHeight)
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

bool leftDown, rightDown, upDown, downDown, spaceDown;


Entity *player = NULL;
int bulletSprite;
float bulletDelay = 0.0f;

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
	    case SDLK_LEFT: leftDown = true; break;
	    case SDLK_RIGHT: rightDown = true; break;
	    case SDLK_SPACE: spaceDown = true; break;
	    }
	}
      if(event.type == SDL_KEYUP)
	{
	  switch(event.key.keysym.sym)
	    {
	    case SDLK_LEFT: leftDown = false; break;
	    case SDLK_RIGHT: rightDown = false; break;
	    case SDLK_SPACE: spaceDown = false; break;
	    }
	}
    }

  player->velocity = vec2d(0,0);
  if(leftDown)
    {
      player->velocity.x -= 100.0f;
    }
  if(rightDown)
    {
      player->velocity.x += 100.0f;
    }

  if(spaceDown && bulletDelay >= 0.1f)
    {
      bulletDelay = 0.0f;
      Entity *newBullet = new AABB(player->position+ vec2d(0,-8),vec2d(3,3),vec2d(0,-300),bulletSprite);
      entities.push_back(newBullet);
    }

}

void Game::update(float dt)
{

  bulletDelay += dt;;

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

  int spriteSheet = gameWindow.load_texture("sprite_sheet.png");
  int redPlayer = gameWindow.create_sprite(spriteSheet,0,0,20,20);
  bulletSprite = gameWindow.create_sprite(spriteSheet,0,80,6,6);

  player = new Player(vec2d(250,450),vec2d(10,10),6,redPlayer);
  entities.push_back(player);

  while(!gameOver)
    {
      handle_input();
      update(gameWindow.get_dt());
      render();
    }
}

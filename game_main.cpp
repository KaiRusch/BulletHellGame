#include "game_main.hpp"


Entity::Entity(vec2d position, vec2d dimensions, vec2d velocity, int sprite) : position(position), dimensions(dimensions), velocity(velocity), sprite(sprite), type(ENTITY)
{
}
void Entity::render(GameWindow *gameWindow)
{
  gameWindow->render_sprite(sprite,(int)position.x,(int)position.y);
}

void Entity::update(float dt)
{
  position += velocity * dt;
}

bool Entity::check_in_bounds(float x, float y, float w, float h)
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

Bullet::Bullet(vec2d position, vec2d dimensions, vec2d velocity, int sprite, int damage, int allegiance) : Entity(position,dimensions,velocity,sprite), damage(damage), allegiance(allegiance)
{
  this->type = BULLET;
}

int Bullet::damage_done(int spaceshipAllegiance)
{
  if(this->allegiance != spaceshipAllegiance)
    {
      this->destroy = true;
      return this->damage;
    }
  return 0;
}

Spaceship::Spaceship(vec2d position, vec2d dimensions, vec2d velocity, int sprite, int health, int allegiance) : Entity(position,dimensions,velocity,sprite), health(health), allegiance(allegiance)
{
  this->type = SPACESHIP;
}

void Spaceship::check_collisions(Quadtree *quadtree)
{
  std::vector<Entity *> inRange = quadtree->get_in_range(position.x-dimensions.x,position.x+dimensions.x,position.y-dimensions.y,position.y+dimensions.y);

  for(int i = 0; i < inRange.size(); ++i)
    {
      switch(inRange[i]->type)
	{
	case BULLET:
	  {
	    if(((Bullet*)inRange[i])->damage_done(allegiance))
	      {
		destroy = true;
	      }
	  }break;
	}
    }

}


KeyboardState::KeyboardState() : left(false), right(false), up(false), down(false), space(false)
{
}

Game::Game()
{
  gameWindow.init("The Quest Fork", 500, 500);
  gameOver = false;
}

int bulletSprite;
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
  Quadtree *newQuadtree = new Quadtree(0,0,500,500);

  newEntities.clear();

  std::vector<Spaceship *> spaceshipEntities;

  for(int i = 0; i < gameEntities.size(); ++i)
    {
      gameEntities[i]->update(dt);
      if(gameEntities[i]->check_in_bounds(-250,-250,1000,1000)
	 && !gameEntities[i]->destroy)
	{

	  if(gameEntities[i]->type == SPACESHIP)
	    {
	      spaceshipEntities.push_back((Spaceship *)gameEntities[i]);
	    }

	  newQuadtree->insert(gameEntities[i]);
	  newEntities.push_back(gameEntities[i]);
	}
      else
	{
	  delete(gameEntities[i]);
	}
    }

  delete(quadtree);
  quadtree = newQuadtree;
  gameEntities = newEntities;

  for(int i = 0; i < spaceshipEntities.size(); ++i)
    {
      spaceshipEntities[i]->check_collisions(quadtree);
    }
}

void Game::render()
{
  gameWindow.render_clear();
  for(int i = 0; i < gameEntities.size(); ++i)
    {
      gameEntities[i]->render(&gameWindow);
    }
  gameWindow.render_present();
}

void Game::run()
{
  int spriteSheet = gameWindow.load_texture("sprite_sheet.png");
  int spaceshipSprite = gameWindow.create_sprite(spriteSheet,0,0,60,20);
  bulletSprite = gameWindow.create_sprite(spriteSheet,60,0,5,5);

  gameEntities.push_back(new Bullet(vec2d(250.0f,250.0f),vec2d(2.5f,2.5f),vec2d(0.0f,100.0f),bulletSprite,1,1));

  gameEntities.push_back(new Spaceship(vec2d(250.0f,450.0f),vec2d(30.0f,10.0f),vec2d(0.0f,0.0f),spaceshipSprite,1,2));

  while(!gameOver)
    {
      handle_input();
      update(gameWindow.get_dt());
      std::cout << gameEntities.size() << " entitites\n";
      render();
    }
}

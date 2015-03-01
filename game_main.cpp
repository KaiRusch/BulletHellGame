#include "game_main.hpp"


Entity::Entity(vec2d position, vec2d dimensions, vec2d velocity, int sprite) : position(position), dimensions(dimensions), velocity(velocity), sprite(sprite), destroy(false), type(ENTITY)
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
  this->type = this->type | BULLET;
}

Pickup::Pickup(vec2d position, vec2d dimensions, vec2d velocity, int sprite, int pickupType) : Entity(position,dimensions,velocity,sprite), pickupType(pickupType)
{
  this->type = this->type | PICKUP;
}

Spaceship::Spaceship(vec2d position, vec2d dimensions, vec2d velocity, int sprite, int health, int allegiance, std::vector<Entity *> *gameEntities) : Entity(position,dimensions,velocity,sprite), health(health), allegiance(allegiance), gameEntities(gameEntities)
{
  this->type = this->type | SPACESHIP;
}

void Spaceship::check_collisions(Quadtree *quadtree)
{
  std::vector<Entity *> inRange = quadtree->get_in_range
    (position.x-dimensions.x,position.y-dimensions.y,
     2*dimensions.x, 2*dimensions.y);

  for(int i = 0; i < inRange.size(); ++i)
    {
      if((inRange[i]->position - position).length() <= 20.0f)
	{
	  if(inRange[i]->type & BULLET == BULLET)
	    {
	      Bullet *bullet = (Bullet*) inRange[i];
	      if(bullet->allegiance != allegiance)
		{
		  destroy = true;
		  bullet->destroy = true;
		}
	    }
	}
    }
}

Player::Player(vec2d position, vec2d dimensions, int sprite, int health, int allegiance, std::vector<Entity *> *gameEntities) : Spaceship(position,dimensions,vec2d(0.0f,0.0f),sprite,health,allegiance,gameEntities), delay(0.0f), firingMode(SINGLE_FIRE)
{
  this->type = this->type | PLAYER;
}

int bulletSprite;

void Player::handle_input(KeyboardState *keyboardState)
{
  velocity = vec2d(0.0f,0.0f);

  if(keyboardState->left)
    {
      velocity.x -= 100.0f;
    }
  if(keyboardState->right)
    {
      velocity.x += 100.0f;
    }  
  if(keyboardState->up)
    {
      velocity.y -= 100.0f;
    }
  if(keyboardState->down)
    {
      velocity.y += 100.0f;
    }
  if(keyboardState->space && delay > 0.08f)
    {
      if(firingMode == SINGLE_FIRE || firingMode == TRIPLE_FIRE)
	{
	  Bullet *bullet = new Bullet(position + vec2d(0.0f,-10.0f),
				      vec2d(2.5f,2.5f),
				      vec2d(0.0f,-250.0f),bulletSprite,
				      1, GOOD);
	  gameEntities->push_back(bullet);
	}
	
      if(firingMode == DOUBLE_FIRE || firingMode == TRIPLE_FIRE)
	{
	  Bullet *bullet1 = new Bullet(position + vec2d(20.0f,0.0f),
				       vec2d(2.5f,2.5f),
				       vec2d(0.0f,-250.0f),bulletSprite,
				       1, GOOD);
	  Bullet *bullet2 = new Bullet(position + vec2d(-20.0f,0.0f),
				       vec2d(2.5f,2.5f),
				       vec2d(0.0f,-250.0f),bulletSprite,
				       1, GOOD);
	  gameEntities->push_back(bullet1);
	  gameEntities->push_back(bullet2);
	}
      delay = 0.0f;
    }
}

void Player::update(float dt)
{
  delay += dt;
  Spaceship::update(dt);
}

void Player::check_collisions(Quadtree *quadtree)
{
  std::vector<Entity *> inRange = quadtree->get_in_range
    (position.x-dimensions.x,position.y-dimensions.y,
     2*dimensions.x, 2*dimensions.y);

  for(int i = 0; i < inRange.size(); ++i)
    {
      if((inRange[i]->position - position).length() <= 20.0f 
	 && inRange[i] != this)
	{
	  if((inRange[i]->type & BULLET) == BULLET)
	    {
	      Bullet *bullet = (Bullet*) inRange[i];
	      if(bullet->allegiance != allegiance)
		{
		  destroy = true;
		  bullet->destroy = true;
		}
	    }
	  if((inRange[i]->type & PICKUP) == PICKUP)
	    {
	      Pickup *pickup = (Pickup *) inRange[i];
	      pickup->destroy = true;
	      switch(pickup->pickupType)
		{
		case DOUBLE:
		  {
		    this->firingMode = DOUBLE_FIRE;
		  } break;
		case TRIPLE:
		  {
		    this->firingMode = TRIPLE_FIRE;
		  } break;
		}
	    }
	}
    }
}

void Player::keep_in_screen(float x, float y, float w, float h)
{
  if(position.x - dimensions.x < x)
    {
      position.x = x + dimensions.x;
    }
  if(position.x + dimensions.x > x + w)
    {
      position.x = x + w - dimensions.x;
    }
  if(position.y - dimensions.y < y)
    {
      position.y = y + dimensions.y;
    }
  if(position.y + dimensions.y > y + h)
    {
      position.y = y + h - dimensions.y;
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
	    case SDLK_UP: keyboardState.up = true; break;
	    case SDLK_DOWN: keyboardState.down = true; break;
	    }
	}
      if(event.type == SDL_KEYUP)
	{
	  switch(event.key.keysym.sym)
	    {
	    case SDLK_LEFT: keyboardState.left = false; break;
	    case SDLK_RIGHT: keyboardState.right = false; break;
	    case SDLK_SPACE: keyboardState.space = false; break;
	    case SDLK_UP: keyboardState.up = false; break;
	    case SDLK_DOWN: keyboardState.down = false; break;
	    }
	}
    }
}

void Game::update(float dt)
{
  std::vector<Entity *> tempEntities;
  std::vector<Player *> playerEntities;
  std::vector<Spaceship *> spaceshipEntities;

  for(int i = 0; i < gameEntities.size(); ++i)
    {
      if(gameEntities[i]->check_in_bounds(-250,-250,1000,1000)
	 && !gameEntities[i]->destroy)
	 {
	  tempEntities.push_back(gameEntities[i]);
	  if((gameEntities[i]->type & SPACESHIP) == SPACESHIP)
	    {
	      spaceshipEntities.push_back((Spaceship *)gameEntities[i]);
	    }
	  if((gameEntities[i]->type & PLAYER) == PLAYER)
	    {
	      playerEntities.push_back((Player *)gameEntities[i]);
	    }
	}
      else
	{
	  delete(gameEntities[i]);
	}
    }
    
  gameEntities = tempEntities;


  //Handle player input
  for(int i = 0; i < playerEntities.size(); ++i)
    {
      playerEntities[i]->handle_input(&keyboardState);
    }


  //Update all game entities
  Quadtree quadtree = Quadtree(0,0,500,500);
  for(int i = 0; i < gameEntities.size(); ++i)
    {
      gameEntities[i]->update(dt);
      quadtree.insert(gameEntities[i]);
    }

  //Check for collisions of spaceships
  for(int i = 0; i < spaceshipEntities.size(); ++i)
    {
      spaceshipEntities[i]->check_collisions(&quadtree);
    }

  //Keep player on screen
  for(int i = 0; i < playerEntities.size(); ++i)
    {
      playerEntities[i]->keep_in_screen(0,0,500,500);
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
  int enemySprite = gameWindow.create_sprite(spriteSheet,60,5,15,15);
  int tripleFireSprite = gameWindow.create_sprite(spriteSheet,162,0,12,12);
  bulletSprite = gameWindow.create_sprite(spriteSheet,60,0,5,5);

  Player *player = new Player(vec2d(250.0f,450.0f),vec2d(30.0f,10.0f),
			      spaceshipSprite, 1, GOOD, &gameEntities);

  gameEntities.push_back(player);

  Spaceship *enemy = new Spaceship(vec2d(250.0f,50.0f),
				   vec2d(7.5f,7.5f),
				   vec2d(0.0f,0.0f),
				   enemySprite, 1, 
				   EVIL, &gameEntities);

  gameEntities.push_back(enemy);

  Pickup *pickup = new Pickup(vec2d(250.0f,0.0f),
			      vec2d(6.0f,6.0f),
			      vec2d(0.0f,100.0f),
			      tripleFireSprite,
			      TRIPLE);

  gameEntities.push_back(pickup);

  while(!gameOver)
    {
      handle_input();
      update(gameWindow.get_dt());
      std::cout << gameEntities.size() << " entitites\n";
      render();
    }
}

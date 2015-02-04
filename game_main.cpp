#include "game_main.h"

//----------------GAME LOOP------------------

float y = 0;

int circle = -1;
int stuff = -1;

Game::Game()
{
  gameOver = false;
}

void Game::handle_input(unsigned int eventMask)
{
  if(eventMask & QUIT_GAME == QUIT_GAME)
    {
      gameOver = true;
    }
}

void Game::update(float dt)
{
  y += 100 * dt;
}

void Game::render()
{
  render_clear();
  render_texture(stuff,50,y,600,40);
  render_update();
}

void Game::run()
{

  circle = load_texture("dot.bmp");
  stuff = load_texture("test.png");

  while(!gameOver)
    {
      unsigned int eventMask = 0;
      poll_input(eventMask);
      handle_input(eventMask);
      update(get_dt());
      render();
    }
}

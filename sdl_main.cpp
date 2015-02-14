#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <string>
#include <vector>
#include "game_main.h"

#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 500

int main(int argc, char **argv)
{
  Game game;
  game.run();
  return 0;
}

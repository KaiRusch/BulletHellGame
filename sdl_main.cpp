#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <string>
#include <vector>
#include "game_main.h"

#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 500

SDL_Renderer *renderer;
SDL_Window *window;

SDL_Event sdlEvent;

std::vector<SDL_Texture*> sdlTextures;

int currentTicks = 0;

bool init(const char *windowTitle, int windowWidth, int windowHeight)
{
  if(SDL_Init(SDL_INIT_VIDEO))
    {
      std::cout << "SDL_Init error: " << SDL_GetError() << std::endl; 
      return false;
    }

  if(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG != IMG_INIT_PNG)
    {
      std::cout << "IMG_Init error: " << IMG_GetError() << std::endl; 
      SDL_Quit();
      return false;
    }

  window = 
    SDL_CreateWindow(windowTitle, 
		     SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
		     windowWidth, windowHeight, SDL_WINDOW_SHOWN);

  if(window == NULL)
    {
      std::cout << "SDL_CreateWindow" << SDL_GetError() << std::endl; 
      IMG_Quit();
      SDL_Quit();
      return false;
    }

  renderer = 
    SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  if(renderer == NULL)
    {
      SDL_DestroyWindow(window);
      std::cout << "SDL_CreateRenderer" << SDL_GetError() << std::endl; 
      IMG_Quit();
      SDL_Quit();
      return false;
    }


  return true;
}

void render_clear()
{
  SDL_RenderClear(renderer);
}

void render_update()
{
  SDL_RenderPresent(renderer);
}

int load_texture(const char *filename)
{
  SDL_Texture *texture = IMG_LoadTexture(renderer, filename);
  if(texture == NULL)
    {
      std::cout << "IMG_LoadTexture error: " << IMG_GetError() << std::endl;
      return -1;
    }
  
  sdlTextures.push_back(texture);
  return sdlTextures.size()-1;

}

void render_texture(int textureID, int x, int y, int w, int h)
{

  SDL_Rect destination;
  destination.x = x;
  destination.y = y;
  destination.w = w;
  destination.h = h;

  SDL_RenderCopy(renderer, sdlTextures[textureID], NULL, &destination);
}

void poll_input(unsigned int &eventMask)
{
  while(SDL_PollEvent(&sdlEvent))
    {
      if(sdlEvent.type == SDL_QUIT)
	{
	  eventMask = eventMask | QUIT_GAME;
	}
    }
}

float get_dt()
{
  int tempTicks = SDL_GetTicks();
  float dt = (tempTicks - currentTicks)/1000.0f;
  currentTicks = tempTicks;
  return dt;
}

int main(int argc, char **argv)
{
  if(!init("The Quest Fork", 500, 500))
    {
      return -1;
    }

  Game game;

  game.load_texture = &load_texture;
  game.poll_input = &poll_input;
  game.get_dt = &get_dt;
  game.render_clear = &render_clear;
  game.render_update = &render_update;
  game.render_texture = &render_texture;

  game.run();

  return 0;
}

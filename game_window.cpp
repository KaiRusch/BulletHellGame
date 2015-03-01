#include "game_window.hpp"

void GameWindow::init(const char* windowTitle, int windowWidth, int windowHeight)
{

  currentTicks = 0;

  if(SDL_Init(SDL_INIT_VIDEO))
    {
      std::cout << "SDL_Init error: " << SDL_GetError() << std::endl; 
      return;
    }

  if(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG != IMG_INIT_PNG)
    {
      std::cout << "IMG_Init error: " << IMG_GetError() << std::endl; 
      SDL_Quit();
      return;
    }

  this->window = 
    SDL_CreateWindow(windowTitle, 
		     SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
		     windowWidth, windowHeight, SDL_WINDOW_SHOWN);

  if(this->window == NULL)
    {
      std::cout << "SDL_CreateWindow" << SDL_GetError() << std::endl; 
      IMG_Quit();
      SDL_Quit();
      return;
    }

  this->renderer = 
    SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  if(this->renderer == NULL)
    {
      SDL_DestroyWindow(this->window);
      std::cout << "SDL_CreateRenderer" << SDL_GetError() << std::endl; 
      IMG_Quit();
      SDL_Quit();
      return;
    }
}

GameWindow::~GameWindow()
{
  for(int i = 0; i < sdlTextures.size(); ++i)
    {
      SDL_DestroyTexture(sdlTextures[i]);
    }
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  IMG_Quit();
  SDL_Quit();

}

int GameWindow::load_texture(const char *filename)
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

int GameWindow::create_sprite(int textureID, int x, int y, int w, int h)
{
  Sprite newSprite;
  newSprite.texture = sdlTextures[textureID];
  
  SDL_Rect clip;
  clip.x = x;
  clip.y = y;
  clip.w = w;
  clip.h = h;

  newSprite.clip = clip;

  sprites.push_back(newSprite);

  return sprites.size()-1;
}

void GameWindow::render_texture(int textureID, int x, int y)
{

  SDL_Rect destination;
  destination.x = x;
  destination.y = y;
  SDL_QueryTexture(sdlTextures[textureID], NULL, NULL, &destination.w, &destination.h);

  SDL_RenderCopy(renderer, sdlTextures[textureID], NULL, &destination);
}

void GameWindow::render_clear()
{
  SDL_RenderClear(renderer);
};

void GameWindow::render_present()
{
  SDL_RenderPresent(renderer);
};

void GameWindow::render_sprite(int spriteID, int x, int y)
{

  if(spriteID < 0)
    {
      return;
    }

  SDL_Rect destination;
  destination.x = x - sprites[spriteID].clip.w/2;
  destination.y = y - sprites[spriteID].clip.h/2;
  destination.w = sprites[spriteID].clip.w;
  destination.h = sprites[spriteID].clip.h;

  SDL_RenderCopy(renderer, sprites[spriteID].texture, &sprites[spriteID].clip, &destination);
}

float GameWindow::get_dt()
{
  int tempTicks = SDL_GetTicks();
  float dt = (tempTicks - currentTicks)/1000.0f;
  currentTicks = tempTicks;
  return dt;
}


#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>

struct Sprite
{
  SDL_Texture *texture;
  SDL_Rect clip;
};

class GameWindow
{
private:

  int currentTicks;

  SDL_Renderer *renderer;
  SDL_Window *window;

  std::vector<SDL_Texture *> sdlTextures;
  std::vector<Sprite> sprites;

public:

  void init(const char* windowTitle, int windowWidth, int windowHeight);
  ~GameWindow();

  int load_texture(const char* filename);
  int create_sprite(int textureID, int x, int y, int w, int h);
  void render_clear();
  void render_present();
  void render_texture(int textureID, int x, int y);
  void render_sprite(int spriteID, int x, int y);
  float get_dt();

};


#endif GAME_WINDOW_H

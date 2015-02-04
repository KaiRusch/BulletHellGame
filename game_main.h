#ifndef GAME_MAIN_H
#define GAME_MAIN_H

#define QUIT_GAME 1

class Game
{
 private:

  bool gameOver;
  
  void handle_input(unsigned int eventMask);
  void update(float dt);
  void render();
  
 public:

  int (*load_texture)(const char *filename);

  float (*get_dt)();

  void (*poll_input)(unsigned int &eventMask);

  void (*render_clear)();
  void (*render_update)();
  void (*render_texture)(int textureID, int x, int y, int w, int h);

  Game();
  void run();

};

#endif //GAME_MAIN_H

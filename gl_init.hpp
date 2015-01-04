#ifndef GL_INIT_H
#define GL_INIT_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <cstdio>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>

//The game window
extern SDL_Window *gameWindow;

//The openGL context
extern SDL_GLContext context;

//Initialize SDL2, openGL and create a gameWindow
//Returns false if failed, true if succesful
bool init_gl(const char *caption, int SCREEN_WIDTH, int SCREEN_HEIGHT);

//Closes SDL2, openGL and destroys releated objects
void clean_up();

//Creates an OpenGL program from a vertex and fragment shader
//Returns ID of the program
GLuint create_program(const char *vertexFilePath, const char *fragmentFilePath);

#endif // GL_INIT_H

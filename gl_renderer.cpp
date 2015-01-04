#include "gl_init.hpp"
#define GLM_FORCE_RADIANS
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <SOIL.h>


class Texture
{
    GLuint textureID;

public:

    Texture(const char *textureFilePath)
    {
        textureID = SOIL_load_OGL_texture(textureFilePath,SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_INVERT_Y);
    }

};

class Sprite
{
    GLuint elementBufferID;
    Texture texture;

public:


};

void render_sprite(Sprite sprite, float x, float y, float w, float h, float angle)
{
  glm::mat4 model(1.0f);

  model = 
    glm::translate(glm::mat4(1.0f),glm::vec3(x,y,0.0f))*
    glm::rotate(glm::mat4(1.0f),angle,glm::vec3(0.0f,0.0f,1.0f))*
    glm::scale(glm::mat4(1.0f),glm::vec3(w,h,1.0f));

  glm::mat4 view(1.0f);

  //TODO Find some way of making this more clean 
  glm::mat4 proj = glm::ortho(0.0f,500.0f,500.0f,0.0f);

  glm::mat4 mvp = proj*view*model;

  GLint uniMVP = glGetUniformLocation(program,"mvp");

  glUniformMatrix4fv(uniMVP,1,GL_FALSE,glm::value_ptr(mvp));

  glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
}

#include "vec2d.h"
#include "gl_init.hpp"
#define GLM_FORCE_RADIANS
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <vector>
#include <SOIL.h>
#include <sstream>

//Quit flag for program
bool quitGame = false;

SDL_Event windowEvent;

//Dimensions of the game window
#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 500

//ID of the shader program
GLuint program;

GLuint texture;

//Creates vertex array object, vertex buffer for square and element array buffer
void create_buffers(GLuint &vertexArrayID, GLuint &vertexBufferID, GLuint &elementBufferID, GLuint programID)
{
    //Creates and binds vertex array boject
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    //Vertex data of square
    GLfloat vertexData[] = {
        -0.5f, -0.5f, 0.0f, 0.0f, //Top left
        0.5f, -0.5f, 1.0f, 0.0f, //Top right
        0.5f,  0.5f, 1.0f, 1.0f, //Bottom right
        -0.5f, 0.5f, 0.0f, 1.0f //Bottom left
    };

    //Creates and binds vertex buffer
    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

    //Specifies the layout of the vertex data
    GLint posAttrib = glGetAttribLocation(programID, "pvPosition");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), 0);

    GLint uvAttrib = glGetAttribLocation(programID, "pvUvCoords");
    glEnableVertexAttribArray(uvAttrib);
    glVertexAttribPointer(uvAttrib, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat),(void*)(2*sizeof(GLfloat)));

    //Element indices for square
    GLuint elements[] =
    {
        0,1,2,
        0,2,3
    };

    //Creates and binds element buffer
    glGenBuffers(1,&elementBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements),elements, GL_STATIC_DRAW);
}

//Renders square at coordinates
void render_square(float x, float y, float w, float h, float angle)
{
    //Model matrix
    glm::mat4 model(1.0f);

    //Apply scaling, roatation and translation
    model =
    //Translation
    glm::translate(glm::mat4(1.0f),glm::vec3(x,y,0.0f))*
    //Rotation
    glm::rotate(glm::mat4(1.0f),angle,glm::vec3(0.0f,0.0f,1.0f))*
    //Scale
    glm::scale(glm::mat4(1.0f),glm::vec3(w,h,1.0f));

    //View matrix
    glm::mat4 view(1.0f);

    //Projection matrix
    glm::mat4 proj = glm::ortho(0.0f,(float)SCREEN_WIDTH,(float)SCREEN_HEIGHT,0.0f);

    //Create model view projection matrix
    glm::mat4 mvp = proj*view*model;

    GLint uniMVP = glGetUniformLocation(program,"mvp");

    glUniformMatrix4fv(uniMVP,1,GL_FALSE,glm::value_ptr(mvp));

    glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
}

GLuint load_texture(const char* filepath)
{
    GLuint textureID = SOIL_load_OGL_texture(filepath,SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_INVERT_Y);
    return textureID;
}

//-------------AABB CLASS----------------

class AABB
{
public:

    vec2d position;
    vec2d dimensions;

    AABB(){};

    AABB(vec2d position, vec2d dimensions)
    {
        this->position = position;
        this->dimensions = dimensions;
    };

    virtual void draw()
    {
        render_square(position.x,position.y,2*dimensions.x,2*dimensions.y,0);
    };

};


bool aabb_intersect(AABB *A, AABB *B)
{

    if(A->position.x - A->dimensions.x > B->position.x + B->dimensions.x)
    {
        return false;
    }
    if(A->position.x + A->dimensions.x < B->position.x - B->dimensions.x)
    {
        return false;
    }
    if(A->position.y - A->dimensions.y > B->position.y + B->dimensions.y)
    {
        return false;
    }
    if(A->position.y + A->dimensions.y < B->position.y - B->dimensions.y)
    {
        return false;
    }
    return true;
}

//-------------BULLET CLASS--------------

class Bullet : public AABB
{
public:

    vec2d velocity;
    vec2d acceleration;

    float deleteBullet;


    Bullet(vec2d position, vec2d dimensions, float speed, float angle) : AABB(position, dimensions)
    {
        this->velocity = vec2d(speed*cosf(angle),speed*sinf(angle));
        this->acceleration = vec2d(0.0f,0.0f);
        this->deleteBullet = false;

    };

    virtual void update(float dt)
    {
        position+= velocity*dt;

        if(position.x + dimensions.x < -SCREEN_WIDTH/3.0f || position.x > 1.33f*SCREEN_WIDTH || position.y + dimensions.y < -SCREEN_HEIGHT/3.0f || position.y > 1.33f*SCREEN_HEIGHT)
        {
            deleteBullet = true;
        }


    };
};

class CurvedBullet : public Bullet
{
public:

    vec2d originalVelocity;
    vec2d centerPosition;
    vec2d acceleration;

    float k;

    CurvedBullet(vec2d position, vec2d dimensions, float speed, float angle, vec2d centerPosition) : Bullet(position,dimensions,speed,angle)
    {
        this->originalVelocity = velocity;
        this->centerPosition = centerPosition;
        this->k = 10.0f;
    };

    virtual void update(float dt)
    {
        acceleration = -k*(position-centerPosition);
        velocity+=acceleration*dt;
        centerPosition += originalVelocity*dt;

        Bullet::update(dt);

    };

};


std::vector<Bullet*> bullets;

class Quadtree
{
public:

    int capacity;

    Quadtree *northEast;
    Quadtree *northWest;
    Quadtree *southEast;
    Quadtree *southWest;

    std::vector<AABB*> contents;

    AABB boundary;

    Quadtree(AABB boundary)
    {
        this->boundary = boundary;
        this->capacity = 5;

        this->northEast = NULL;
        this->northWest = NULL;
        this->southEast = NULL;
        this->southWest = NULL;

    };

    ~Quadtree()
    {
        delete(northEast);
        delete(northWest);
        delete(southEast);
        delete(southWest);
    }

    bool insert(AABB *element)
    {
        if(!aabb_intersect(element,&(this->boundary)))
        {
            return false;
        }

        if(contents.size() < capacity)
        {
            contents.push_back(element);
            return true;
        }

        if(northEast == NULL)
        {
            subdivide();
        }

        northEast->insert(element);
        northWest->insert(element);
        southEast->insert(element);
        southWest->insert(element);

        return true;

    };

    void subdivide()
    {
        this->northEast =
        new Quadtree
        (AABB(vec2d(this->boundary.position.x+this->boundary.dimensions.x/2.0f,this->boundary.position.y-this->boundary.dimensions.y/2.0f),
                vec2d(this->boundary.dimensions.x/2.0f,this->boundary.dimensions.y/2.0f))
         );


        this->northWest =
        new Quadtree
        (AABB(vec2d(this->boundary.position.x-this->boundary.dimensions.x/2.0f,this->boundary.position.y-this->boundary.dimensions.y/2.0f),
                vec2d(this->boundary.dimensions.x/2.0f,this->boundary.dimensions.y/2.0f))
         );


        this->southEast =
        new Quadtree
        (AABB(vec2d(this->boundary.position.x+this->boundary.dimensions.x/2.0f,this->boundary.position.y+this->boundary.dimensions.y/2.0f),
                vec2d(this->boundary.dimensions.x/2.0f,this->boundary.dimensions.y/2.0f))
         );


        this->southWest =
        new Quadtree
        (AABB(vec2d(this->boundary.position.x-this->boundary.dimensions.x/2.0f,this->boundary.position.y+this->boundary.dimensions.y/2.0f),
                vec2d(this->boundary.dimensions.x/2.0f,this->boundary.dimensions.y/2.0f))
         );

    };

    std::vector<AABB*> query_range(AABB *range)
    {
        std::vector<AABB *> inRange;

        if(!aabb_intersect(&(this->boundary),range))
        {
            return inRange;
        }

        for(int i = 0; i < contents.size(); ++i)
        {
            if(aabb_intersect(contents[i],range))
            {
                inRange.push_back(contents[i]);
            }
        }

        if(northWest == NULL)
        {
            return inRange;
        }

        std::vector<AABB*> northEastInRange = northEast->query_range(range);
        std::vector<AABB*> northWestInRange = northWest->query_range(range);
        std::vector<AABB*> southEastInRange = southEast->query_range(range);
        std::vector<AABB*> southWestInRange = southWest->query_range(range);

        inRange.insert(inRange.end(),northEastInRange.begin(),northEastInRange.end());
        inRange.insert(inRange.end(),northWestInRange.begin(),northWestInRange.end());
        inRange.insert(inRange.end(),southEastInRange.begin(),southEastInRange.end());
        inRange.insert(inRange.end(),southWestInRange.begin(),southWestInRange.end());

        return inRange;
    }

};


void create_curved_bullets(vec2d center, float angle, float width, float speed)
{


    vec2d leftPos(center.x - width*sinf(angle)/2.0f,center.y + width*cosf(angle)/2.0f);
    vec2d rightPos(center.x + width*sinf(angle)/2.0f,center.y - width*cosf(angle)/2.0f);

    bullets.push_back
    (
        new CurvedBullet(leftPos,vec2d(5.0f,5.0f),speed,angle,center)
     );
    bullets.push_back
    (
        new CurvedBullet(rightPos,vec2d(5.0f,5.0f),speed,angle,center)
     );
}

//-------------PLAYER CLASS-----------------

class Player : public AABB
{
public:

    vec2d velocity;

    bool left;
    bool right;
    bool up;
    bool down;

    Player(vec2d position, vec2d dimensions) : AABB(position, dimensions)
    {
        this->velocity = vec2d(0.0f,0.0f);

        left = false;
        right = false;
        up = false;
        down = false;

    };

    void handle_events(SDL_Event *event)
    {
        if(event->type == SDL_KEYDOWN)
        {
            switch(event->key.keysym.sym)
            {
                case SDLK_LEFT: left = true; break;
                case SDLK_RIGHT: right = true; break;
                case SDLK_UP: up = true; break;
                case SDLK_DOWN: down = true; break;
                case SDLK_SPACE:
                    for(int i = 0; i < 100; i++)
                    {
                        float angle = 2*M_PI/100.0f*(float)i;
                        bullets.push_back(new Bullet(this->position + vec2d(50.0f*cosf(angle),50.0f*sinf(angle)),vec2d(5.0f,5.0f),100.0f,angle));
                    }
                break;
            }
        }
        if(event->type == SDL_KEYUP)
        {
            switch(event->key.keysym.sym)
            {
                case SDLK_LEFT: left = false; break;
                case SDLK_RIGHT: right = false; break;
                case SDLK_UP: up = false; break;
                case SDLK_DOWN: down = false; break;
            }
        }
    };

    bool check_collision(AABB *A)
    {
        if
        (
        (this->position.x-A->position.x)*(this->position.x-A->position.x)
         +(this->position.y-A->position.y)*(this->position.y-A->position.y)
        < (this->dimensions.x+ A->dimensions.x)*(this->dimensions.x+ A->dimensions.x))
        {
            return true;
        }

        return false;
    }

    void update(float dt)
    {
        position += velocity *dt;

        velocity = vec2d(0.0f,0.0f);
        if(right)
            velocity.x += 100;
        if(left)
            velocity.x += -100;
        if(up)
            velocity.y += -100;
        if(down)
            velocity.y += 100;

    };
};

Player player(vec2d(SCREEN_WIDTH/2.0f,SCREEN_HEIGHT-10.0f),vec2d(3.0f,3.0f));



//-------------------GAME LOOP-------------------------

//Handle input and window events
void handle_events(SDL_Event *event)
{
    if(SDL_PollEvent(event))
    {

        player.handle_events(event);

        if(event->type == SDL_QUIT)
        {
            quitGame = true;
        }
        if(event->type == SDL_KEYUP)
        {
            if(event->key.keysym.sym == SDLK_ESCAPE)
            {
                quitGame = true;
            }
        }
    }
}

//Update game logic
void update(float dt)
{

    Quadtree quadtree(AABB(vec2d(SCREEN_WIDTH/2.0f,SCREEN_HEIGHT/2.0f),vec2d(SCREEN_WIDTH/2.0f,SCREEN_HEIGHT/2.0f)));

    std::vector<Bullet*> newBullets;
    for(int i = 0; i < bullets.size(); ++i)
    {
        bullets[i]->update(dt);
        if(bullets[i]->deleteBullet)
        {
            delete(bullets[i]);
        }
        else
        {
            newBullets.push_back(bullets[i]);
            quadtree.insert(bullets[i]);
        }
    }

    bullets = newBullets;

    player.update(dt);

    AABB range(player.position,4.0f*player.dimensions);

    std::vector<AABB*> inRange = quadtree.query_range(&range);


    for(int i = 0; i < inRange.size(); ++i)
    {

        if(player.check_collision(inRange[i]))
        {
            quitGame = true;
        }
    }
}

//Render to the screen
void render()
{

    //Set clear color
    glClearColor(0.0f,0.0f,0.0f,1.0f);

    //Clear color buffer with clear color
    glClear(GL_COLOR_BUFFER_BIT);

    for(int i = 0; i < bullets.size(); ++i)
    {
        bullets[i]->draw();
    }

    player.draw();

    //Draw frame buffer to game window
    SDL_GL_SwapWindow(gameWindow);
}

//------------------------MAIN---------------------------

int main(int argc, char *argv[])
{

    if(!init_gl("The Quest for K",SCREEN_WIDTH,SCREEN_HEIGHT))
    {
        std::printf("Failed to initialize SDL2 and openGL");
        clean_up();
        return -1;
    }
    //Create and use shader program
    program = create_program("Source/Shaders/vertex.glsl","Source/Shaders/fragment.glsl");
    glUseProgram(program);

    GLuint vertexArrayID, vertexBufferID, elementBufferID;

    create_buffers(vertexArrayID, vertexBufferID, elementBufferID, program);


    texture = load_texture("Assets/bullet.png");

    float time = 0.0f;
    float newTime = 0.0f;
    float dt = 0.0f;

    float delay = 0.0f;


    float angle = M_PI;

    int frames = 0;
    float tFrames = 0.0f;


    //Game loop
    while(!quitGame)
    {
        newTime = SDL_GetTicks()/1000.0f;
        dt = newTime - time;
        time = newTime;

        delay += dt;
        tFrames += dt;

        if(dt<= 0.1f)
        {
            frames++;


            handle_events(&windowEvent);

            if(delay >= 0.4)
            {
                create_curved_bullets(vec2d((float)SCREEN_WIDTH/2.0f,0.0f),M_PI/2.0f,100.0f,100.0f);
                delay = 0.0f;
            }

            update(dt);

            render();

            if(frames == 60)
            {
                 std::stringstream fps;
                fps << 60.0f/tFrames << " " << bullets.size();
                SDL_SetWindowTitle(gameWindow,fps.str().c_str());
                frames = 0;
                tFrames = 0.0f;
            }



        }
    }

    glDeleteProgram(program);
    glDeleteBuffers(1,&vertexBufferID);
    glDeleteBuffers(1,&elementBufferID);
    glDeleteVertexArrays(1, &vertexArrayID);

    for(int i = 0; i < bullets.size(); ++i)
    {
        delete(bullets[i]);
    }

    clean_up();
    return 0;
}

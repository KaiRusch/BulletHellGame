#include <SDL.h>
#include "vec2d.h"
#include <vector>
#include <sstream>

//Quit flag for program
bool quitGame = false;

SDL_Event windowEvent;

//Dimensions of the game window
#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 500

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
void handle_events()
{
}

//Update game logic
void update()
{
}

//Render to the screen
void render()
{
}

//------------------------MAIN---------------------------

int main(int argc, char *argv[])
{

  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window *window = SDL_CreateWindow("The Quest Fork", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_SWSURFACE);

  SDL_Event event;

  bool quit = false;
  while(!quit)
    {
      while(SDL_PollEvent(&event))
	{
	  if(event.type == SDL_QUIT)
	    {
	      quit = true;
	    }
	}
      handle_events();
      update();
      render();
    }

  SDL_Quit();

  return 0;
}

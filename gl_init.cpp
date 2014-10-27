#include "gl_init.hpp"

//The game window
SDL_Window *gameWindow = NULL;

//The openGL context
SDL_GLContext context;

//Initialize SDL2, openGL and create a gameWindow, takes screen dimensions and window titleas paremeters
//Returns false if failed, true if succesful
bool init_gl(const char *caption, int SCREEN_WIDTH, int SCREEN_HEIGHT)
{
    //Initialize SDL2 video
    SDL_Init(SDL_INIT_VIDEO);

    //Set SDL2 OpenGl attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    //Create the game window
    gameWindow = SDL_CreateWindow(caption, SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL );

    if(gameWindow == NULL)
    {
        std::printf("Failed to create game window\n");
        return false;
    }

    //Create the openGl context
    context = SDL_GL_CreateContext(gameWindow);

    //Initialize GLEW
    glewExperimental = true;
    GLenum glewInitResult = glewInit();
    //Check for error while initializing GLEW
    if(glewInitResult != 0)
    {
        std::printf("%s\n",glewGetErrorString(glewInitResult));
        return false;
    }

    return true;
}

//Closes SDL2, openGL and destroys releated objects
void clean_up()
{
    //Destroy the openGl context
    SDL_GL_DeleteContext(context);

    //Destroy the game window
    SDL_DestroyWindow(gameWindow);

    //Quit SDL2
    SDL_Quit();
}


//Creates an OpenGL program from a vertex and fragment shader
//Returns ID of the program
GLuint create_program(const char *vertexFilePath, const char *fragmentFilePath)
{
    //Create the shaders
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    //The code of the vertex shader
    std::string vertexShaderCode;

    //Open the vertex shader code file
    std::fstream vertexShaderStream(vertexFilePath,std::ios::in);

    //Read the vertex shader code to vertexShaderCode
    if(vertexShaderStream.is_open())
    {
        std::string line ="";
        while(std::getline(vertexShaderStream,line))
        {
            vertexShaderCode += "\n" + line;
        }
        vertexShaderStream.close();
    }

    //The code of the fragment shader
    std::string fragmentShaderCode;

    //Open the file with the fragment shader code
    std::ifstream fragmentShaderStream(fragmentFilePath,std::ios::in);

    //Read the fragment shader code to fragmentShaderCode
    if(fragmentShaderStream.is_open())
    {
        std::string line ="";
        while(std::getline(fragmentShaderStream,line))
        {
            fragmentShaderCode+= "\n" + line;
        }
        fragmentShaderStream.close();
    }

    //Result of compilation and linking
    GLint result = GL_FALSE;

    //Length of compilation and linking log message
    int infoLogLength;

    printf("Compiling shader: %s\n",vertexFilePath);

    //C string of vertexShaderCode
    char const *vertexSourcePointer = vertexShaderCode.c_str();

    //Compile vertex shader
    glShaderSource(vertexShaderID,1, &vertexSourcePointer, NULL);
    glCompileShader(vertexShaderID);

    //Get status of compilation
    glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);

    //Get status message
    std::vector <char> vertexShaderErrorMessage(infoLogLength);
    glGetShaderInfoLog(vertexShaderID, infoLogLength, NULL, &vertexShaderErrorMessage[0]);

    //Print compilation status message
    printf("%s\n", &vertexShaderErrorMessage[0]);

    printf("Compiling shader: %s\n",fragmentFilePath);

    //C string of fragmentShaderCode
    char const *fragmentSourcePointer = fragmentShaderCode.c_str();

    //Compile fragment shader
    glShaderSource(fragmentShaderID,1, &fragmentSourcePointer, NULL);
    glCompileShader(fragmentShaderID);

    //Get status of compilation
    glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);

    //Get status message
    std::vector <char> fragmentShaderErrorMessage(infoLogLength);
    glGetShaderInfoLog(fragmentShaderID, infoLogLength, NULL, &fragmentShaderErrorMessage[0]);

    //Print status message
    printf("%s\n", &fragmentShaderErrorMessage[0]);

    printf("Linking program\n");

    //Create program
    GLuint programID = glCreateProgram();

    //Attach shaders to program
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);

    //Link program
    glLinkProgram(programID);

    //Get program link status
    glGetProgramiv(programID, GL_LINK_STATUS, &result);
    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);

    //Get status message
    std::vector<char> programErrorMessage(std::max(infoLogLength,int(1)));
    glGetShaderInfoLog(programID, infoLogLength, NULL, &programErrorMessage[0]);

    //Print status message
    printf("%s\n", &programErrorMessage[0]);

    //Delete shaders
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    return programID;
}

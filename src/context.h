#include <SDL2/SDL.h>
#include <glad/glad.h>

//--------------------------------------------------------
// SDL window
//--------------------------------------------------------
// window dimensions
int windowWidth  = 800;
int windowHeight = 600;

int windowPosx = 0;
int windowPosy = 0;

// window pointer
SDL_Window *window;

int mouse_lastX, mouse_lastY, mouse_offsetX, mouse_offsetY;
bool firstMouse = true;

static bool init() {
    // SDL init
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        printf("%s - SDL could not initialize! SDL Error: %s\n", __FUNCTION__, SDL_GetError());
        return false;
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_GL_SetSwapInterval(1);

    SDL_SetRelativeMouseMode(SDL_TRUE);

    // SDL window creation
    Uint32 windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

    window = SDL_CreateWindow("oui bonjour", windowPosx, windowPosy, windowWidth, windowHeight, windowFlags);
    if (window == NULL) {
        printf("%s - Window could not be created! SDL Error: %s\n", __FUNCTION__, SDL_GetError());
        return false;
    }

    // GL context creation
    SDL_GLContext windowGLContext = SDL_GL_CreateContext(window);
    if (windowGLContext == NULL) {
        printf("%s - OpenGL context could not be created! SDL Error: %s\n", __FUNCTION__, SDL_GetError());
        return false;
    }
    // glad load
    gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress);

    return true;
}

static void quit() {
    SDL_Quit();
    exit(0);
}





    // // glfw: whenever the mouse moves, this callback is called
    // // -------------------------------------------------------
    // void mouse_callback(GLFWwindow *window, double xpos, double ypos)
    // {
    //     if (firstMouse)
    //     {
    //         lastX = xpos;
    //         lastY = ypos;
    //         firstMouse = false;
    //     }

    //     float xoffset = xpos - lastX;
    //     float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    //     lastX = xpos;
    //     lastY = ypos;

    //     //cam.ProcessMouseMovement(xoffset, yoffset);
    // }

    // // glfw: whenever the mouse scroll wheel scrolls, this callback is called
    // // ----------------------------------------------------------------------
    // void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
    // {
    //     cam.ProcessMouseScroll(yoffset);
    // }

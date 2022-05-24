//-----------------------------------------------------
/*

  A minimal OpenGL Cmake project

    uses: SDL2, glad, OpenGL, [openVR]

    additional : lodepng and Linear Algebra lib in  include folder

*/
//-----------------------------------------------------
#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_scancode.h>
#include <glad/glad.h>

#include "OBJ.h"
#include "camera.h"
#include "mesh.h"
#include "shader.h"

#include "context.h"

#include <stdio.h>
#include <stdlib.h>

#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//--------------------------------------------------------
// scene
//--------------------------------------------------------
// geometry
float vertices[] = { -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f };
double deltaTime = 0;

// Mesh tri;
// Shader shader1;
Camera cam;
glm::vec3 defaultCamPosition(0.0f, 0.0f, 10.0f);
glm::vec3 lPosition         = glm::vec3(12.0f, 10.0f, 20.0f);
glm::vec3 defaultLightColor = glm::vec3(1.0f, 1.0f, 1.0f);

Model model;
Shader lambertShader;

//-------------------------------------------------
//                      INIT
//-------------------------------------------------
static void initialisation() {
    if (!init()) {
        printf("failed\n");
    }

    cam = Camera(defaultCamPosition);

    // model         = Model("/home/huan/git/specular-manifold-sampling-AG/results/Test/meshes/panel_octave_004.obj");
    model = Model("../ressources/monkeyHD.obj");

    lambertShader = Shader("../shader/vertex.vs", "../shader/frag_PBR_Base.fs");
}

//-------------------------------------------------
//                     INPUT
//-------------------------------------------------
static void input() {
    // cam.processMouseMovement((float) mouse_offsetX / (float)windowWidth, (float) mouse_offsetY / (float)windowHeight);
    int xpos, ypos;
    SDL_GetRelativeMouseState(&xpos, &ypos);
    cam.processMouseMovement((float) xpos, (float) -ypos);

    mouse_offsetX = 0;
    mouse_offsetY = 0;

    const Uint8* kb = SDL_GetKeyboardState(NULL);

    if(kb[SDL_SCANCODE_W] == 1) cam.ProcessKeyboard(FORWARD, deltaTime);
    if(kb[SDL_SCANCODE_S] == 1) cam.ProcessKeyboard(BACKWARD, deltaTime);
    if(kb[SDL_SCANCODE_A] == 1) cam.ProcessKeyboard(LEFT, deltaTime);
    if(kb[SDL_SCANCODE_D] == 1) cam.ProcessKeyboard(RIGHT, deltaTime);
}

static void mouse_callback() {

    // Get mouse position
    int xpos, ypos;
    SDL_GetRelativeMouseState(&xpos, &ypos);

    mouse_offsetX = xpos;
    mouse_offsetY = ypos;
    std::cout << "mouse raw " << mouse_offsetX << "; " << mouse_offsetY << std::endl;
}

static void handle_key_down(SDL_Keysym *keysym) {
    switch (keysym->sym) {
        case SDLK_ESCAPE:
            quit();
            break;

        default:
            break;
    }
}

static void event() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {

        switch (event.type) {
            case SDL_KEYDOWN:
                handle_key_down(&event.key.keysym);
                break;
            case SDL_QUIT:
                quit();
                break;
                // case SDL_MOUSEMOTION:
                //     // mouse_callback();
                //     break;
        }
    }
}

//-------------------------------------------------
//                     UPDATE
//-------------------------------------------------
static void update() { // update camera
    // cam.updateCameraVectors();

    glm::mat4 projection = glm::perspective(glm::radians(cam.Zoom), (float) windowWidth / (float) windowHeight, 0.1f, 100.0f);
    glm::mat4 view       = cam.GetViewMatrix();

    // for(int i=0;i<4;i++){
    //   for(int j=0;j<4;j++){

    //     std::cout << view[i][j] << ", ";
    //   }
    //   std::cout << std::endl << std::endl;
    // }

    // send camera date to shaders
    lambertShader.use();

    lambertShader.setMat4("projection", projection);
    lambertShader.setMat4("view", view);
    lambertShader.setVec3("viewPos", cam.Position);

    // pass light infos
    lambertShader.setVec3("light.position", lPosition);
    lambertShader.setVec3("light.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
    lambertShader.setVec3("light.diffuse", glm::vec3(2.0f, 2.0f, 2.0f));
    lambertShader.setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));


    // material tuning
    lambertShader.setFloat("material.metalness", 1.0f);
    lambertShader.setFloat("material.roughness", 0.2);
    lambertShader.setVec3("material.albedo", glm::vec3(0.50f, 1.0f, 1.0f));

    // render the loaded model
    glm::mat4 modelMat = glm::mat4(1.0f);
    // model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f,
    // 0.0f));
    modelMat = glm::rotate(modelMat, float(glm::radians(-3.0f)), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMat = glm::translate(modelMat, glm::vec3(0.0f, 0.0f,
                                                  0.0f)); // translate it down so it's at the center of the scene
    modelMat = glm::scale(modelMat, glm::vec3(0.50f, 0.50f,
                                              0.50f)); // it's a bit too big for our scene, so scale it down

    glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(modelMat));

    lambertShader.setMat4("model", modelMat);
    lambertShader.setMat4("normalMatrix", normalMatrix);
}

//-------------------------------------------------
//                     DRAW
//-------------------------------------------------
static void draw(SDL_Window *window) { model.draw(lambertShader); }

int main(int argc, char *argv[]) {

    initialisation();

    glViewport(0, 0, windowWidth, windowHeight);

    Uint64 NOW  = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;
    glEnable(GL_DEPTH_TEST);
    while (1) {
        glClearColor(0.1f, 0.01f, 0.01f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        LAST = NOW;
        NOW  = SDL_GetPerformanceCounter();

        deltaTime = (double) ((NOW - LAST) * 1000 / (double) SDL_GetPerformanceFrequency());

        event();
        input();

        update();

        draw(window);

        SDL_GL_SwapWindow(window);
    }

    return 0;
}
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

    model = Model("/home/huan/git/specular-manifold-sampling-AG/results/Test/meshes/panel_octave_004.obj");
    // model = Model("../ressources/monkeyHD.obj");

    lambertShader = Shader("../shader/vertex.vs", "../shader/frag_PBR_IBL.fs");

    // IBL
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    float *data = stbi_loadf("../ressources/IBL/Arches_E_PineTree/Arches_E_PineTree_Iradiance.hdr", &width, &height, &nrComponents, 0);
    unsigned int hdrTexture;
    if (data) {
        glActiveTexture(GL_TEXTURE0 + 0);
        lambertShader.setInt("IBL", 0);
        glGenTextures(1, &hdrTexture);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        glActiveTexture(GL_TEXTURE0);
    } else {
        std::cout << "Failed to load HDR image." << std::endl;
    }
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

    const Uint8 *kb = SDL_GetKeyboardState(NULL);

    if (kb[SDL_SCANCODE_W] == 1)
        cam.ProcessKeyboard(FORWARD, deltaTime);
    if (kb[SDL_SCANCODE_S] == 1)
        cam.ProcessKeyboard(BACKWARD, deltaTime);
    if (kb[SDL_SCANCODE_A] == 1)
        cam.ProcessKeyboard(LEFT, deltaTime);
    if (kb[SDL_SCANCODE_D] == 1)
        cam.ProcessKeyboard(RIGHT, deltaTime);
    if (kb[SDL_SCANCODE_Q] == 1)
        cam.ProcessKeyboard(UP, deltaTime);
    if (kb[SDL_SCANCODE_E] == 1)
        cam.ProcessKeyboard(DOWN, deltaTime);
}

static void mouse_callback() {

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
        }
    }
}

//-------------------------------------------------
//                     UPDATE
//-------------------------------------------------
static void update() {

    glm::mat4 projection = glm::perspective(glm::radians(cam.Zoom), (float) windowWidth / (float) windowHeight, 0.1f, 100.0f);
    glm::mat4 view       = cam.GetViewMatrix();

    // Scene --> shader
    shader_scene_input shader_scene_input;
    shader_scene_input.cam_projection   = projection;
    shader_scene_input.cam_view         = view;
    shader_scene_input.cam_pos          = cam.Position;
    shader_scene_input.light_pos        = lPosition;
    shader_scene_input.light_ambient_c  = glm::vec3(0.05f, 0.05f, 0.05f);
    shader_scene_input.light_specular_c = glm::vec3(1.0f, 1.0f, 1.0f);
    shader_scene_input.light_diffuse_c  = glm::vec3(1.0f, 1.0f, 1.0f);

    lambertShader.sendShader_scene_input(shader_scene_input);

    // Model --> shader
    glm::mat4 modelMat = glm::mat4(1.0f);
    modelMat           = glm::rotate(modelMat, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    modelMat = glm::translate(modelMat, glm::vec3(0.0f, 0.0f, 0.0f)); 
    modelMat = glm::scale(modelMat, glm::vec3(0.50f, 0.50f, 0.50f));  

    glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(modelMat));

    lambertShader.setMat4("model", modelMat);
    lambertShader.setMat4("normalMatrix", normalMatrix);

    shader_model_input shader_model_input;
    shader_model_input.model_transform              = modelMat;
    shader_model_input.model_invtranspose_transform = normalMatrix;
    shader_model_input.mat_albedo_c                 = glm::vec3(0.50f, 1.0f, 1.0f);
    shader_model_input.mat_metalness                = 0.1f;
    shader_model_input.mat_roughness                = 0.6f;

    lambertShader.sendShader_model_input(shader_model_input);
}

//-------------------------------------------------
//                     DRAW
//-------------------------------------------------
static void draw(SDL_Window *window) { model.draw(lambertShader); }

//-------------------------------------------------
//                     MAIN
//-------------------------------------------------
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
#include <cstdio>

#define IMGUI_IMPL_OPENGL_LOADER_GLAD

#include <glad/glad.h>  // Initialize with gladLoadGL()

#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "view/gui_manager.h"
#endif

/** FUNCTIONS **/

void input();

void update();

void render();


/** GLOBAL VARIABLES **/
ImVec4 clear_color = ImVec4(0.01f, 0.05f, 0.1f, 1.00f);

glm::vec3 cameraPos = glm::vec3(0.0f, 3.f, 10.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, -3.0f, -10.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 0.996f, 0.08f);
glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

float deltaTime = 0.0f;  // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
float currentFrame;


int main(int, char **) {

    gui::init();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // Shader reflectShader("res/shaders/basic.vert", "res/shaders/reflect.frag");
    // Shader refractShader("res/shaders/basic.vert", "res/shaders/refract.frag");

    // Main loop
    while (!glfwWindowShouldClose(gui::window)) {
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Process I/O operations here
        gui::process_input();

        // Update game objects' state here
        //update();

        // OpenGL rendering code here
        render();

        gui::draw_gui();
        gui::end_frame();


    }

    gui::terminate();

    return 0;
}

void update() {}

void render() {
    // clear
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
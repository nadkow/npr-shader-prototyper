#include <cstdio>

#define IMGUI_IMPL_OPENGL_LOADER_GLAD

#include <glad/glad.h>  // Initialize with gladLoadGL()

#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <ImGuizmo.h>
#include <GLFW/glfw3.h> // Include glfw3.h after our OpenGL definitions
#include "gl_version.h"

#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "logic/common.h"
#include "logic/Object.h"

ObjectManager object_manager;

#include "logic/FileManager.h"
#include "view/gui_manager.h"
#include "rendering/ObjectRenderer.h"

void update();
void render();

int main(int, char **) {

    gui::init();
    renderer::init();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);

    //load default 3d model
    object_manager = ObjectManager();
    object_manager.importModel("res/models/rat/rat2.obj");
    object_manager.createLightObject();

    //load files
    files::readModelFiles();

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

    renderer::draw();
}
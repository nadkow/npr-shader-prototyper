#ifndef NPRSPR_GUI_MANAGER_H
#define NPRSPR_GUI_MANAGER_H

#include "imgui.h"
#include "imgui_impl/imgui_impl_glfw.h"
#include "imgui_impl/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h> // Include glfw3.h after our OpenGL definitions
#include "gl_version.h"

extern glm::mat4 projection;

namespace gui {

    constexpr int32_t WINDOW_WIDTH = 1920;
    constexpr int32_t WINDOW_HEIGHT = 1080;
    GLFWwindow *window = nullptr;


    static void glfw_error_callback(int error, const char *description) {
        fprintf(stderr, "Glfw Error %d: %s\n", error, description);
    }

    void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods){};

    void process_input() {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    }

    bool init_glfw() {
        // Setup window
        glfwSetErrorCallback(glfw_error_callback);
        if (!glfwInit()) {
            spdlog::error("Failed to initalize GLFW!");
            return false;
        }

        // GL 4.6 + GLSL 460
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_VERSION_MAJOR);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_VERSION_MINOR);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

        // Create window with graphics context
        window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "NPRSPR", NULL, NULL);
        if (window == NULL) {
            spdlog::error("Failed to create GLFW Window!");
            return false;
        }

        glfwMakeContextCurrent(window);
        glfwSwapInterval(1); // Enable VSync - fixes FPS at the refresh rate of your screen

        bool err = !gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

        if (err) {
            spdlog::error("Failed to initialize OpenGL loader!");
            return false;
        }

        return true;
    }

    void init_imgui() {
        // Setup Dear ImGui binding
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void) io;
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);

        // Setup style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        // Load Fonts
        // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
        // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
        // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
        // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
        // - Read 'misc/fonts/README.txt' for more instructions and details.
        // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
        //io.Fonts->AddFontDefault();
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
        //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
        //IM_ASSERT(font != NULL);
    }

    bool init() {
        if (!init_glfw()) {
            spdlog::error("Failed to initialize project!");
            return EXIT_FAILURE;
        }

        spdlog::info("Initialized project.");

        init_imgui();
        spdlog::info("Initialized ImGui.");

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetKeyCallback(window, key_callback);
    }

    void imgui_begin() {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void imgui_render() {

        ImGui::Begin("Hello");

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);
        ImGui::End();
    }

    void imgui_end() {
        ImGui::Render();
        int display_w, display_h;
        glfwMakeContextCurrent(window);
        glfwGetFramebufferSize(window, &display_w, &display_h);

        glViewport(0, 0, display_w, display_h);

        projection = glm::perspective(glm::radians(45.0f), (float) display_w / display_h, 0.1f, 100.0f);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void draw_gui() {
        // Draw ImGui
        imgui_begin();
        imgui_render();
        imgui_end(); // this call effectively renders ImGui
    }

    void end_frame() {
        // End frame and swap buffers (double buffering)
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        glfwMakeContextCurrent(window);
        glfwSwapBuffers(window);
    }

    void terminate() {
        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(window);
        glfwTerminate();
    }
}
#endif //NPRSPR_GUI_MANAGER_H

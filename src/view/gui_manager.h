#ifndef NPRSPR_GUI_MANAGER_H
#define NPRSPR_GUI_MANAGER_H

static ImGuizmo::OPERATION currentGizmoOperation(ImGuizmo::TRANSLATE);

namespace gui {

    GLFWwindow *window = nullptr;
    Object *selectedObject = nullptr;
    LightObject *selectedLight = nullptr;
    GeneralObject *activeSelected = nullptr;

    static GraphEditor::Options options;
    static GraphEditor::ViewState viewState;
    static GraphEditor::FitOnScreen fit = GraphEditor::Fit_None;

    static void glfw_error_callback(int error, const char *description) {
        fprintf(stderr, "Glfw Error %d: %s\n", error, description);
    }

    void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height+19);
        if (height) projection = glm::perspective(glm::radians(45.0f), (float) width / height, 0.1f, 100.0f);
    }

    void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods){
        if (key == GLFW_KEY_W && action == GLFW_PRESS) {
            currentGizmoOperation = ImGuizmo::TRANSLATE;
        }
        if (key == GLFW_KEY_E && action == GLFW_PRESS) {
            currentGizmoOperation = ImGuizmo::ROTATE;
        }
        if (key == GLFW_KEY_R && action == GLFW_PRESS) {
            currentGizmoOperation = ImGuizmo::SCALE;
        }
        if (key == GLFW_KEY_F && action == GLFW_PRESS) {
            if (selectedObject) selectedObject->showcaseFlat();
        }
        if (key == GLFW_KEY_G && action == GLFW_PRESS) {
            if (selectedObject) selectedObject->showcaseFresnel();
        }
        if (key == GLFW_KEY_P && action == GLFW_PRESS) {
            if (selectedObject) selectedObject->showcasePoint();
        }
        if (key == GLFW_KEY_L && action == GLFW_PRESS) {
            if (selectedObject) selectedObject->showcasePointTex();
        }
        if (key == GLFW_KEY_T && action == GLFW_PRESS) {
            if (selectedObject) selectedObject->showcaseTex();
        }
        if (key == GLFW_KEY_C && action == GLFW_PRESS) {
            if (selectedObject) selectedObject->showcaseColorize();
        }
        if (key == GLFW_KEY_S && action == GLFW_PRESS) {
            if (selectedObject) selectedObject->showcaseSpecular();
        }
        if (key == GLFW_KEY_O && action == GLFW_PRESS) {
            if (selectedObject) selectedObject->showcasePointReversed();
        }
        if (key == GLFW_KEY_I && action == GLFW_PRESS) {
            if (selectedObject) selectedObject->showcaseRing();
        }
    }

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
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    }

    void imgui_begin() {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void render_graph_editor() {

        ImGui::Begin("Graph Editor", NULL, 0);
        if (ImGui::Button("Fit all nodes")) {
            fit = GraphEditor::Fit_AllNodes;
        }
        ImGui::SameLine();
        if (ImGui::Button("Fit selected nodes")) {
            fit = GraphEditor::Fit_SelectedNodes;
        }
        ImGui::SameLine();
        ImGui::Checkbox("Hide node slot names", &options.mDrawIONameOnHover);

        if (selectedObject)
            GraphEditor::Show(selectedObject->delegate, options, viewState, true, &fit);

        ImGui::End();
    }

    void render_object_list() {
        ImGui::Begin("Object list");

        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
        if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
        {
            if (ImGui::BeginTabItem("meshes"))
            {
                for (Object *ob : object_manager.objects) {
                    if (ImGui::Selectable(ob->name.c_str(), activeSelected == ob)) {
                        selectedObject = ob;
                        activeSelected = ob;
                    }
                }
                ImGui::Separator();
                if (ImGui::Button("+")) object_manager.addNewModel("res/models/rat/rat2.obj");
                ImGui::SameLine();
                if (ImGui::Button("-")) {
                    object_manager.deleteObject(selectedObject);
                    activeSelected = nullptr;
                    selectedObject = nullptr;
                }
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("lights"))
            {
                for (LightObject *ob : object_manager.lightObjects) {
                    if (ImGui::Selectable(ob->name.c_str(), activeSelected == ob)) {
                        selectedLight = ob;
                        activeSelected = ob;
                        stack::activeLight = ob;
                    }
                }
                ImGui::Separator();
                if (ImGui::Button("+")) object_manager.addNewLight();
                ImGui::SameLine();
                if (ImGui::Button("-")) {
                    object_manager.deleteLight(selectedLight);
                    activeSelected = nullptr;
                    selectedLight = nullptr;
                }
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::Separator();
        if (activeSelected) {
            if (activeSelected == selectedObject) {
                // display selected object properties
                if (ImGui::BeginCombo("model", selectedObject->filename.c_str())) {
                    for (auto &modelFile : files::modelFiles) {
                        bool is_selected = (selectedObject->filename == modelFile);
                        if (ImGui::Selectable(modelFile.c_str(), is_selected)) {
                            selectedObject->changeFile(modelFile);
                        }
                        if (is_selected) {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                // for colorize test
                ImGui::ColorEdit3("color", (float*)&stack::color);
            } else if (activeSelected == selectedLight) {
                // display selected light properties
                if (ImGui::BeginCombo("type", lightTypeNames[selectedLight->type])) {
                    for (int i = 0; i <2; i++) {
                        bool is_selected = (selectedLight->type == i);
                        if (ImGui::Selectable(lightTypeNames[i], is_selected)) {
                            selectedLight->changeType(i);
                        }
                        if (is_selected) {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                ImGui::ColorEdit3("color", (float*)&selectedLight->color);
            }
        }
        ImGui::End();
    }

    void draw_imguizmo() {
        ImGuizmo::BeginFrame();
        ImGuiIO& io = ImGui::GetIO();
        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

        float viewManipulateRight = io.DisplaySize.x;
        float viewManipulateTop = 0;

        ImGuizmo::ViewManipulate(&view[0][0], camDist, ImVec2(viewManipulateRight - 128, viewManipulateTop), ImVec2(128, 128), 0x10101010);

        if (activeSelected) {
            ImGuizmo::Manipulate(&view[0][0], &projection[0][0], currentGizmoOperation, ImGuizmo::LOCAL, activeSelected->node.getTranslationMatrix(), nullptr, nullptr, nullptr, nullptr);
        }
    }

    void imgui_render() {

        //ImGui::SetNextWindowSize({0, 0});
        draw_imguizmo();
        render_object_list();
        render_graph_editor();
    }

    void imgui_end() {
        ImGui::Render();
        glfwMakeContextCurrent(window);
        glfwGetFramebufferSize(window, &display_w, &display_h);

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

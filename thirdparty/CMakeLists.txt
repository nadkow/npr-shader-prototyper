# CPM - package manager
include(CPM)

# OpenGL
find_package(OpenGL REQUIRED)

# glad
set(GLAD_DIR ${CMAKE_CURRENT_LIST_DIR}/glad)
set(glad_SOURCE_DIR ${GLAD_DIR}/include CACHE INTERNAL "")
add_library(glad STATIC ${GLAD_DIR}/src/glad.c)
target_include_directories(glad PRIVATE ${GLAD_DIR}/include)

# stb_image
set(STB_IMAGE_DIR ${CMAKE_CURRENT_LIST_DIR}/stb_image)
set(stb_image_SOURCE_DIR ${STB_IMAGE_DIR} CACHE INTERNAL "")
add_library(stb_image STATIC ${STB_IMAGE_DIR}/stb_image.cpp)
target_include_directories(stb_image PRIVATE ${STB_IMAGE_DIR})

# ImGuizmo
set(IMGUIZMO_DIR ${CMAKE_CURRENT_LIST_DIR}/imguizmo)
set (imguizmo_SOURCE_DIR ${IMGUIZMO_DIR} CACHE INTERNAL "")
add_library(imguizmo STATIC ${IMGUIZMO_DIR}/ImGuizmo.cpp)
add_library(graph_editor STATIC ${IMGUIZMO_DIR}/GraphEditor.cpp)
target_include_directories(imguizmo PRIVATE ${IMGUIZMO_DIR})
target_include_directories(graph_editor PRIVATE ${IMGUIZMO_DIR})

# ImGui implementations
set(IMGUI_IMPL_DIR ${CMAKE_CURRENT_LIST_DIR}/imgui_impl)
set(imgui_impl_SOURCE_DIR ${IMGUI_IMPL_DIR} CACHE INTERNAL "")
add_library(imgui_impl STATIC ${IMGUI_IMPL_DIR}/imgui_impl_glfw.cpp ${IMGUI_IMPL_DIR}/imgui_impl_opengl3.cpp)
target_include_directories(imgui_impl PRIVATE ${IMGUI_IMPL_DIR})

# other
CPMAddPackage("gh:assimp/assimp@5.2.5")
CPMAddPackage("gh:glfw/glfw#3.3.8")
CPMAddPackage("gh:g-truc/glm#0.9.9.8")
CPMAddPackage("gh:ocornut/imgui#v1.90.4-docking")
CPMAddPackage("gh:gabime/spdlog@1.10.0")
CPMAddPackage("gh:pantor/inja@3.4.0")

set(imgui_SOURCE_DIR ${imgui_SOURCE_DIR} CACHE INTERNAL "")
add_library(imgui STATIC ${imgui_SOURCE_DIR}/imgui.cpp
					     ${imgui_SOURCE_DIR}/imgui_demo.cpp
					     ${imgui_SOURCE_DIR}/imgui_draw.cpp
						 ${imgui_SOURCE_DIR}/imgui_tables.cpp
					     ${imgui_SOURCE_DIR}/imgui_widgets.cpp)

target_include_directories(imguizmo PRIVATE ${imgui_SOURCE_DIR})
target_include_directories(graph_editor PRIVATE ${imgui_SOURCE_DIR})
target_include_directories(imgui_impl PRIVATE ${imgui_SOURCE_DIR} ${glfw_SOURCE_DIR})

set_target_properties(glad
                      stb_image 
                      assimp 
                      glfw 
                      glm
					  imgui
					  imgui_impl
                      imguizmo
					  graph_editor
					  inja
                      spdlog PROPERTIES FOLDER "thirdparty")

if (TARGET zlibstatic)
    set_target_properties(zlibstatic PROPERTIES FOLDER "thirdparty")
endif()
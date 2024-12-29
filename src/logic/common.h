#ifndef NPRSPR_COMMON_H
#define NPRSPR_COMMON_H

constexpr int32_t WINDOW_WIDTH = 1920;
constexpr int32_t WINDOW_HEIGHT = 1080;

ImVec4 clear_color = ImVec4(0.3f, 0.3f, 0.3f, 1.00f);

float camDist = 10.f;
glm::vec3 cameraPos = glm::vec3(0.0f, 3.f, camDist);
glm::vec3 cameraFront = glm::vec3(0.0f, -3.0f, -camDist);
glm::vec3 cameraUp = glm::vec3(0.0f, 0.996f, 0.08f);
glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

float deltaTime = 0.0f;  // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
float currentFrame;

#endif //NPRSPR_COMMON_H

#ifndef NPRSPR_OBJECT_H
#define NPRSPR_OBJECT_H

#include <utility>

#include "Node.h"

extern glm::mat4 view;
extern glm::mat4 projection;

enum lightType {directional, point, cone};

class Object {

public:

    bool visible = true;
    std::string name;

    explicit Object(std::string filepath) : filepath(std::move(filepath)) {
        //rootNode.addChild(&ratNode);
        model = Model(std::filesystem::absolute(this->filepath));
        shader = ShaderProgram("res/shaders/model.vert", "res/shaders/model.frag");
        extractFilename();
    }

    Object() = default;

    void draw() {
        shader.use();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setMat4("transform", node.getTransform());
        model.Draw(shader);
    }

private:
    std::string filepath;
    Model model;
    Node node;
    ShaderProgram shader;

    void extractFilename() {
        // extract name from filepath
        std::size_t found = filepath.find_last_of("/\\");
        //std::cout << " path: " << filepath.substr(0,found) << '\n';
        name = filepath.substr(found+1);
    }
};

class LightObject {

public:
    ImVec4 color;
    lightType type;
    std::string name;

    LightObject(std::string name) {
        this->name = std::move(name);
    }

private:
    Model model;
    Node node;
    ShaderProgram shader;
    glm::vec4 baseDirection;
};

class ObjectManager {

public:

    std::vector<Object> objects;
    std::vector<LightObject> lightObjects;

    void importModel(const std::string& filename) {
        objects.emplace_back(filename);
    }

    void createLightObject() {
        std::string number = "light";
        number.append(std::to_string(lightObjects.size()));
        lightObjects.emplace_back(number);
    }

    void draw() {
        // draw all objects
        // TODO this draws every frame so make list of visible objects (update list when visibility changes) instead of if
        for (auto obj : objects) {
            if (obj.visible) obj.draw();
        }
    }

private:
    Node rootNode;
};

#endif //NPRSPR_OBJECT_H

#ifndef NPRSPR_OBJECT_H
#define NPRSPR_OBJECT_H

#include "Node.h"

extern glm::mat4 view;

class Object {

public:

    bool visible;

    explicit Object(std::string filename) : filename(std::move(filename)) {
        //rootNode.addChild(&ratNode);
        model = Model(std::filesystem::absolute(this->filename));
        shader = Shader("res/shaders/model.vert", "res/shaders/model.frag");
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
    std::string filename;
    Model model;
    Node node;
    Shader shader;
};

class ObjectManager {

public:

    void importModel(const std::string& filename) {
        objects.emplace_back(filename);
    }

    void draw() {
        for (auto obj : objects) {
            obj.draw();
        }
    }

private:

    std::vector<Object> objects;
    Node rootNode;
};

#endif //NPRSPR_OBJECT_H

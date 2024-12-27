#ifndef NPRSPR_OBJECT_H
#define NPRSPR_OBJECT_H

#include <utility>

#include "Node.h"

extern glm::mat4 view;
extern glm::mat4 projection;

enum lightType {directional, point};

static int globalId;

class GeneralObject {
public:
    std::string name;
    bool visible = true;
    Node node;
protected:
    int id;
};

class Object : public GeneralObject {

public:

    std::string filename;

    explicit Object(std::string filepath) : filename(std::move(filepath)) {
        model = Model(std::filesystem::absolute(filename));
        shader = ShaderProgram("res/shaders/model.vert", "res/shaders/model.frag");
        extractFilename();
        name.append(std::to_string(globalId));
        id = globalId;
        globalId++;
    }

    Object() = default;

    void draw() {
        shader.use();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setMat4("transform", node.getTransform());
        model.Draw(shader);
    }

    void changeFile(const std::string& newfilename) {
        model = Model(std::filesystem::absolute(newfilename));
        filename = newfilename;
        extractFilename();
        name.append(std::to_string(id));
    }

private:
    Model model;
    ShaderProgram shader;

    void extractFilename() {
        // extract name from filepath
        std::size_t found = filename.find_last_of("/\\");
        name = filename.substr(found+1);
    }
};

class LightObject : public GeneralObject {

public:
    ImVec4 color = ImVec4(1.f, 1.f, 1.f, 1.f);
    lightType type;

    LightObject() {
        name = "light";
        name.append(std::to_string(globalId));
        id = globalId;
        globalId++;
        type = point;
    }

private:
    Model model;
    ShaderProgram shader;
    glm::vec4 baseDirection = glm::vec4(0.f, -1.f, 0.f, 1.f);
};

class ObjectManager {

public:

    std::vector<Object*> objects;
    std::vector<LightObject*> lightObjects;

    void importModel(const std::string& filename) {
        objects.push_back(new Object(filename));
    }

    void createLightObject() {
        lightObjects.push_back(new LightObject());
    }

    void addNewModel(const std::string& filename) {
        importModel(filename);
    }

    void deleteObject(Object *ob) {
        auto it = std::remove(objects.begin(), objects.end(), ob);
        objects.erase(it, objects.end());
        delete ob;
    }

    void addNewLight() {
        createLightObject();
    }

    void deleteLight(LightObject *ob) {
        auto it = std::remove(lightObjects.begin(), lightObjects.end(), ob);
        lightObjects.erase(it, lightObjects.end());
        delete ob;
    }

    void draw() {
        // draw all objects
        // TODO this draws every frame so make list of visible objects (update list when visibility changes) instead of if
        for (Object *obj : objects) {
            if (obj->visible) obj->draw();
        }
    }

private:
    Node rootNode;
};

#endif //NPRSPR_OBJECT_H

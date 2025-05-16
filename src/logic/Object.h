#ifndef NPRSPR_OBJECT_H
#define NPRSPR_OBJECT_H

#include <utility>

class Object : public GeneralObject {

public:

    std::string filename;
    GraphEditorDelegate delegate;
    GraphManager graph_manager;

    explicit Object(std::string filepath) : filename(std::move(filepath)) {
        model = Model(std::filesystem::absolute(filename));
        shader = new ShaderProgram("res/shaders/default.vert", "res/shaders/default.frag");
        extractFilename();
        name.append(std::to_string(globalId));
        id = globalId;
        globalId++;

        graph_manager = GraphManager();
        events::addListener(&graph_manager);

        delegate.addFinalNode();
        graph_manager.finalNode = delegate.finalNode;
        delegate.finalNode->setShaderProgram(shader);
    }

    Object() = default;

    void draw() {
        // update context before drawing
        //stack::model = &model;
        stack::node = &node;
        shader->use();
        shader->setMat4("projection", projection);
        shader->setMat4("view", view);
        shader->setMat4("transform", stack::node->getTransform());
        shader->setVec3("viewPos", cameraPos);
        model.Draw(*shader);
    }

    void changeFile(const std::string& newfilename) {
        model = Model(std::filesystem::absolute(newfilename));
        filename = newfilename;
        extractFilename();
        name.append(std::to_string(id));
    }

    void changeDelegate(GraphEditorDelegate* d) {
        delegate = *d;
        graph_manager.finalNode = delegate.finalNode;
        shader = delegate.finalNode->shader;
    }

private:
    Model model;
    ShaderProgram* shader;

    void extractFilename() {
        // extract name from filepath
        std::size_t found = filename.find_last_of("/\\");
        name = filename.substr(found + 1);
    }
};



class ObjectManager {

public:

    std::vector<Object *> objects;
    std::vector<LightObject *> lightObjects;

    void createLightObject() {
        lightObjects.push_back(new LightObject());
    }

    void addNewModel(const std::string &filename) {
        objects.push_back(new Object(filename));
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
        for (Object *obj: objects) {
            if (obj->visible) obj->draw();
        }
    }

    void draw(int objectIndex) {
        objects[objectIndex]->draw();
    }

private:
    Node rootNode;
};

#endif //NPRSPR_OBJECT_H

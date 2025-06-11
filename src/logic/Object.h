#ifndef NPRSPR_OBJECT_H
#define NPRSPR_OBJECT_H

#include <utility>

class Object : public GeneralObject {

public:

    std::string filename;
    GraphEditorDelegate delegate;
    std::shared_ptr<GraphManager> graph_manager;

    explicit Object(std::string filepath) : filename(std::move(filepath)) {
        model = Model(std::filesystem::absolute(filename));
        shader = new ShaderProgram("res/shaders/default.vert", "res/shaders/default.frag");
        extractFilename();
        name.append(std::to_string(globalId));
        id = globalId;
        globalId++;

        graph_manager = std::make_shared<GraphManager>();
        graph_manager->initialize();
        delegate.addFinalNode(id);
        graph_manager->finalNode = delegate.finalNode;
        delegate.finalNode->setShaderProgram(shader);
    }

    Object() = default;

    ~Object() {
        if (shader) {
            delete shader;
            shader = nullptr;
        }
        // The GraphManager will be automatically cleaned up by shared_ptr
        // and its destructor will handle unregistering from events
    }

    int get_id() {
        return id;
    }

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
        graph_manager->finalNode = delegate.finalNode;
        shader = delegate.finalNode->shader;
        std::string msg = "Changed delegate. ShaderProgram set to ";
        msg.append(std::to_string(shader->ID));
        spdlog::debug(msg);
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

    std::vector<std::unique_ptr<Object>> objects;
    std::shared_ptr<std::vector<LightObject*>> lightObjects;

    ObjectManager() {
        lightObjects = lightObjectsVector;
    }

    void createLightObject() {
        lightObjects->push_back(new LightObject(lightObjects));
    }

    void addNewModel(const std::string &filename) {
        objects.push_back(std::make_unique<Object>(filename));
    }

    void deleteObject(Object *ob) {
        auto it = std::find_if(objects.begin(), objects.end(),
            [ob](const std::unique_ptr<Object>& ptr) { return ptr.get() == ob; });
        if (it != objects.end()) {
            objects.erase(it);
        }
    }

    void addNewLight() {
        createLightObject();
    }

    void deleteLight(LightObject *ob) {
        if (lightObjects->size() == 1) return;
        auto it = std::remove(lightObjects->begin(), lightObjects->end(), ob);
        lightObjects->erase(it, lightObjects->end());
        delete ob;
    }

    void draw() {
        for (const auto& obj : objects) {
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

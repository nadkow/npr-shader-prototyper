#ifndef NPRSPR_LIGHTOBJECT_H
#define NPRSPR_LIGHTOBJECT_H
enum lightType {
    directional, point
};

const char* lightTypeNames[] = {"directional", "point"};

class LightObject : public GeneralObject {

public:

    glm::vec4 color = glm::vec4(1.f, 1.f, 1.f, 1.f);
    lightType type;

    LightObject() {
        name = "light";
        name.append(std::to_string(globalId));
        id = globalId;
        globalId++;
        type = point;
        node = Node();
    }

    void changeType(int val){
        type = static_cast<lightType>(val);
    }

private:

    Model model;
    ShaderProgram shader;
    glm::vec4 baseDirection = glm::vec4(0.f, -1.f, 0.f, 1.f);
};
#endif //NPRSPR_LIGHTOBJECT_H

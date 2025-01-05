#ifndef NPRSPR_NODEINSTANCE_H
#define NPRSPR_NODEINSTANCE_H

#include <variant>

class NodeInstance {

public:
    using dataType = std::variant<int, float, ImVec2, ImVec4>;
    std::vector<dataType*> inputs;
    std::vector<dataType> defaultInputs;
    std::vector<dataType*> outputs;
    enum slotType {DATA, SHADER, NONE} inputType, outputType;

    virtual void drawNode(ImRect rect, float factor) = 0;

    // inNode - receiving node, outNode - outputting node
    static bool connect(NodeInstance* outNode, int outSlot, NodeInstance* inNode, int inSlot) {
        if (inNode->inputType == outNode->outputType) {
            if(inNode->inputType == DATA) {
                // data to shader or data to data
                // do data types match?
                if (outNode->outputs[outSlot]->index() == inNode->inputs[inSlot]->index()) {
                    inNode->inputs[inSlot] = outNode->outputs[outSlot];
                    inNode->updateConnect(inSlot);
                    return true;
                }
                return false;
            } else {
                // shader to final output
                return true;
            }
        }
        return false;
    };

    // set the input slot of inNode to default value
    static void disconnect(NodeInstance* inNode, int inSlot) {
        inNode->inputs[inSlot] = &(inNode->defaultInputs[inSlot]);
    };

private:

    virtual void updateConnect(int inSlot) = 0;
    virtual void updateDisconnect(int inSlot) = 0;
};

// base class for shader nodes
class ShaderNodeInstance : public NodeInstance {
public:
    ShaderNodeInstance() {
         inputType = DATA;
         outputType = SHADER;
    }
};

// base class for data nodes
class DataNodeInstance : public NodeInstance {
public:
    DataNodeInstance() {
        inputType = DATA;
        outputType = DATA;
    }
};

/* SHADER NODES */

class DrawFlat : public ShaderNodeInstance {

public:
    // default value for unconnected socket
    dataType color = ImVec4(1.0, 1.0, 1.0, 1.);

    DrawFlat() {
        defaultInputs.push_back(color);
        inputs.push_back(&color);
    };

    void drawNode(ImRect rect, float factor) override {
        ImGui::SetCursorPosX(rect.Min.x + 10);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10 * factor);
        ImGui::ColorEdit4("Color", (float *) inputs[0], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel /*| ImGuiColorEditFlags_NoPicker */);

    }

    void updateConnect(int inSlot) override {

    }

    void updateDisconnect(int inSlot) override {

    }

};

/* FINAL OUTPUT NODE */

class DrawFinal : public NodeInstance {

public:
    DrawFinal() {
        inputType = SHADER;
        outputType = NONE;
    }

    void drawNode(ImRect rect, float factor) override {}

    void updateConnect(int inSlot) override {

    }

    void updateDisconnect(int inSlot) override {

    }
};

/* DATA NODES */

class ColorNode : public DataNodeInstance {

public:
    // default value for unconnected socket
    dataType color = ImVec4(1.0, 1.0, 1.0, 1.);

    ColorNode() {
        defaultInputs.push_back(color);
        inputs.push_back(&defaultInputs[0]);
        outputs.push_back(inputs[0]);
    };

    void drawNode(ImRect rect, float factor) override {
        ImGui::SetCursorPosX(rect.Min.x + 10);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10 * factor);
        ImGui::ColorEdit4("Color", (float *) inputs[0], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
        ImGui::ColorEdit4("Color default", (float *) &defaultInputs[0], ImGuiColorEditFlags_NoInputs );
        ImGui::ColorEdit4("Color output", (float *) outputs[0], ImGuiColorEditFlags_NoInputs );
    }

    void updateConnect(int inSlot) override {
        //outputs[0] = inputs[0];
    }

    void updateDisconnect(int inSlot) override {

    }
};
#endif //NPRSPR_NODEINSTANCE_H

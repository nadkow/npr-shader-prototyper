#ifndef NPRSPR_NODEINSTANCE_H
#define NPRSPR_NODEINSTANCE_H

#include <variant>

class NodeInstance {

public:

    struct socket {
        int8_t slot;
        NodeInstance* node;
    };

    using dataType = std::variant<int, float, ImVec2, ImVec4>;
    socket* inputs;
    std::vector<dataType*> defaultInputs;
    std::vector<dataType*> currentInputs;
    std::vector<NodeInstance*>* outputs;
    std::vector<dataType> defaultOutputs;
    bool* outputDirtyFlags = nullptr;
    enum slotType {DATA, SHADER, NONE} inputType, outputType;

    virtual ~NodeInstance() { delete[] inputs; delete[] outputs; delete[] outputDirtyFlags;}

    virtual void drawNode(ImRect rect, float factor) = 0;

    virtual int getOutputCount() = 0;
    virtual int getInputCount() = 0;

    void pullData() {
        // pull to input socket
        for (int i = 0; i < getInputCount(); i++)
            // pull if connected
            if (inputs[i].node)
                currentInputs[i] = inputs[i].node->currentInputs[inputs[i].slot];
            else currentInputs[i] = defaultInputs[i];
    }

    // inNode - receiving node, outNode - outputting node
    static void connect(NodeInstance* outNode, int outSlot, NodeInstance* inNode, int inSlot) {
            if(inNode->inputType == DATA) {
                // data to shader or data to data
                    inNode->inputs[inSlot].node = outNode;
                    outNode->outputs[outSlot].push_back(inNode);
                    inNode->outputDirtyFlags[inSlot] = true;
                    inNode->updateConnect(outNode, inSlot);
            } else {
                // shader to final output
                inNode->updateConnect(outNode, inSlot);
                //inNode->inputs[inSlot].node = outNode;
            }
    };

    static bool canConnect(NodeInstance* outNode, int outSlot, NodeInstance* inNode, int inSlot) {
        if (inNode->inputType == outNode->outputType && inNode != outNode) {
            if(inNode->inputType == DATA) {
                // data to shader or data to data
                // do data types match?
                return outNode->defaultOutputs[outSlot].index() == inNode->defaultInputs[inSlot]->index();
            } else {
                // shader to final output
                return true;
            }
        }
        return false;
    };

    // set the input slot of inNode to default value
    static void disconnect(NodeInstance* inNode, int inSlot, NodeInstance* outNode, int outSlot) {
        outNode->deleteConnection(outSlot, inNode);
        inNode->updateDisconnect(outNode, inSlot);
    };

protected:

    virtual void updateConnect(NodeInstance* node, int inSlot) = 0;
    virtual void updateDisconnect(NodeInstance* node, int inSlot) = 0;

    void deleteConnection(int outSlot, NodeInstance* inNode) {
        auto it = std::remove(outputs[outSlot].begin(), outputs[outSlot].end(), inNode);
        outputs[outSlot].erase(it, outputs[outSlot].end());
    }

    void triggerEvent() {
        NodeEvent ev(this);
        events::fireEvent(&ev);
    }
};

// base class for shader nodes
class ShaderNodeInstance : public NodeInstance {
public:

    ShaderNodeInstance() {
         inputType = DATA;
         outputType = SHADER;
         outputDirtyFlags = new bool[1];
    }

    ShaderNodeInstance(ShaderProgram* shaderProgram) {
        inputType = DATA;
        outputType = SHADER;
        outputDirtyFlags = new bool[1];
        this->shaderProgram = shaderProgram;
    }

    int getOutputCount() override {
        // the stream ends at shader nodes
        return 0;
    }

    void recompile() {
        // TODO make it so only connected nodes recompile
        shaderProgram->recompile(path);
    }

    ShaderProgram* shaderProgram;
    const char* path;
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
    static constexpr int inputCount = 1;
    // default value for unconnected socket
    dataType color = ImVec4(1.0, 1.0, 1.0, 1.);

    DrawFlat() : ShaderNodeInstance(new ShaderProgram("res/shaders/flat.vert", "res/shaders/flat.frag")) {
        //inputs
        inputs = new socket [inputCount]{};
        defaultInputs.push_back(&color);
        currentInputs.push_back(&color);
        // outputs
        outputs = new std::vector<NodeInstance*>[1]{};
        path = "res/shaders/flat.frag";
    };

    void drawNode(ImRect rect, float factor) override {
        ImGui::SetCursorPosX(rect.Min.x + 10);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10 * factor);
        ImGui::ColorEdit4("Color", (float *) currentInputs[0], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel /*| ImGuiColorEditFlags_NoPicker */);

    }

    void updateConnect(NodeInstance* node, int inSlot) override {
        // TODO update flags here
        triggerEvent();
    }

    void updateDisconnect(NodeInstance* node, int inSlot) override {
        // TODO same
        inputs[inSlot].node = nullptr;
        triggerEvent();
    }

    int getInputCount() override {
        return 1;
    }

};

/* FINAL OUTPUT NODE */

class DrawFinal : public NodeInstance {

public:

    DrawFinal(GraphEditor::Template* t) {
        inputType = SHADER;
        outputType = NONE;
        finalNodeTemplate = t;

        //inputs = connectedNodes.data(); TODO
    }

    void drawNode(ImRect rect, float factor) override {
        for (NodeInstance* node : connectedNodes) {
            if (node) {
                ImGui::SetCursorPosX(rect.Min.x + 10);
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 21 * factor);
                ImGui::Text("shader");
            } else {
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 38 * factor);
            }
        }
    }
    int getOutputCount() override { return 0;}
    int getInputCount() override { return 0;}

    void updateConnect(NodeInstance* node, int inSlot) override {
        if (inSlot >= lastOccupiedSlot && inSlot < 64) {
            finalNodeTemplate->mInputCount += 1;
            finalNodeTemplate->mHeight += 40;
            lastOccupiedSlot = inSlot;
            connectedNodes.insert(connectedNodes.end()-1, node);
        } else {
            connectedNodes[inSlot] = node;
        }
    }

    void updateDisconnect(NodeInstance* node, int inSlot) override {
        if (inSlot == lastOccupiedSlot) {
            finalNodeTemplate->mInputCount -= 1;
            finalNodeTemplate->mHeight -= 40;
            //inputs = connectedNodes.data();
            lastOccupiedSlot -= 1;
            connectedNodes.erase(connectedNodes.begin()+inSlot);
        } else {
            connectedNodes[inSlot] = nullptr;
        }
    }

private:
    GraphEditor::Template* finalNodeTemplate;
    int lastOccupiedSlot = 0;
    std::vector<NodeInstance*> connectedNodes = {nullptr};
    std::vector<bool> inputDirtyFlag = {false};
};

/* DATA NODES */

class ColorNode : public DataNodeInstance {

public:
    static constexpr int inputCount = 1;
    static constexpr int outputCount = 1;
    // default value for unconnected socket
    dataType color = ImVec4(1.0, 1.0, 1.0, 1.);

    ColorNode() {
        //inputs
        inputs = new socket [inputCount]{};
        defaultInputs.push_back(&color);
        currentInputs.push_back(&color);
        // outputs
        outputs = new std::vector<NodeInstance*>[outputCount]{};
        defaultOutputs.push_back(color);
        outputDirtyFlags = new bool[outputCount];
    }

    int getOutputCount() override {
        return outputCount;
    }

    int getInputCount() override {
        return inputCount;
    }

    void drawNode(ImRect rect, float factor) override {
        ImGui::SetCursorPosX(rect.Min.x + 10);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10 * factor);
        ImGui::ColorEdit4("Color", (float *) currentInputs[0], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
    }

    void updateConnect(NodeInstance* node, int inSlot) override {
        triggerEvent();
    }

    void updateDisconnect(NodeInstance* node, int inSlot) override {
        inputs[inSlot].node = nullptr;
        triggerEvent();
    }
};

class FloatNode : public DataNodeInstance {

public:
    static constexpr int inputCount = 1;
    static constexpr int outputCount = 1;

    dataType value = 1.f;

    FloatNode() {
        //inputs
        inputs = new socket [inputCount]{};
        defaultInputs.push_back(&value);
        currentInputs.push_back(&value);
        // outputs
        outputs = new std::vector<NodeInstance*>[outputCount]{};
        defaultOutputs.emplace_back(1.f);
        outputDirtyFlags = new bool[outputCount];
    };

    int getOutputCount() override {
        return outputCount;
    }

    int getInputCount() override {
        return inputCount;
    }

    void drawNode(ImRect rect, float factor) override {
        ImGui::SetCursorPosX(rect.Min.x + 10);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10 * factor);
        ImGui::PushItemWidth(80);
        ImGui::InputFloat("value", (float *) currentInputs[0]);
    }

    void updateConnect(NodeInstance* node, int inSlot) override {
        triggerEvent();
    }

    void updateDisconnect(NodeInstance* node, int inSlot) override {
        inputs[inSlot].node = nullptr;
        triggerEvent();
    }
};

class CombineVec4Node : public DataNodeInstance {

public:
    static constexpr int inputCount = 4;
    static constexpr int outputCount = 1;
    //output
    dataType vec = ImVec4{.0, .0, .0, .0};
    // default inputs
    dataType x = 0.f;
    dataType y = 0.f;
    dataType z = 0.f;
    dataType w = 0.f;

    CombineVec4Node() {
        //inputs
        inputs = new socket [inputCount]{};
        defaultInputs.push_back(&x);
        defaultInputs.push_back(&y);
        defaultInputs.push_back(&z);
        defaultInputs.push_back(&w);

        currentInputs.push_back(&x);
        currentInputs.push_back(&y);
        currentInputs.push_back(&z);
        currentInputs.push_back(&w);
        // outputs
        outputs = new std::vector<NodeInstance*>[outputCount]{};
        defaultOutputs.emplace_back(vec);
        outputDirtyFlags = new bool[outputCount];
    };

    int getOutputCount() override {
        return outputCount;
    }

    int getInputCount() override {
        return inputCount;
    }

    void drawNode(ImRect rect, float factor) override {
        ImGui::SetCursorPosX(rect.Min.x + 10);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10 * factor);
        ImGui::PushItemWidth(80);
        ImGui::InputFloat("x", (float *) currentInputs[0]);
        ImGui::SetCursorPosX(rect.Min.x + 10);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10 * factor);
        ImGui::InputFloat("y", (float *) currentInputs[1]);
        ImGui::SetCursorPosX(rect.Min.x + 10);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10 * factor);
        ImGui::InputFloat("z", (float *) currentInputs[2]);
        ImGui::SetCursorPosX(rect.Min.x + 10);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10 * factor);
        ImGui::InputFloat("w", (float *) currentInputs[3]);
    }

    void updateConnect(NodeInstance* node, int inSlot) override {
        triggerEvent();
    }

    void updateDisconnect(NodeInstance* node, int inSlot) override {
        inputs[inSlot].node = nullptr;
        triggerEvent();
    }
};
#endif //NPRSPR_NODEINSTANCE_H

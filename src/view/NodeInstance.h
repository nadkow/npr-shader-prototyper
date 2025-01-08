#ifndef NPRSPR_NODEINSTANCE_H
#define NPRSPR_NODEINSTANCE_H

#include <variant>

class NodeInstance {

public:
    using dataType = std::variant<int, float, ImVec2, ImVec4>;
    NodeInstance** inputs;
    std::vector<dataType> defaultInputs;
    NodeInstance** outputs;
    std::vector<dataType> defaultOutputs;
    enum slotType {DATA, SHADER, NONE} inputType, outputType;

    virtual ~NodeInstance() { delete[] inputs; delete[] outputs;}

    virtual void drawNode(ImRect rect, float factor) = 0;

    virtual int getOutputCount() = 0;

    // inNode - receiving node, outNode - outputting node
    static bool connect(NodeInstance* outNode, int outSlot, NodeInstance* inNode, int inSlot) {
        if (inNode->inputType == outNode->outputType) {
            if(inNode->inputType == DATA) {
                // data to shader or data to data
                // do data types match?
                if (outNode->defaultOutputs[outSlot].index() == inNode->defaultInputs[inSlot].index()) {
                    inNode->inputs[inSlot] = outNode;
                    outNode->outputs[outSlot] = inNode;
                    inNode->updateConnect(inSlot);
                    return true;
                }
                return false;
            } else {
                // shader to final output
                inNode->updateConnect(inSlot);
                inNode->inputs[inSlot] = outNode;
                return true;
            }
        }
        return false;
    };

    // set the input slot of inNode to default value
    static void disconnect(NodeInstance* inNode, int inSlot, NodeInstance* outNode, int outSlot) {
        inNode->inputs[inSlot] = nullptr;
        outNode->outputs[outSlot] = nullptr;
        inNode->updateDisconnect(inSlot);
    };

protected:

    virtual void updateConnect(int inSlot) = 0;
    virtual void updateDisconnect(int inSlot) = 0;

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
    }

    int getOutputCount() override {
        // the stream ends at shader nodes
        return 0;
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
    static constexpr int inputCount = 1;
    // default value for unconnected socket
    dataType color = ImVec4(1.0, 1.0, 1.0, 1.);

    DrawFlat() {
        //inputs
        inputs = new NodeInstance * [inputCount];
        inputs[0] = nullptr;
        defaultInputs.push_back(color);
        // outputs
        outputs = new NodeInstance * [1];
    };

    void drawNode(ImRect rect, float factor) override {
        ImGui::SetCursorPosX(rect.Min.x + 10);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10 * factor);
        ImGui::ColorEdit4("Color", (float *) &color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel /*| ImGuiColorEditFlags_NoPicker */);

    }

    void updateConnect(int inSlot) override {
        // TODO update flags here
        triggerEvent();
    }

    void updateDisconnect(int inSlot) override {
        // TODO same
        triggerEvent();
    }

};

/* FINAL OUTPUT NODE */

class DrawFinal : public NodeInstance {

public:

    DrawFinal(GraphEditor::Template* t) {
        inputType = SHADER;
        outputType = NONE;
        finalNodeTemplate = t;

        inputs = connectedNodes.data();
    }

    void drawNode(ImRect rect, float factor) override {}
    int getOutputCount() override { return 0;}

    void updateConnect(int inSlot) override {
        if (inSlot > lastOccupiedSlot && inSlot < 64) {
            finalNodeTemplate->mInputCount += 1;
            finalNodeTemplate->mHeight += 40;
            lastOccupiedSlot = inSlot;
            // reserve space for new connection
            connectedNodes.push_back(nullptr);
            inputs = connectedNodes.data();
        }
    }

    void updateDisconnect(int inSlot) override {
        if (inSlot == lastOccupiedSlot) {
            finalNodeTemplate->mInputCount -= 1;
            finalNodeTemplate->mHeight -= 40;
            connectedNodes.resize(lastOccupiedSlot+1);
            inputs = connectedNodes.data();
            lastOccupiedSlot -= 1;
        }
    }

private:
    GraphEditor::Template* finalNodeTemplate;
    int lastOccupiedSlot = 0;
    std::vector<NodeInstance*> connectedNodes = {nullptr};
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
        inputs = new NodeInstance * [inputCount];
        inputs[0] = nullptr;
        defaultInputs.push_back(color);
        // outputs
        outputs = new NodeInstance * [outputCount];
        outputs[0] = nullptr;
        defaultOutputs.push_back(color);
    }

    int getOutputCount() override {
        return outputCount;
    }

    void drawNode(ImRect rect, float factor) override {
        ImGui::SetCursorPosX(rect.Min.x + 10);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10 * factor);
        ImGui::ColorEdit4("Color", (float *) &color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
    }

    void updateConnect(int inSlot) override {
        triggerEvent();
    }

    void updateDisconnect(int inSlot) override {
        triggerEvent();
    }
};

class FloatNode : public DataNodeInstance {

public:
    static constexpr int inputCount = 1;
    static constexpr int outputCount = 1;

    FloatNode() {
        //inputs
        inputs = new NodeInstance * [inputCount];
        inputs[0] = nullptr;
        defaultInputs.emplace_back(1.f);
        // outputs
        outputs = new NodeInstance * [outputCount];
        outputs[0] = nullptr;
        defaultOutputs.emplace_back(1.f);
    };

    int getOutputCount() override {
        return outputCount;
    }

    void drawNode(ImRect rect, float factor) override {
        ImGui::SetCursorPosX(rect.Min.x + 10);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10 * factor);
        ImGui::InputFloat("value", (float *) &defaultInputs[0]);
    }

    void updateConnect(int inSlot) override {
        triggerEvent();
    }

    void updateDisconnect(int inSlot) override {
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
    float x = 0;
    float y = 0;
    float z = 0;
    float w = 0;

    CombineVec4Node() {
        //inputs
        inputs = new NodeInstance * [inputCount]{};
        defaultInputs.emplace_back(x);
        defaultInputs.emplace_back(y);
        defaultInputs.emplace_back(z);
        defaultInputs.emplace_back(w);
        // outputs
        outputs = new NodeInstance * [outputCount];
        outputs[0] = nullptr;
        defaultOutputs.emplace_back(vec);
    };

    int getOutputCount() override {
        return outputCount;
    }

    void drawNode(ImRect rect, float factor) override {
        ImGui::SetCursorPosX(rect.Min.x + 10);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10 * factor);
        ImGui::InputFloat("x", (float *) &defaultInputs[0]);
        ImGui::InputFloat("y", (float *) &defaultInputs[1]);
        ImGui::InputFloat("z", (float *) &defaultInputs[2]);
        ImGui::InputFloat("w", (float *) &defaultInputs[3]);
    }

    void updateConnect(int inSlot) override {
        /*
        ImVec4 temp = std::get<ImVec4>(vec);
        switch(inSlot) {
            case 0:
                temp.x = *inputs[0];
                break;
            case 1:
                // code block
                break;
            case 2:
                // code block
                break;
            case 3:
                // code block
                break;

        }*/
        triggerEvent();
    }

    void updateDisconnect(int inSlot) override {
        triggerEvent();
    }
};
#endif //NPRSPR_NODEINSTANCE_H

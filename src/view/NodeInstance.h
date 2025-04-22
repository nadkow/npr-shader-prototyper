#ifndef NPRSPR_NODEINSTANCE_H
#define NPRSPR_NODEINSTANCE_H

#include <variant>

enum slotType {DATA, SHADER, NONE};

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
    slotType inputType, outputType;

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

    int getOutputCount() override {
        // the stream ends at shader nodes
        return 0;
    }

    virtual void recompile() = 0;

    std::string shader_text;
    const char* path;
    block::pass_name shader_type;
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

    DrawFlat() : ShaderNodeInstance() {
        //inputs
        inputs = new socket [inputCount]{};
        defaultInputs.push_back(&color);
        currentInputs.push_back(&color);
        // outputs
        outputs = new std::vector<NodeInstance*>[1]{};
        path = "res/shader_templates/flat.frag";
        shader_type = FLAT;
    };

    void drawNode(ImRect rect, float factor) override {
        ImGui::SetCursorPosX(rect.Min.x + 10);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10 * factor);
        if(ImGui::ColorEdit4("Color", (float *) currentInputs[0], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel /*| ImGuiColorEditFlags_NoPicker */))
            triggerEvent();
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

    void recompile() override{
        // not recompile, just edit the file
        inja::Environment env;
        inja::Template temp = env.parse_template(path);
        inja::json data;
        ImVec4 tempcolor = std::get<ImVec4>(*currentInputs[0]);
        data["color"]["r"] = tempcolor.x;
        data["color"]["g"] = tempcolor.y;
        data["color"]["b"] = tempcolor.z;
        env.write(temp, data, "res/shaders/flat.frag");
        shader_text = env.render_file(path, data);
        block::recompile(shader_type);
    }

};

class DrawFresnel : public ShaderNodeInstance {

public:
    static constexpr int inputCount = 1;
    // default value for unconnected socket
    dataType color = ImVec4(1.0, 1.0, 1.0, 1.);

    DrawFresnel() : ShaderNodeInstance() {
        //inputs
        inputs = new socket [inputCount]{};
        defaultInputs.push_back(&color);
        currentInputs.push_back(&color);
        // outputs
        outputs = new std::vector<NodeInstance*>[1]{};
        path = "res/shader_templates/fresnel.frag";
        shader_type = FRESNEL;
    };

    void drawNode(ImRect rect, float factor) override {
        ImGui::SetCursorPosX(rect.Min.x + 10);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10 * factor);
        if(ImGui::ColorEdit4("Color", (float *) currentInputs[0], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel /*| ImGuiColorEditFlags_NoPicker */))
            triggerEvent();
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

    void recompile() override{
        // not recompile, just edit the file
        inja::Environment env;
        inja::Template temp = env.parse_template(path);
        inja::json data;
        ImVec4 tempcolor = std::get<ImVec4>(*currentInputs[0]);
        data["color"]["r"] = tempcolor.x;
        data["color"]["g"] = tempcolor.y;
        data["color"]["b"] = tempcolor.z;
        env.write(temp, data, "res/shaders/fresnel.frag");
        shader_text = env.render_file(path, data);
        block::recompile(shader_type);
    }

};

/* FINAL OUTPUT NODE */

class DrawFinal : public NodeInstance {

public:

    DrawFinal(GraphEditor::Template* t) {
        inputType = SHADER;
        outputType = NONE;
        finalNodeTemplate = t;
        //shaderStack = shader;

        //inputs = connectedNodes.data(); TODO
    }

    void setShaderStack(ShaderStack* s) {
        shaderStack = s;
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
        triggerEvent();
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
        triggerEvent();
    }

    void recompile() {
        int i = 1;
        shaderStack->blocks.resize(connectedNodes.size()+1);
        for (NodeInstance* node : connectedNodes) {
            if (node) {
                shaderStack->blocks[i] = dynamic_cast<ShaderNodeInstance *>(node)->shader_type;
            } else {
                shaderStack->blocks[i] = PASS;
            }
            i++;
        }
    }

private:
    ShaderStack* shaderStack;
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
        if(ImGui::ColorEdit4("Color", (float *) currentInputs[0], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
            triggerEvent();
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

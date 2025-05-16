#ifndef NPRSPR_NODEINSTANCE_H
#define NPRSPR_NODEINSTANCE_H

#include <variant>

enum slotType {DATA, SHADER, NONE};
enum nodeType {FINAL, SHADER_FLAT, SHADER_FRESNEL, SHADER_COLORIZE, SHADER_RING, SHADER_TEXTURE, DATA_COLOR, DATA_FLOAT, DATA_COMBINEVEC4};
const char* nodeNames[] {"final", "flat", "fresnel", "colorize", "metal (ring)", "texture", "color", "float", "combine vec4"};

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
    std::vector<dataType*> currentOutputs;
    bool* outputDirtyFlags = nullptr;
    slotType inputType, outputType;

    virtual ~NodeInstance() { delete[] inputs; delete[] outputs; delete[] outputDirtyFlags;}

    virtual void drawNode(ImRect rect, float factor) = 0;

    virtual int getOutputCount() = 0;
    virtual int getInputCount() = 0;

    void pullData() {
        // pull to input socket
        for (int i = 0; i < getInputCount(); i++) {
            // pull if connected
            if (inputs[i].node)
                currentInputs[i] = inputs[i].node->currentOutputs[inputs[i].slot];
            else currentInputs[i] = defaultInputs[i];
        }
        recalculateOutput();
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
    virtual void recalculateOutput() = 0;

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

    void recalculateOutput() override {}

    std::string shader_text;
    const char* path;
    //block::pass_name shader_type;
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
    ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel;

    DrawFlat() : ShaderNodeInstance() {
        //inputs
        inputs = new socket [inputCount]{};
        defaultInputs.push_back(&color);
        currentInputs.push_back(&color);
        // outputs
        outputs = new std::vector<NodeInstance*>[1]{};
        path = "res/shader_templates/flat.frag";
        // can't invoke recompile() here ugh
        inja::Environment env;
        inja::Template temp = env.parse_template(path);
        inja::json data;
        ImVec4 tempcolor = std::get<ImVec4>(*currentInputs[0]);
        data["color"]["r"] = tempcolor.x;
        data["color"]["g"] = tempcolor.y;
        data["color"]["b"] = tempcolor.z;
        shader_text = env.render(temp, data);
    };

    void drawNode(ImRect rect, float factor) override {
        ImGui::SetCursorPosX(rect.Min.x + 10);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10 * factor);
        if(ImGui::ColorEdit4("Color", (float *) currentInputs[0], flags))
            triggerEvent();
    }

    void updateConnect(NodeInstance* node, int inSlot) override {
        triggerEvent();
        flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoPicker;
    }

    void updateDisconnect(NodeInstance* node, int inSlot) override {
        inputs[inSlot].node = nullptr;
        triggerEvent();
        flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel;
    }

    int getInputCount() override {
        return 1;
    }

    void recompile() override {
        // not recompile, just edit the file
        inja::Environment env;
        inja::Template temp = env.parse_template(path);
        inja::json data;
        ImVec4 tempcolor = std::get<ImVec4>(*currentInputs[0]);
        data["color"]["r"] = tempcolor.x;
        data["color"]["g"] = tempcolor.y;
        data["color"]["b"] = tempcolor.z;
        shader_text = env.render(temp, data);
    }

};

class DrawColorize : public ShaderNodeInstance {

public:
    static constexpr int inputCount = 1;
    // default value for unconnected socket
    dataType color = ImVec4(1.0, 1.0, 1.0, 1.);
    ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel;

    DrawColorize() : ShaderNodeInstance() {
        //inputs
        inputs = new socket [inputCount]{};
        defaultInputs.push_back(&color);
        currentInputs.push_back(&color);
        // outputs
        outputs = new std::vector<NodeInstance*>[1]{};
        path = "res/shader_templates/colorize.frag";
        // not recompile, just edit the file
        inja::Environment env;
        inja::Template temp = env.parse_template(path);
        inja::json data;
        ImVec4 tempcolor = std::get<ImVec4>(*currentInputs[0]);
        data["color"]["r"] = tempcolor.x;
        data["color"]["g"] = tempcolor.y;
        data["color"]["b"] = tempcolor.z;
        shader_text = env.render(temp, data);
    };

    void drawNode(ImRect rect, float factor) override {
        ImGui::SetCursorPosX(rect.Min.x + 10);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10 * factor);
        if(ImGui::ColorEdit4("Color", (float *) currentInputs[0], flags))
            triggerEvent();
    }

    void updateConnect(NodeInstance* node, int inSlot) override {
        triggerEvent();
        flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoPicker;
    }

    void updateDisconnect(NodeInstance* node, int inSlot) override {
        inputs[inSlot].node = nullptr;
        triggerEvent();
        flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel;
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
        shader_text = env.render(temp, data);
    }

};

class DrawRing : public ShaderNodeInstance {

public:
    static constexpr int inputCount = 1;
    // default value for unconnected socket
    dataType bands = 8;
    ImGuiInputTextFlags flags = ImGuiInputTextFlags_None;

    DrawRing() : ShaderNodeInstance() {
        //inputs
        inputs = new socket [inputCount]{};
        defaultInputs.push_back(&bands);
        currentInputs.push_back(&bands);
        // outputs
        outputs = new std::vector<NodeInstance*>[1]{};
        path = "res/shader_templates/ring.frag";
        // not recompile, just edit the file
        inja::Environment env;
        inja::Template temp = env.parse_template(path);
        inja::json data;
        data["bands"] = std::get<int>(bands);
        shader_text = env.render(temp, data);
    };

    void drawNode(ImRect rect, float factor) override {
        ImGui::SetCursorPosX(rect.Min.x + 10);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10 * factor);
        ImGui::PushItemWidth(80);
        if(ImGui::InputInt("bands", (int *) &bands, 1, 1, flags)) {
            if (std::get<int>(bands) < 1) bands = 1;
            else if (std::get<int>(bands) > 32) bands = 32;
            triggerEvent();
        }
    }

    void updateConnect(NodeInstance* node, int inSlot) override {
        triggerEvent();
        flags = ImGuiInputTextFlags_ReadOnly;
    }

    void updateDisconnect(NodeInstance* node, int inSlot) override {
        inputs[inSlot].node = nullptr;
        triggerEvent();
        flags = ImGuiInputTextFlags_None;
    }

    int getInputCount() override {
        return 1;
    }

    void recompile() override{
        // not recompile, just edit the file
        inja::Environment env;
        inja::Template temp = env.parse_template(path);
        inja::json data;
        data["bands"] = std::get<int>(bands);
        shader_text = env.render(temp, data);
    }

};

class DrawTexture : public ShaderNodeInstance {

public:
    static constexpr int inputCount = 0;

    DrawTexture() : ShaderNodeInstance() {
        // outputs
        outputs = new std::vector<NodeInstance*>[1]{};
        path = "";
        //shader_type = TEXTURE;
    };

    void drawNode(ImRect rect, float factor) override {
        ImGui::SetCursorPosX(rect.Min.x + 10);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10 * factor);
        ImGui::PushItemWidth(80);
        ImGui::Text("default texture");
    }

    void updateConnect(NodeInstance* node, int inSlot) override {
        triggerEvent();
    }

    void updateDisconnect(NodeInstance* node, int inSlot) override {
        inputs[inSlot].node = nullptr;
        triggerEvent();
    }

    int getInputCount() override {
        return 0;
    }

    void recompile() override{

    }

};

static int graphCount = 0;

class DrawFresnel : public ShaderNodeInstance {

public:
    static constexpr int inputCount = 1;
    // default value for unconnected socket
    dataType color = ImVec4(1.0, 1.0, 1.0, 1.);
    ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel;

    DrawFresnel() : ShaderNodeInstance() {
        //inputs
        inputs = new socket [inputCount]{};
        defaultInputs.push_back(&color);
        currentInputs.push_back(&color);
        // outputs
        outputs = new std::vector<NodeInstance*>[1]{};
        path = "res/shader_templates/fresnel.frag";
        // not recompile, just edit the file
        inja::Environment env;
        inja::Template temp = env.parse_template(path);
        inja::json data;
        ImVec4 tempcolor = std::get<ImVec4>(*currentInputs[0]);
        data["color"]["r"] = tempcolor.x;
        data["color"]["g"] = tempcolor.y;
        data["color"]["b"] = tempcolor.z;
        shader_text = env.render(temp, data);
    };

    void drawNode(ImRect rect, float factor) override {
        ImGui::SetCursorPosX(rect.Min.x + 10);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10 * factor);
        if(ImGui::ColorEdit4("Color", (float *) currentInputs[0], flags))
            triggerEvent();
    }

    void updateConnect(NodeInstance* node, int inSlot) override {
        triggerEvent();
        flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoPicker;
    }

    void updateDisconnect(NodeInstance* node, int inSlot) override {
        inputs[inSlot].node = nullptr;
        triggerEvent();
        flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel;
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
        shader_text = env.render(temp, data);
    }

};

/* FINAL OUTPUT NODE */

class DrawFinal : public NodeInstance {

public:
    std::string filename;
    ShaderProgram* shader;

    DrawFinal(GraphEditor::Template* t) {
        inputType = SHADER;
        outputType = NONE;
        finalNodeTemplate = t;
        graphCount++;
        filename = "res/shaders/graph";
        filename.append(std::to_string(graphCount));
        filename.append(".frag");
    }

    DrawFinal() {
        inputType = SHADER;
        outputType = NONE;
        graphCount++;
        filename = "res/shaders/graph";
        filename.append(std::to_string(graphCount));
        filename.append(".frag");
    }

    void setTemplate(GraphEditor::Template* t) {
        finalNodeTemplate = t;
    }

    void setShaderProgram(ShaderProgram* s) {
        shader = s;
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
            connectedNodes.insert(connectedNodes.end()-1, dynamic_cast<ShaderNodeInstance *>(node));
        } else {
            connectedNodes[inSlot] = dynamic_cast<ShaderNodeInstance *>(node);
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
        std::ofstream out(filename, std::ios::out | std::ios::trunc);
        if (!out) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return;
        }
        std::ifstream prefix("res/shader_templates/default.frag", std::ios::in);
        out << std::string((std::istreambuf_iterator<char>(prefix)),std::istreambuf_iterator<char>());
        prefix.close();
        for (ShaderNodeInstance* node : connectedNodes) {
            if (node) {
                out << node->shader_text << "\n";
            }
            i++;
        }
        out << "}";
        out.close();
        shader->recompile("res/shaders/default.vert", filename.c_str());
    }

    void compile() {
        shader = new ShaderProgram("res/shaders/default.vert", filename.c_str());
    }

    void recalculateOutput() override {}

private:
    GraphEditor::Template* finalNodeTemplate;
    int lastOccupiedSlot = 0;
    std::vector<ShaderNodeInstance*> connectedNodes = {nullptr};
    std::vector<bool> inputDirtyFlag = {false};
};

/* DATA NODES */

class ColorNode : public DataNodeInstance {

public:
    static constexpr int inputCount = 1;
    static constexpr int outputCount = 1;
    // default value for unconnected socket
    dataType color = ImVec4(1.0, 1.0, 1.0, 1.);
    ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel;

    ColorNode() {
        //inputs
        inputs = new socket [inputCount]{};
        defaultInputs.push_back(&color);
        currentInputs.push_back(&color);
        // outputs
        outputs = new std::vector<NodeInstance*>[outputCount]{};
        defaultOutputs.push_back(color);
        currentOutputs.push_back(&color);
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
        if(ImGui::ColorEdit4("Color", (float *) currentOutputs[0], flags))
            // so that the shader updates also
            triggerEvent();
    }

    void updateConnect(NodeInstance* node, int inSlot) override {
        // update inputs
        triggerEvent();
        flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoPicker;
    }

    void updateDisconnect(NodeInstance* node, int inSlot) override {
        // update inputs
        inputs[inSlot].node = nullptr;
        triggerEvent();
        flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel;
    }

    void recalculateOutput() override {
        currentOutputs[0] = currentInputs[0];
    }
};

class FloatNode : public DataNodeInstance {

public:
    static constexpr int inputCount = 1;
    static constexpr int outputCount = 1;

    dataType value = 1.f;
    ImGuiInputTextFlags flags = ImGuiInputTextFlags_None;

    FloatNode() {
        //inputs
        inputs = new socket [inputCount]{};
        defaultInputs.push_back(&value);
        currentInputs.push_back(&value);
        // outputs
        outputs = new std::vector<NodeInstance*>[outputCount]{};
        defaultOutputs.emplace_back(1.f);
        currentOutputs.push_back(&value);
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
        if(ImGui::InputFloat("value", (float *) currentOutputs[0], 0.f, 0.f, "%.3f", flags))
            triggerEvent();
    }

    void updateConnect(NodeInstance* node, int inSlot) override {
        triggerEvent();
        flags = ImGuiInputTextFlags_ReadOnly;
    }

    void updateDisconnect(NodeInstance* node, int inSlot) override {
        inputs[inSlot].node = nullptr;
        triggerEvent();
        flags = ImGuiInputTextFlags_None;
    }

    void recalculateOutput() override {
        currentOutputs[0] = currentInputs[0];
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
    ImGuiInputTextFlags flags[4] = {ImGuiInputTextFlags_None, ImGuiInputTextFlags_None, ImGuiInputTextFlags_None, ImGuiInputTextFlags_None};

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
        currentOutputs.emplace_back(&vec);
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
        if(ImGui::InputFloat("x", (float *) currentInputs[0], 0.f, 0.f, "%.3f", flags[0]))
            triggerEvent();
        ImGui::SetCursorPosX(rect.Min.x + 10);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10 * factor);
        if(ImGui::InputFloat("y", (float *) currentInputs[1], 0.f, 0.f, "%.3f", flags[1]))
            triggerEvent();
        ImGui::SetCursorPosX(rect.Min.x + 10);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10 * factor);
        if(ImGui::InputFloat("z", (float *) currentInputs[2], 0.f, 0.f, "%.3f", flags[2]))
            triggerEvent();
        ImGui::SetCursorPosX(rect.Min.x + 10);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10 * factor);
        if(ImGui::InputFloat("w", (float *) currentInputs[3], 0.f, 0.f, "%.3f", flags[3]))
            triggerEvent();
    }

    void updateConnect(NodeInstance* node, int inSlot) override {
        triggerEvent();
        flags[inSlot] = ImGuiInputTextFlags_ReadOnly;
    }

    void updateDisconnect(NodeInstance* node, int inSlot) override {
        inputs[inSlot].node = nullptr;
        triggerEvent();
        flags[inSlot] = ImGuiInputTextFlags_None;
    }

    void recalculateOutput() override {
        std::get<ImVec4>(vec) = ImVec4(std::get<float>(*currentInputs[0]), std::get<float>(*currentInputs[1]), std::get<float>(*currentInputs[2]), std::get<float>(*currentInputs[3]));
    }
};

/**
std::unique_ptr<NodeInstance> instantiateNode(nodeType type) {
    switch(type) {
        case SHADER_FLAT: return std::make_unique<DrawFlat>();
        case SHADER_FRESNEL: return std::make_unique<DrawFresnel>();
        case DATA_COLOR: return std::make_unique<ColorNode>();
        case DATA_FLOAT: return std::make_unique<FloatNode>();
        case DATA_COMBINEVEC4: return std::make_unique<CombineVec4Node>();
        default: return nullptr;
    }
}
 **/

NodeInstance* instantiateNode(nodeType type) {
    switch(type) {
        case FINAL: return new DrawFinal();
        case SHADER_FLAT: return new DrawFlat();
        case SHADER_FRESNEL: return new DrawFresnel();
        case SHADER_COLORIZE: return new DrawColorize();
        case SHADER_RING: return new DrawRing();
        case SHADER_TEXTURE: return new DrawTexture();
        case DATA_COLOR: return new ColorNode();
        case DATA_FLOAT: return new FloatNode();
        case DATA_COMBINEVEC4: return new CombineVec4Node();
        default: return nullptr;
    }
}
#endif //NPRSPR_NODEINSTANCE_H

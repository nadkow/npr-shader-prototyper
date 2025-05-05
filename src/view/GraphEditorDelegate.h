#ifndef NPRSPR_GRAPHEDITORDELEGATE_H
#define NPRSPR_GRAPHEDITORDELEGATE_H

#include "NodeInstance.h"

#define DEFAULT_HEAD_COLOR IM_COL32(80, 80, 110, 255)
#define SHADER_HEAD_COLOR IM_COL32(80, 110, 80, 255)
#define BG_COLOR IM_COL32(60, 60, 60, 255)
#define OVER_BG_COLOR IM_COL32(70, 70, 70, 255)
#define SHADER_SLOT_COLOR IM_COL32(100, 240, 100, 255)
#define DEFAULT_SLOT_COLOR IM_COL32(180, 180, 190, 255)

template <typename T, std::size_t N>
struct Array
{
    T data[N];
    const size_t size() const { return N; }

    const T operator [] (size_t index) const { return data[index]; }
    operator T* () {
        T* p = new T[N];
        memcpy(p, data, sizeof(data));
        return p;
    }
};

template <typename T, typename ... U> Array(T, U...)->Array<T, 1 + sizeof...(U)>;

const char* finalNodeInputNames[64] = {"Layer 1", "Layer 2", "Layer 3", "Layer 4", "Layer 5", "Layer 6", "Layer 7","Layer 8","Layer 9","Layer 10",
                                       "Layer 11", "Layer 12", "Layer 13", "Layer 14", "Layer 15", "Layer 16", "Layer 17","Layer 18","Layer 19","Layer 20"};

GraphEditor::Template finalNodeTemplate = {
        IM_COL32(110, 80, 80, 255),
        BG_COLOR,
        OVER_BG_COLOR,
        1,
        finalNodeInputNames,
        nullptr,
        0,
        nullptr,
        nullptr,
        60
};

DrawFinal* finalNode = new DrawFinal(&finalNodeTemplate);

struct GraphEditorDelegate : public GraphEditor::Delegate
{
    bool AllowedLink(GraphEditor::NodeIndex from, GraphEditor::SlotIndex slotIndexInput, GraphEditor::NodeIndex to, GraphEditor::SlotIndex slotIndexOutput) override
    {
        return (NodeInstance::canConnect(mNodes[to].instance, slotIndexOutput, mNodes[from].instance, slotIndexInput));
    }

    void SelectNode(GraphEditor::NodeIndex nodeIndex, bool selected) override
    {
        mNodes[nodeIndex].mSelected = selected;
    }

    void MoveSelectedNodes(const ImVec2 delta) override
    {
        for (auto& node : mNodes)
        {
            if (!node.mSelected)
            {
                continue;
            }
            node.x += delta.x;
            node.y += delta.y;
        }
    }

    void RightClick(GraphEditor::NodeIndex nodeIndex, GraphEditor::SlotIndex slotIndexInput, GraphEditor::SlotIndex slotIndexOutput) override
    {
        // RMB click is detected anywhere but yields garbage value if not clicked on node slot
    }

    void displayNewNodeMenu(ImVec2 mousePos) override {
        ImGui::SetNextWindowPos(mousePos);
        ImGui::SetNextWindowSize(ImVec2{90,0});
        if(ImGui::BeginPopup("Add new node")) {
            ImGui::TextColored({.5, .5, .5, 1.f}, "Add new node");
            if (ImGui::BeginMenu("data nodes"))
            {
                for (NodeBlueprint nb: nodeBlueprints) {
                    if (nb.type >= DATA_COLOR) {
                        if (ImGui::Selectable(nb.name, false)) {
                            mNodes.push_back(
                                    {nb.name, static_cast<GraphEditor::TemplateIndex>(nb.type), mousePos.x, mousePos.y,
                                     true, instantiateNode(nb.type)});
                        }
                    }
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("shader nodes"))
            {
                for (NodeBlueprint nb: nodeBlueprints) {
                    if (nb.type < DATA_COLOR) {
                        if (ImGui::Selectable(nb.name, false)) {
                            mNodes.push_back(
                                    {nb.name, static_cast<GraphEditor::TemplateIndex>(nb.type), mousePos.x, mousePos.y,
                                     true, instantiateNode(nb.type)});
                        }
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::EndPopup();
        }
    }

    void AddLink(GraphEditor::NodeIndex inputNodeIndex, GraphEditor::SlotIndex inputSlotIndex, GraphEditor::NodeIndex outputNodeIndex, GraphEditor::SlotIndex outputSlotIndex) override
    {
        mLinks.push_back({ inputNodeIndex, inputSlotIndex, outputNodeIndex, outputSlotIndex });
        NodeInstance::connect(mNodes[inputNodeIndex].instance, inputSlotIndex, mNodes[outputNodeIndex].instance, outputSlotIndex);
    }

    void DelLink(GraphEditor::LinkIndex linkIndex) override
    {
        GraphEditor::Link link = mLinks[linkIndex];
        NodeInstance::disconnect(mNodes[link.mOutputNodeIndex].instance, link.mOutputSlotIndex, mNodes[link.mInputNodeIndex].instance, link.mInputSlotIndex);
        mLinks.erase(mLinks.begin() + linkIndex);
    }

    void CustomDraw(ImDrawList* drawList, float factor, ImRect rectangle, GraphEditor::NodeIndex nodeIndex) override
    {
        mNodes[nodeIndex].instance->drawNode(rectangle, factor);
    }

    const size_t GetTemplateCount() override
    {
        return sizeof(mTemplates) / sizeof(GraphEditor::Template);
    }

    const GraphEditor::Template GetTemplate(GraphEditor::TemplateIndex index) override
    {
        if (index) return mTemplates[index];
        return finalNodeTemplate;
    }

    const size_t GetNodeCount() override
    {
        return mNodes.size();
    }

    const GraphEditor::Node GetNode(GraphEditor::NodeIndex index) override
    {
        const auto& myNode = mNodes[index];
        if (myNode.templateIndex)
            return GraphEditor::Node
                {
                        myNode.name,
                        myNode.templateIndex,
                        ImRect(ImVec2(myNode.x, myNode.y), ImVec2(myNode.x + 120, myNode.y + mTemplates[myNode.templateIndex].mHeight)),
                        myNode.mSelected
                };
        return GraphEditor::Node
                {
                        myNode.name,
                        myNode.templateIndex,
                        ImRect(ImVec2(myNode.x, myNode.y), ImVec2(myNode.x + 120, myNode.y + finalNodeTemplate.mHeight)),
                        myNode.mSelected
                };
    }

    const size_t GetLinkCount() override
    {
        return mLinks.size();
    }

    const GraphEditor::Link GetLink(GraphEditor::LinkIndex index) override
    {
        return mLinks[index];
    }

    // REFER TO nodeType ENUM FOR ORDER
    static inline const GraphEditor::Template mTemplates[] = {
            // FINAL output node template PLACEHOLDER
            {
                    IM_COL32(110, 80, 80, 255),
                    BG_COLOR,
                    OVER_BG_COLOR,
                    2,
                    Array{"Layer 1", "Layer 2"},
                    Array{ SHADER_SLOT_COLOR, SHADER_SLOT_COLOR},
                    0,
                    nullptr,
                    nullptr,
                    120
            },
            // SHADER_FLAT
            {
                    SHADER_HEAD_COLOR,
                    BG_COLOR,
                    OVER_BG_COLOR,
                    1,
                    Array{"Color"},
                    Array{ DEFAULT_SLOT_COLOR},
                    1,
                    Array{"Shader"},
                    Array{ SHADER_SLOT_COLOR},
                    80
            },
            // SHADER_FRESNEL
            {
                    SHADER_HEAD_COLOR,
                    BG_COLOR,
                    OVER_BG_COLOR,
                    1,
                    Array{"Color"},
                    Array{ DEFAULT_SLOT_COLOR},
                    1,
                    Array{"Shader"},
                    Array{ SHADER_SLOT_COLOR},
                    80
            },
            // SHADER_COLORIZE
            {
                    SHADER_HEAD_COLOR,
                    BG_COLOR,
                    OVER_BG_COLOR,
                    1,
                    Array{"Color"},
                    Array{ DEFAULT_SLOT_COLOR},
                    1,
                    Array{"Shader"},
                    Array{ SHADER_SLOT_COLOR},
                    80
            },
            // SHADER_RING
            {
                    SHADER_HEAD_COLOR,
                    BG_COLOR,
                    OVER_BG_COLOR,
                    1,
                    Array{"int"},
                    Array{ DEFAULT_SLOT_COLOR},
                    1,
                    Array{"Shader"},
                    Array{ SHADER_SLOT_COLOR},
                    80
            },
            // DATA_COLOR
            {
                    DEFAULT_HEAD_COLOR,
                    BG_COLOR,
                    OVER_BG_COLOR,
                    1,
                    Array{"Color"},
                    Array{ DEFAULT_SLOT_COLOR},
                    1,
                    Array{"Color"},
                    Array{ DEFAULT_SLOT_COLOR},
                    80
            },
            // DATA_FLOAT
            {
                    DEFAULT_HEAD_COLOR,
                    BG_COLOR,
                    OVER_BG_COLOR,
                    1,
                    Array{"Value"},
                    Array{ DEFAULT_SLOT_COLOR},
                    1,
                    Array{"Value"},
                    Array{ DEFAULT_SLOT_COLOR},
                    80
            },
            // DATA_COMBINEVEC4
            {
                    DEFAULT_HEAD_COLOR,
                    BG_COLOR,
                    OVER_BG_COLOR,
                    4,
                    Array{"x", "y", "z", "w"},
                    Array{ DEFAULT_SLOT_COLOR, DEFAULT_SLOT_COLOR, DEFAULT_SLOT_COLOR, DEFAULT_SLOT_COLOR},
                    1,
                    Array{"Vec4"},
                    Array{ DEFAULT_SLOT_COLOR},
                    160
            }
    };

    struct Node
    {
        const char* name;
        GraphEditor::TemplateIndex templateIndex;
        float x, y;
        bool mSelected;
        NodeInstance* instance;
    };

    struct NodeBlueprint
    {
        const char* name;
        nodeType type;
    };

    std::vector<NodeBlueprint> nodeBlueprints = {
            {
                    "color",
                    DATA_COLOR
            },
            {
                    "float",
                    DATA_FLOAT
            },
            {
                    "combine vec4",
                    DATA_COMBINEVEC4
            },
            {
                    "flat",
                    SHADER_FLAT
            },
            {
                    "fresnel",
                    SHADER_FRESNEL
            },
            {
                "colorize",
                SHADER_COLORIZE
            },
            {
                "metal (ring)",
                SHADER_RING
            }
    };

    std::vector<Node> mNodes = {
            {
                    "final output",
                    block::PREPARE,
                    400, 400,
                    false,
                    finalNode
            }
    };

    std::vector<GraphEditor::Link> mLinks = {};
};

#endif //NPRSPR_GRAPHEDITORDELEGATE_H

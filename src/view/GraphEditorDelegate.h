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

struct GraphEditorDelegate : public GraphEditor::Delegate
{
    bool AllowedLink(GraphEditor::NodeIndex from, GraphEditor::SlotIndex slotIndexInput, GraphEditor::NodeIndex to, GraphEditor::SlotIndex slotIndexOutput) override
    {
        return (NodeInstance::connect(mNodes[to].instance, slotIndexOutput, mNodes[from].instance, slotIndexInput));
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
    { //add new node
    }

    void AddLink(GraphEditor::NodeIndex inputNodeIndex, GraphEditor::SlotIndex inputSlotIndex, GraphEditor::NodeIndex outputNodeIndex, GraphEditor::SlotIndex outputSlotIndex) override
    {
        mLinks.push_back({ inputNodeIndex, inputSlotIndex, outputNodeIndex, outputSlotIndex });
    }

    void DelLink(GraphEditor::LinkIndex linkIndex) override
    {
        GraphEditor::Link link = mLinks[linkIndex];
        NodeInstance::disconnect(mNodes[link.mOutputNodeIndex].instance, link.mOutputSlotIndex);
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
        return mTemplates[index];
    }

    const size_t GetNodeCount() override
    {
        return mNodes.size();
    }

    const GraphEditor::Node GetNode(GraphEditor::NodeIndex index) override
    {
        const auto& myNode = mNodes[index];
        return GraphEditor::Node
                {
                        myNode.name,
                        myNode.templateIndex,
                        ImRect(ImVec2(myNode.x, myNode.y), ImVec2(myNode.x + 120, myNode.y + mTemplates[myNode.templateIndex].mHeight)),
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

    // Graph datas
    static inline const GraphEditor::Template mTemplates[] = {
            // final output node template
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
            // unlit shader
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
            // color
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
            // float
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
            // combine vec4
            {
                    DEFAULT_HEAD_COLOR,
                    BG_COLOR,
                    OVER_BG_COLOR,
                    4,
                    Array{"x", "y", "z", "w"},
                    Array{ DEFAULT_SLOT_COLOR},
                    1,
                    Array{"Vec4"},
                    Array{ DEFAULT_SLOT_COLOR},
                    320
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

    std::vector<Node> mNodes = {
            {
                    "final output",
                    block::PREPARE,
                    400, 400,
                    false,
                    new DrawFinal()
            },
            {
                    block::passes_names[block::FLAT].c_str(),
                    block::FLAT,
                    200, 200,
                    false,
                    new DrawFlat()
            },
            {
                    "color",
                    2,
                    80, 80,
                    false,
                    new ColorNode()
            },
            {
                    "color",
                    2,
                    80, 80,
                    false,
                    new ColorNode()
            },
            {
                    "float",
                    3,
                    80, 80,
                    false,
                    new FloatNode()
            },
            {
                    "combine vec4",
                    4,
                    80, 80,
                    false,
                    new CombineVec4Node()
            }
    };

    std::vector<GraphEditor::Link> mLinks = { {1, 0, 0, 0} };
};

#endif //NPRSPR_GRAPHEDITORDELEGATE_H

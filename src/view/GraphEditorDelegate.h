#ifndef NPRSPR_GRAPHEDITORDELEGATE_H
#define NPRSPR_GRAPHEDITORDELEGATE_H

#define BG_COLOR IM_COL32(60, 60, 60, 255)
#define OVER_BG_COLOR IM_COL32(70, 70, 70, 255)
#define SHADER_SLOT_COLOR IM_COL32(100, 240, 100, 255)

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
    bool AllowedLink(GraphEditor::NodeIndex from, GraphEditor::NodeIndex to) override
    {
        return true;
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
        mLinks.erase(mLinks.begin() + linkIndex);
    }

    void CustomDraw(ImDrawList* drawList, ImRect rectangle, GraphEditor::NodeIndex nodeIndex) override
    {
        drawList->AddLine(rectangle.Min, rectangle.Max, IM_COL32(0, 0, 0, 255));
        drawList->AddText(rectangle.Min, IM_COL32(255, 128, 64, 255), std::to_string(nodeIndex).c_str());
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
                        ImRect(ImVec2(myNode.x, myNode.y), ImVec2(myNode.x + 200, myNode.y + 200)),
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
                    IM_COL32(160, 190, 160, 255),
                    BG_COLOR,
                    OVER_BG_COLOR,
                    2,
                    Array{"Shader 1", "Shader 2"},
                    Array{ SHADER_SLOT_COLOR, SHADER_SLOT_COLOR},
                    0,
                    nullptr,
                    nullptr
            },
            // unlit
            {
                    IM_COL32(160, 160, 180, 255),
                    IM_COL32(100, 100, 140, 255),
                    IM_COL32(110, 110, 150, 255),
                    1,
                    Array{"Color"},
                    nullptr,
                    1,
                    Array{"Shader"},
                    Array{ SHADER_SLOT_COLOR}
            }
    };

    struct Node
    {
        const char* name;
        GraphEditor::TemplateIndex templateIndex;
        float x, y;
        bool mSelected;
    };

    std::vector<Node> mNodes = {
            {
                    block::passes_names[block::FLAT].c_str(),
                    block::FLAT,
                    0, 0,
                    false
            },
            {
                    block::passes_names[block::PREPARE].c_str(),
                    block::PREPARE,
                    400, 400,
                    false
            }
    };

    std::vector<GraphEditor::Link> mLinks = { {0, 0, 1, 0} };
};

#endif //NPRSPR_GRAPHEDITORDELEGATE_H

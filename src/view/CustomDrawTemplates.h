#ifndef NPRSPR_CUSTOMDRAWTEMPLATES_H
#define NPRSPR_CUSTOMDRAWTEMPLATES_H

class DrawTemplate {

public:
    virtual void drawNode(ImRect rect, float factor) = 0;
};

class DrawFlat : public DrawTemplate {

public:
    ImVec4 color = {.0, .0, .0, 1.};

    DrawFlat() = default;

    void drawNode(ImRect rect, float factor) override {
        ImGui::SetCursorPosX(rect.Min.x + 10);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY()+10 * factor);
        ImGui::ColorEdit4("Color", (float*)&color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
    }
};

class DrawFinal : public DrawTemplate {

public:
    std::vector<DrawTemplate*> connectedNodes;

    DrawFinal() = default;

    void drawNode(ImRect rect, float factor) override {}
};
#endif //NPRSPR_CUSTOMDRAWTEMPLATES_H

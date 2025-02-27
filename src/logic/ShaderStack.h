#ifndef NPRSPR_SHADERSTACK_H
#define NPRSPR_SHADERSTACK_H

#include "../rendering/stack_state.h"
#include "../rendering/shader_block.h"

using namespace block;

class ShaderStack {

public:
    bool swf = false;
    bool swfr = false;
    bool swp = false;
    bool swpt = false;
    bool swt = false;
    bool swc = false;

    ShaderStack() = default;

    void draw() {
        for (const auto &render_pass: blocks) block::passes[render_pass]();
    }

    void showcaseFlat() {
        swf = !swf;
        if (swf)
        blocks.push_back(FLAT);
        else {
            auto it = std::remove(blocks.begin(), blocks.end(), FLAT);
            blocks.erase(it, blocks.end());
        }
    }

    void showcaseFresnel() {
        swfr = !swfr;
        if (swfr)
            blocks.push_back(FRESNEL);
        else {
            auto it = std::remove(blocks.begin(), blocks.end(), FRESNEL);
            blocks.erase(it, blocks.end());
        }
    }

    void showcasePoint() {
        swp = !swp;
        if (swp)
            blocks.push_back(POINTLIGHT);
        else {
            auto it = std::remove(blocks.begin(), blocks.end(), POINTLIGHT);
            blocks.erase(it, blocks.end());
        }
    }

    void showcasePointTex() {
        swpt = !swpt;
        if (swpt)
            blocks.push_back(POINTLIGHTTEX);
        else {
            auto it = std::remove(blocks.begin(), blocks.end(), POINTLIGHTTEX);
            blocks.erase(it, blocks.end());
        }
    }

    void showcaseTex() {
        swt = !swt;
        if (swt)
            blocks.push_back(TEXTURE);
        else {
            auto it = std::remove(blocks.begin(), blocks.end(), TEXTURE);
            blocks.erase(it, blocks.end());
        }
    }

    void showcaseColorize() {
        swc = !swc;
        if (swc)
            blocks.push_back(COLORIZE);
        else {
            auto it = std::remove(blocks.begin(), blocks.end(), COLORIZE);
            blocks.erase(it, blocks.end());
        }
    }

private:

    std::vector<pass_name> blocks = {PREPARE, TEXTURE};

};

#endif //NPRSPR_SHADERSTACK_H

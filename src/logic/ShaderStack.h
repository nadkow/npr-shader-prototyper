#ifndef NPRSPR_SHADERSTACK_H
#define NPRSPR_SHADERSTACK_H

#include "../rendering/stack_state.h"
#include "../rendering/shader_block.h"

using namespace block;

class ShaderStack {

public:
    bool swf = false;
    bool swfr = false;

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

private:

    std::vector<pass_name> blocks = {PREPARE};

};

#endif //NPRSPR_SHADERSTACK_H

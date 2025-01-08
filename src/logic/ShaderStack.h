#ifndef NPRSPR_SHADERSTACK_H
#define NPRSPR_SHADERSTACK_H

#include "../rendering/stack_state.h"
#include "../rendering/shader_block.h"

using namespace block;

class ShaderStack {

public:

    ShaderStack() = default;

    void draw() {
        for (const auto &render_pass: blocks) block::passes[render_pass]();
    }

private:

    std::vector<pass_name> blocks = {PREPARE};

};

#endif //NPRSPR_SHADERSTACK_H

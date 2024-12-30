#ifndef NPRSPR_SHADERSTACK_H
#define NPRSPR_SHADERSTACK_H

#include "../rendering/StackState.h"
#include "../rendering/ShaderBlock.h"

using namespace block;

class ShaderStack {

public:

    ShaderStack() = default;

    void draw() {
        for (const auto &render_pass: blocks) render_pass();
    }

private:

    std::vector<std::function<void()>> blocks = {prepare};

};

#endif //NPRSPR_SHADERSTACK_H

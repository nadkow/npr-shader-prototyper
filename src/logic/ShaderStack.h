#ifndef NPRSPR_SHADERSTACK_H
#define NPRSPR_SHADERSTACK_H

#include "../rendering/StackState.h"
#include "../rendering/ShaderBlock.h"

using namespace block;

class ShaderStack {
public:
    ShaderStack() = default;

    ShaderStack( Model* m, Node* n) {
        model = m;
        node = n;
    }

    void draw() {
        // update context before drawing
        stack::model = model;
        stack::node = node;
        for (const auto& render_pass : blocks) render_pass();
    }

private:
    std::vector<std::function<void()>> blocks = {flat};
    Model* model;
    Node* node;
};

#endif //NPRSPR_SHADERSTACK_H

#ifndef NPRSPR_SHADERBLOCK_H
#define NPRSPR_SHADERBLOCK_H

#include "ShaderProgram.h"

namespace block {

    ShaderProgram prepareProgram, flatProgram, finalizeProgram;

    void init() {
        //prepareProgram = ShaderProgram("res/shaders/prepare.vert", "res/shaders/prepare.frag");
        flatProgram = ShaderProgram("res/shaders/model.vert", "res/shaders/model.frag");
        //finalizeProgram = ShaderProgram("res/shaders/finalize.vert", "res/shaders/finalize.frag");
    }

    void prepare() {
        // render data to buffer
        prepareProgram.use();
        prepareProgram.setMat4("projection", projection);
        prepareProgram.setMat4("view", view);
        prepareProgram.setMat4("transform", stack::node->getTransform());
        stack::model->Draw(prepareProgram);
    }

    void flat() {
        // color mesh with flat color
        flatProgram.use();
        flatProgram.setMat4("projection", projection);
        flatProgram.setMat4("view", view);
        flatProgram.setMat4("transform", stack::node->getTransform());
        stack::model->Draw(flatProgram);
    }

    void finalize() {
        // render to screen
        finalizeProgram.use();
        finalizeProgram.setMat4("projection", projection);
        finalizeProgram.setMat4("view", view);
        finalizeProgram.setMat4("transform", stack::node->getTransform());
        stack::model->Draw(finalizeProgram);
    }
}

#endif //NPRSPR_SHADERBLOCK_H

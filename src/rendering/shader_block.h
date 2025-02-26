#ifndef NPRSPR_SHADER_BLOCK_H
#define NPRSPR_SHADER_BLOCK_H

#include "ShaderProgram.h"

namespace block {

    // optional passes
    void prepare();
    void flat();
    void fresnel();

    std::vector<std::function<void()>> passes = {prepare, flat, fresnel};
    std::vector<std::string> passes_names = {"prepare", "flat color", "fresnel"};
    enum pass_name {PREPARE, FLAT, FRESNEL};

    ShaderProgram prepareProgram, flatProgram, fresnelProgram, finalizeProgram;
    unsigned int quadVAO, quadVBO;

    // quad for the final render
    float quadVertices[] = {
            // positions                // texture coords
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,};

    void init() {
        prepareProgram = ShaderProgram("res/shaders/prepare.vert", "res/shaders/prepare.frag");
        flatProgram = ShaderProgram("res/shaders/flat.vert", "res/shaders/flat.frag");
        finalizeProgram = ShaderProgram("res/shaders/finalize.vert", "res/shaders/finalize.frag");
        fresnelProgram = ShaderProgram("res/shaders/fresnel.vert", "res/shaders/fresnel.frag");
        // TODO UBO
        //flatProgram.use();
        //flatProgram.setInt("gPosition", 0);
        //flatProgram.setInt("gNormal", 1);
        //flatProgram.setInt("gRender", 2);
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    }

    void prepare() {
        // render data to buffer
        prepareProgram.use();
        // TODO UBO
        prepareProgram.setMat4("projection", projection);
        prepareProgram.setMat4("view", view);
        prepareProgram.setMat4("transform", stack::node->getTransform());
        stack::model->Draw(prepareProgram);
    }

    void flat() {
        // color mesh with flat color
        flatProgram.use();
        // TODO UBO
        flatProgram.setMat4("projection", projection);
        flatProgram.setMat4("view", view);
        flatProgram.setMat4("transform", stack::node->getTransform());
        stack::model->Draw(flatProgram);
    }

    void fresnel() {
        std::swap(stack::iRender, stack::gRender);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, stack::gRender, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, stack::iRender);
        fresnelProgram.use();
        // TODO UBO
        fresnelProgram.setMat4("projection", projection);
        fresnelProgram.setMat4("view", view);
        fresnelProgram.setMat4("transform", stack::node->getTransform());
        fresnelProgram.setVec3("viewPos", cameraPos);
        fresnelProgram.setVec2("resolution", glm::vec2(display_w, display_h+19));
        stack::model->Draw(fresnelProgram);
    }

    void finalize() {
        // unbind gbuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, stack::gRender);
        // render to screen
        finalizeProgram.use();
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }
}

#endif //NPRSPR_SHADER_BLOCK_H

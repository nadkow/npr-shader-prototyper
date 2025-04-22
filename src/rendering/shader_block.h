#ifndef NPRSPR_SHADER_BLOCK_H
#define NPRSPR_SHADER_BLOCK_H

#include "ShaderProgram.h"



namespace block {

    // optional passes
    void prepare();
    void flat();
    void fresnel();
    void pointLight();
    void pointLightTex();
    void pointLightRev();
    void dirLight();
    void texture();
    void colorize();
    void specular();
    void ring();
    void pass(){};

    std::vector<std::function<void()>> passes = {prepare, flat, fresnel, pointLight, pointLightRev, pointLightTex, texture, colorize, specular, ring, pass};
    // passes names for display and file names
    const char* passes_names[] = {"prepare", "flat color", "fresnel", "point light", "point light reversed", "textured point light", "texture", "colorize", "specular", "ring"};
    enum pass_name {PREPARE, FLAT, FRESNEL, POINTLIGHT, POINTLIGHTREV, POINTLIGHTTEX, TEXTURE, COLORIZE, SPECULAR, RING, PASS};

    ShaderProgram prepareProgram, flatProgram, fresnelProgram, pointProgram, pointProgramRev, pointProgramTex, textureProgram, colorizeProgram, specularProgram, ringProgram, finalizeProgram;
    ShaderProgram* programs[] = {&prepareProgram, &flatProgram, &fresnelProgram, &pointProgram, &pointProgramRev, &pointProgramTex, &textureProgram, &colorizeProgram, &specularProgram, &ringProgram, &finalizeProgram};
    const char* vs[] = {"res/shaders/prepare.vert", "res/shaders/flat.vert", "res/shaders/fresnel.vert", "res/shaders/point.vert", "res/shaders/point.vert", "res/shaders/pointTextured.vert", "res/shaders/texture.vert", "res/shaders/colorize.vert", "res/shaders/specular.vert", "res/shaders/ring.vert"};
    const char* fs[] = {"res/shaders/prepare.frag", "res/shaders/flat.frag", "res/shaders/fresnel.frag", "res/shaders/point.frag", "res/shaders/pointRev.frag", "res/shaders/pointTextured.frag", "res/shaders/texture.frag", "res/shaders/colorize.frag", "res/shaders/specular.frag", "res/shaders/ring.frag"};
    unsigned int quadVAO, quadVBO;

    // quad for the final render
    float quadVertices[] = {
            // positions                // texture coords
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,};

    void init() {
        finalizeProgram = ShaderProgram("res/shaders/finalize.vert", "res/shaders/finalize.frag");
        ringProgram = ShaderProgram("res/shaders/ring.vert", "res/shaders/ring.frag");
        for (int i=0; i < PASS; i++) {
            *programs[i] = ShaderProgram(vs[i], fs[i]);
        }
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

    void recompile(pass_name shader) {
        programs[shader]->recompile(vs[shader], fs[shader]);
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

    void pointLight() {
        std::swap(stack::iRender, stack::gRender);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, stack::gRender, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, stack::iRender);
        pointProgram.use();
        pointProgram.setVec3("pointPos", stack::activeLight->node.getGlobalTranslation());
        pointProgram.setVec4("pointColor", stack::activeLight->color);
        // TODO UBO
        pointProgram.setMat4("projection", projection);
        pointProgram.setMat4("view", view);
        pointProgram.setMat4("transform", stack::node->getTransform());
        pointProgram.setVec2("resolution", glm::vec2(display_w, display_h+19));
        stack::model->Draw(pointProgram);
    }

    void pointLightRev() {
        std::swap(stack::iRender, stack::gRender);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, stack::gRender, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, stack::iRender);
        pointProgramRev.use();
        pointProgramRev.setVec3("pointPos", stack::activeLight->node.getGlobalTranslation());
        pointProgramRev.setVec4("pointColor", stack::activeLight->color);
        // TODO UBO
        pointProgramRev.setMat4("projection", projection);
        pointProgramRev.setMat4("view", view);
        pointProgramRev.setMat4("transform", stack::node->getTransform());
        pointProgramRev.setVec2("resolution", glm::vec2(display_w, display_h+19));
        stack::model->Draw(pointProgramRev);
    }

    void pointLightTex() {
        std::swap(stack::iRender, stack::gRender);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, stack::gRender, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, stack::iRender);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, stack::model->getDefaultTexture());
        pointProgramTex.use();
        pointProgramTex.setInt("iRender", 0);
        pointProgramTex.setInt("tex", 1);
        pointProgramTex.setVec3("pointPos", stack::activeLight->node.getGlobalTranslation());
        pointProgramTex.setVec4("pointColor", stack::activeLight->color);
        // TODO UBO
        pointProgramTex.setMat4("projection", projection);
        pointProgramTex.setMat4("view", view);
        pointProgramTex.setMat4("transform", stack::node->getTransform());
        pointProgramTex.setVec2("resolution", glm::vec2(display_w, display_h+19));
        stack::model->Draw(pointProgramTex);
    }

    void specular() {
        std::swap(stack::iRender, stack::gRender);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, stack::gRender, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, stack::iRender);
        specularProgram.use();
        specularProgram.setVec3("pointPos", stack::activeLight->node.getGlobalTranslation());
        specularProgram.setVec4("pointColor", stack::activeLight->color);
        specularProgram.setVec3("viewPos", cameraPos);
        // TODO UBO
        specularProgram.setMat4("projection", projection);
        specularProgram.setMat4("view", view);
        specularProgram.setMat4("transform", stack::node->getTransform());
        specularProgram.setVec2("resolution", glm::vec2(display_w, display_h+19));
        stack::model->Draw(specularProgram);
    }

    void ring() {
        ringProgram.use();
        // TODO UBO
        ringProgram.setMat4("projection", projection);
        ringProgram.setMat4("view", view);
        ringProgram.setMat4("transform", stack::node->getTransform());
        ringProgram.setVec3("viewPos", cameraPos);
        stack::model->Draw(ringProgram);
    }

    void texture() {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, stack::model->getDefaultTexture());
        textureProgram.use();
        // TODO UBO
        textureProgram.setMat4("projection", projection);
        textureProgram.setMat4("view", view);
        textureProgram.setMat4("transform", stack::node->getTransform());
        stack::model->Draw(textureProgram);
    }

    void colorize() {
        std::swap(stack::iRender, stack::gRender);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, stack::gRender, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, stack::iRender);
        colorizeProgram.use();
        // TODO UBO
        colorizeProgram.setMat4("projection", projection);
        colorizeProgram.setMat4("view", view);
        colorizeProgram.setMat4("transform", stack::node->getTransform());
        colorizeProgram.setVec2("resolution", glm::vec2(display_w, display_h+19));
        colorizeProgram.setVec3("color", stack::color);
        stack::model->Draw(colorizeProgram);
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

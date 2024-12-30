#ifndef NPRSPR_OBJECTRENDERER_H
#define NPRSPR_OBJECTRENDERER_H

namespace renderer {

    void init() {
        // buffers
        // configure g-buffer framebuffer
        glGenFramebuffers(1, &stack::gBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, stack::gBuffer);
        // position color buffer
        glGenTextures(1, &stack::gPosition);
        glBindTexture(GL_TEXTURE_2D, stack::gPosition);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, stack::gPosition, 0);
        // normal color buffer
        glGenTextures(1, &stack::gNormal);
        glBindTexture(GL_TEXTURE_2D, stack::gNormal);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, stack::gNormal, 0);
        // final color
        glGenTextures(1, &stack::gRender);
        glBindTexture(GL_TEXTURE_2D, stack::gRender);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, stack::gRender, 0);
        // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
        unsigned int attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
        glDrawBuffers(3, attachments);
        // create and attach depth buffer (renderbuffer)
        unsigned int rboDepth;
        glGenRenderbuffers(1, &rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WINDOW_WIDTH, WINDOW_HEIGHT);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
        // finally check if framebuffer is complete
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            spdlog::error("Framebuffer not complete");
        // unbind
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        block::init();
    }

    void draw() {
        // draw all objects
        // bind custom buffer to draw to
        glBindFramebuffer(GL_FRAMEBUFFER, stack::gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        object_manager.draw();
        // render final image
        block::finalize();
    }
}

#endif //NPRSPR_OBJECTRENDERER_H

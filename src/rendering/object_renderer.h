#ifndef NPRSPR_OBJECT_RENDERER_H
#define NPRSPR_OBJECT_RENDERER_H

namespace renderer {

    void init() {}

    void draw() {
        // draw all objects
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        object_manager.draw();
    }
}

#endif //NPRSPR_OBJECT_RENDERER_H

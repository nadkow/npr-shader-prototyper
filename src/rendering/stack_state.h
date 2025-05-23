#ifndef NPRSPR_STACK_STATE_H
#define NPRSPR_STACK_STATE_H

namespace stack {

    Model* model;
    Node* node;
    unsigned int gBuffer;
    unsigned int gPosition, gNormal, gRender, iRender;
    LightObject* activeLight;
    glm::vec3 color;

}

#endif //NPRSPR_STACK_STATE_H

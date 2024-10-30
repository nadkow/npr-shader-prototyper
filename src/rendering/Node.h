#ifndef OPENGLGP_NODE_H
#define OPENGLGP_NODE_H

#include "glad/glad.h"
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class Node {
public:
    unsigned int vbo;

    Node() {
        transf = glm::mat4(1.0f);
        rotat = glm::mat4(1.0f);
    }

    Node(int idx, glm::mat4 mat, unsigned int v) {
        id = idx;
        transf = mat;
        vbo = v;
        rotat = glm::mat4(1.0f);
    }

    Node(glm::mat4 mat) {
        transf = mat;
        rotat = glm::mat4(1.0f);
    }

    void addChild(Node* n) {
        n->parent = this;
        children.push_back(n);
    }

    void removeChild() {
        for (auto & child : children) {
            child->parent = nullptr;
            child->transform(transf);
        }
        children.clear();
    }

    void update() {

    }

    void updateChild(int idx) {
        children[idx]->update();
    }

    void updateChildren() {
        for (auto & child : children) {
            child->update();
        }
    }

    glm::mat4 getTransform() {
        if (parent == nullptr) return transf * rotat;
        return parent->getTransform() * transf * rotat;
    }

    glm::mat4 getTranslation() {
        return transf;
    }

    glm::vec3 getGlobalTranslation() {
        if (parent == nullptr) return transf * glm::vec4(0.f, 0.f, 0.f, 1.0f);
        return parent->getTranslation() * transf * glm::vec4(0.f, 0.f, 0.f, 1.0f);
    }

    glm::vec3 getGlobalTransform() {
        if (parent == nullptr) return transf * glm::vec4(0.f, 0.f, 0.f, 1.0f);
        return parent->getTransform() * transf * glm::vec4(0.f, 0.f, 0.f, 1.0f);
    }

    glm::mat4 getRotation() {
        return rotat;
    }

    void transform(glm::mat4 trans_mat) {
        transf *= trans_mat;
    }

    void translation(glm::mat4 trans_mat) {
        transf = trans_mat;
    }

    void translate(double x, double y, double z) {
        transf = glm::translate(transf, {x, y, z});
    }

    void transformChild(int idx, glm::mat4 trans_mat) {
        children[idx]->transform(trans_mat);
    }

    void rotate(glm::mat4 trans_mat) {
        rotat *= trans_mat;
    }

    void rotation(glm::mat4 trans_mat) {
        rotat = trans_mat;
    }

    void rotateChild(int idx, glm::mat4 trans_mat) {
        children[idx]->rotate(trans_mat);
    }

    Node* getChild(int idx) {
        return children[idx];
    }

private:
    int id;
    glm::mat4 transf;
    glm::mat4 rotat;
    Node* parent = nullptr;
    std::vector<Node*> children;

};

#endif //OPENGLGP_NODE_H

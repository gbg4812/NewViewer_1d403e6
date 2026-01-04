#include "DrawBounding.h"
#include "glwidget.h"
#include "object.h"
#include "qopenglext.h"
#include <vector>

void DrawBounding::onPluginLoad() {
    GLWidget *g = glwidget();
    g->makeCurrent();
    std::vector<float> verts = std::vector<float>(8 * 3);
    for (int x = 0; x <= 1; x++) {
        for (int y = 0; y <= 1; y++) {
            for (int z = 0; z <= 1; z++) {
                verts.push_back(x);
                verts.push_back(y);
                verts.push_back(z);
            }
        }
    }

    std::vector<uint> idexos = {0, 1, 3, 0, 3, 2, 4, 6, 7, 4, 7, 5,
                                5, 7, 3, 5, 3, 1, 0, 2, 6, 0, 6, 4,
                                2, 3, 7, 2, 7, 6, 0, 4, 5, 0, 5, 1};
    g->glGenVertexArrays(1, cubVAO);
    g->glBindVertexArray(cubVAO);
    g->glGenBuffers(1, cubVBO);
    g->glBindBuffer(GL_ARRAY_BUFFER, cubVBO);
    g->glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(),
                    GL_STATIC_DRAW);
    g->glEnableVertexAttribArray(cubVBO);
    g->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    g->glBindVertexArray(0);
}

void DrawBounding::preFrame() {}

void DrawBounding::postFrame() {}

void DrawBounding::onObjectAdd() {}

bool DrawBounding::drawScene() {
    return false; // return true only if implemented
}

bool DrawBounding::drawObject(int) {
    return false; // return true only if implemented
}

bool DrawBounding::paintGL() {
    return false; // return true only if implemented
}

void DrawBounding::keyPressEvent(QKeyEvent *) {}

void DrawBounding::mouseMoveEvent(QMouseEvent *) {}

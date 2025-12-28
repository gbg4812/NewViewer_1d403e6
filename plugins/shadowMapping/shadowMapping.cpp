#include "shadowMapping.h"
#include "glwidget.h"
#include "qopenglext.h"
#include "qopenglshaderprogram.h"
#include <GL/gl.h>

void ShadowMapping::onPluginLoad() {
    GLWidget *g = glwidget();
    int w = g->width();
    int h = g->height();

    // Create depth texture to render to
    g->glGenRenderbuffers(1, &depthRenderBuff);
    g->glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuff);
    g->glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
    g->glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Create Framebuffer
    g->glGenFramebuffers(1, &shadowMapFrameBuffer);

    // Bind the depth texture to the framebuffer
    g->glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFrameBuffer);
    g->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                 GL_RENDERBUFFER, depthRenderBuff);

    if (g->glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
        GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER::NOT COMPLETE" << std::endl;
    }
    g->glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // setupDebugQuad();
}

bool ShadowMapping::paintGL() {
    GLWidget *g = glwidget();
    g->glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFrameBuffer);
    g->glClearColor(0.8f, 0.8f, 0.8f, 0.0f);
    g->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (drawPlugin())
        drawPlugin()->drawScene();
    g->glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // drawDebugQuad();

    return true; // return true only if implemented
}

void ShadowMapping::setupDebugQuad() {

    GLWidget *g = glwidget();
    QOpenGLShaderProgram program = QOpenGLShaderProgram();
    program.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                    "./shaders/fullscreen.vert");
    program.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                    "./shaders/fullscreen.frag");

    program.link();
    std::cout << program.log().toStdString() << std::endl;
    debugProgram = program.programId();

    GLuint vposLoc = program.attributeLocation("vpos");

    g->glGenVertexArrays(1, &debugQuadVAO);
    g->glBindVertexArray(debugQuadVAO);
    g->glGenBuffers(1, &debugQuadVBO);
    g->glBindBuffer(GL_ARRAY_BUFFER, debugQuadVBO);
    g->glBufferData(GL_ARRAY_BUFFER, debugQuad.size() * sizeof(QVector3D),
                    debugQuad.data(), GL_STATIC_DRAW);
    g->glEnableVertexAttribArray(vposLoc);
    g->glVertexAttribPointer(vposLoc, 3, GL_FLOAT, false, 0, 0);
    g->glBindVertexArray(0);
}
void ShadowMapping::drawDebugQuad() {
    GLWidget *g = glwidget();
    g->glUseProgram(debugProgram);

    g->glBindVertexArray(debugQuadVAO);
    g->glDrawArrays(GL_TRIANGLES, 0, debugQuad.size());

    g->glBindVertexArray(0);
}

void ShadowMapping::keyPressEvent(QKeyEvent *) {}

void ShadowMapping::mouseMoveEvent(QMouseEvent *) {}

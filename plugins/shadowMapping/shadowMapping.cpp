#include "shadowMapping.h"
#include "QBoxLayout"
#include "glwidget.h"
#include "materialDialog.h"
#include "qapplication.h"
#include "qevent.h"
#include "qnamespace.h"
#include "qopenglext.h"
#include "qopenglshaderprogram.h"
#include <GL/gl.h>

#define TEX_SIZE 2048

#define CHECK(g) printOglerror(g, __LINE__, __FUNCTION__, __FILE__)
#define CLEAR_FLAGS(g) g->glGetError()

void printOglerror(GLWidget *g, int line, const char function[],
                   const char file[]) {
    GLuint error = g->glGetError();
    switch (error) {
    case GL_INVALID_ENUM: {
        std::cout << line << "::" << function << "::" << file
                  << "::GL_INVALID_ENUM" << std::endl;
    } break;
    case GL_INVALID_OPERATION: {
        std::cout << line << "::" << function << "::" << file
                  << "::GL_INVALID_OPERATION" << std::endl;
    } break;
    case GL_INVALID_VALUE: {
        std::cout << line << "::" << function << "::" << file
                  << "::GL_INVALID_VALUE" << std::endl;
    } break;
    case GL_INVALID_FRAMEBUFFER_OPERATION: {
        std::cout << line << "::" << function << "::" << file
                  << "::GL_INVALID_FRAMEBUFFER_OPERATION" << std::endl;
    } break;
    case GL_OUT_OF_MEMORY: {
        std::cout << line << "::" << function << "::" << file
                  << "::GL_OUT_OF_MEMORY" << std::endl;
    } break;
    }
}

void ShadowMapping::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_M) {
        MaterialDialog mtDig = MaterialDialog(glwidget(), material);
        material = mtDig.getMaterial();
    }
    if (event->key() == Qt::Key_E) {
        QString file = QFileDialog::getOpenFileName(
            glwidget(), "Set Env Map", QDir::homePath(),
            "Image Files (*.png *.jpg *.bmp)");
        QImage env_map = QImage(file);
        setEnvContent(env_map);
    }

    Plugin::keyReleaseEvent(event);
};

void ShadowMapping::setEnvContent(QImage texture) {
    envMapEnabled = true;
    texture = texture.convertToFormat(QImage::Format_RGBA32FPx4);
    GLWidget *g = glwidget();
    g->glBindTexture(GL_TEXTURE_2D, envMapTex);
    g->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, texture.width(),
                    texture.height(), 0, GL_RGBA, GL_FLOAT,
                    texture.constBits());
    g->glBindTexture(GL_TEXTURE_2D, 0);
}

void ShadowMapping::wheelEvent(QWheelEvent *event) {
    auto modifiers = QApplication::queryKeyboardModifiers();
    if (modifiers == Qt::KeyboardModifier::ControlModifier) {
        lightCamera.incrementAngleX(event->angleDelta().y() / 8.0);
    } else {
        lightCamera.incrementAngleY(event->angleDelta().y() / 8.0);
    }
}

void ShadowMapping::onPluginLoad() {

    GLWidget *g = glwidget();
    g->makeCurrent();
    g->glEnable(GL_MULTISAMPLE);

    w = g->width();
    h = g->height();

    float radius = scene()->boundingBox().radius();

    // camera definition
    lightCamera.init(scene()->boundingBox());
    lightCamera.setAngleX(45);
    lightCamera.setAngleY(30);
    lightCamera.setAngleZ(0);
    lightCamera.setFov(100);
    lightCamera.setAspectRatio(1.0);
    lightCamera.setDistance(2.0 * radius);

    lightCamera.updateClippingPlanes(scene()->boundingBox());

    // Shader to write the depth texture
    shadowMapProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                             "./shaders/shadow_map.vert");
    shadowMapProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                             "./shaders/shadow_map.frag");

    shadowMapProgram.link();
    std::cout << shadowMapProgram.log().toStdString() << std::endl;

    // Create Framebuffer
    g->glGenFramebuffers(1, &shadowMapFrameBuffer);

    // Bind the depth texture to the framebuffer
    g->glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFrameBuffer);

    // Create depth texture to render to
    g->glGenTextures(1, &depthRenderBuff);

    g->glBindTexture(GL_TEXTURE_2D, depthRenderBuff);
    g->glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, TEX_SIZE, TEX_SIZE, 0,
                    GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    g->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    g->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    g->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    g->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    g->glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            depthRenderBuff, 0);
    g->glDrawBuffer(GL_NONE);
    g->glReadBuffer(GL_NONE);

    if (g->glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
        GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER::NOT COMPLETE" << std::endl;
    }

    g->glBindTexture(GL_TEXTURE_2D, 0);
    g->glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Shader to render
    shadowRenderingProgram.addShaderFromSourceFile(
        QOpenGLShader::Vertex, "./shaders/shadow_render.vert");
    shadowRenderingProgram.addShaderFromSourceFile(
        QOpenGLShader::Fragment, "./shaders/shadow_render.frag");
    std::cout << shadowRenderingProgram.log().toStdString() << std::endl;

    setupFullScreenQuad();

    // Env texture

    g->glGenTextures(1, &envMapTex);
    g->glBindTexture(GL_TEXTURE_2D, envMapTex);
    g->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    g->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    g->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    g->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    g->glBindTexture(GL_TEXTURE_2D, 0);
}

void ShadowMapping::onObjectAdd() {
    float radius = scene()->boundingBox().radius();

    // camera definition
    lightCamera.init(scene()->boundingBox());
    lightCamera.setAngleX(45);
    lightCamera.setAngleY(30);
    lightCamera.setAngleZ(0);
    lightCamera.setFov(100);
    lightCamera.setAspectRatio(1.0);
    lightCamera.setDistance(2.0 * radius);

    lightCamera.updateClippingPlanes(scene()->boundingBox());
}

bool ShadowMapping::paintGL() {

    GLWidget *g = glwidget();
    g->makeCurrent();

    // Render shadow map
    g->glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFrameBuffer);
    g->glViewport(0, 0, TEX_SIZE, TEX_SIZE);
    g->glClear(GL_DEPTH_BUFFER_BIT);
    shadowMapProgram.bind();

    QMatrix4x4 lightMatrix =
        lightCamera.projectionMatrix() * lightCamera.viewMatrix();
    shadowMapProgram.setUniformValue("MVP", lightMatrix);

    glCullFace(GL_FRONT);

    if (drawPlugin())
        drawPlugin()->drawScene();
    glCullFace(GL_BACK);

    shadowMapProgram.release();
    g->glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Render scene with shadows
    g->makeCurrent();
    g->glViewport(0, 0, w, h);
    g->glClearColor(1.f, 1.f, 1.f, 1.f);
    g->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (envMapEnabled) {
        g->glActiveTexture(GL_TEXTURE0);
        g->glBindTexture(GL_TEXTURE_2D, envMapTex);

        drawFullScreenQuad();
    }

    if (shadowMapProgram.isLinked())
        shadowRenderingProgram.bind();

    g->glActiveTexture(GL_TEXTURE0);
    g->glBindTexture(GL_TEXTURE_2D, depthRenderBuff);

    shadowRenderingProgram.setUniformValue("projectionMatrix",
                                           camera()->projectionMatrix());
    shadowRenderingProgram.setUniformValue("modelViewMatrix",
                                           camera()->viewMatrix());
    shadowRenderingProgram.setUniformValue("lightMatrix", lightMatrix);

    shadowRenderingProgram.setUniformValue(
        "lightPos", camera()
                        ->viewMatrix()
                        .map(QVector4D(lightCamera.getObs(), 1.0))
                        .toVector3D());
    shadowRenderingProgram.setUniformValue(
        "normalMatrix", camera()->viewMatrix().normalMatrix());

    shadowRenderingProgram.setUniformValue("matDiffuse", material.matDiffuse);
    shadowRenderingProgram.setUniformValue("matSpecular", material.matSpecular);
    shadowRenderingProgram.setUniformValue("matShininess",
                                           material.matShininess);
    shadowRenderingProgram.setUniformValue("matRoughness",
                                           material.matRoughness);

    if (drawPlugin())
        drawPlugin()->drawScene();

    shadowRenderingProgram.release();

    return true; // return true only if implemented
}

void ShadowMapping::setupFullScreenQuad() {

    GLWidget *g = glwidget();
    envProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                       "./shaders/fullscreen.vert");
    envProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                       "./shaders/fullscreen.frag");

    envProgram.link();
    std::cout << envProgram.log().toStdString() << std::endl;

    GLuint vposLoc = envProgram.attributeLocation("vpos");
    GLuint texCoordLoc = envProgram.attributeLocation("texCoord");

    g->glGenVertexArrays(1, &debugQuadVAO);
    g->glBindVertexArray(debugQuadVAO);

    // Positions
    g->glGenBuffers(1, &debugQuadVBO);
    g->glBindBuffer(GL_ARRAY_BUFFER, debugQuadVBO);
    g->glBufferData(GL_ARRAY_BUFFER, debugQuad.size() * sizeof(QVector3D),
                    debugQuad.data(), GL_STATIC_DRAW);
    g->glEnableVertexAttribArray(vposLoc);
    g->glVertexAttribPointer(vposLoc, 3, GL_FLOAT, false, 0, 0);

    // TexCoords
    g->glGenBuffers(1, &texCoordVBO);
    g->glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
    g->glBufferData(GL_ARRAY_BUFFER, texCoordQuad.size() * sizeof(QVector2D),
                    texCoordQuad.data(), GL_STATIC_DRAW);
    g->glEnableVertexAttribArray(texCoordLoc);
    g->glVertexAttribPointer(texCoordLoc, 2, GL_FLOAT, false, 0, 0);

    g->glBindVertexArray(0);
}
void ShadowMapping::drawFullScreenQuad() {
    GLWidget *g = glwidget();
    g->makeCurrent();
    g->glDisable(GL_DEPTH_TEST);
    envProgram.bind();

    g->glBindVertexArray(debugQuadVAO);
    g->glDrawArrays(GL_TRIANGLES, 0, debugQuad.size());
    g->glBindVertexArray(0);

    g->glEnable(GL_DEPTH_TEST);

    envProgram.release();
}

#ifndef _SHADOWMAPPING_H
#define _SHADOWMAPPING_H

#include "camera.h"
#include "materialDialog.h"
#include "plugin.h"
#include "qopenglshaderprogram.h"
#include "qvector3d.h"

class ShadowMapping : public QObject, public Plugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "Plugin")
    Q_INTERFACES(Plugin)

  public:
    void onPluginLoad() override;

    bool paintGL() override;

    void onObjectAdd() override;

    void keyReleaseEvent(QKeyEvent *) override;

    void wheelEvent(QWheelEvent *) override;

  private:
    // add private methods and attributes here
    GLuint depthRenderBuff;
    GLuint shadowMapFrameBuffer;

    GLuint debugQuadVAO;
    GLuint debugQuadVBO;
    GLuint texCoordVBO;

    // env map
    GLuint envMapTex;
    bool envMapEnabled = false;

    int w, h;

    gbgMaterial material;

    QColor lightAmbient;
    QColor lightColor;
    float lightIntensity;

    QOpenGLShaderProgram envProgram;
    QOpenGLShaderProgram shadowMapProgram;
    QOpenGLShaderProgram shadowRenderingProgram;

    vector<float> debugQuad = {
        -1, -1, 0, 1, 1, 0, -1, 1, 0, -1, -1, 0, 1, -1, 0, 1, 1, 0,
    };
    vector<float> texCoordQuad = {
        0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1,
    };

    void setupFullScreenQuad();
    void drawFullScreenQuad();

    void setEnvContent(QImage texture);

    Camera lightCamera;
};

#endif

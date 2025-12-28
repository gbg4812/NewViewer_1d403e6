#ifndef _SHADOWMAPPING_H
#define _SHADOWMAPPING_H

#include "plugin.h"
#include "qvector3d.h"

class ShadowMapping : public QObject, public Plugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "Plugin")
    Q_INTERFACES(Plugin)

  public:
    void onPluginLoad();
    void preFrame();
    void postFrame();

    void onObjectAdd();
    bool drawScene();
    bool drawObject(int);

    bool paintGL();

    void keyPressEvent(QKeyEvent *);
    void mouseMoveEvent(QMouseEvent *);

  private:
    // add private methods and attributes here
    GLuint depthRenderBuff;
    GLuint shadowMapFrameBuffer;

    GLuint debugQuadVAO;
    GLuint debugQuadVBO;

    GLuint debugProgram;

    vector<QVector3D> debugQuad = {
        QVector3D(-1, -1, 0), QVector3D(1, 1, 0),  QVector3D(-1, 1, 0),
        QVector3D(-1, -1, 0), QVector3D(1, -1, 0), QVector3D(1, 1, 0),
    };

    void setupDebugQuad();
    void drawDebugQuad();
};

#endif

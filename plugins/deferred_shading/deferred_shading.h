#ifndef _DEFERRED_SHADING_H
#define _DEFERRED_SHADING_H

#include "plugin.h" 
#include <QPainter>

class Deferred_shading: public QObject, public Plugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "Plugin") 
    Q_INTERFACES(Plugin)

    public:
        void onPluginLoad();
        bool paintGL();
        void postFrame();
        void keyPressEvent(QKeyEvent*);

    private:
        void compileAndLinkShaders();
        void initGbuffer();
        void initScreenQuad();

        QOpenGLShaderProgram* deferredProgram;
        QOpenGLShaderProgram* gbuffer_colorAndSpecularProgram;
        QOpenGLShaderProgram* gbuffer_normalProgram;
        QOpenGLShaderProgram* gbuffer_positionProgram;

        GLuint VAO_screenQuad;
        GLuint colorAndSpecularMapId;
        GLuint normalMapId;
        GLuint positionMapId;

        QPainter painter;
        unsigned int mode;
};

#endif

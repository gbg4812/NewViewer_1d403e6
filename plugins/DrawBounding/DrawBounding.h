#ifndef _DRAWBOUNDING_H
#define _DRAWBOUNDING_H

#include "plugin.h" 

class DrawBounding: public QObject, public Plugin
{
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
	GLuint cubVAO;
	GLuint cubVBO;
	GLuint cubIDX;
	
	QOpenGLShaderProgram BBProgram;
	
	 std::vector<GLuint> idexos;
	 std::vector<float> verts;
};

#endif

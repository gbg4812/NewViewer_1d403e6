#include "DrawBounding.h"
#include "glwidget.h"
#include "object.h"
#include "qopenglext.h"
#include <GL/glu.h>
#include <vector>
#include <iostream>

#define printOpenGLError() printOglError(__FILE__, __LINE__)
#define CHECK() printOglError(__FILE__, __LINE__,__FUNCTION__)
#define DEBUG() cout << __FILE__ << " " << __LINE__ << " " << __FUNCTION__ << "\n";

int printOglError(const char file[], int line, const char func[]) 
{
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    if (glErr != GL_NO_ERROR)
    {
        printf("glError in file %s @ line %d: %s function: %s\n",
			     file, line, gluErrorString(glErr), func);
        retCode = 1;
    }
    return retCode;
}


void DrawBounding::onPluginLoad() {
    GLWidget *g = glwidget();
    g->makeCurrent();
    CHECK();
    
    
    // Carregar shaders
    
    BBProgram.addShaderFromSourceCode(QOpenGLShader::Vertex, 
    R"(#version 330 core
    	layout(location = 0) in vec3 vpos;
    	uniform mat4 viewProjectionMatrix;
    	uniform mat4 boundingMatrix;
	void main() {
		gl_Position = viewProjectionMatrix * boundingMatrix  * vec4(vpos, 1.0);
		//gl_Position = vec4(vpos, 1.0);
	}
    )"
    );
    
    BBProgram.addShaderFromSourceCode(QOpenGLShader::Fragment, 
    R"(#version 330 core
    	
    	out vec4 fcolor;
	void main() {
		fcolor = vec4(1.0, 0.0, 0.0, 1.0);
	}
    )"
    );
    
    BBProgram.link();
    
    std::cout << BBProgram.log().toStdString() << std::endl;

    for (int x = 0; x <= 1; x++) {
        for (int y = 0; y <= 1; y++) {
            for (int z = 0; z <= 1; z++) {
            	verts.push_back(z);
            	verts.push_back(y);
                verts.push_back(x);
            }
        }
    }

    idexos = {0, 1, 3, 0, 3, 2, 4, 6, 7, 4, 7, 5,
                                5, 7, 3, 5, 3, 1, 0, 2, 6, 0, 6, 4,
                                2, 3, 7, 2, 7, 6, 0, 4, 5, 0, 5, 1};
    g->glGenVertexArrays(1, &cubVAO);
    g->glBindVertexArray(cubVAO);
    
    g->glGenBuffers(1, &cubIDX);
    g->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubIDX);
    g->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idexos.size() * sizeof(uint), idexos.data(),
                    GL_STATIC_DRAW);
    
    g->glGenBuffers(1, &cubVBO);
    g->glBindBuffer(GL_ARRAY_BUFFER, cubVBO);
    g->glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(),
                    GL_STATIC_DRAW);
    CHECK();
    g->glEnableVertexAttribArray(0);
    g->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

    g->glBindVertexArray(0);
    CHECK();



}

void DrawBounding::preFrame() {}

void DrawBounding::postFrame() {
	auto& objects = scene()->objects();
	
	GLWidget *g = glwidget();
	CHECK();
    	g->makeCurrent();
    	BBProgram.bind();
	QMatrix4x4 VM = camera()->viewMatrix();
	QMatrix4x4 PM = camera()->projectionMatrix();
	QMatrix4x4 MVP = PM * VM;

    	BBProgram.setUniformValue("viewProjectionMatrix",  MVP);   

	g->glBindVertexArray(cubVAO);

	for(auto& obj : objects) {
		QMatrix4x4 bM = QMatrix4x4();
		Box bbox = obj.boundingBox();
		bM.translate(bbox.center().x(), bbox.center().y(), bbox.center().z());
		bM.scale(bbox.max().x() - bbox.min().x() , bbox.max().y() - bbox.min().y(), bbox.max().z() - bbox.min().z());
		bM.translate(-0.5, -0.5, -0.5);
		
	    	BBProgram.setUniformValue("boundingMatrix", bM);
		g->glDrawElements(GL_LINE_STRIP, idexos.size(), GL_UNSIGNED_INT ,0);
		
	}
	
	BBProgram.release();
	CHECK();

}

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

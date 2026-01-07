// GLarena, a plugin based platform to teach OpenGL programming
// © Copyright 2012-2018, ViRVIG Research Group, UPC, https://www.virvig.eu
//
// This file is part of GLarena
//
// GLarena is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "resaltat_sel.h"

void Resaltat_sel::onPluginLoad() {
    // Carregar shader, compile & link
    QString vs_src =
        "#version 330 core\n"
        "uniform mat4 modelViewProjectionMatrix;"
        "in vec3 vertex;"
        "in vec3 normal;"
        "in vec3 color;"

        "out vec4 col;"
        "void main() {"
        "  gl_Position = modelViewProjectionMatrix * vec4(vertex + normalize(normal) * 0.001,1.0);"
        "  col=vec4(color,1.0);"
        "}";
    vs = new QOpenGLShader(QOpenGLShader::Vertex, this);
    vs->compileSourceCode(vs_src);
    cout << "VS log:" << vs->log().toStdString() << endl;

    QString fs_src = "#version 330 core\n"
                     "out vec4 fragColor;"
                     "in vec4 col;"
                     "uniform int n;"
                     "void main() {"
                     "  fragColor=vec4(1.0, 0.5, 0.0, 0.3);"
                     "}";
    fs = new QOpenGLShader(QOpenGLShader::Fragment, this);
    fs->compileSourceCode(fs_src);
    cout << "FS log:" << fs->log().toStdString() << endl;

    program = new QOpenGLShaderProgram(this);
    program->addShader(vs);
    program->addShader(fs);
    program->link();
    cout << "Link log:" << program->log().toStdString() << endl;
}

void Resaltat_sel::preFrame() {

}

void Resaltat_sel::postFrame() {
    // bind shader and define uniforms
   	int seleccionat = scene()->selectedObject();
    if(seleccionat > -1) {
	 program->bind();
	 QMatrix4x4 scale;
	 Box bbox = scene()->objects()[seleccionat].boundingBox();
	 QMatrix4x4 MVP = camera()->projectionMatrix() * camera()->viewMatrix() * scale;
	 program->setUniformValue("modelViewProjectionMatrix", MVP);

    	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	drawPlugin()->drawObject(seleccionat);
	glDisable(GL_BLEND);
	program->release();
    }

    
    
}

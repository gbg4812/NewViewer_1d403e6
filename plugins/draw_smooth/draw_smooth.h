// GLarena, a plugin based platform to teach OpenGL programming
// © Copyright 2012-2024, ViRVIG Research Group, UPC, https://www.virvig.eu
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

#ifndef _DRAW_SMOOTH_H
#define _DRAW_SMOOTH_H

#include "plugin.h"
#include <vector>
#include <QPainter>

using namespace std;

class Draw_smooth: public QObject, public Plugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "Plugin") 
	Q_INTERFACES(Plugin)

    public:
        ~Draw_smooth();

        void onSceneClear();
        void onPluginLoad();
        void onObjectAdd();
        bool drawObject(int i);
        bool drawScene();
        void postFrame();
        void keyPressEvent(QKeyEvent*);

    private:
        void addVBO(unsigned int currentObject);
        void cleanUp();

        bool useIndices = false;

        // We will create a VAO for each object in the scene
        vector<GLuint> VAOs;           // ID of VAOs
        vector<GLuint> coordBuffers;   // ID of vertex coordinates buffer 
        vector<GLuint> normalBuffers;  // ID of normal components buffer 
        vector<GLuint> stBuffers;      // ID of (s,t) buffer 
        vector<GLuint> colorBuffers;   // ID of color buffer  
        vector<GLuint> indexBuffers;   // ID of the index buffer
        vector<GLuint> numElements;    // Size of each VBO

        QPainter painter;
};

#endif

#include "deferred_shading.h"
#include "glwidget.h"

const int IMAGE_WIDTH = 1024;
const int IMAGE_HEIGHT = IMAGE_WIDTH;

void Deferred_shading::compileAndLinkShaders()
{
    GLWidget & g = *glwidget();
    g.makeCurrent();

    // Compile and link the Deferred Shading shader ------------
    // this shader will calculate the lighting based on the G buffer information,
    // that is, the position, normal and color textures.
    QOpenGLShader* vs = new QOpenGLShader(QOpenGLShader::Vertex, this);
    vs->compileSourceFile(g.getPluginPath() + "/../deferred_shading/deferredshading.vert");
    QOpenGLShader* fs = new QOpenGLShader(QOpenGLShader::Fragment, this);
    fs->compileSourceFile(g.getPluginPath() + "/../deferred_shading/deferredshading.frag");
    deferredProgram = new QOpenGLShaderProgram(this);
    deferredProgram->addShader(vs);
    deferredProgram->addShader(fs);
    deferredProgram->link();

    // Compile and link the Color and Specular Map shader -------------
    // this shader will store in the color buffer the color information (diffuse material)
    // in the RGB components, and the specular information (material shininess) in the Alpha component
    vs = new QOpenGLShader(QOpenGLShader::Vertex, this);
    vs->compileSourceFile(g.getPluginPath() + "/../deferred_shading/gbuffer_colorAndSpecular.vert");
    fs = new QOpenGLShader(QOpenGLShader::Fragment, this);
    fs->compileSourceFile(g.getPluginPath() + "/../deferred_shading/gbuffer_colorAndSpecular.frag");
    gbuffer_colorAndSpecularProgram = new QOpenGLShaderProgram(this);
    gbuffer_colorAndSpecularProgram->addShader(vs);
    gbuffer_colorAndSpecularProgram->addShader(fs);
    gbuffer_colorAndSpecularProgram->link();

    // Compile and link the Normal Map shader --------------------
    // this shader will store in the color buffer the normal information (per fragment)
    vs = new QOpenGLShader(QOpenGLShader::Vertex, this);
    vs->compileSourceFile(g.getPluginPath() + "/../deferred_shading/gbuffer_normal.vert");
    fs = new QOpenGLShader(QOpenGLShader::Fragment, this);
    fs->compileSourceFile(g.getPluginPath() + "/../deferred_shading/gbuffer_normal.frag");
    gbuffer_normalProgram = new QOpenGLShaderProgram(this);
    gbuffer_normalProgram->addShader(vs);
    gbuffer_normalProgram->addShader(fs);
    gbuffer_normalProgram->link();

    // Compile and link the Position Map shader --------------------
    // this shader will store in the color buffer the normal information (per fragment)
    vs = new QOpenGLShader(QOpenGLShader::Vertex, this);
    vs->compileSourceFile(g.getPluginPath() + "/../deferred_shading/gbuffer_position.vert");
    fs = new QOpenGLShader(QOpenGLShader::Fragment, this);
    fs->compileSourceFile(g.getPluginPath() + "/../deferred_shading/gbuffer_position.frag");
    gbuffer_positionProgram = new QOpenGLShaderProgram(this);
    gbuffer_positionProgram->addShader(vs);
    gbuffer_positionProgram->addShader(fs);
    gbuffer_positionProgram->link();
}

void Deferred_shading::initGbuffer()
{
    GLWidget & g = *glwidget();
    g.makeCurrent();

    // Color map texture
    g.glGenTextures(1, &colorAndSpecularMapId);
    g.glBindTexture(GL_TEXTURE_2D, colorAndSpecularMapId);
    g.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    g.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    g.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    g.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    g.glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, IMAGE_WIDTH, IMAGE_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    g.glBindTexture(GL_TEXTURE_2D, 0);

    // Normal map texture
    g.glGenTextures(1, &normalMapId);
    g.glBindTexture(GL_TEXTURE_2D, normalMapId);
    g.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    g.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    g.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    g.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    g.glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, IMAGE_WIDTH, IMAGE_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    g.glBindTexture(GL_TEXTURE_2D, 0);

    // Position map texture
    g.glGenTextures(1, &positionMapId);
    g.glBindTexture(GL_TEXTURE_2D, positionMapId);
    g.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    g.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    g.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    g.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    g.glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, IMAGE_WIDTH, IMAGE_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    g.glBindTexture(GL_TEXTURE_2D, 0);
}

void Deferred_shading::initScreenQuad()
{
    GLWidget & g = *glwidget();
    g.makeCurrent();

    // Create & bind empty VAO
    g.glGenVertexArrays(1, &VAO_screenQuad);
    g.glBindVertexArray(VAO_screenQuad);

    // Create VBO with (x,y,z) coordinates
    float coords[] = { -1, -1, 0, 
                        1, -1, 0, 
                        -1,  1, 0, 
                        1,  1, 0};

    GLuint VBO_coords;
    g.glGenBuffers(1, &VBO_coords);
    g.glBindBuffer(GL_ARRAY_BUFFER, VBO_coords);
    g.glBufferData(GL_ARRAY_BUFFER, sizeof(coords), coords, GL_STATIC_DRAW);
    g.glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    g.glEnableVertexAttribArray(0);
    g.glBindVertexArray(0);
}

void Deferred_shading::onPluginLoad()
{
    mode = 0;
    compileAndLinkShaders();
    initGbuffer();
    initScreenQuad();
}

bool Deferred_shading::paintGL()
{
    GLWidget & g = *glwidget();

    QMatrix4x4 MVP = camera()->projectionMatrix() * camera()->viewMatrix();

    g.glClearColor(0,0,0,0);

    // Pass 1. Draw the scene and store color and specular information
    if (mode == 0 || mode == 3)
    {
        g.glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        gbuffer_colorAndSpecularProgram->bind();
        gbuffer_colorAndSpecularProgram->setUniformValue("modelViewProjectionMatrix", MVP); 
        gbuffer_colorAndSpecularProgram->setUniformValue("matDiffuse", QColor(80, 200, 40, 0));
        gbuffer_colorAndSpecularProgram->setUniformValue("matShininess", 1.0f); // (0, 1.]  on 1. representa la màxima brillantor
        if (drawPlugin()) drawPlugin()->drawScene();

        // TODO: Copy from the color buffer to the corresponding texture
    }

    // Pass 2. Draw the scene and store normal information
    if (mode == 1 || mode == 3)
    {
        g.glClearColor(0.5, 0.5, 0.5, 0);
        g.glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        gbuffer_normalProgram->bind();
        gbuffer_normalProgram->setUniformValue("modelViewProjectionMatrix", MVP); 

        if (drawPlugin()) drawPlugin()->drawScene();
        g.glClearColor(0,0,0,0);

        // TODO: Copy from the color buffer to the corresponding texture
    }

    // Pass 3. Draw the scene and store positional information
    if (mode == 2 || mode == 3)
    {
        g.glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        gbuffer_positionProgram->bind();
        gbuffer_positionProgram->setUniformValue("modelViewProjectionMatrix", MVP); 
        gbuffer_positionProgram->setUniformValue("BoundingBoxMin", scene()->boundingBox().min());
        gbuffer_positionProgram->setUniformValue("BoundingBoxMax", scene()->boundingBox().max());

        if (drawPlugin()) drawPlugin()->drawScene();

        // TODO: Copy from the color buffer to the corresponding texture
    }

    // Pass 4. Deferred shading
    if (mode == 3)
    {
        // Activate G-Buffer
        // TODO: activate the necessary textures

        g.glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        deferredProgram->bind();
        deferredProgram->setUniformValue("BoundingBoxMin", scene()->boundingBox().min());
        deferredProgram->setUniformValue("BoundingBoxMax", scene()->boundingBox().max());
        deferredProgram->setUniformValue("colorAndSpecularMap", 0);
        deferredProgram->setUniformValue("normalMap", 1);
        deferredProgram->setUniformValue("positionMap", 2);
        deferredProgram->setUniformValue("cameraPosition", camera()->getObs());
        deferredProgram->setUniformValue("SIZE", float(IMAGE_WIDTH));  

        // draw a quad covering all the screen
        g.glBindVertexArray (VAO_screenQuad);
        g.glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        g.glBindVertexArray(0);

        g.defaultProgram()->bind();
        g.glBindTexture(GL_TEXTURE_2D, 0);
    }

    return true;
}

void Deferred_shading::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_E) 
    {
        // 'E' key was pressed.
        mode = (mode + 1) % 4;
    } 
}

void Deferred_shading::postFrame()
{
    QFont font;
    font.setPixelSize(32);
    painter.begin(glwidget());
    painter.setFont(font);
    painter.setPen(Qt::white); 
    int x = 15;
    int y = 40;
    QString s;
    if (mode == 0) s = QString("Color and Specular Map");
    else if (mode == 1) s = QString("Normal Map");
    else if (mode == 2) s = QString("Position Map");
    else if (mode == 3) s = QString("Deferred Shading");
    painter.drawText(x, y, s);
    painter.end();
}
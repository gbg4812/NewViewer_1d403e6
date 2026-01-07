#version 330 core

layout (location = 0) in vec3 vertex;

uniform mat4 modelViewProjectionMatrix;
uniform vec3 BoundingBoxMin;
uniform vec3 BoundingBoxMax;

out vec3 vPos;

void main()
{
    vPos = (vertex - BoundingBoxMin) / (BoundingBoxMax - BoundingBoxMin);
    gl_Position = modelViewProjectionMatrix * vec4(vertex,1.0);
}


#version 330 core

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;

uniform mat4 modelViewProjectionMatrix;

out vec3 vNormal;

void main()
{
    vNormal = normal;
    gl_Position = modelViewProjectionMatrix * vec4(vertex,1.0);
}


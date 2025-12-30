#version 330 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 colors;
layout(location = 3) in vec2 texCoord;

uniform int objectID;

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 lightMatrix;
uniform mat3 normalMatrix;

out vec4 lpos;
out vec4 vpos;
out vec3 vcolor;
out vec3 vnorm;


void main(){

    gl_Position = projectionMatrix * modelViewMatrix  * vec4(vertex, 1.0f);
    lpos = lightMatrix * vec4(vertex, 1.0f);
    vpos = modelViewMatrix * vec4(vertex, 1.0f);
    vcolor = colors;
    vnorm = normalMatrix * normal;

}

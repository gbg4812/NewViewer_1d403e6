#version 330 core

layout(location=0) in vec3 vpos; 
layout(location=1) in vec2 texCoord;

out vec2 vtexCoord;


void main() {
    gl_Position = vec4(vpos, 1.0f);
    vtexCoord = texCoord;
}

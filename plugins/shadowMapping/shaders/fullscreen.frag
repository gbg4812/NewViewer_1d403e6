#version 330 core


out vec4 fragColor;

in vec2 vtexCoord;

uniform sampler2D shadMap;

void main() {
    fragColor = vec4(texture(shadMap, vtexCoord).rgb, 1.0f);
}

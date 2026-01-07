#version 330 core

in vec3 vNormal;

out vec4 fragColor;

void main()
{
    vec3 N = normalize(vNormal) * 0.5 + 0.5;
    // TODO: write the correct value in the color buffer
    fragColor = vec4(1.0);
}


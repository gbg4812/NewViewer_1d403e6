#version 330 core
out vec4 fragColor;

uniform sampler2D colorMap;

uniform float SIZE;


void main()
{
    vec2 st = (gl_FragCoord.xy - vec2(0.5)) / SIZE;


    vec4 tmp = texture(colorMap, st);

        
    fragColor = tmp;
    
}


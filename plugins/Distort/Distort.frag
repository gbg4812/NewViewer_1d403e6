#version 330 core
out vec4 fragColor;

uniform sampler2D colorMap;

uniform float SIZE;

uniform float time;


void main()
{
    vec2 st = (gl_FragCoord.xy - vec2(0.5)) / SIZE;

    vec4 tmp = texture(colorMap, st+vec2( 0.01 * sin(10.0*time+30.0* st.s)));
        
    fragColor = tmp;
    
}


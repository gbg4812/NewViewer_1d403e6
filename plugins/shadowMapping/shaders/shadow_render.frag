#version 330 core


out vec4 fragColor;

in vec4 lpos;
in vec4 vpos;
in vec3 vnorm;
in vec3 vcolor;

uniform sampler2D depthMap;

uniform vec3 lightPos;

uniform int objectID;

uniform vec4 matDiffuse;
uniform vec4 matSpecular;
uniform float matShininess;
uniform float matRoughness;


void main(){

   vec3 fragPos = lpos.xyz / lpos.w;
   fragPos *= 0.5;
   fragPos += 0.5;

   //vec3 randColor = vec3(sin(objectID), cos(objectID), sin(objectID+0.2));
       
   
   float texelSize = 1.0 / (float)textureSize(depthMap, 0).x;
   float shadow = 0;
   float counter = 0;
   vec3 n = normalize(vnorm);
   vec3 L = normalize(lightPos - vpos.xyz);
   float NdotL = dot(n, L);
   vec3 R = reflect(-L, n);
   float VdotR = dot(normalize(-vpos.xyz), R);

   for(float x = -1.0; x < 1.0; x += 1.0 ) {
        for(float y = -1.0; y < 1.0; y+= 1.0) {
            float closestDepth = texture(depthMap, fragPos.xy + vec2(x, y) * texelSize).r; 
            float  bias = max(0.025 * (1.0 - NdotL), 0.005);
            shadow += fragPos.z  - bias > closestDepth  ? 1.0 : 0.0;
            counter++;
        }
   }

   shadow /= counter;
   shadow = 1.0 - shadow;
   shadow = shadow * 0.8;
   fragColor = matDiffuse*shadow*max(0.0, NdotL) + (1.0 - matRoughness) * matSpecular*shadow*pow(max(0.0, VdotR), matShininess);
}

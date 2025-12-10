#version 460 core

layout (location = 0) out vec4 g_position; 
layout (location = 1) out vec4 g_normal;
layout (location = 2) out vec4 g_diffSpec;

in vec3 fragmentPos;
in vec2 texCoord;
in mat3 tangentToWorld;

uniform sampler2D u_albedoMap;
uniform sampler2D u_specularMap;
uniform sampler2D u_normalMap;


vec3 normalFromMap = texture(u_normalMap, texCoord).rgb;
vec3 normal = normalize(tangentToWorld * (normalFromMap * 2.0 - 1.0));

void main()
{
     
     g_position = vec4(fragmentPos,1.0); 

     g_normal = vec4(normal,1.0);
     
     vec3 diffuse = texture(u_albedoMap,texCoord).rgb;
     float specular = texture(u_specularMap,texCoord).r;
     
     g_diffSpec = vec4(diffuse,specular);
     
   
}
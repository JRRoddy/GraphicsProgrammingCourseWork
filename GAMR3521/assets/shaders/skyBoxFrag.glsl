#version 460 core

layout(location = 0) out vec4 colour;

in vec3 cubeTextureCoords; 

uniform samplerCube u_cubeMap;

void main()
{
  
  
  colour = texture(u_cubeMap,cubeTextureCoords);



} 




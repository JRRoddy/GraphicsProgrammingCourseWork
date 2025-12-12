
#version 460 core 

layout(location  = 0) in vec3 a_vertexPos;


uniform mat4 u_model;





out vec3  vs_fragmentPos;

void main()
{


   vs_fragmentPos = vec3(u_model * vec4(a_vertexPos,1.0));


}
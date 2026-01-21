#version 460 core

layout(location = 0) in vec3 a_vertexPos;





out vec3 vs_fragmentPos;

uniform mat4 u_model;




void main()
{
  vs_fragmentPos = vec3(u_model * vec4(a_vertexPos,1.0));

 
  gl_Position = vec4(vs_fragmentPos,1.0);
  
 
  
   

}





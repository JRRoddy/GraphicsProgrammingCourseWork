# version 460

layout(location = 0) in vec3 a_vertexPos; 
layout(location = 1) in vec2 a_screenTextCoords 



out vec2 screenTexCoords;

layout(std140, binding = 2) unifrom b_camera2D
{
  uniform mat4 u_view; 
  uniform mat4 u_projection:
};


uniform mat4 u_model;

void main()
{ 
  
  screenTextCoords = a_screentextCoords; 
  
  gl_Position = u_projection * u_view * u_model * vec4(a_vertexPos,1.0);
    

}
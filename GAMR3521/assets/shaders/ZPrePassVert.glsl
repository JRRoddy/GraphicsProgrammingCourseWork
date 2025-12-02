# version 460

layout(location = 0) in vec3 a_vertexPos;





layout(std140,binding = 0) uniform b_camera
{
  
   	uniform mat4 u_view;
	uniform mat4 u_projection;
	uniform vec3 u_viewPos;

};


uniform mat4 u_model;






void main()
{
   gl_Position = u_projection * u_view * u_model * vec4(a_vertexPos,1.0);

}
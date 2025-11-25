# version 460 

layout(location = 0) in vec3 a_cubeVertexPos;

uniform mat4 u_skyBoxView; 
uniform mat4 u_model;

layout (std140, binding = 0) uniform b_camera
{
	uniform mat4 u_view;
	uniform mat4 u_projection;
	uniform vec3 u_viewPos;
};
out vec3 cubeTextureCoords;
void main()
{
  cubeTextureCoords = a_cubeVertexPos;

  gl_Position = u_projection * u_skyBoxView * u_model * vec4(a_cubeVertexPos,1.0); 

}
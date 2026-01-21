#version 460 core
			
layout(location = 0) in vec3 a_vertexPosition;
layout(location = 1) in vec2 a_texCoord;

out vec2 texCoord; 
layout (std140, binding = 0) uniform b_camera
{
	uniform mat4 u_view;
	uniform mat4 u_projection;
	uniform vec3 u_viewPos;
};

layout (std140, binding = 5) uniform b_lightPassCamera
{
  uniform mat4 u_lightPassview; 
  uniform mat4 u_lightPassProjection;
};





uniform mat4 u_model;

void main()
{
    
	texCoord = a_texCoord;
	gl_Position = u_lightPassProjection * u_lightPassview * u_model * vec4(a_vertexPosition,1.0);


}